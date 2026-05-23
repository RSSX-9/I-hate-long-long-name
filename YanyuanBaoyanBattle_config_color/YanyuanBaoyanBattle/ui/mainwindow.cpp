#include "mainwindow.h"

#include "buffdialog.h"
#include "eventlog.h"
#include "examdialog.h"
#include "../libcore/gameconfig.h"
#include "recruitdialog.h"

#include <QHeaderView>
#include <QFont>
#include <QColor>
#include <QBrush>
#include <QLabel>
#include <QMessageBox>
#include <QtMath>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QProgressBar>

namespace {
// 状态标签色彩辅助函数
QColor statusColor(const QString &status)
{
    if (status == QStringLiteral("在读")) return QColor(GameConfig::GoodStatusColor);
    if (status == QStringLiteral("休学")) return QColor(GameConfig::BadStatusColor);
    return QColor(GameConfig::AttributeInactiveColor); // 转学
}

// 封装的高级动态颜色进度条组件
QWidget* createAttributeProgressBar(int value, bool active)
{
    auto *container = new QWidget();
    auto *layout = new QHBoxLayout(container);
    layout->setContentsMargins(6, 2, 6, 2);

    auto *bar = new QProgressBar(container);
    bar->setRange(GameConfig::AttributeMin, GameConfig::AttributeMax);
    bar->setValue(value);
    bar->setTextVisible(true);
    bar->setAlignment(Qt::AlignCenter);

    // 映射颜色到系统配置
    QString barColor;
    if (!active) {
        barColor = QString::fromLatin1(GameConfig::AttributeInactiveColor);
    } else if (value < GameConfig::AttributeDangerThreshold) {
        barColor = QString::fromLatin1(GameConfig::AttributeDangerColor);
    } else if (value < GameConfig::AttributeWarningThreshold) {
        barColor = QString::fromLatin1(GameConfig::AttributeWarningColor);
    } else {
        barColor = QString::fromLatin1(GameConfig::AttributeHighColor);
    }

    bar->setStyleSheet(QString(R"(
        QProgressBar {
            border: 1px solid #e2e8f0;
            border-radius: 4px;
            background-color: #f1f5f9;
            color: %2;
            font-weight: bold;
            font-size: 11px;
        }
        QProgressBar::chunk {
            background-color: %1;
            border-radius: 3px;
        }
    )").arg(barColor, value > 45 ? "#ffffff" : "#1e293b"));

    layout->addWidget(bar);
    return container;
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_manager(new GameManager(this))
{
    setWindowTitle(QStringLiteral("燕园保研战 - 校园高级模拟看板"));
    m_examSystem.loadFromResource();
    buildLayout();

    connect(m_recruitButton, &QPushButton::clicked, this, &MainWindow::startRecruitment);
    connect(m_nextButton, &QPushButton::clicked, m_manager, &GameManager::advance);
    connect(m_examButton, &QPushButton::clicked, this, &MainWindow::doSubstituteExam);
    connect(m_sellButton, &QPushButton::clicked, this, &MainWindow::sellSelectedStudent);

    connect(m_manager, &GameManager::stateChanged, this, &MainWindow::refreshUi);
    connect(m_manager, &GameManager::logAdded, m_logWidget, &EventLogWidget::appendLog);
    connect(m_manager, &GameManager::requestBuffChoice, this, &MainWindow::handleBuffChoice);
    connect(m_manager, &GameManager::gameEnded, this, &MainWindow::showGameEnd);
    connect(m_manager, &GameManager::errorMessage, this, &MainWindow::showError);

    refreshUi();
}

void MainWindow::buildLayout()
{
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // ================= 1. 顶部数据大屏看板 (Dashboard Panel) =================
    auto *dashboardLayout = new QHBoxLayout();
    dashboardLayout->setSpacing(12);

    auto cards = QVector<QPair<QString, QLabel**>>{
        {QStringLiteral("学期进度 / TIME"), &m_semesterValueLabel},
        {QStringLiteral("当前阶段 / GAME PHASE"), &m_phaseValueLabel},
        {QStringLiteral("学院财政 / BUDGET"), &m_coinValueLabel},
        {QStringLiteral("保研现状 (目标 60%) / KPI"), &m_kpiValueLabel}
    };

    for (const auto &pair : cards) {
        auto *card = new QFrame(this);
        card->setObjectName(QStringLiteral("MetricCard"));
        auto *cardLayout = new QVBoxLayout(card);
        cardLayout->setSpacing(4);
        cardLayout->setContentsMargins(10, 8, 10, 8);

        auto *title = new QLabel(pair.first, card);
        title->setStyleSheet(QStringLiteral("font-size: 11px; color: #64748b; font-weight: bold;"));

        *(pair.second) = new QLabel(QStringLiteral("-"), card);
        (*(pair.second))->setStyleSheet(QStringLiteral("font-size: 18px; font-weight: bold; color: #0f172a;"));

        cardLayout->addWidget(title);
        cardLayout->addWidget(*(pair.second));
        dashboardLayout->addWidget(card);
    }
    mainLayout->addLayout(dashboardLayout);

    // ================= 2. 中间核心分栏显示 (Splitter Panel) =================
    auto *splitter = new QSplitter(Qt::Horizontal, this);

    // 左侧：学生动态视图表格
    m_studentTable = new QTableWidget(this);
    m_studentTable->setColumnCount(7);
    m_studentTable->setHorizontalHeaderLabels({
        QStringLiteral("姓名"), QStringLiteral("状态"),
        QStringLiteral("成绩 (>=90)"), QStringLiteral("健康 (>=70)"), QStringLiteral("心理 (>=70)"),
        QStringLiteral("身价 (金币)"), QStringLiteral("是否达标")
    });
    m_studentTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_studentTable->verticalHeader()->setVisible(false);
    m_studentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_studentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_studentTable->setSelectionMode(QAbstractItemView::SingleSelection);
    splitter->addWidget(m_studentTable);

    // 右侧：实时日志系统
    m_logWidget = new EventLogWidget(this);
    splitter->addWidget(m_logWidget);

    // 设定初始分割比例
    splitter->setStretchFactor(0, 7);
    splitter->setStretchFactor(1, 3);
    mainLayout->addWidget(splitter);

    // ================= 3. 底部现代控制台 (Control Console) =================
    auto *controlLayout = new QHBoxLayout();

    m_recruitButton = new QPushButton(QStringLiteral("🏫 开局招生咨询"), this);
    m_recruitButton->setObjectName(QStringLiteral("recruitButton"));

    m_examButton = new QPushButton(QStringLiteral("📝 安排精锐替考"), this);
    m_examButton->setObjectName(QStringLiteral("examButton"));

    m_sellButton = new QPushButton(QStringLiteral("✈️ 协助转学发展"), this);
    m_sellButton->setObjectName(QStringLiteral("sellButton"));

    m_nextButton = new QPushButton(QStringLiteral("⏩ 推进教学日程 (下一阶段)"), this);
    m_nextButton->setObjectName(QStringLiteral("nextButton"));
    m_nextButton->setMinimumHeight(40); // 突出主推按钮

    controlLayout->addWidget(m_recruitButton);
    controlLayout->addWidget(m_examButton);
    controlLayout->addWidget(m_sellButton);
    controlLayout->addStretch();
    controlLayout->addWidget(m_nextButton);

    mainLayout->addLayout(controlLayout);
}

void MainWindow::refreshUi()
{
    // 1. 刷新看板指标
    if (m_manager->semester() == 0) {
        m_semesterValueLabel->setText(QStringLiteral("尚未开学"));
        m_semesterValueLabel->setStyleSheet(QStringLiteral("font-size: 18px; font-weight: bold; color: #64748b;"));
    } else {
        m_semesterValueLabel->setText(QStringLiteral("第 %1 / %2 学期").arg(m_manager->semester()).arg(GameConfig::TotalSemesters));
        m_semesterValueLabel->setStyleSheet(QStringLiteral("font-size: 18px; font-weight: bold; color: #3b82f6;"));
    }

    m_phaseValueLabel->setText(m_manager->phaseText());
    m_coinValueLabel->setText(QStringLiteral("%1 金币").arg(m_manager->coins()));

    const int rec = m_manager->recommendedCount();
    const int den = m_manager->finalDenominator();
    const double rate = m_manager->currentRecommendRate() * 100.0;
    m_kpiValueLabel->setText(QStringLiteral("%1% (%2/%3人)").arg(rate, 0, 'f', 1).arg(rec).arg(den));
    if (rate >= GameConfig::TargetRecommendRate * 100.0) {
        m_kpiValueLabel->setStyleSheet(QStringLiteral("font-size: 18px; font-weight: bold; color: #10b981;"));
    } else {
        m_kpiValueLabel->setStyleSheet(QStringLiteral("font-size: 18px; font-weight: bold; color: #f59e0b;"));
    }

    // 2. 刷新按钮可用性
    const bool notStarted = (m_manager->phase() == GamePhase::NotStarted);
    const bool finished = (m_manager->phase() == GamePhase::Finished);

    m_recruitButton->setEnabled(notStarted);
    m_nextButton->setEnabled(!finished);
    m_examButton->setEnabled(m_manager->canSubstitute());
    m_sellButton->setEnabled(m_manager->canSell());

    if (m_manager->canSubstitute()) {
        m_examButton->setText(QStringLiteral("📝 安排精锐替考 (%1/%2)").arg(m_manager->substitutesUsed()).arg(m_manager->substituteLimit()));
    } else {
        m_examButton->setText(QStringLiteral("📝 安排精锐替考 (非决策期)"));
    }

    // 3. 重绘数据表
    updateStudentTable();
}

void MainWindow::updateStudentTable()
{
    QVector<Student> students = m_manager->students();
    m_studentTable->setRowCount(students.size());

    for (int i = 0; i < students.size(); ++i) {
        const Student &s = students[i];
        const bool active = s.isActive();

        // 姓名
        auto *nameItem = new QTableWidgetItem(s.name());
        if (!active) nameItem->setForeground(QBrush(QColor(GameConfig::AttributeInactiveColor)));
        m_studentTable->setItem(i, 0, nameItem);

        // 状态标签
        auto *statusItem = new QTableWidgetItem(s.statusText());
        statusItem->setForeground(QBrush(statusColor(s.statusText())));
        statusItem->setFont(QFont(statusItem->font().family(), -1, QFont::Bold));
        m_studentTable->setItem(i, 1, statusItem);

        // 成绩、健康、心理维度 (动态进度条注入)
        m_studentTable->setCellWidget(i, 2, createAttributeProgressBar(s.score(), active));
        m_studentTable->setCellWidget(i, 3, createAttributeProgressBar(s.health(), active));
        m_studentTable->setCellWidget(i, 4, createAttributeProgressBar(s.mood(), active));

        // 市场身价
        auto *priceItem = new QTableWidgetItem(QStringLiteral("%1 金币").arg(s.marketPrice()));
        priceItem->setTextAlignment(Qt::AlignCenter);
        m_studentTable->setItem(i, 5, priceItem);

        // 是否满足保研基本线
        auto *recItem = new QTableWidgetItem(s.isRecommended() ? QStringLiteral("🎉 已达标") : QStringLiteral("❌ 未满足"));
        if (s.isRecommended()) {
            recItem->setForeground(QBrush(QColor(GameConfig::GoodStatusColor)));
            recItem->setFont(QFont(recItem->font().family(), -1, QFont::Bold));
        } else {
            recItem->setForeground(QBrush(QColor(GameConfig::AttributeInactiveColor)));
        }
        m_studentTable->setItem(i, 6, recItem);
    }
}

int MainWindow::selectedStudentRow() const
{
    return m_studentTable->currentRow();
}

void MainWindow::startRecruitment()
{
    RecruitDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        m_manager->startGame(dialog.selectedStudents());
    }
}

void MainWindow::doSubstituteExam()
{
    int row = selectedStudentRow();
    if (row < 0) {
        showError(QStringLiteral("请先在左侧表格中选择一名在读学生。"));
        return;
    }

    QVector<Student> students = m_manager->students();
    if (row >= students.size() || !students[row].isActive()) {
        showError(QStringLiteral("只能为当前【在读】的学生安排替考。"));
        return;
    }

    // 随机抽取5道题
    QVector<Question> qList = m_examSystem.randomQuestions(GameConfig::SubstituteQuestionCount);
    ExamDialog dialog(qList, students[row].name(), this);
    if (dialog.exec() == QDialog::Accepted) {
        m_manager->applySubstituteExam(row, dialog.accuracy());
    }
}

void MainWindow::sellSelectedStudent()
{
    int row = selectedStudentRow();
    if (row < 0) {
        showError(QStringLiteral("请先在左侧表格中选择一名要申请转学的学生。"));
        return;
    }

    QVector<Student> students = m_manager->students();
    if (row >= students.size() || !students[row].isActive()) {
        showError(QStringLiteral("该学生已经休学或已转学。"));
        return;
    }

    auto reply = QMessageBox::question(this, QStringLiteral("确认转学"),
                                       QStringLiteral("确定要协助【%1】转学吗？预计回收资金 %2 金币。")
                                           .arg(students[row].name())
                                           .arg(qRound(students[row].marketPrice() * GameConfig::SellIncomeRate)),
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        m_manager->sellStudent(row);
    }
}

void MainWindow::handleBuffChoice(const QVector<Buff> &choices, const QString &title)
{
    BuffDialog dialog(choices, title, this);
    if (dialog.exec() == QDialog::Accepted) {
        m_manager->applyChosenBuff(dialog.selectedIndex());
    } else {
        m_manager->applyChosenBuff(0); // 默认第一项
    }
}

void MainWindow::showGameEnd(bool win, const QString &message)
{
    if (win) {
        QMessageBox::information(this, QStringLiteral("🎉 战役胜利！"), message);
    } else {
        QMessageBox::critical(this, QStringLiteral("😭 满盘皆输"), message);
    }
}

void MainWindow::showError(const QString &message)
{
    QMessageBox::warning(this, QStringLiteral("提示"), message);
}

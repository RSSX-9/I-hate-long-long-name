#include "mainwindow.h"

#include "buffdialog.h"
#include "eventlog.h"
#include "examdialog.h"
#include "gameconfig.h"
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

namespace {
QColor attributeColor(int value, bool active)
{
    if (!active) {
        return QColor(GameConfig::AttributeInactiveColor);
    }
    if (value < GameConfig::AttributeDangerThreshold) {
        return QColor(GameConfig::AttributeDangerColor);
    }
    if (value < GameConfig::AttributeWarningThreshold) {
        return QColor(GameConfig::AttributeWarningColor);
    }
    if (value >= GameConfig::AttributeHighThreshold) {
        return QColor(GameConfig::AttributeHighColor);
    }
    return QColor(GameConfig::AttributeNormalColor);
}

QTableWidgetItem *attributeItem(int value, bool active)
{
    auto *item = new QTableWidgetItem(QString::number(value));
    item->setForeground(QBrush(attributeColor(value, active)));

    QFont font = item->font();
    if (active && (value < GameConfig::AttributeWarningThreshold || value >= GameConfig::AttributeHighThreshold)) {
        font.setBold(true);
    }
    item->setFont(font);

    return item;
}

QTableWidgetItem *coloredTextItem(const QString &text, const QColor &color, bool bold = false)
{
    auto *item = new QTableWidgetItem(text);
    item->setForeground(QBrush(color));

    QFont font = item->font();
    font.setBold(bold);
    item->setFont(font);

    return item;
}
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_manager(new GameManager(this))
{
    setWindowTitle(QStringLiteral("燕园保研战 - 校园经营模拟器"));
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

void MainWindow::startRecruitment()
{
    RecruitDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        m_manager->startGame(dialog.selectedStudents());
    }
}

void MainWindow::refreshUi()
{
    const QString status = QStringLiteral("学期：%1 / %2    阶段：%3    金币：%4    替考：%5/%6")
            .arg(m_manager->semester() == 0 ? QStringLiteral("-") : QString::number(m_manager->semester()))
            .arg(GameConfig::TotalSemesters)
            .arg(m_manager->phaseText())
            .arg(m_manager->coins())
            .arg(m_manager->substitutesUsed())
            .arg(m_manager->substituteLimit());

    m_statusLabel->setText(status);

    const int rec = m_manager->recommendedCount();
    const int den = m_manager->finalDenominator();
    m_rateLabel->setText(QStringLiteral("当前保研人数：%1/%2，保研率：%3%（目标 %4%）")
                         .arg(rec)
                         .arg(den)
                         .arg(m_manager->currentRecommendRate() * 100.0, 0, 'f', 1)
                         .arg(GameConfig::TargetRecommendRate * 100.0, 0, 'f', 0));

    updateStudentTable();

    const GamePhase phase = m_manager->phase();
    m_recruitButton->setEnabled(phase == GamePhase::NotStarted || phase == GamePhase::Finished);
    m_nextButton->setEnabled(phase != GamePhase::NotStarted && phase != GamePhase::Finished);
    m_examButton->setEnabled(m_manager->canSubstitute());
    m_sellButton->setEnabled(m_manager->canSell());

    if (phase == GamePhase::MidtermPreparation) {
        m_nextButton->setText(QStringLiteral("进入期中考试"));
    } else if (phase == GamePhase::Settlement) {
        m_nextButton->setText(m_manager->semester() >= GameConfig::TotalSemesters ? QStringLiteral("最终结算") : QStringLiteral("进入下一学期"));
    } else {
        m_nextButton->setText(QStringLiteral("推进阶段"));
    }
}

void MainWindow::handleBuffChoice(const QVector<Buff> &choices, const QString &title)
{
    BuffDialog dialog(choices, title, this);
    if (dialog.exec() == QDialog::Accepted) {
        m_manager->applyChosenBuff(dialog.selectedIndex());
    } else {
        m_manager->applyChosenBuff(0);
    }
}

void MainWindow::doSubstituteExam()
{
    const int row = selectedStudentRow();
    if (row < 0) {
        showError(QStringLiteral("请先在学生表中选择一名在读学生。"));
        return;
    }

    QVector<Question> questions = m_examSystem.randomQuestions(GameConfig::SubstituteQuestionCount);
    const QVector<Student> students = m_manager->students();
    if (row >= students.size()) {
        return;
    }

    ExamDialog dialog(questions, students[row].name(), this);
    if (dialog.exec() == QDialog::Accepted) {
        m_manager->applySubstituteExam(row, dialog.accuracy());
    }
}

void MainWindow::sellSelectedStudent()
{
    const int row = selectedStudentRow();
    if (row < 0) {
        showError(QStringLiteral("请先在学生表中选择一名在读学生。"));
        return;
    }

    const QVector<Student> students = m_manager->students();
    if (row >= students.size()) {
        return;
    }

    const int income = qRound(students[row].marketPrice() * GameConfig::SellIncomeRate);
    const int ret = QMessageBox::question(this,
                                          QStringLiteral("确认转学"),
                                          QStringLiteral("确定出售【%1】吗？可获得 %2 金币，但 TA 将无法保研。")
                                          .arg(students[row].name())
                                          .arg(income));
    if (ret == QMessageBox::Yes) {
        m_manager->sellStudent(row);
    }
}

void MainWindow::showGameEnd(bool win, const QString &message)
{
    QMessageBox::information(this,
                             win ? QStringLiteral("游戏胜利") : QStringLiteral("游戏失败"),
                             message);
}

void MainWindow::showError(const QString &message)
{
    QMessageBox::warning(this, QStringLiteral("提示"), message);
}

int MainWindow::selectedStudentRow() const
{
    const QList<QTableWidgetItem *> selected = m_studentTable->selectedItems();
    if (selected.isEmpty()) {
        return -1;
    }
    return selected.first()->row();
}

void MainWindow::buildLayout()
{
    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);

    auto *title = new QLabel(QStringLiteral("《燕园保研战》校园经营模拟器"), this);
    title->setStyleSheet(QStringLiteral("font-size: %1px; font-weight: bold;").arg(GameConfig::MainTitleFontSize));
    mainLayout->addWidget(title);

    m_statusLabel = new QLabel(this);
    m_rateLabel = new QLabel(this);
    mainLayout->addWidget(m_statusLabel);
    mainLayout->addWidget(m_rateLabel);

    auto *buttonLayout = new QHBoxLayout();
    m_recruitButton = new QPushButton(QStringLiteral("招生/重新开始"), this);
    m_nextButton = new QPushButton(QStringLiteral("推进阶段"), this);
    m_examButton = new QPushButton(QStringLiteral("为选中学生替考"), this);
    m_sellButton = new QPushButton(QStringLiteral("出售选中学生"), this);

    buttonLayout->addWidget(m_recruitButton);
    buttonLayout->addWidget(m_nextButton);
    buttonLayout->addWidget(m_examButton);
    buttonLayout->addWidget(m_sellButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    auto *splitter = new QSplitter(this);

    m_studentTable = new QTableWidget(this);
    m_studentTable->setColumnCount(8);
    m_studentTable->setHorizontalHeaderLabels({
        QStringLiteral("姓名"),
        QStringLiteral("成绩"),
        QStringLiteral("健康"),
        QStringLiteral("心理"),
        QStringLiteral("市场价"),
        QStringLiteral("状态"),
        QStringLiteral("保研"),
        QStringLiteral("说明")
    });
    m_studentTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_studentTable->verticalHeader()->setVisible(false);
    m_studentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_studentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_studentTable->setSelectionMode(QAbstractItemView::SingleSelection);

    m_logWidget = new EventLogWidget(this);

    splitter->addWidget(m_studentTable);
    splitter->addWidget(m_logWidget);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);
    mainLayout->addWidget(splitter);

    auto *help = new QLabel(QStringLiteral(
        "流程：招生 → 推进阶段 → 选择 Buff → 替考/考试/事件 → 结算出售 → %1 学期后按目标保研率判定。"
        "休学学生不计入分母；转学学生仍占初始名额但不能保研，出售需谨慎。")
                                  .arg(GameConfig::TotalSemesters), this);
    help->setWordWrap(true);
    mainLayout->addWidget(help);

    setCentralWidget(central);
}

void MainWindow::updateStudentTable()
{
    const QVector<Student> students = m_manager->students();
    m_studentTable->setRowCount(students.size());

    for (int row = 0; row < students.size(); ++row) {
        const Student &s = students[row];

        const QString recommended = s.isRecommended() ? QStringLiteral("是") : QStringLiteral("否");
        QString note;
        if (s.isRecommended()) {
            note = QStringLiteral("成绩≥%1 且健康/心理≥%2")
                   .arg(GameConfig::RecommendScore)
                   .arg(GameConfig::RecommendHealth);
        } else if (!s.isActive()) {
            note = QStringLiteral("非在读，不能继续培养");
        } else {
            note = QStringLiteral("继续培养");
        }

        m_studentTable->setItem(row, 0, new QTableWidgetItem(s.name()));
        m_studentTable->setItem(row, 1, attributeItem(s.score(), s.isActive()));
        m_studentTable->setItem(row, 2, attributeItem(s.health(), s.isActive()));
        m_studentTable->setItem(row, 3, attributeItem(s.mood(), s.isActive()));
        m_studentTable->setItem(row, 4, new QTableWidgetItem(QString::number(s.marketPrice())));
        m_studentTable->setItem(row, 5, new QTableWidgetItem(s.statusText()));
        m_studentTable->setItem(row, 6, coloredTextItem(recommended,
                                                            QColor(s.isRecommended() ? GameConfig::GoodStatusColor : GameConfig::BadStatusColor),
                                                            true));
        m_studentTable->setItem(row, 7, new QTableWidgetItem(note));
    }
}

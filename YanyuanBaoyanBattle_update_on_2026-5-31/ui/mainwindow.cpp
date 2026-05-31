#include "mainwindow.h"

#include "buffdialog.h"
#include "eventlog.h"
#include "examdialog.h"
#include "gameconfig.h"
#include "recruitdialog.h"
#include "uistyle.h"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSize>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QtMath>

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

QString htmlText(const QString &text)
{
    return text.toHtmlEscaped().replace(QStringLiteral("\n"), QStringLiteral("<br>"));
}

QString signedValue(int value)
{
    return value >= 0
            ? QStringLiteral("+%1").arg(value)
            : QString::number(value);
}

QString studentNoteText(const Student &s)
{
    if (s.isRecommended()) {
        return QStringLiteral("已满足保研线，继续维持状态，安心冲刺最终结算。");
    }
    if (s.isSold()) {
        return QStringLiteral("已协助转学发展，不再参与后续培养与保研统计。");
    }
    if (s.isSuspended()) {
        return QStringLiteral("当前已休学，无法继续参与培养，需要在前期更稳住三维属性。");
    }
    if (s.score() < GameConfig::RecommendScore) {
        return QStringLiteral("当前最需要补强的是成绩，优先选择能稳定提分的行动与 Buff。");
    }
    if (s.health() < GameConfig::RecommendHealth) {
        return QStringLiteral("健康偏低，建议先恢复体力，避免考试后掉到危险区。");
    }
    if (s.mood() < GameConfig::RecommendMood) {
        return QStringLiteral("心理状态需要安抚，先稳住心态再冲刺学业表现。");
    }
    return QStringLiteral("三项属性接近达标，继续保持即可，注意不要在考试后跌出保研线。");
}

bool eventLooksPositive(const Event &event)
{
    return event.scoreDelta() + event.healthDelta() + event.moodDelta() + event.coinDelta() / 10 >= 0;
}

QTableWidgetItem *attributeItem(int value, bool active)
{
    auto *item = new QTableWidgetItem(QString::number(value));
    item->setForeground(QBrush(attributeColor(value, active)));
    item->setTextAlignment(Qt::AlignCenter);

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
    item->setTextAlignment(Qt::AlignCenter);

    QFont font = item->font();
    font.setBold(bold);
    item->setFont(font);

    return item;
}

QTableWidgetItem *centeredItem(const QString &text)
{
    auto *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignCenter);
    return item;
}

void setButtonVariant(QPushButton *button, const QString &variant, const QString &iconPath)
{
    button->setProperty("variant", variant);
    button->setCursor(Qt::PointingHandCursor);
    button->setIcon(QIcon(iconPath));
    button->setIconSize(QSize(22, 22));
}

QFrame *metricCard(const QString &labelText, QLabel **valueLabel, QWidget *parent)
{
    auto *card = new QFrame(parent);
    card->setObjectName(QStringLiteral("MetricCard"));
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 12, 18, 12);
    layout->setSpacing(3);

    auto *label = new QLabel(labelText, card);
    label->setObjectName(QStringLiteral("MetricLabel"));
    *valueLabel = new QLabel(QStringLiteral("-"), card);
    (*valueLabel)->setObjectName(QStringLiteral("MetricValue"));
    (*valueLabel)->setMinimumWidth(120);

    layout->addWidget(label);
    layout->addWidget(*valueLabel);
    return card;
}

QFrame *sectionCard(const QString &titleText, QWidget *content, QWidget *parent)
{
    auto *card = new QFrame(parent);
    card->setObjectName(QStringLiteral("CardPanel"));
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 16);
    layout->setSpacing(10);

    auto *title = new QLabel(titleText, card);
    title->setObjectName(QStringLiteral("SectionTitle"));
    layout->addWidget(title);
    layout->addWidget(content);
    return card;
}

void showPrettyMessage(QWidget *parent,
                       const QString &title,
                       const QString &emoji,
                       const QString &headline,
                       const QString &body,
                       QMessageBox::Icon icon = QMessageBox::NoIcon)
{
    QMessageBox box(parent);
    box.setWindowTitle(title);
    box.setIcon(icon);
    box.setTextFormat(Qt::RichText);
    box.setText(QStringLiteral(
                    "<div style='min-width:360px; padding:2px 2px 4px 2px;'>"
                    "<div style='font-size:34px; margin-bottom:2px;'>%1</div>"
                    "<div style='font-size:18px; font-weight:900; color:#8f000b; margin-bottom:8px;'>%2</div>"
                    "<div style='font-size:13px; color:#5b4038; line-height:160%;'>%3</div>"
                    "</div>")
                .arg(emoji, headline.toHtmlEscaped(), htmlText(body)));
    box.setStandardButtons(QMessageBox::Ok);
    if (auto *okButton = box.button(QMessageBox::Ok)) {
        okButton->setText(QStringLiteral("知道啦"));
    }
    box.exec();
}

QMessageBox::StandardButton showPrettyQuestion(QWidget *parent,
                                               const QString &title,
                                               const QString &emoji,
                                               const QString &headline,
                                               const QString &body,
                                               const QString &yesText,
                                               const QString &noText)
{
    QMessageBox box(parent);
    box.setWindowTitle(title);
    box.setIcon(QMessageBox::Question);
    box.setTextFormat(Qt::RichText);
    box.setText(QStringLiteral(
                    "<div style='min-width:360px; padding:2px 2px 4px 2px;'>"
                    "<div style='font-size:34px; margin-bottom:2px;'>%1</div>"
                    "<div style='font-size:18px; font-weight:900; color:#8f000b; margin-bottom:8px;'>%2</div>"
                    "<div style='font-size:13px; color:#5b4038; line-height:160%;'>%3</div>"
                    "</div>")
                .arg(emoji, headline.toHtmlEscaped(), htmlText(body)));
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    box.setDefaultButton(QMessageBox::No);
    if (auto *yesButton = box.button(QMessageBox::Yes)) {
        yesButton->setText(yesText);
    }
    if (auto *noButton = box.button(QMessageBox::No)) {
        noButton->setText(noText);
    }
    return static_cast<QMessageBox::StandardButton>(box.exec());
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_manager(new GameManager(this))
{
    setWindowTitle(QStringLiteral("燕园保研战 - 校园经营模拟器"));
    setWindowIcon(QIcon(QStringLiteral(":/ui/app_icon.svg")));
    m_examSystem.loadFromResource();
    buildLayout();

    connect(m_recruitButton, &QPushButton::clicked, this, &MainWindow::startRecruitment);
    connect(m_nextButton, &QPushButton::clicked, m_manager, &GameManager::advance);
    connect(m_examButton, &QPushButton::clicked, this, &MainWindow::doSubstituteExam);
    connect(m_sellButton, &QPushButton::clicked, this, &MainWindow::sellSelectedStudent);
    connect(m_guideButton, &QPushButton::clicked, this, &MainWindow::showGuide);

    connect(m_manager, &GameManager::stateChanged, this, &MainWindow::refreshUi);
    connect(m_manager, &GameManager::logAdded, m_logWidget, &EventLogWidget::appendLog);
    connect(m_manager, &GameManager::requestBuffChoice, this, &MainWindow::handleBuffChoice);
    connect(m_manager, &GameManager::requestShowEvent, this, &MainWindow::handleEventShow);
    connect(m_manager, &GameManager::showDeductLivingCost, this, &MainWindow::handleLivingCost);
    connect(m_manager, &GameManager::requestShowExamResult, this, &MainWindow::handleExamResult);
    connect(m_manager, &GameManager::gameEnded, this, &MainWindow::showGameEnd);
    connect(m_manager, &GameManager::errorMessage, this, &MainWindow::showError);
    connect(m_studentTable, &QTableWidget::itemSelectionChanged, this, &MainWindow::handleStudentSelection);

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
    const QString semesterText = m_manager->semester() == 0
                                     ? QStringLiteral("-")
                                     : QString::number(m_manager->semester());

    const QString status = QStringLiteral("学期：%1 / %2    阶段：%3    金币：%4    替考：%5/%6")
            .arg(semesterText)
            .arg(GameConfig::TotalSemesters)
            .arg(m_manager->phaseText())
            .arg(m_manager->coins())
            .arg(m_manager->substitutesUsed())
            .arg(m_manager->substituteLimit());

    m_statusLabel->setText(status);
    updateMetricCards();

    const int rec = m_manager->recommendedCount();
    const int den = m_manager->finalDenominator();
    m_rateLabel->setText(QStringLiteral("当前保研人数：%1/%2 · 胜利目标：%3 人")
                             .arg(rec)
                             .arg(den)
                             .arg(GameConfig::WinRecommendCount));
    m_rateProgress->setMaximum(qMax(1, qMax(den, GameConfig::WinRecommendCount)));
    m_rateProgress->setValue(rec);
    m_rateProgress->setFormat(QStringLiteral("保研进度  %v / %m"));

    updateStudentTable();
    handleStudentSelection();

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

void MainWindow::updateMetricCards()
{
    m_semesterValueLabel->setText(m_manager->semester() == 0
                                      ? QStringLiteral("未开局")
                                      : QStringLiteral("%1 / %2").arg(m_manager->semester()).arg(GameConfig::TotalSemesters));
    m_phaseValueLabel->setText(m_manager->phaseText());
    m_coinValueLabel->setText(QStringLiteral("%1 金币").arg(m_manager->coins()));
    m_substituteValueLabel->setText(QStringLiteral("%1 / %2").arg(m_manager->substitutesUsed()).arg(m_manager->substituteLimit()));
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

void MainWindow::handleEventShow(const Event &event)
{
    const QString targetText = event.target() == EventTarget::AllStudents
            ? QStringLiteral("全体学生")
            : QStringLiteral("随机一位学生");
    const QString body = QStringLiteral("%1\n\n🎯 作用对象：%2\n📘 成绩：%3    💪 健康：%4\n🙂 心理：%5    💰 金币：%6")
            .arg(event.description())
            .arg(targetText)
            .arg(signedValue(event.scoreDelta()))
            .arg(signedValue(event.healthDelta()))
            .arg(signedValue(event.moodDelta()))
            .arg(signedValue(event.coinDelta()));
    showPrettyMessage(this,
                      QStringLiteral("校园事件"),
                      eventLooksPositive(event) ? QStringLiteral("🎉") : QStringLiteral("🌧️"),
                      event.name(),
                      body,
                      QMessageBox::Information);
}

void MainWindow::handleLivingCost(const QString &message)
{
    showPrettyMessage(this,
                      QStringLiteral("学期日常结算"),
                      QStringLiteral("💰"),
                      QStringLiteral("生活费用已扣除"),
                      message,
                      QMessageBox::Information);
}

void MainWindow::handleExamResult(const QString &title, const QString &message)
{
    const QString headline = QStringLiteral("%1已结束").arg(title);
    const QString body = message + QStringLiteral("\n\n记得继续关注健康与心理，别让期末后属性掉出安全线。") ;
    showPrettyMessage(this,
                      title,
                      title.contains(QStringLiteral("期末")) ? QStringLiteral("🎓") : QStringLiteral("📝"),
                      headline,
                      body,
                      QMessageBox::Information);
}

void MainWindow::doSubstituteExam()
{
    const int row = selectedStudentRow();
    if (row < 0) {
        showError(QStringLiteral("请先在学生表中选择一名在读学生。"));
        return;
    }

    const QVector<Student> students = m_manager->students();
    if (row >= students.size()) {
        showError(QStringLiteral("选择无效，请重新选择。"));
        return;
    }

    if (!m_manager->canSubstitute()) {
        showError(QStringLiteral("当前阶段不能替考，或本学期替考次数已用完。"));
        return;
    }
    if (!students[row].isActive()) {
        showError(QStringLiteral("该学生当前不在读，不能替考。"));
        return;
    }
    if (m_manager->coins() < GameConfig::SubstituteCost) {
        showError(QStringLiteral("金币不足，无法支付替考费用。"));
        return;
    }

    if (m_manager->isStudentSubstituted(row)) {
        showError(QStringLiteral("该学生本学期已接受过替考，不能重复替考。"));
        return;
    }

    QVector<Question> questions = m_examSystem.randomQuestions(GameConfig::SubstituteQuestionCount);
    ExamDialog dialog(questions, students[row].name(), this);
    if (dialog.exec() == QDialog::Accepted) {
        const double acc = dialog.accuracy();
        const int delta = qRound(acc * GameConfig::SubstituteScoreMultiplier);
        const int correctCount = qRound(acc * questions.size());
        m_manager->applySubstituteExam(row, acc);

        const QString emoji = acc >= 0.8 ? QStringLiteral("😄")
                              : acc >= 0.6 ? QStringLiteral("🙂")
                              : QStringLiteral("😢");
        const QString headline = acc >= 0.8 ? QStringLiteral("替考发挥出色！")
                                : acc >= 0.6 ? QStringLiteral("替考顺利完成")
                                : QStringLiteral("替考已结束");
        const QString body = QStringLiteral("【%1】的替考任务已完成。\n\n✅ 答对 %2 / %3 题\n📈 正确率：%4%\n📘 成绩提升：+%5\n💸 行动花费：-%6 金币")
                .arg(students[row].name())
                .arg(correctCount)
                .arg(questions.size())
                .arg(acc * 100.0, 0, 'f', 0)
                .arg(delta)
                .arg(GameConfig::SubstituteCost);
        showPrettyMessage(this,
                          QStringLiteral("替考行动反馈"),
                          emoji,
                          headline,
                          body,
                          QMessageBox::Information);
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
    const QMessageBox::StandardButton ret = showPrettyQuestion(
                this,
                QStringLiteral("确认协助转学发展"),
                QStringLiteral("🧭"),
                QStringLiteral("是否协助该学生转学发展？"),
                QStringLiteral("对象：%1\n可回收资源：%2 金币\n\n协助转学后，该学生将不再参与后续培养，也无法计入保研人数。")
                .arg(students[row].name())
                .arg(income),
                QStringLiteral("确认协助"),
                QStringLiteral("再想想"));
    if (ret == QMessageBox::Yes) {
        m_manager->sellStudent(row);
    }
}

void MainWindow::showGameEnd(bool win, const QString &message)
{
    showPrettyMessage(this,
                      win ? QStringLiteral("游戏胜利") : QStringLiteral("游戏结束"),
                      win ? QStringLiteral("🏆") : QStringLiteral("🌙"),
                      win ? QStringLiteral("恭喜达成保研目标！") : QStringLiteral("本轮经营暂告一段落"),
                      message,
                      QMessageBox::Information);
}

void MainWindow::showError(const QString &message)
{
    showPrettyMessage(this,
                      QStringLiteral("提示"),
                      QStringLiteral("⚠️"),
                      QStringLiteral("当前操作无法执行"),
                      message,
                      QMessageBox::Warning);
}

void MainWindow::showGuide()
{
    showPrettyMessage(this,
                      QStringLiteral("玩法说明"),
                      QStringLiteral("📚"),
                      QStringLiteral("燕园保研战 · 玩法速览"),
                      QStringLiteral("目标：%1 学期后至少 %2 名学生满足保研条件。\n\n保研条件：在读、成绩 ≥ %3、健康 ≥ %4、心理 ≥ %5。\n\n推荐节奏：先稳住健康和心理，再冲刺成绩；结算阶段可协助高风险学生转学发展，用来补充金币与稳定队伍。")
                      .arg(GameConfig::TotalSemesters)
                      .arg(GameConfig::WinRecommendCount)
                      .arg(GameConfig::RecommendScore)
                      .arg(GameConfig::RecommendHealth)
                      .arg(GameConfig::RecommendMood),
                      QMessageBox::Information);
}

void MainWindow::handleStudentSelection()
{
    const int row = m_studentTable->currentRow();
    const QVector<Student> students = m_manager->students();
    if (row < 0 || row >= students.size()) {
        m_selectedStudentLabel->setText(QStringLiteral("👈 点击学生查看即时反馈"));
        m_selectedStudentDetailLabel->setText(QStringLiteral("选中一名学生后，这里会高亮展示当前状态、保研判断与下一步培养建议。"));
        return;
    }

    const Student &s = students[row];
    const QString stageTag = s.isRecommended()
            ? QStringLiteral("🌟 保研达标")
            : (s.isActive() ? QStringLiteral("📍 培养中") : QStringLiteral("🧭 已离队"));

    m_selectedStudentLabel->setText(QStringLiteral("已选中：%1 · %2")
                                    .arg(s.name())
                                    .arg(stageTag));
    m_selectedStudentDetailLabel->setText(QStringLiteral("状态：%1\n📘 成绩 %2   💪 健康 %3   🙂 心理 %4   💰 参考价 %5\n建议：%6")
                                          .arg(s.statusText())
                                          .arg(s.score())
                                          .arg(s.health())
                                          .arg(s.mood())
                                          .arg(s.marketPrice())
                                          .arg(studentNoteText(s)));
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
    central->setObjectName(QStringLiteral("GameRoot"));
    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(22, 18, 22, 18);
    mainLayout->setSpacing(14);

    auto *hero = new QFrame(this);
    hero->setObjectName(QStringLiteral("HeroPanel"));
    auto *heroLayout = new QHBoxLayout(hero);
    heroLayout->setContentsMargins(26, 22, 26, 22);
    heroLayout->setSpacing(22);

    auto *heroTextLayout = new QVBoxLayout();
    heroTextLayout->setSpacing(7);
    auto *subtitle = new QLabel(QStringLiteral("PKU CAMPUS MANAGEMENT · PROGRAMMING META-LEARNING"), hero);
    subtitle->setObjectName(QStringLiteral("HeroSubtitle"));
    subtitle->setMaximumWidth(430);

    auto *title = new QLabel(QStringLiteral("燕园保研战"), hero);
    title->setObjectName(QStringLiteral("GameTitle"));

    auto *description = new QLabel(QStringLiteral("招募、培养、抉择，在八个学期里打造属于你的燕园保研队伍。"), hero);
    description->setObjectName(QStringLiteral("HeroDescription"));
    description->setWordWrap(true);

    heroTextLayout->addWidget(subtitle, 0, Qt::AlignLeft);
    heroTextLayout->addWidget(title);
    heroTextLayout->addWidget(description);
    heroTextLayout->addStretch();
    heroLayout->addLayout(heroTextLayout, 4);

    auto *goalLayout = new QVBoxLayout();
    goalLayout->setSpacing(9);
    m_rateLabel = new QLabel(hero);
    m_rateLabel->setObjectName(QStringLiteral("RateLabel"));
    m_rateProgress = new QProgressBar(hero);
    m_rateProgress->setTextVisible(true);
    goalLayout->addWidget(m_rateLabel);
    goalLayout->addWidget(m_rateProgress);
    goalLayout->addStretch();
    heroLayout->addLayout(goalLayout, 2);
    mainLayout->addWidget(hero);

    auto *metricsLayout = new QGridLayout();
    metricsLayout->setHorizontalSpacing(12);
    metricsLayout->setVerticalSpacing(12);
    metricsLayout->addWidget(metricCard(QStringLiteral("当前学期"), &m_semesterValueLabel, this), 0, 0);
    metricsLayout->addWidget(metricCard(QStringLiteral("经营阶段"), &m_phaseValueLabel, this), 0, 1);
    metricsLayout->addWidget(metricCard(QStringLiteral("资金余额"), &m_coinValueLabel, this), 0, 2);
    metricsLayout->addWidget(metricCard(QStringLiteral("本学期替考"), &m_substituteValueLabel, this), 0, 3);
    mainLayout->addLayout(metricsLayout);

    auto *controlPanel = new QFrame(this);
    controlPanel->setObjectName(QStringLiteral("ControlPanel"));
    auto *buttonLayout = new QHBoxLayout(controlPanel);
    buttonLayout->setContentsMargins(16, 13, 16, 13);
    buttonLayout->setSpacing(10);

    m_recruitButton = new QPushButton(QStringLiteral("开局招生"), this);
    m_nextButton = new QPushButton(QStringLiteral("推进阶段"), this);
    m_examButton = new QPushButton(QStringLiteral("为选中学生替考"), this);
    m_sellButton = new QPushButton(QStringLiteral("协助转学发展"), this);
    m_guideButton = new QPushButton(QStringLiteral("玩法说明"), this);

    setButtonVariant(m_recruitButton, QStringLiteral("primary"), QStringLiteral(":/ui/recruit.svg"));
    setButtonVariant(m_nextButton, QStringLiteral("gold"), QStringLiteral(":/ui/next.svg"));
    setButtonVariant(m_examButton, QStringLiteral("primary"), QStringLiteral(":/ui/exam.svg"));
    setButtonVariant(m_sellButton, QStringLiteral("danger"), QStringLiteral(":/ui/sell.svg"));
    setButtonVariant(m_guideButton, QString(), QStringLiteral(":/ui/help.svg"));

    buttonLayout->addWidget(m_recruitButton);
    buttonLayout->addWidget(m_nextButton);
    buttonLayout->addWidget(m_examButton);
    buttonLayout->addWidget(m_sellButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_guideButton);
    mainLayout->addWidget(controlPanel);

    auto *splitter = new QSplitter(this);
    splitter->setChildrenCollapsible(false);

    m_studentTable = new QTableWidget(this);
    m_studentTable->setObjectName(QStringLiteral("StudentTable"));
    m_studentTable->setColumnCount(8);
    m_studentTable->setHorizontalHeaderLabels({
        QStringLiteral("姓名"),
        QStringLiteral("成绩"),
        QStringLiteral("健康"),
        QStringLiteral("心理"),
        QStringLiteral("参考价"),
        QStringLiteral("状态"),
        QStringLiteral("保研"),
        QStringLiteral("培养建议")
    });
    m_studentTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_studentTable->verticalHeader()->setVisible(false);
    m_studentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_studentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_studentTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_studentTable->setAlternatingRowColors(true);
    m_studentTable->setShowGrid(false);
    m_studentTable->setWordWrap(false);
    m_studentTable->setMinimumWidth(700);

    auto *studentPanel = new QWidget(this);
    auto *studentPanelLayout = new QVBoxLayout(studentPanel);
    studentPanelLayout->setContentsMargins(0, 0, 0, 0);
    studentPanelLayout->setSpacing(10);
    studentPanelLayout->addWidget(m_studentTable, 1);

    auto *selectionCard = new QFrame(this);
    selectionCard->setObjectName(QStringLiteral("SelectionCard"));
    auto *selectionLayout = new QVBoxLayout(selectionCard);
    selectionLayout->setContentsMargins(14, 12, 14, 12);
    selectionLayout->setSpacing(6);
    m_selectedStudentLabel = new QLabel(QStringLiteral("👈 点击学生查看即时反馈"), selectionCard);
    m_selectedStudentLabel->setObjectName(QStringLiteral("SelectionName"));
    m_selectedStudentDetailLabel = new QLabel(QStringLiteral("选中一名学生后，这里会高亮展示当前状态、保研判断与下一步培养建议。"), selectionCard);
    m_selectedStudentDetailLabel->setObjectName(QStringLiteral("SelectionDetail"));
    m_selectedStudentDetailLabel->setWordWrap(true);
    selectionLayout->addWidget(m_selectedStudentLabel);
    selectionLayout->addWidget(m_selectedStudentDetailLabel);
    studentPanelLayout->addWidget(selectionCard);

    m_logWidget = new EventLogWidget(this);

    splitter->addWidget(sectionCard(QStringLiteral("学生培养面板"), studentPanel, this));
    splitter->addWidget(sectionCard(QStringLiteral("事件日志"), m_logWidget, this));
    splitter->setStretchFactor(0, 5);
    splitter->setStretchFactor(1, 2);
    mainLayout->addWidget(splitter, 1);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName(QStringLiteral("TipLabel"));
    m_statusLabel->setWordWrap(true);
    mainLayout->addWidget(m_statusLabel);

    auto *help = new QLabel(QStringLiteral(
                                "流程：招生 → 推进阶段 → 选择 Buff → 替考/考试/事件 → 结算阶段协助转学发展 → %1 学期后需有至少 %2 名学生保研。"
                                "休学学生不计入保研；已转学学生不能保研且占用初始名额，操作前请谨慎评估。")
                                .arg(GameConfig::TotalSemesters)
                                .arg(GameConfig::WinRecommendCount), this);
    help->setObjectName(QStringLiteral("TipLabel"));
    help->setWordWrap(true);
    mainLayout->addWidget(help);

    setCentralWidget(central);
}

void MainWindow::updateStudentTable()
{
    const QVector<Student> students = m_manager->students();
    const int previousRow = m_studentTable->currentRow();
    m_studentTable->setRowCount(students.size());

    for (int row = 0; row < students.size(); ++row) {
        const Student &s = students[row];

        QString recommended;
        QColor recommendedColor;
        if (s.isRecommended()) {
            recommended = QStringLiteral("达标");
            recommendedColor = QColor(GameConfig::GoodStatusColor);
        } else if (!s.isActive()) {
            recommended = QStringLiteral("不计入");
            recommendedColor = QColor(GameConfig::AttributeInactiveColor);
        } else {
            recommended = QStringLiteral("培养中");
            recommendedColor = QColor(GameConfig::BadStatusColor);
        }
        const QString note = studentNoteText(s);

        m_studentTable->setItem(row, 0, centeredItem(s.name()));
        m_studentTable->setItem(row, 1, attributeItem(s.score(), s.isActive()));
        m_studentTable->setItem(row, 2, attributeItem(s.health(), s.isActive()));
        m_studentTable->setItem(row, 3, attributeItem(s.mood(), s.isActive()));
        m_studentTable->setItem(row, 4, centeredItem(QString::number(s.marketPrice())));
        m_studentTable->setItem(row, 5, centeredItem(s.statusText()));
        m_studentTable->setItem(row, 6, coloredTextItem(recommended,
                                                         recommendedColor,
                                                         true));
        m_studentTable->setItem(row, 7, centeredItem(note));

        if (!s.isActive()) {
            for (int col = 0; col < m_studentTable->columnCount(); ++col) {
                if (auto *item = m_studentTable->item(row, col)) {
                    item->setForeground(QBrush(QColor(GameConfig::AttributeInactiveColor)));
                    item->setBackground(QBrush(QColor(246, 239, 232)));
                }
            }
        }
        m_studentTable->setRowHeight(row, 38);
    }

    if (previousRow >= 0 && previousRow < students.size()) {
        m_studentTable->setCurrentCell(previousRow, 0);
        m_studentTable->selectRow(previousRow);
    }
}

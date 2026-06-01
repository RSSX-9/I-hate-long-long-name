#include "gamemanager.h"

#include "gameconfig.h"

#include <QRandomGenerator>
#include <QtMath>
#include <algorithm>
#include <random>

GameManager::GameManager(QObject *parent)
    : QObject(parent)
{
}

int GameManager::coins() const { return m_coins; }
int GameManager::semester() const { return m_semester; }
int GameManager::initialStudentCount() const { return m_initialStudentCount; }
int GameManager::substitutesUsed() const { return m_substitutesUsed; }
int GameManager::substituteLimit() const { return GameConfig::SubstituteLimit; }
GamePhase GameManager::phase() const { return m_phase; }
QVector<Student> GameManager::students() const { return m_students; }
QStringList GameManager::logs() const { return m_logs; }

int GameManager::recommendedCount() const
{
    int count = 0;
    for (const Student &s : m_students) {
        if (s.isRecommended()) {
            ++count;
        }
    }
    return count;
}

int GameManager::finalDenominator() const
{
    int count = 0;
    for (const Student &s : m_students) {
        if (!s.isSuspended()) {
            ++count;
        }
    }
    return qMax(1, count);
}

double GameManager::currentRecommendRate() const
{
    return static_cast<double>(recommendedCount()) / static_cast<double>(finalDenominator());
}

QString GameManager::phaseText() const
{
    switch (m_phase) {
    case GamePhase::NotStarted: return QStringLiteral("未开始");
    case GamePhase::SemesterStart: return QStringLiteral("学期初");
    case GamePhase::MidtermPreparation: return QStringLiteral("期中前/替考决策");
    case GamePhase::MidtermExam: return QStringLiteral("期中考试");
    case GamePhase::LateSemester: return QStringLiteral("期中后至期末前");
    case GamePhase::FinalExam: return QStringLiteral("期末考试");
    case GamePhase::Settlement: return QStringLiteral("学期结算/可转学");
    case GamePhase::Finished: return QStringLiteral("游戏结束");
    }
    return QStringLiteral("未知");
}

bool GameManager::canSubstitute() const
{
    return m_phase == GamePhase::MidtermPreparation && m_substitutesUsed < GameConfig::SubstituteLimit;
}

bool GameManager::canSell() const
{
    return m_phase == GamePhase::Settlement;
}

bool GameManager::canSubstituteStudent(int studentIndex) const
{
    if (!canSubstitute()) return false;
    if (studentIndex < 0 || studentIndex >= m_students.size()) return false;
    if (!m_students[studentIndex].isActive()) return false;
    if (m_coins < GameConfig::SubstituteCost) return false;
    if (m_substitutedIndices.contains(studentIndex)) return false;
    return true;
}

bool GameManager::isStudentSubstituted(int studentIndex) const
{
    return m_substitutedIndices.contains(studentIndex);
}

void GameManager::startGame(const QVector<Student> &students)
{
    if (students.size() < GameConfig::MinRecruitCount) {
        emit errorMessage(QStringLiteral("至少需要招收 %1 名学生。").arg(GameConfig::MinRecruitCount));
        return;
    }

    m_students = students;
    m_initialStudentCount = students.size();
    m_coins = GameConfig::StartingFunds;
    int recruitCost = 0;
    for (const Student &s : m_students) {
        recruitCost += s.basePrice();
    }
    m_coins -= recruitCost;

    if (m_coins < 0) {
        emit errorMessage(QStringLiteral("金币不足，无法开始游戏。"));
        return;
    }

    m_semester = 1;
    m_substitutesUsed = 0;
    m_substitutedIndices.clear();
    m_logs.clear();
    m_phase = GamePhase::SemesterStart;
    m_waitingForBuff = false;

    addLog(QStringLiteral("游戏开始：初始经费 %1，招生花费 %2，共招收 %3 名学生，剩余金币 %4。")
           .arg(GameConfig::StartingFunds)
           .arg(recruitCost)
           .arg(m_students.size())
           .arg(m_coins));
    emit stateChanged();
}

void GameManager::advance()
{
    if (m_phase == GamePhase::NotStarted) {
        emit errorMessage(QStringLiteral("请先完成招生。"));
        return;
    }
    if (m_phase == GamePhase::Finished) {
        emit errorMessage(QStringLiteral("游戏已经结束。"));
        return;
    }
    if (m_waitingForBuff) {
        emit errorMessage(QStringLiteral("请先完成 Buff 选择。"));
        return;
    }

    switch (m_phase) {
    case GamePhase::SemesterStart:
        addLog(QStringLiteral("===== 第 %1 学期开始 =====").arg(m_semester));
        deductLivingCost();
        if (m_coins < 0) {
            finishGame(false, QStringLiteral("生活费扣除后金币为负，游戏失败。"));
            return;
        }
        requestBuffAndEvent(GamePhase::MidtermPreparation, QStringLiteral("学期初 Buff 选择"));
        return;

    case GamePhase::MidtermPreparation:
        m_phase = GamePhase::MidtermExam;
        addLog(QStringLiteral("进入期中考试。若还想替考，请在点击前完成。"));
        emit stateChanged();
        return;

    case GamePhase::MidtermExam:
        simulateExamForAll(QStringLiteral("期中考试"));
        triggerRandomEvent();
        if (m_coins < 0) {
            finishGame(false, QStringLiteral("期中事件后金币为负，游戏失败。"));
            return;
        }
        m_phase = GamePhase::LateSemester;
        emit stateChanged();
        return;

    case GamePhase::LateSemester:
        requestBuffAndEvent(GamePhase::FinalExam, QStringLiteral("期末前 Buff 选择"));
        return;

    case GamePhase::FinalExam:
        simulateExamForAll(QStringLiteral("期末考试"));
        triggerRandomEvent();
        checkSuspensions();
        if (m_coins < 0) {
            finishGame(false, QStringLiteral("休学公关费导致金币为负，游戏失败。"));
            return;
        }
        m_phase = GamePhase::Settlement;
        addLog(QStringLiteral("进入学期结算。现在可以协助学生转学发展，或继续下一学期。"));
        emit stateChanged();
        return;

    case GamePhase::Settlement:
        if (m_semester >= GameConfig::TotalSemesters) {
            const int rec = recommendedCount();
            const int required = GameConfig::WinRecommendCount;
            if (m_initialStudentCount >= GameConfig::MinRecruitCount && rec >= required) {
                finishGame(true, QStringLiteral("最终保研人数 %1 人，达到目标 %2 人，胜利！")
                                     .arg(rec).arg(required));
            } else {
                finishGame(false, QStringLiteral("最终保研人数 %1 人，未达到目标 %2 人，失败。")
                                      .arg(rec).arg(required));
            }
            return;
        }

        ++m_semester;
        m_substitutesUsed = 0;
        m_substitutedIndices.clear();
        m_phase = GamePhase::SemesterStart;
        addLog(QStringLiteral("准备进入第 %1 学期。").arg(m_semester));
        emit stateChanged();
        return;

    default:
        break;
    }
}

void GameManager::applyChosenBuff(int index)
{
    if (!m_waitingForBuff || index < 0 || index >= m_pendingBuffChoices.size()) {
        emit errorMessage(QStringLiteral("当前没有可应用的 Buff。"));
        return;
    }

    const Buff buff = m_pendingBuffChoices[index];
    applyBuff(buff);
    triggerRandomEvent();

    m_waitingForBuff = false;
    m_pendingBuffChoices.clear();

    if (m_coins < 0) {
        finishGame(false, QStringLiteral("Buff 或事件导致金币为负，游戏失败。"));
        return;
    }

    m_phase = m_phaseAfterBuff;
    emit stateChanged();
}

void GameManager::applySubstituteExam(int studentIndex, double accuracy)
{
    if (!canSubstitute()) {
        emit errorMessage(QStringLiteral("当前阶段不能替考，或替考次数已用完。"));
        return;
    }
    if (studentIndex < 0 || studentIndex >= m_students.size()) {
        emit errorMessage(QStringLiteral("请选择有效学生。"));
        return;
    }
    if (!m_students[studentIndex].isActive()) {
        emit errorMessage(QStringLiteral("只能为在读学生替考。"));
        return;
    }
    if (m_coins < GameConfig::SubstituteCost) {
        emit errorMessage(QStringLiteral("金币不足，无法支付替考行动成本。"));
        return;
    }
    if (m_substitutedIndices.contains(studentIndex)) {
        emit errorMessage(QStringLiteral("该学生本学期已接受过替考，不能重复替考。"));
        return;
    }

    accuracy = qBound(0.0, accuracy, 1.0);
    const int delta = qRound(accuracy * GameConfig::SubstituteScoreMultiplier);
    m_students[studentIndex].addScore(delta);
    m_coins -= GameConfig::SubstituteCost;
    ++m_substitutesUsed;

    addLog(QStringLiteral("替考：%1 正确率 %2%，成绩 +%3，金币 -%4。本学期已替考 %5/%6。")
           .arg(m_students[studentIndex].name())
           .arg(accuracy * 100.0, 0, 'f', 0)
           .arg(delta)
           .arg(GameConfig::SubstituteCost)
           .arg(m_substitutesUsed)
           .arg(GameConfig::SubstituteLimit));
    m_substitutedIndices.insert(studentIndex);
    emit stateChanged();
}

void GameManager::sellStudent(int studentIndex)
{
    if (!canSell()) {
        emit errorMessage(QStringLiteral("只能在学期结算阶段协助学生转学发展。"));
        return;
    }
    if (studentIndex < 0 || studentIndex >= m_students.size()) {
        emit errorMessage(QStringLiteral("请选择有效学生。"));
        return;
    }
    if (!m_students[studentIndex].isActive()) {
        emit errorMessage(QStringLiteral("该学生已不在读，不能出售。"));
        return;
    }

    const int income = m_students[studentIndex].marketPrice();
    const QString name = m_students[studentIndex].name();
    m_students[studentIndex].markSold();
    m_coins += income;
    addLog(QStringLiteral("协助转学发展：%1，获得金币 %2。注意：该学生后续不再计入保研人数。")
           .arg(name)
           .arg(income));
    checkActiveCountAndFail();
    emit stateChanged();
}

void GameManager::addLog(const QString &message)
{
    m_logs << message;
    emit logAdded(message);
}

void GameManager::deductLivingCost()
{
    int active = 0;
    for (const Student &s : m_students) {
        if (s.isActive()) {
            ++active;
        }
    }
    const int cost = active * GameConfig::LivingCostPerStudent;
    m_coins -= cost;
    addLog(QStringLiteral("扣除生活费：在读 %1 人 × %2 = %3，剩余金币 %4。")
           .arg(active)
           .arg(GameConfig::LivingCostPerStudent)
           .arg(cost)
           .arg(m_coins));
    emit showDeductLivingCost(QStringLiteral("扣除生活费：在读 %1 人 × %2 = %3，剩余金币 %4。")
                              .arg(active)
                              .arg(GameConfig::LivingCostPerStudent)
                              .arg(cost)
                              .arg(m_coins));
}

void GameManager::requestBuffAndEvent(GamePhase phaseAfterBuff, const QString &title)
{
    QVector<Buff> library = Buff::library();
    std::mt19937 engine(QRandomGenerator::global()->generate());
    std::shuffle(library.begin(), library.end(), engine);

    m_pendingBuffChoices.clear();
    const int choiceCount = qMin(3, library.size());
    for (int i = 0; i < choiceCount; ++i) {
        m_pendingBuffChoices << library[i];
    }

    m_phaseAfterBuff = phaseAfterBuff;
    m_waitingForBuff = true;
    emit requestBuffChoice(m_pendingBuffChoices, title);
}

void GameManager::applyBuff(const Buff &buff)
{
    m_coins += buff.coinDelta();

    if (buff.target() == EffectTarget::AllStudents) {
        for (Student &s : m_students) {
            s.applyDelta(buff.scoreDelta(), buff.healthDelta(), buff.moodDelta());
        }
        addLog(QStringLiteral("Buff 生效：%1。").arg(buff.summary().replace('\n', QStringLiteral(" "))));
        return;
    }

    const int index = randomActiveStudentIndex();
    if (index >= 0) {
        m_students[index].applyDelta(buff.scoreDelta(), buff.healthDelta(), buff.moodDelta());
        addLog(QStringLiteral("Buff 生效：%1，作用于 %2。")
               .arg(buff.name())
               .arg(m_students[index].name()));
    } else {
        addLog(QStringLiteral("Buff 生效：%1，但当前没有在读学生可作用。").arg(buff.name()));
    }
}

void GameManager::triggerRandomEvent()
{
    QVector<Event> library = Event::library();
    if (library.isEmpty()) return;

    const int index = QRandomGenerator::global()->bounded(library.size());
    applyEvent(library[index]);
}

void GameManager::applyEvent(const Event &event)
{
    emit requestShowEvent(event);

    m_coins += event.coinDelta();

    if (event.target() == EventTarget::AllStudents) {
        for (Student &s : m_students) {
            s.applyDelta(event.scoreDelta(), event.healthDelta(), event.moodDelta());
        }
        addLog(QStringLiteral("随机事件：%1。").arg(event.summary().replace('\n', QStringLiteral(" "))));
        return;
    }

    const int index = randomActiveStudentIndex();
    if (index >= 0) {
        m_students[index].applyDelta(event.scoreDelta(), event.healthDelta(), event.moodDelta());
        addLog(QStringLiteral("随机事件：%1，作用于 %2。")
               .arg(event.name())
               .arg(m_students[index].name()));
    } else {
        addLog(QStringLiteral("随机事件：%1，但当前没有在读学生可作用。").arg(event.name()));
    }
}

int GameManager::randomActiveStudentIndex() const
{
    QVector<int> indices;
    for (int i = 0; i < m_students.size(); ++i) {
        if (m_students[i].isActive()) {
            indices << i;
        }
    }
    if (indices.isEmpty()) {
        return -1;
    }
    return indices[QRandomGenerator::global()->bounded(indices.size())];
}

void GameManager::simulateExamForAll(const QString &examName)
{
    for (Student &s : m_students) {
        s.simulateExam();
    }
    addLog(QStringLiteral("%1结束：所有在读学生成绩随机波动，健康 %2，心理 %3。")
           .arg(examName)
           .arg(GameConfig::ExamHealthDelta, 0, 10, QLatin1Char('+'))
           .arg(GameConfig::ExamMoodDelta, 0, 10, QLatin1Char('+')));

    emit requestShowExamResult(examName,
                               QStringLiteral("%1结束！所有在读学生成绩随机波动，健康%2，心理%3。")
                                   .arg(examName)
                                   .arg(GameConfig::ExamHealthDelta, 0, 10, QLatin1Char('+'))
                                   .arg(GameConfig::ExamMoodDelta, 0, 10, QLatin1Char('+')));
}

void GameManager::checkSuspensions()
{
    int suspended = 0;
    for (Student &s : m_students) {
        if (s.isActive() && (s.score() < GameConfig::SuspensionThreshold || s.health() < GameConfig::SuspensionThreshold || s.mood() < GameConfig::SuspensionThreshold)) {
            s.markSuspended();
            m_coins -= GameConfig::SuspendPenalty;
            ++suspended;
            addLog(QStringLiteral("休学警报：%1 任一属性低于 %2，支付公关费 %3。").arg(s.name()).arg(GameConfig::SuspensionThreshold).arg(GameConfig::SuspendPenalty));
        }
    }
    if (suspended == 0) {
        addLog(QStringLiteral("本学期无人休学。"));
    }
    checkActiveCountAndFail();
}

void GameManager::checkActiveCountAndFail()
{
    int activeCount = 0;
    for (const Student &s : m_students) {
        if (s.isActive()) activeCount++;
    }
    if (activeCount < GameConfig::WinRecommendCount) {
        finishGame(false, QStringLiteral("在读学生仅剩 %1 人，已不足目标保研人数 %2 人，游戏失败。")
                              .arg(activeCount)
                              .arg(GameConfig::WinRecommendCount));
    }
}

void GameManager::finishGame(bool win, const QString &message)
{
    m_phase = GamePhase::Finished;
    addLog(message);
    emit stateChanged();
    emit gameEnded(win, message);
}

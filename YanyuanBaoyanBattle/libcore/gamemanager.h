#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QSet>

#include "student.h"
#include "buff.h"
#include "event.h"
#include "gameconfig.h"

enum class GamePhase
{
    NotStarted,
    SemesterStart,
    MidtermPreparation,
    MidtermExam,
    LateSemester,
    FinalExam,
    Settlement,
    Finished
};

class GameManager : public QObject
{
    Q_OBJECT

public:
    explicit GameManager(QObject *parent = nullptr);

    int coins() const;
    int semester() const;
    int initialStudentCount() const;
    int substitutesUsed() const;
    int substituteLimit() const;
    double currentRecommendRate() const;
    int recommendedCount() const;
    int finalDenominator() const;
    void deductLivingCost();
    GamePhase phase() const;
    QString phaseText() const;
    QVector<Student> students() const;
    QStringList logs() const;

    bool canSubstitute() const;
    bool canSell() const;
    bool canSubstituteStudent(int studentIndex) const;
    bool isStudentSubstituted(int studentIndex) const;

public slots:
    void startGame(const QVector<Student> &students);
    void advance();
    void applyChosenBuff(int index);
    void applySubstituteExam(int studentIndex, double accuracy);
    void sellStudent(int studentIndex);

signals:
    void stateChanged();
    void logAdded(const QString &message);
    void requestBuffChoice(const QVector<Buff> &choices, const QString &title);
    void gameEnded(bool win, const QString &message);
    void errorMessage(const QString &message);
    void requestShowEvent(const Event& event);
    void requestShowExamResult(const QString &title, const QString &message);
    void showDeductLivingCost(const QString& message);
    void requestShowSuspension(const QString &message);

private:
    QVector<Student> m_students;
    QSet<int> m_substitutedIndices;
    QStringList m_logs;
    int m_coins = GameConfig::StartingFunds;
    int m_semester = 0;
    int m_initialStudentCount = 0;
    int m_substitutesUsed = 0;

    GamePhase m_phase = GamePhase::NotStarted;
    QVector<Buff> m_pendingBuffChoices;
    GamePhase m_phaseAfterBuff = GamePhase::MidtermPreparation;
    bool m_waitingForBuff = false;

    void addLog(const QString &message);
    void requestBuffAndEvent(GamePhase phaseAfterBuff, const QString &title);
    void applyBuff(const Buff &buff);
    void triggerRandomEvent();
    void applyEvent(const Event &event);
    int randomActiveStudentIndex() const;
    void simulateExamForAll(const QString &examName);
    void checkSuspensions();
    void checkActiveCountAndFail();
    void finishGame(bool win, const QString &message);
};

#pragma once

#include <QMainWindow>

#include "gamemanager.h"
#include "examsystem.h"

class QLabel;
class QPushButton;
class QProgressBar;
class QTableWidget;
class EventLogWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void startRecruitment();
    void refreshUi();
    void showSuspension(const QString &msg);
    void handleBuffChoice(const QVector<Buff> &choices, const QString &title);
    void handleEventShow(const Event& event);
    void handleLivingCost(const QString& message);
    void doSubstituteExam();
    void sellSelectedStudent();
    void showGameEnd(bool win, const QString &message);
    void showError(const QString &message);
    void handleExamResult(const QString &title, const QString &message);
    void showGuide();
    void handleStudentSelection();

private:
    GameManager *m_manager = nullptr;
    ExamSystem m_examSystem;

    QLabel *m_statusLabel = nullptr;
    QLabel *m_rateLabel = nullptr;
    QLabel *m_semesterValueLabel = nullptr;
    QLabel *m_phaseValueLabel = nullptr;
    QLabel *m_coinValueLabel = nullptr;
    QLabel *m_substituteValueLabel = nullptr;
    QLabel *m_selectedStudentLabel = nullptr;
    QLabel *m_selectedStudentDetailLabel = nullptr;
    QProgressBar *m_rateProgress = nullptr;
    QTableWidget *m_studentTable = nullptr;
    EventLogWidget *m_logWidget = nullptr;

    QPushButton *m_recruitButton = nullptr;
    QPushButton *m_nextButton = nullptr;
    QPushButton *m_examButton = nullptr;
    QPushButton *m_sellButton = nullptr;
    QPushButton *m_guideButton = nullptr;

    int selectedStudentRow() const;
    void buildLayout();
    void updateStudentTable();
    void updateMetricCards();
};

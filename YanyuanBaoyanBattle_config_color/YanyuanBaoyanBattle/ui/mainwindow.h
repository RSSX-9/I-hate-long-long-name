#pragma once

#include <QMainWindow>
#include "../libcore/gamemanager.h"
#include "../libcore/examsystem.h"

class QLabel;
class QPushButton;
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
    void handleBuffChoice(const QVector<Buff> &choices, const QString &title);
    void doSubstituteExam();
    void sellSelectedStudent();
    void showGameEnd(bool win, const QString &message);
    void showError(const QString &message);

private:
    GameManager *m_manager = nullptr;
    ExamSystem m_examSystem;

    // ----- 新增现代化仪表盘标签 -----
    QLabel *m_semesterValueLabel = nullptr;
    QLabel *m_phaseValueLabel = nullptr;
    QLabel *m_coinValueLabel = nullptr;
    QLabel *m_kpiValueLabel = nullptr;

    QTableWidget *m_studentTable = nullptr;
    EventLogWidget *m_logWidget = nullptr;

    QPushButton *m_recruitButton = nullptr;
    QPushButton *m_nextButton = nullptr;
    QPushButton *m_examButton = nullptr;
    QPushButton *m_sellButton = nullptr;

    int selectedStudentRow() const;
    void buildLayout();
    void updateStudentTable();
};

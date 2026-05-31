#pragma once

#include <QDialog>
#include <QVector>

#include "examsystem.h"

class QLabel;
class QButtonGroup;
class QRadioButton;
class QPushButton;
class QVBoxLayout;

class ExamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExamDialog(const QVector<Question> &questions,
                        const QString &studentName,
                        QWidget *parent = nullptr);

    double accuracy() const;

private slots:
    void submitCurrent();

private:
    QVector<Question> m_questions;
    int m_currentIndex = 0;
    int m_correctCount = 0;
    bool m_currentSubmitted = false;

    QLabel *m_titleLabel = nullptr;
    QLabel *m_feedbackEmojiLabel = nullptr;
    QLabel *m_promptLabel = nullptr;
    QLabel *m_resultLabel = nullptr;
    QButtonGroup *m_optionGroup = nullptr;
    QVBoxLayout *m_optionsLayout = nullptr;
    QPushButton *m_submitButton = nullptr;

    void renderQuestion();
    void clearOptions();
};

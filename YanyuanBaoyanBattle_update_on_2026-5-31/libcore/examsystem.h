#pragma once

#include <QString>
#include <QStringList>
#include <QVector>

struct Question
{
    QString prompt;
    QStringList options;
    int answerIndex = 0;
    QString explanation;
};

class ExamSystem
{
public:
    bool loadFromResource(const QString &path = QStringLiteral(":/questions.json"));
    QVector<Question> randomQuestions(int count) const;
    int questionCount() const;

private:
    QVector<Question> m_questions;
    void loadFallbackQuestions();
};

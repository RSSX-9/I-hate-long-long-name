#pragma once

#include <QString>
#include <QtGlobal>

class Student
{
public:
    Student();
    Student(const QString &name, int score, int health, int mood, int basePrice);

    QString name() const;
    int score() const;
    int health() const;
    int mood() const;
    int basePrice() const;
    int marketPrice() const;

    bool isActive() const;
    bool isSold() const;
    bool isSuspended() const;
    bool isRecommended() const;
    QString statusText() const;

    void addScore(int delta);
    void addHealth(int delta);
    void addMood(int delta);
    void applyDelta(int scoreDelta, int healthDelta, int moodDelta);
    void simulateExam();

    void markSold();
    void markSuspended();

    bool operator<(const Student &other) const;

private:
    QString m_name;
    int m_score = 0;
    int m_health = 0;
    int m_mood = 0;
    int m_basePrice = 0;
    bool m_sold = false;
    bool m_suspended = false;

    static int clampAttribute(int value);
};

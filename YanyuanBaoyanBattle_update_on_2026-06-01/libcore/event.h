#pragma once

#include <QString>
#include <QVector>

enum class EventTarget
{
    AllStudents,
    RandomStudent
};

class Event
{
public:
    Event();
    Event(const QString &name,
          const QString &description,
          EventTarget target,
          int scoreDelta,
          int healthDelta,
          int moodDelta,
          int coinDelta);

    QString name() const;
    QString description() const;
    EventTarget target() const;
    int scoreDelta() const;
    int healthDelta() const;
    int moodDelta() const;
    int coinDelta() const;
    QString summary() const;

    static QVector<Event> library();

private:
    QString m_name;
    QString m_description;
    EventTarget m_target = EventTarget::AllStudents;
    int m_scoreDelta = 0;
    int m_healthDelta = 0;
    int m_moodDelta = 0;
    int m_coinDelta = 0;
};

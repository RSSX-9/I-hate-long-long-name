#pragma once

#include <QString>
#include <QVector>
#include <QMetaType>

enum class EffectTarget
{
    AllStudents,
    RandomStudent
};

class Buff
{
public:
    Buff();
    Buff(const QString &name,
         const QString &description,
         EffectTarget target,
         int scoreDelta,
         int healthDelta,
         int moodDelta,
         int coinDelta);

    QString name() const;
    QString description() const;
    EffectTarget target() const;
    int scoreDelta() const;
    int healthDelta() const;
    int moodDelta() const;
    int coinDelta() const;

    QString summary() const;

    static QVector<Buff> library();

private:
    QString m_name;
    QString m_description;
    EffectTarget m_target = EffectTarget::AllStudents;
    int m_scoreDelta = 0;
    int m_healthDelta = 0;
    int m_moodDelta = 0;
    int m_coinDelta = 0;
};

Q_DECLARE_METATYPE(Buff)
Q_DECLARE_METATYPE(QVector<Buff>)

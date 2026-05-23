#include "event.h"

#include "gameconfig.h"

Event::Event()
{
}

Event::Event(const QString &name,
             const QString &description,
             EventTarget target,
             int scoreDelta,
             int healthDelta,
             int moodDelta,
             int coinDelta)
    : m_name(name),
      m_description(description),
      m_target(target),
      m_scoreDelta(scoreDelta),
      m_healthDelta(healthDelta),
      m_moodDelta(moodDelta),
      m_coinDelta(coinDelta)
{
}

QString Event::name() const { return m_name; }
QString Event::description() const { return m_description; }
EventTarget Event::target() const { return m_target; }
int Event::scoreDelta() const { return m_scoreDelta; }
int Event::healthDelta() const { return m_healthDelta; }
int Event::moodDelta() const { return m_moodDelta; }
int Event::coinDelta() const { return m_coinDelta; }

QString Event::summary() const
{
    QString targetText = (m_target == EventTarget::AllStudents)
            ? QStringLiteral("全体")
            : QStringLiteral("随机一人");
    return QStringLiteral("%1：%2\n对象：%3，成绩%4，健康%5，心理%6，金币%7")
            .arg(m_name)
            .arg(m_description)
            .arg(targetText)
            .arg(m_scoreDelta, 0, 10, QLatin1Char('+'))
            .arg(m_healthDelta, 0, 10, QLatin1Char('+'))
            .arg(m_moodDelta, 0, 10, QLatin1Char('+'))
            .arg(m_coinDelta, 0, 10, QLatin1Char('+'));
}

QVector<Event> Event::library()
{
    return {
        Event(QStringLiteral("期末考试泄题风波"), QStringLiteral("全院复查，学生成绩小幅下降。"), EventTarget::AllStudents, GameConfig::EventValue::LeakScore, 0, GameConfig::EventValue::LeakMood, GameConfig::EventValue::LeakCoins),
        Event(QStringLiteral("操场翻修完成"), QStringLiteral("大家运动热情提升。"), EventTarget::AllStudents, 0, GameConfig::EventValue::PlaygroundHealth, 0, 0),
        Event(QStringLiteral("宿舍停水"), QStringLiteral("生活质量下降，心理受损。"), EventTarget::AllStudents, 0, GameConfig::EventValue::WaterOutageHealth, GameConfig::EventValue::WaterOutageMood, 0),
        Event(QStringLiteral("学院宣传片爆火"), QStringLiteral("校友捐赠到账。"), EventTarget::AllStudents, 0, 0, 0, GameConfig::EventValue::PromoCoins),
        Event(QStringLiteral("大作业延期"), QStringLiteral("随机一人压力下降，成绩也更稳。"), EventTarget::RandomStudent, GameConfig::EventValue::HomeworkScore, 0, GameConfig::EventValue::HomeworkMood, 0),
        Event(QStringLiteral("自行车摔倒"), QStringLiteral("随机一人健康下降。"), EventTarget::RandomStudent, 0, GameConfig::EventValue::BikeFallHealth, GameConfig::EventValue::BikeFallMood, 0),
        Event(QStringLiteral("保研讲座"), QStringLiteral("目标感提升。"), EventTarget::AllStudents, GameConfig::EventValue::SeminarScore, 0, GameConfig::EventValue::SeminarMood, GameConfig::EventValue::SeminarCoins),
        Event(QStringLiteral("食堂新品"), QStringLiteral("快乐和健康都有小幅提升。"), EventTarget::AllStudents, 0, GameConfig::EventValue::CanteenHealth, GameConfig::EventValue::CanteenMood, GameConfig::EventValue::CanteenCoins)
    };
}

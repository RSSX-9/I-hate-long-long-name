#include "buff.h"

#include "gameconfig.h"

Buff::Buff()
{
}

Buff::Buff(const QString &name,
           const QString &description,
           EffectTarget target,
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

QString Buff::name() const { return m_name; }
QString Buff::description() const { return m_description; }
EffectTarget Buff::target() const { return m_target; }
int Buff::scoreDelta() const { return m_scoreDelta; }
int Buff::healthDelta() const { return m_healthDelta; }
int Buff::moodDelta() const { return m_moodDelta; }
int Buff::coinDelta() const { return m_coinDelta; }

QString Buff::summary() const
{
    QString targetText = (m_target == EffectTarget::AllStudents)
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

QVector<Buff> Buff::library()
{
    return {
        Buff(QStringLiteral("学霸笔记"), QStringLiteral("优秀学长姐分享复习资料。"), EffectTarget::AllStudents, GameConfig::BuffValue::StudyNoteScore, 0, 0, 0),
        Buff(QStringLiteral("流感季节"), QStringLiteral("宿舍楼集体感冒。"), EffectTarget::AllStudents, 0, GameConfig::BuffValue::FluHealth, 0, 0),
        Buff(QStringLiteral("奖学金"), QStringLiteral("学院拿到一笔临时经费。"), EffectTarget::AllStudents, 0, 0, 0, GameConfig::BuffValue::ScholarshipCoins),
        Buff(QStringLiteral("心理咨询周"), QStringLiteral("学生压力得到缓解。"), EffectTarget::AllStudents, 0, 0, GameConfig::BuffValue::CounselingMood, GameConfig::BuffValue::CounselingCoins),
        Buff(QStringLiteral("早八风暴"), QStringLiteral("连续早八让所有人状态下滑。"), EffectTarget::AllStudents, GameConfig::BuffValue::MorningClassScore, GameConfig::BuffValue::MorningClassHealth, GameConfig::BuffValue::MorningClassMood, 0),
        Buff(QStringLiteral("五四操场夜跑"), QStringLiteral("运动氛围提升健康。"), EffectTarget::AllStudents, 0, GameConfig::BuffValue::NightRunHealth, GameConfig::BuffValue::NightRunMood, 0),
        Buff(QStringLiteral("竞赛加训"), QStringLiteral("随机一人疯狂刷题。"), EffectTarget::RandomStudent, GameConfig::BuffValue::ContestTrainingScore, GameConfig::BuffValue::ContestTrainingHealth, GameConfig::BuffValue::ContestTrainingMood, GameConfig::BuffValue::ContestTrainingCoins),
        Buff(QStringLiteral("家长赞助"), QStringLiteral("随机一人带来赞助。"), EffectTarget::RandomStudent, 0, 0, GameConfig::BuffValue::ParentSponsorMood, GameConfig::BuffValue::ParentSponsorCoins),
        Buff(QStringLiteral("未名湖散步"), QStringLiteral("随机一人心态大好。"), EffectTarget::RandomStudent, 0, GameConfig::BuffValue::LakeWalkHealth, GameConfig::BuffValue::LakeWalkMood, 0),
        Buff(QStringLiteral("助教答疑"), QStringLiteral("答疑课提升全体成绩。"), EffectTarget::AllStudents, GameConfig::BuffValue::TeachingAssistantScore, 0, 0, GameConfig::BuffValue::TeachingAssistantCoins)
    };
}

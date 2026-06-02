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
        Event(QStringLiteral("期末考试泄题风波"), QStringLiteral("全院复查，学生成绩小幅下降。"), EventTarget::AllStudents, -3, 0, -2, -30),
        Event(QStringLiteral("操场翻修完成"), QStringLiteral("大家运动热情提升。"), EventTarget::AllStudents, 0, 5, 0, 0),
        Event(QStringLiteral("宿舍停水"), QStringLiteral("生活质量下降，心理受损。"), EventTarget::AllStudents, 0, -2, -5, 0),
        Event(QStringLiteral("学院宣传片爆火"), QStringLiteral("校友捐赠到账。"), EventTarget::AllStudents, 0, 0, 0, 150),
        Event(QStringLiteral("大作业延期"), QStringLiteral("随机一人压力下降，成绩也更稳。"), EventTarget::RandomStudent, 5, 0, 8, 0),
        Event(QStringLiteral("自行车摔倒"), QStringLiteral("随机一人健康下降。"), EventTarget::RandomStudent, 0, -14, -3, 0),
        Event(QStringLiteral("保研讲座"), QStringLiteral("目标感提升。"), EventTarget::AllStudents, 3, 0, 4, -20),
        Event(QStringLiteral("食堂新品"), QStringLiteral("快乐和健康都有小幅提升。"), EventTarget::AllStudents, 0, 3, 3, -50),
        Event(QStringLiteral("临时停电"), QStringLiteral("图书馆突然停电，正在学习的学生心态崩了。"), EventTarget::AllStudents, -2, 0, -6, 0),
        Event(QStringLiteral("体育测试通知"), QStringLiteral("下周体育测试，紧急锻炼。"), EventTarget::AllStudents, 0, 4, -3, 0),
        Event(QStringLiteral("心理普查"), QStringLiteral("学校组织心理普查，发现问题及时疏导。"), EventTarget::AllStudents, 0, 0, 5, 0),
        Event(QStringLiteral("校园网崩溃"), QStringLiteral("断网半天，作业提交延期。"), EventTarget::AllStudents, -3, 0, -4, 0),
        Event(QStringLiteral("知名教授公开课"), QStringLiteral("全校轰动，学术氛围提升。"), EventTarget::AllStudents, 4, 0, 2, 0),
        Event(QStringLiteral("辅导员查寝"), QStringLiteral("查到违规电器，扣分并通报。"), EventTarget::RandomStudent, -5, 0, -8, 0),
        Event(QStringLiteral("社团文化节"), QStringLiteral("随机一人参加社团活动，快乐增加。"), EventTarget::RandomStudent, 0, 2, 10, 0),
        Event(QStringLiteral("校友捐赠自习室"), QStringLiteral("安静学习环境，成绩提升。"), EventTarget::AllStudents, 3, 0, 2, 0),
        Event(QStringLiteral("流感疫苗免费接种"), QStringLiteral("全体健康小幅上升。"), EventTarget::AllStudents, 0, 4, 0, 0),
        Event(QStringLiteral("期末考试时间提前"), QStringLiteral("复习时间缩短，全体焦虑。"), EventTarget::AllStudents, -4, 0, -5, 0),
        Event(QStringLiteral("匿名举报作弊"), QStringLiteral("随机一人被诬陷，心理重创。"), EventTarget::RandomStudent, 0, 0, -12, 0),
        Event(QStringLiteral("导师科研奖励"), QStringLiteral("导师发奖金，随机一人获得零花钱。"), EventTarget::RandomStudent, 0, 0, 5, 80),
        Event(QStringLiteral("交换生机会"), QStringLiteral("随机一人获得出国交换名额，但需自费部分。"), EventTarget::RandomStudent, 8, 0, 12, -60),
        Event(QStringLiteral("食堂涨价"), QStringLiteral("伙食费上涨，全体学生钱包缩水。"), EventTarget::AllStudents, 0, -1, -2, -40),
        Event(QStringLiteral("校内招聘会"), QStringLiteral("实习机会增多，部分学生放弃学业准备求职。"), EventTarget::AllStudents, -2, 0, 3, 0),
        Event(QStringLiteral("睡眠日公益活动"), QStringLiteral("号召早睡，健康提升但复习时间减少。"), EventTarget::AllStudents, -1, 3, 2, 0),
        Event(QStringLiteral("创业大赛获奖"), QStringLiteral("团队获奖，奖金入账，成员忙碌消耗健康。"), EventTarget::RandomStudent, 3, -3, 8, 100),
        Event(QStringLiteral("暴雨停课"), QStringLiteral("停课一天，学生摸鱼快乐但后续补课压力增大。"), EventTarget::AllStudents, -2, 0, 8, 0),
        Event(QStringLiteral("爱心献血"), QStringLiteral("自愿献血，健康下降但获得补贴和荣誉。"), EventTarget::AllStudents, 0, -5, 6, 50),
        Event(QStringLiteral("论文抽检"), QStringLiteral("高年级论文被抽检，紧张导致焦虑。"), EventTarget::AllStudents, 0, 0, -6, 0),
        Event(QStringLiteral("跳蚤市场开放"), QStringLiteral("买卖旧物，随机一人赚到零花钱。"), EventTarget::RandomStudent, 0, 0, 4, 30),
        Event(QStringLiteral("院系合并传闻"), QStringLiteral("学生人心惶惶，学习效率下降。"), EventTarget::AllStudents, -3, 0, -4, 0),
        Event(QStringLiteral("最强寝室评比"), QStringLiteral("竞争激烈，全体熬夜整理寝室。"), EventTarget::AllStudents, 0, -2, 2, 0)
    };
}
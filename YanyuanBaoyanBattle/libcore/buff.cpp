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
        Buff(QStringLiteral("学霸笔记"), QStringLiteral("优秀学长姐分享复习资料。"), EffectTarget::AllStudents, 2, 1, 5, 0),
        Buff(QStringLiteral("流感季节"), QStringLiteral("宿舍楼集体感冒，但同学互帮互助让人暖心。"), EffectTarget::AllStudents, 0, -3, 8, 0),
        Buff(QStringLiteral("奖学金"), QStringLiteral("学院拿到一笔临时经费。"), EffectTarget::AllStudents, 0, 0, 6, 100),
        Buff(QStringLiteral("心理咨询周"), QStringLiteral("学生压力得到缓解。"), EffectTarget::AllStudents, 0, 0, 7, -20),
        Buff(QStringLiteral("五四操场夜跑"), QStringLiteral("运动氛围提升健康。"), EffectTarget::AllStudents, 0, 8, 5, 0),
        Buff(QStringLiteral("竞赛加训"), QStringLiteral("随机一人疯狂刷题。"), EffectTarget::RandomStudent, 6, -3, -2, -20),
        Buff(QStringLiteral("家长赞助"), QStringLiteral("随机一人带来赞助。"), EffectTarget::RandomStudent, 0, 0, 10, 60),
        Buff(QStringLiteral("未名湖散步"), QStringLiteral("随机一人心态大好。"), EffectTarget::RandomStudent, 0, 5, 10, 0),
        Buff(QStringLiteral("助教答疑"), QStringLiteral("答疑课提升全体成绩。"), EffectTarget::AllStudents, 3, 0, 5, -30),
        Buff(QStringLiteral("期末冲刺班"), QStringLiteral("集体刷夜复习，成绩提升但身心俱疲。"), EffectTarget::AllStudents, 6, -2, -3, -40),
        Buff(QStringLiteral("健身房打卡"), QStringLiteral("坚持锻炼，健康上升。"), EffectTarget::AllStudents, 0, 8, 4, 0),
        Buff(QStringLiteral("校园歌手大赛"), QStringLiteral("文艺活动放松心情。"), EffectTarget::AllStudents, 0, 0, 8, -10),
        Buff(QStringLiteral("外卖红包"), QStringLiteral("随机一人省钱加餐。"), EffectTarget::RandomStudent, 0, 10, 4, 25),
        Buff(QStringLiteral("考试周断电"), QStringLiteral("复习计划被打乱，学校发放应急补助。"), EffectTarget::AllStudents, -3, 0, -4, 100),
        Buff(QStringLiteral("体育打卡"), QStringLiteral("强制锻炼，健康提升但略有不快。"), EffectTarget::AllStudents, 0, 8, -2, 0),
        Buff(QStringLiteral("大牛讲座"), QStringLiteral("开拓视野，成绩和心情双提升。"), EffectTarget::AllStudents, 2, 1, 6, 0),
        Buff(QStringLiteral("宿舍聚餐"), QStringLiteral("增进感情，心情和金币各有所得。"), EffectTarget::AllStudents, 0, 4, 8, -30),
        Buff(QStringLiteral("图书馆占座风波"), QStringLiteral("争抢座位引发不快，但来回跑动也算锻炼。"), EffectTarget::AllStudents, 0, 4, -1, 0),
        Buff(QStringLiteral("勤工助学"), QStringLiteral("随机一人打工赚外快。"), EffectTarget::RandomStudent, 0, -3, 3, 40),
        Buff(QStringLiteral("编程马拉松"), QStringLiteral("随机一人技术突飞猛进但透支健康。"), EffectTarget::RandomStudent, 5, -3, 2, 0),
        Buff(QStringLiteral("院系联谊"), QStringLiteral("社交活动带来快乐。"), EffectTarget::AllStudents, 0, 0, 6, -50),
        Buff(QStringLiteral("雾霾停课"), QStringLiteral("停课休息但学业压力延后。"), EffectTarget::AllStudents, 0, -3, 7, 0),
        Buff(QStringLiteral("英语四六级加训"), QStringLiteral("刷题提升成绩。"), EffectTarget::AllStudents, 2, -1, -1, 0),
        Buff(QStringLiteral("捡到校园卡"), QStringLiteral("随机一人拾金不昧，心情大悦。"), EffectTarget::RandomStudent, 0, 0, 10, 40),
        Buff(QStringLiteral("补考通知"), QStringLiteral("成绩不佳压力山大，但开始规律作息。"), EffectTarget::AllStudents, -3, 8, -3, 0),
        Buff(QStringLiteral("草坪音乐节"), QStringLiteral("全校狂欢，心情暴涨。"), EffectTarget::AllStudents, 0, 6, 6, -20),
        Buff(QStringLiteral("实习内推"), QStringLiteral("随机一人获得机会，心情和钱包双赢。"), EffectTarget::RandomStudent, 0, 0, 8, 80),
        Buff(QStringLiteral("熬夜赶工"), QStringLiteral("随机一人爆肝，成绩小升，健康大降。"), EffectTarget::RandomStudent, 5, -6, -2, 0),
        Buff(QStringLiteral("校友返校日"), QStringLiteral("校友捐赠，学院经费增加。"), EffectTarget::AllStudents, 0, 0, 8, 50),
        Buff(QStringLiteral("阳光晨跑"), QStringLiteral("清晨跑步唤醒一天活力。"), EffectTarget::AllStudents, 0, 10, 5, 0),
        Buff(QStringLiteral("心理辅导沙龙"), QStringLiteral("专业辅导释放压力。"), EffectTarget::AllStudents, 0, 2, 7, -10),
        Buff(QStringLiteral("水果福利"), QStringLiteral("学院发放新鲜水果，补充维C。"), EffectTarget::AllStudents, 0, 6, 4, 0),
        Buff(QStringLiteral("春日赏花"), QStringLiteral("校园花开烂漫，心情愉悦。"), EffectTarget::AllStudents, 0, 4, 5, 0),
        Buff(QStringLiteral("瑜伽课程"), QStringLiteral("身心合一，缓解疲劳。"), EffectTarget::AllStudents, 0, 8, 6, -5),
        Buff(QStringLiteral("室友庆生"), QStringLiteral("随机一人收到室友惊喜。"), EffectTarget::RandomStudent, 0, 1, 10, -40),
        Buff(QStringLiteral("社团出游"), QStringLiteral("集体户外活动，放松身心。"), EffectTarget::AllStudents, 0, 3, 8, -30),
        Buff(QStringLiteral("睡眠日倡议"), QStringLiteral("强调充足睡眠，改善作息。"), EffectTarget::AllStudents, 0, 7, 4, 0),
        Buff(QStringLiteral("志愿活动"), QStringLiteral("帮助他人，收获快乐。"), EffectTarget::AllStudents, 0, 3, 6, -5),
        Buff(QStringLiteral("期末减压茶话会"), QStringLiteral("师生畅谈，缓解焦虑。"), EffectTarget::AllStudents, 0, 4, 4, -10)
    };
}

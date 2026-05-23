#include "student.h"

#include "gameconfig.h"

#include <QRandomGenerator>
#include <QtMath>

Student::Student()
{
}

Student::Student(const QString &name, int score, int health, int mood, int basePrice)
    : m_name(name),
      m_score(clampAttribute(score)),
      m_health(clampAttribute(health)),
      m_mood(clampAttribute(mood)),
      m_basePrice(basePrice)
{
}

QString Student::name() const { return m_name; }
int Student::score() const { return m_score; }
int Student::health() const { return m_health; }
int Student::mood() const { return m_mood; }
int Student::basePrice() const { return m_basePrice; }

int Student::marketPrice() const
{
    if (!isActive()) {
        return 0;
    }
    const double value = m_score * GameConfig::MarketScoreWeight
            + m_health * GameConfig::MarketHealthWeight
            + m_mood * GameConfig::MarketMoodWeight;
    return qBound(GameConfig::MarketMinPrice, qRound(value), GameConfig::MarketMaxPrice);
}

bool Student::isActive() const
{
    return !m_sold && !m_suspended;
}

bool Student::isSold() const
{
    return m_sold;
}

bool Student::isSuspended() const
{
    return m_suspended;
}

bool Student::isRecommended() const
{
    return isActive() && m_score >= GameConfig::RecommendScore
            && m_health >= GameConfig::RecommendHealth
            && m_mood >= GameConfig::RecommendMood;
}

QString Student::statusText() const
{
    if (m_suspended) {
        return QStringLiteral("休学");
    }
    if (m_sold) {
        return QStringLiteral("转学");
    }
    return QStringLiteral("在读");
}

void Student::addScore(int delta)
{
    if (!isActive()) return;
    m_score = clampAttribute(m_score + delta);
}

void Student::addHealth(int delta)
{
    if (!isActive()) return;
    m_health = clampAttribute(m_health + delta);
}

void Student::addMood(int delta)
{
    if (!isActive()) return;
    m_mood = clampAttribute(m_mood + delta);
}

void Student::applyDelta(int scoreDelta, int healthDelta, int moodDelta)
{
    addScore(scoreDelta);
    addHealth(healthDelta);
    addMood(moodDelta);
}

void Student::simulateExam()
{
    if (!isActive()) return;

    const int randomWave = QRandomGenerator::global()->bounded(GameConfig::ExamRandomMin,
                                                                    GameConfig::ExamRandomMaxExclusive);
    int pressurePenalty = 0;

    if (m_health < GameConfig::ExamLowHealthThreshold) pressurePenalty += GameConfig::ExamLowHealthPenalty;
    if (m_mood < GameConfig::ExamLowMoodThreshold) pressurePenalty += GameConfig::ExamLowMoodPenalty;
    if (m_health >= GameConfig::ExamGoodConditionThreshold
            && m_mood >= GameConfig::ExamGoodConditionThreshold) {
        pressurePenalty += GameConfig::ExamGoodConditionBonus;
    }

    addScore(randomWave + pressurePenalty);
    addHealth(GameConfig::ExamHealthDelta);
    addMood(GameConfig::ExamMoodDelta);
}

void Student::markSold()
{
    if (!m_suspended) {
        m_sold = true;
    }
}

void Student::markSuspended()
{
    if (!m_sold) {
        m_suspended = true;
    }
}

bool Student::operator<(const Student &other) const
{
    return marketPrice() < other.marketPrice();
}

int Student::clampAttribute(int value)
{
    return qBound(GameConfig::AttributeMin, value, GameConfig::AttributeMax);
}

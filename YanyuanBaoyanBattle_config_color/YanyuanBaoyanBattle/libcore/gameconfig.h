#pragma once

namespace GameConfig
{
// ----- 核心流程 -----
inline constexpr int StartingFunds = 3000;
inline constexpr int RecruitBudget = 1600;
inline constexpr int MinRecruitCount = 4;
inline constexpr int TotalSemesters = 8;
inline constexpr double TargetRecommendRate = 0.60;

// ----- 学生属性 -----
inline constexpr int AttributeMin = 0;
inline constexpr int AttributeMax = 100;
inline constexpr int RecommendScore = 90;
inline constexpr int RecommendHealth = 70;
inline constexpr int RecommendMood = 70;
inline constexpr int SuspensionThreshold = 10;

// ----- 每学期经营 -----
inline constexpr int LivingCostPerStudent = 50;
inline constexpr int SuspendPenalty = 200;

// ----- 替考 -----
inline constexpr int SubstituteLimit = 3;
inline constexpr int SubstituteCost = 20;
inline constexpr double SubstituteScoreMultiplier = 15.0;

// ----- 转学出售 -----
inline constexpr double SellIncomeRate = 0.8;

// ----- 考试波动 -----
inline constexpr int ExamRandomMin = -5;
inline constexpr int ExamRandomMaxExclusive = 6;
inline constexpr int ExamLowHealthThreshold = 50;
inline constexpr int ExamLowMoodThreshold = 50;
inline constexpr int ExamLowHealthPenalty = -3;
inline constexpr int ExamLowMoodPenalty = -3;
inline constexpr int ExamGoodConditionThreshold = 85;
inline constexpr int ExamGoodConditionBonus = 2;
inline constexpr int ExamHealthDelta = -2;
inline constexpr int ExamMoodDelta = -3;

// ----- 市场价 -----
inline constexpr double MarketScoreWeight = 2.2;
inline constexpr double MarketHealthWeight = 1.4;
inline constexpr double MarketMoodWeight = 1.4;
inline constexpr int MarketMinPrice = 50;
inline constexpr int MarketMaxPrice = 520;



// ----- UI 尺寸和显示 -----
inline constexpr int RecruitDialogWidth = 760;
inline constexpr int RecruitDialogHeight = 430;
inline constexpr int ExamDialogWidth = 620;
inline constexpr int ExamDialogHeight = 420;
inline constexpr int MainTitleFontSize = 24;
inline constexpr int ExamPromptFontSize = 16;
inline constexpr int SubstituteQuestionCount = 5;

// ----- UI 属性颜色 -----
inline constexpr int AttributeHighThreshold = 85;
inline constexpr int AttributeWarningThreshold = 60;
inline constexpr int AttributeDangerThreshold = 30;

inline constexpr const char *AttributeHighColor = "#2e7d32";
inline constexpr const char *AttributeNormalColor = "#202124";
inline constexpr const char *AttributeWarningColor = "#ef6c00";
inline constexpr const char *AttributeDangerColor = "#c62828";
inline constexpr const char *AttributeInactiveColor = "#757575";
inline constexpr const char *GoodStatusColor = "#2e7d32";
inline constexpr const char *BadStatusColor = "#c62828";

// ----- 招生候选人 -----
struct RecruitCandidateConfig
{
    const char *name;
    int score;
    int health;
    int mood;
    int basePrice;
    bool defaultSelected;
};

inline constexpr RecruitCandidateConfig RecruitCandidates[] = {
    {"学霸张三", 85, 75, 70, 300, true},
    {"运动李四", 70, 90, 80, 280, false},
    {"乐观王五", 60, 70, 95, 250, true},
    {"普通赵六", 70, 70, 70, 220, true},
    {"卷王小陈", 90, 55, 60, 260, false},
    {"佛系小林", 55, 80, 88, 210, true},
    {"社团达人", 65, 75, 85, 230, false},
    {"科研新星", 82, 62, 68, 270, false},
    {"夜跑高手", 68, 95, 62, 240, false},
    {"稳定小周", 75, 78, 78, 260, false}
};

// ----- Buff 数值 -----
namespace BuffValue
{
inline constexpr int StudyNoteScore = 5;
inline constexpr int FluHealth = -10;
inline constexpr int ScholarshipCoins = 120;
inline constexpr int CounselingMood = 8;
inline constexpr int CounselingCoins = -40;
inline constexpr int MorningClassScore = 2;
inline constexpr int MorningClassHealth = -4;
inline constexpr int MorningClassMood = -6;
inline constexpr int NightRunHealth = 7;
inline constexpr int NightRunMood = 2;
inline constexpr int ContestTrainingScore = 12;
inline constexpr int ContestTrainingHealth = -4;
inline constexpr int ContestTrainingMood = -3;
inline constexpr int ContestTrainingCoins = -30;
inline constexpr int ParentSponsorMood = 5;
inline constexpr int ParentSponsorCoins = 80;
inline constexpr int LakeWalkHealth = 2;
inline constexpr int LakeWalkMood = 14;
inline constexpr int TeachingAssistantScore = 7;
inline constexpr int TeachingAssistantCoins = -60;
}

// ----- 随机事件数值 -----
namespace EventValue
{
inline constexpr int LeakScore = -3;
inline constexpr int LeakMood = -2;
inline constexpr int LeakCoins = -30;
inline constexpr int PlaygroundHealth = 5;
inline constexpr int WaterOutageHealth = -2;
inline constexpr int WaterOutageMood = -5;
inline constexpr int PromoCoins = 150;
inline constexpr int HomeworkScore = 5;
inline constexpr int HomeworkMood = 8;
inline constexpr int BikeFallHealth = -14;
inline constexpr int BikeFallMood = -3;
inline constexpr int SeminarScore = 3;
inline constexpr int SeminarMood = 4;
inline constexpr int SeminarCoins = -20;
inline constexpr int CanteenHealth = 3;
inline constexpr int CanteenMood = 3;
inline constexpr int CanteenCoins = -50;
}
}

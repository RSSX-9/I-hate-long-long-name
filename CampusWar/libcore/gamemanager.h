#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include <vector>
#include <string>
#include <functional>
#include "student.h"
#include "buff.h"
#include "event.h"

enum class GamePhase {
    IDLE,
    SEMESTER_START,     // 扣费，等待第一轮Buff选择
    SUBSTITUTE,         // 替考
    MIDTERM_EXAM,       // 期中考试
    MIDTERM_TRANSFER,   // 期中后转学
    POST_MIDTERM,       // 等待第二轮Buff
    FINAL_EXAM,         // 期末考试
    FINAL_SUSPENSION,   // 休学检查
    SETTLEMENT,         // 学期结算
    SEMESTER_END        // 学期结束
};

class GameManager {
public:
    int gold = 1000;
    int semester = 0;          // 当前学期 1~8，0为未开始
    int initialStudentCount = 0;
    std::vector<Student> students;
    std::vector<Event> eventPool;
    std::vector<Buff> buffPool;
    const int passNumber = 6;
    const int livingCostPerStudent = 50;
    const int maxSubstitute = 3;    // 每学期替考人数上限

    GamePhase currentPhase = GamePhase::IDLE;

    GameManager();
    void initStudents(const std::vector<Student>& selected);

    // 阶段驱动方法
    void startSemester();
    void finishBuffSelection(const Buff& chosen);   // 第一轮Buff选择完成
    void skipBuff();                                 // 跳过Buff选择（可选）
    void executeSubstitute(int index, double accuracy);
    void executeMidtermExam();                       // 期中考试并进入转学阶段
    void transferStudent(int index);                // 玩家转学
    void advanceToPostMidterm();                     // 进入第二轮Buff
    void finishPostMidtermBuff(const Buff& chosen);  // 第二轮Buff完成
    void skipPostBuff();                             // 跳过第二轮Buff
    void executeFinalExam();                         // 期末考试 -> 休学检查 -> 结算 -> 学期结束
    bool isGameOver() const;                         // 金币<0立即失败
    bool isVictory() const;                          // 第8学期结束保研率达标

    // 工具方法
    std::vector<Student*> getActiveStudents();
    std::vector<Buff> generateBuffChoices(int num = 2);
    std::string getLog() const { return logBuffer; }
    void addLog(const std::string& msg);

private:
    std::string logBuffer;
    int substituteCount = 0;   // 已替考人数（每学期重置）

    void applyBuffChoice(const Buff& chosen);
    void triggerRandomEvent();
    void processExam(bool isFinal);
    void handleSuspension(Student& s);
    void checkSuspensions();
    void settleSemester();
    void resetSubstituteCount() { substituteCount = 0; }
};

#endif
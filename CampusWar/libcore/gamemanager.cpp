#include "gamemanager.h"
#include <algorithm>
#include <random>
#include <sstream>

static std::mt19937 rng(std::random_device{}());

GameManager::GameManager() {
    // 初始化事件池
    eventPool = {
        //此处填事件
    };

    // 初始化Buff池
    buffPool = {
        //此处填buff
    };
}

std::vector<Student*> GameManager::getActiveStudents() {
    std::vector<Student*> active;
    for (auto& s : students)
        if (!s.isSuspended) active.push_back(&s);
    return active;
}

void GameManager::initStudents(const std::vector<Student>& selected) {
    students = selected;
    gold -= std::accumulate(selected.begin(), selected.end(), 0,
                            [](int sum, const Student& s) { return sum + s.basePrice; });
    initialStudentCount = static_cast<int>(students.size());
    semester = 1;
    addLog("游戏开始！共招收 " + std::to_string(students.size()) + " 名学生。");
}

// --------------------- 阶段驱动 ---------------------
void GameManager::startSemester() {
    resetSubstituteCount();
    int cost = static_cast<int>(getActiveStudents().size()) * livingCostPerStudent;
    gold -= cost;
    std::stringstream ss;
    ss << "===== 第" << semester << "学期 =====";
    addLog(ss.str());
    addLog("扣除生活费 " + std::to_string(cost) + " 金币，剩余 " + std::to_string(gold));
    if (gold < 0) {
        addLog("金币不足，游戏失败！");
        currentPhase = GamePhase::SEMESTER_END;
        return;
    }
    currentPhase = GamePhase::SEMESTER_START; // 等待UI提供Buff选择
}

void GameManager::finishBuffSelection(const Buff& chosen) {
    applyBuffChoice(chosen);
    triggerRandomEvent();
    currentPhase = GamePhase::SUBSTITUTE;
}

void GameManager::skipBuff() {
    triggerRandomEvent();
    currentPhase = GamePhase::SUBSTITUTE;
}

void GameManager::executeSubstitute(int index, double accuracy) {
    if (substituteCount >= maxSubstitute) return;
    if (index < 0 || index >= static_cast<int>(students.size())) return;
    if (students[index].isSuspended) return;
    int boost = static_cast<int>(accuracy * 15);
    students[index].applyBuff(boost, 0, 0);
    addLog(students[index].name + " 替考提升成绩 " + std::to_string(boost));
    substituteCount++;
}

void GameManager::executeMidtermExam() {
    processExam(false);
    currentPhase = GamePhase::MIDTERM_TRANSFER;
}

void GameManager::transferStudent(int index) {
    if (currentPhase != GamePhase::MIDTERM_TRANSFER) return;
    if (index < 0 || index >= static_cast<int>(students.size())) return;
    if (students[index].isSuspended) return; // 休学者不能转学
    int price = students[index].currentPrice();
    gold += static_cast<int>(price * 0.8);
    addLog(students[index].name + " 转学，获得 " + std::to_string(static_cast<int>(price*0.8)) + " 金币");
    students.erase(students.begin() + index);
}

void GameManager::advanceToPostMidterm() {
    currentPhase = GamePhase::POST_MIDTERM; // 等待第二轮Buff
}

void GameManager::finishPostMidtermBuff(const Buff& chosen) {
    applyBuffChoice(chosen);
    triggerRandomEvent();
    executeFinalExam();
}

void GameManager::skipPostBuff() {
    triggerRandomEvent();
    executeFinalExam();
}

void GameManager::executeFinalExam() {
    processExam(true);
    currentPhase = GamePhase::FINAL_SUSPENSION;
    checkSuspensions();
    settleSemester();
    currentPhase = (semester >= 8) ? GamePhase::SEMESTER_END : GamePhase::IDLE;
    if (semester < 8) semester++;
    else addLog("游戏结束！等待结果判定...");
}

// --------------------- 内部逻辑 ---------------------
void GameManager::applyBuffChoice(const Buff& chosen) {
    if (chosen.dGold != 0) {
        gold += chosen.dGold;
        addLog("Buff：获得 " + std::to_string(chosen.dGold) + " 金币");
    }
    if (chosen.isGlobal) {
        for (auto* s : getActiveStudents())
            s->applyBuff(chosen.dGrade, chosen.dHealth, chosen.dMental);
        addLog("Buff：" + chosen.name + "（全体）");
    } else {
        auto active = getActiveStudents();
        if (!active.empty()) {
            int idx = rand() % active.size();
            active[idx]->applyBuff(chosen.dGrade, chosen.dHealth, chosen.dMental);
            addLog("Buff：" + chosen.name + " 对 " + active[idx]->name + " 生效");
        }
    }
}

void GameManager::triggerRandomEvent() {
    if (eventPool.empty()) return;
    int idx = rand() % eventPool.size();
    eventPool[idx].effect(*this);
}

void GameManager::processExam(bool isFinal) {
    std::string exam = isFinal ? "期末" : "期中";
    addLog(exam + "考试开始");
    for (auto* s : getActiveStudents())
        s->takeExam(0.5);
    if (rand() % 100 < 30) triggerRandomEvent();
}

void GameManager::handleSuspension(Student& s) {
    s.isSuspended = true;
    s.basePrice = 0;
    gold -= 200;
    addLog(s.name + " 属性过低休学！扣除200金币公关费");
}

void GameManager::checkSuspensions() {
    for (auto& s : students) {
        if (!s.isSuspended && s.checkSuspend())
            handleSuspension(s);
    }
}

void GameManager::settleSemester() {
    for (auto& s : students) {
        if (s.isSuspended) continue;
        s.updatePrice();
        s.isBaoyan = s.checkBaoyan();
    }
}

std::vector<Buff> GameManager::generateBuffChoices(int num) {
    std::vector<Buff> pool = buffPool;
    std::shuffle(pool.begin(), pool.end(), rng);
    if (pool.size() > static_cast<size_t>(num)) pool.resize(num);
    return pool;
}

bool GameManager::isGameOver() const {
    return gold < 0;
}

bool GameManager::isVictory() const {
    if (semester != 8 || currentPhase != GamePhase::SEMESTER_END) return false;
    int baoyan = std::count_if(students.begin(), students.end(),
                               [](const Student& s) { return s.isBaoyan; });
    return (initialStudentCount > 0) && (baoyan >= passNumber);
}

void GameManager::addLog(const std::string& msg) {
    logBuffer += msg + "\n";
}
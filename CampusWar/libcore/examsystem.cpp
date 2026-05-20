#include "examsystem.h"
#include <random>
#include <algorithm>

ExamSystem::ExamSystem() {
    bank = {
        //这里写入题目
    };
}

std::vector<ExamSystem::Question> ExamSystem::getRandomQuestions(int n) const {
    std::vector<ExamSystem::Question> selection = bank;
    std::shuffle(selection.begin(), selection.end(), std::mt19937(std::random_device{}()));
    if(selection.size() > static_cast<size_t>(n))
        selection.resize(n);
    return selection;
}
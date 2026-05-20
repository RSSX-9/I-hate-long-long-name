#ifndef EXAMSYSTEM_H
#define EXAMSYSTEM_H
#include <string>
#include <vector>

class ExamSystem
{
public:
    struct Question {
        std::string text;
        std::vector<std::string> options;
        int correctIndex;
    };

    ExamSystem();
    std::vector<Question> bank;
    std::vector<Question> getRandomQuestions(int n = 5) const;
};

#endif // EXAMSYSTEM_H

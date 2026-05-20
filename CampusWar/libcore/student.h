#ifndef STUDENT_H
#define STUDENT_H
#include <string>

class Student
{
public:
    std::string name;
    int grade;
    int health;
    int mental;
    int basePrice;
    bool isSuspended;
    bool isBaoyan;

    Student(const std::string& n, int g, int h, int m, int p);
    void applyBuff(int dg, int dh, int dm);
    void takeExam(double difficulty);
    void updatePrice();
    int currentPrice() const;
    bool checkSuspend();
    bool checkBaoyan() const;
};

#endif // STUDENT_H

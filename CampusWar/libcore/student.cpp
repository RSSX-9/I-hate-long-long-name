#include "student.h"
#include <algorithm>

Student::Student(const std::string& n, int g, int h, int m, int p)
    : name(n), grade(g), health(h), mental(m), basePrice(p),
    isSuspended(false), isBaoyan(false) {}

void Student::applyBuff(int dg, int dh, int dm) {
    grade = std::clamp(grade + dg, 0, 100);
    health = std::clamp(health + dh, 0, 100);
    mental = std::clamp(mental + dm, 0, 100);
}

void Student::takeExam(double difficulty) {
    if(isSuspended) return;
    double performance = (grade * 0.5 + health * 0.25 + mental * 0.25) / 100.0;
    double change = (static_cast<double>(rand() % 21 - 10) / 100.0) * 10.0 + (difficulty - 0.5) * 5.0;
    grade = std::clamp(static_cast<int>(grade + change * performance), 0, 100);
}

void Student::updatePrice() {
    if(isSuspended) {
        basePrice = 0;
        return;
    }
    basePrice = static_cast<int>(basePrice * (0.6 * grade / 100.0 + 0.2 * health / 100.0 + 0.2 * mental / 100.0));
    if(basePrice < 10) basePrice = 10;
}

int Student::currentPrice() const {
    return isSuspended ? 0 : basePrice;
}

bool Student::checkSuspend() {
    if(grade < 10 || health < 10 || mental < 10) {
        isSuspended = true;
        return true;
    }
    return false;
}

bool Student::checkBaoyan() const {
    return grade >= 90 && health >= 70 && mental >= 70;
}
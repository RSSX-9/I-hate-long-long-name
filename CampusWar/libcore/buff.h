#ifndef BUFF_H
#define BUFF_H
#include <string>

struct Buff {
    std::string name;
    std::string desc;
    int dGrade = 0;
    int dHealth = 0;
    int dMental = 0;
    int dGold = 0;
    bool isGlobal = true;
};

#endif // BUFF_H

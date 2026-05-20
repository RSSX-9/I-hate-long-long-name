#ifndef EVENT_H
#define EVENT_H
#include <string>
#include <functional>
class GameManager;

struct Event {
    std::string name;
    std::string desc;
    std::function<void(GameManager&)> effect;
};

#endif // EVENT_H

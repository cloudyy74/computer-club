#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct Event {
    uint32_t time;
    int32_t id;
    std::vector<std::string> args;
    std::string rawLine;
};
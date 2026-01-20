#pragma once
#include <cstdint>
#include <optional>
#include <string>

struct Table {
    uint32_t id;
    bool occupied = false;
    std::string client;
    std::optional<uint32_t> occupiedFrom{};
    uint32_t totalMinutes = 0;
    uint32_t revenue = 0;
};

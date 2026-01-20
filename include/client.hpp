#pragma once
#include <cstdint>
#include <optional>
#include <string>

struct Client {
    std::string name;
    bool inClub = false;
    std::optional<uint32_t> tableID{};
};
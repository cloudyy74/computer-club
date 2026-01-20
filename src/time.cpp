#include "time.hpp"

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

uint32_t parseTime(const std::string& s) {
    uint32_t h = std::stoi(s.substr(0, 2));
    uint32_t m = std::stoi(s.substr(3, 2));
    return h * 60 + m;
}

std::string formatTime(uint32_t minutes) {
    uint32_t h = minutes / 60;
    uint32_t m = minutes % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << h << ":" << std::setw(2) << std::setfill('0') << m;
    return oss.str();
}

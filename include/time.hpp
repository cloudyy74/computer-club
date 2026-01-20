#pragma once
#include <cstdint>
#include <string>

uint32_t parseTime(const std::string& s);

std::string formatTime(uint32_t minutes);
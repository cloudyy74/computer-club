#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

#include "club.hpp"
#include "time.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_input_file>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "cant open file at path: " << argv[1] << std::endl;
        return 1;
    }

    uint32_t tablesCount;
    inputFile >> tablesCount;

    std::string openTimeStr, closeTimeStr;
    inputFile >> openTimeStr >> closeTimeStr;

    uint32_t pricePerHour;
    inputFile >> pricePerHour;

    Club club(tablesCount, parseTime(openTimeStr), parseTime(closeTimeStr), pricePerHour);

    std::string line;
    while (std::getline(inputFile, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        std::string timeStr;
        int32_t id;
        iss >> timeStr >> id;
        std::vector<std::string> args;
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }

        Event e{parseTime(timeStr), id, args, line};

        try {
            club.processIncomingEvent(e);
        } catch (std::exception& e) {
            std::cout << line << std::endl;
            return 1;
        }
    }
    club.closeDay();
    club.printResult();
    return 0;
}
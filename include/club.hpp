#pragma once
#include <cstdint>
#include <map>
#include <queue>
#include <string>
#include <vector>

#include "client.hpp"
#include "event.hpp"
#include "table.hpp"

class Club {
   public:
    Club(uint32_t tablesCount, uint32_t openTime, uint32_t closeTime, uint32_t pricePerHour);

    void processIncomingEvent(const Event& e);
    void closeDay();
    void printResult() const;

   private:
    uint32_t tablesCount;
    uint32_t openTime;
    uint32_t closeTime;
    uint32_t pricePerHour;

    std::map<std::string, Client> clients;
    std::vector<Table> tables;
    std::vector<Event> events;
    std::queue<std::string> waitingClients;

    void handleClientArrived(const Event& e);
    void handleClientSit(const Event& e);
    void handleClientWait(const Event& e);
    void handleClientLeft(const Event& e);

    void freeTable(uint32_t time, uint32_t tableID);
    bool hasFreeTable() const;

    void addErrorEvent(uint32_t time, const std::string& message);
    void addClientLeftEvent(uint32_t time, const std::string& client);
    void addClientSitEvent(uint32_t time, const std::string& client, uint32_t tableID);

    bool isValidClientName(const std::string& name) const;
};
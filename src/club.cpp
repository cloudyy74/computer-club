#include "club.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "consts.hpp"
#include "time.hpp"

Club::Club(uint32_t tablesCount, uint32_t openTime, uint32_t closeTime, uint32_t pricePerHour)
    : tablesCount(tablesCount),
      openTime(openTime),
      closeTime(closeTime),
      pricePerHour(pricePerHour) {
    for (uint32_t i = 1; i <= tablesCount; ++i) {
        tables.push_back({i});
    }
}

void Club::processIncomingEvent(const Event& e) {
    if (!events.empty() && e.time < events.back().time) {
        throw std::invalid_argument("bad event time");
    }
    events.push_back(e);

    switch (e.id) {
        case consts::IncomingEventClientArrived:
            handleClientArrived(e);
            break;
        case consts::IncomingEventClientSit:
            handleClientSit(e);
            break;
        case consts::IncomingEventClientWait:
            handleClientWait(e);
            break;
        case consts::IncomingEventClientLeft:
            handleClientLeft(e);
            break;
        default:
            break;
    }
}

void Club::handleClientArrived(const Event& e) {
    const std::string& clientName = e.args[0];
    if (!isValidClientName(clientName)) {
        throw std::invalid_argument("bad client name");
    }
    auto it = clients.find(clientName);

    if (it != clients.end() && it->second.inClub) {
        addErrorEvent(e.time, consts::ErrorYouShallNotPass);
        return;
    }
    if (e.time < openTime || e.time >= closeTime) {
        addErrorEvent(e.time, consts::ErrorNotOpenYet);
        return;
    }

    if (it == clients.end()) {
        clients[clientName] = Client{clientName};
    }
    clients[clientName].inClub = true;
}

void Club::handleClientSit(const Event& e) {
    const std::string& clientName = e.args[0];
    if (!isValidClientName(clientName)) {
        throw std::invalid_argument("bad client name");
    }
    uint32_t tableID = std::stoul(e.args[1]);
    if (tableID < 1 || tableID > tablesCount) {
        throw std::invalid_argument("bad table id");
    }
    auto it = clients.find(clientName);

    if (it == clients.end() || !it->second.inClub) {
        addErrorEvent(e.time, consts::ErrorClientUnknown);
        return;
    }
    if (tables[tableID - 1].occupied) {
        addErrorEvent(e.time, consts::ErrorPlaceIsBusy);
        return;
    }

    if (it->second.tableID) {
        freeTable(*it->second.tableID, e.time);
    }

    uint32_t newID = tableID - 1;
    tables[newID].occupied = true;
    tables[newID].client = clientName;
    tables[newID].occupiedFrom = e.time;
    it->second.tableID = tableID;
}

void Club::handleClientWait(const Event& e) {
    const std::string& clientName = e.args[0];
    if (!isValidClientName(clientName)) {
        throw std::invalid_argument("bad client name");
    }
    auto it = clients.find(clientName);

    if (it == clients.end() || !it->second.inClub) {
        addErrorEvent(e.time, consts::ErrorClientUnknown);
        return;
    }
    if (hasFreeTable()) {
        addErrorEvent(e.time, consts::ErrorICanWaitNoLonger);
        return;
    }
    if (static_cast<uint32_t>(waitingClients.size()) >= tablesCount) {
        it->second.inClub = false;
        addClientLeftEvent(e.time, clientName);
        return;
    }

    waitingClients.push(clientName);
}

void Club::handleClientLeft(const Event& e) {
    const std::string& clientName = e.args[0];
    if (!isValidClientName(clientName)) {
        throw std::invalid_argument("bad client name");
    }
    auto it = clients.find(clientName);

    if (it == clients.end() || !it->second.inClub) {
        addErrorEvent(e.time, consts::ErrorClientUnknown);
        return;
    }

    if (it->second.tableID) {
        uint32_t freedTableID = *it->second.tableID;
        freeTable(freedTableID, e.time);
        it->second.tableID.reset();

        if (!waitingClients.empty()) {
            std::string nextClient = waitingClients.front();
            waitingClients.pop();

            Table& t = tables[freedTableID - 1];
            t.occupied = true;
            t.client = nextClient;
            t.occupiedFrom = e.time;
            clients[nextClient].tableID = freedTableID;

            addClientSitEvent(e.time, nextClient, freedTableID);
        }
    }

    it->second.inClub = false;
}

void Club::closeDay() {
    std::vector<std::string> names;
    for (auto& [name, c] : clients) {
        if (c.inClub) names.push_back(name);
    }

    std::sort(names.begin(), names.end());

    for (const auto& name : names) {
        auto& client = clients[name];

        if (client.tableID) {
            freeTable(*client.tableID, closeTime);
            client.tableID.reset();
        }
        client.inClub = false;

        addClientLeftEvent(closeTime, name);
    }
}

void Club::printResult() const {
    std::cout << formatTime(openTime) << std::endl;

    for (const auto& e : events) {
        std::cout << e.rawLine << std::endl;
    }

    std::cout << formatTime(closeTime) << std::endl;
    for (const auto& t : tables) {
        std::cout << t.id << " " << t.revenue << " " << formatTime(t.totalMinutes) << std::endl;
    }
}

void Club::addErrorEvent(uint32_t time, const std::string& message) {
    std::ostringstream oss;
    oss << formatTime(time) << " " << consts::OutcomingEventError << " " << message;
    events.push_back({time, consts::OutcomingEventError, {message}, oss.str()});
}

void Club::addClientLeftEvent(uint32_t time, const std::string& client) {
    std::ostringstream oss;
    oss << formatTime(time) << " " << consts::OutcomingEventClientLeft << " " << client;
    events.push_back({time, consts::OutcomingEventClientLeft, {client}, oss.str()});
}

void Club::addClientSitEvent(uint32_t time, const std::string& client, uint32_t tableID) {
    std::ostringstream oss;
    oss << formatTime(time) << " " << consts::OutcomingEventClientSit << " " << client << " "
        << tableID;
    events.push_back(
        {time, consts::OutcomingEventClientSit, {client, std::to_string(tableID)}, oss.str()});
}

void Club::freeTable(uint32_t tableID, uint32_t endTime) {
    int id = tableID - 1;

    if (!tables[id].occupied) return;

    uint32_t minutes = endTime - *tables[id].occupiedFrom;
    tables[id].totalMinutes += minutes;

    uint32_t hours = std::ceil(minutes / 60.0);
    tables[id].revenue += hours * pricePerHour;

    tables[id].occupied = false;
    tables[id].occupiedFrom.reset();
    tables[id].client.clear();
}

bool Club::hasFreeTable() const {
    for (const auto& t : tables) {
        if (!t.occupied) return true;
    }
    return false;
}

bool Club::isValidClientName(const std::string& name) const {
    return !name.empty() && std::all_of(name.begin(), name.end(), [](char c) {
        return consts::ClientsNameAlphabet.find(c) != std::string::npos;
    });
}
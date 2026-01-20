#pragma once
#include <cstdint>
#include <string>

namespace consts {
inline const std::string ClientsNameAlphabet = "abcdefghijklmnopqrstuvwxyz0123456789_";
inline const std::string ErrorYouShallNotPass = "YouShallNotPass";
inline const std::string ErrorNotOpenYet = "NotOpenYet";
inline const std::string ErrorPlaceIsBusy = "PlaceIsBusy";
inline const std::string ErrorClientUnknown = "ClientUnknown";
inline const std::string ErrorICanWaitNoLonger = "ICanWaitNoLonger!";
inline constexpr uint32_t IncomingEventClientArrived = 1;
inline constexpr uint32_t IncomingEventClientSit = 2;
inline constexpr uint32_t IncomingEventClientWait = 3;
inline constexpr uint32_t IncomingEventClientLeft = 4;
inline constexpr uint32_t OutcomingEventClientLeft = 11;
inline constexpr uint32_t OutcomingEventClientSit = 12;
inline constexpr uint32_t OutcomingEventError = 13;
}  // namespace consts
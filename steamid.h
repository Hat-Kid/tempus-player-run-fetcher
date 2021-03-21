#pragma once

#include <string>
#include <cstdint>

#define STEAMID64_IDENTIFIER 76561197960265728

// When passed a corresponding SteamID type, converts to the other formats.
void ConvertId64(uint64_t steamId64, std::string &steamId3, std::string &steamId);
void ConvertId3(std::string steamId3, std::string &steamId, uint64_t &steamId64);
void ConvertId(std::string steamId, std::string &steamId3, uint64_t &steamId64);

//void ResolveVanityUrl(std::string vanityUrl, std::string &steamId3, std::string &steamId);
//std::string GetDisplayName(SteamId steamId);

struct SteamId
{
public:
    SteamId(uint64_t steamId64);
    SteamId(std::string steamId);
    //SteamId vanityUrl(std::string vanityUrl);

    uint64_t m_szSteamId64; // e.g. 12345678901234567
    std::string m_szSteamId3; // e.g. [U:1:12345678]
    std::string m_szSteamId; // e.g. STEAM_0:1:12345678
    //std::string m_szDisplayName;
    //std::string m_szVanityUrl;
};

void ConvertId64(uint64_t steamId64, std::string &steamId3, std::string &steamId)
{
    auto universe = (steamId64 >> 56) & 0xFF;
    auto accountIdLowBit = steamId64 & 1;
    auto accountIdHighBits = (steamId64) & 0x7FFFFFF;

    steamId = "STEAM_" + std::to_string(universe) + ":" + std::to_string(accountIdLowBit) + ":" + std::to_string(accountIdHighBits);
    steamId3 = "[U:1:" + std::to_string(steamId64 - STEAMID64_IDENTIFIER) + "]";
}

void ConvertId3(std::string steamId3, std::string &steamId, uint64_t &steamId64)
{
    int steamId3Split[3] = { atoi(steamId3.substr(0, 2).c_str()), atoi(steamId3.substr(3, 1).c_str()), atoi(steamId3.substr(5, 8).c_str()) };

    std::string uSteamId(steamId3.substr(5, 8));

    steamId64 = STEAMID64_IDENTIFIER + atoi(uSteamId.c_str());

    std::string parityBit = (steamId3Split[2] % 2) ? "0:" : "1:";

    steamId = "STEAM_0:" + parityBit + std::to_string(steamId3Split[2] / 2);
}

void ConvertId(std::string steamId, std::string &steamId3, uint64_t &steamId64)
{
    int steamIdSplit[3] = { atoi(steamId.substr(6, 1).c_str()), atoi(steamId.substr(8, 1).c_str()), atoi(steamId.substr(10, 8).c_str()) };

    steamId64 = steamIdSplit[2] * 2;

    if (steamIdSplit[1] == 1)
        steamId64 += 1;

    steamId64 += STEAMID64_IDENTIFIER;

    steamId3 = "[U:1:" + std::to_string(steamIdSplit[2] * 2 + steamIdSplit[1]) + "]";
}

SteamId::SteamId(uint64_t steamId64) : m_szSteamId64(steamId64)
{
    ConvertId64(m_szSteamId64, m_szSteamId3, m_szSteamId);
}

SteamId::SteamId(std::string steamId) : m_szSteamId(steamId)
{
    ConvertId(m_szSteamId, m_szSteamId3, m_szSteamId64);
}
#pragma once

#include <chrono>

float ConvertTicksToSeconds(int iTicks);
std::string ConvertMSToHHMMSSMS(std::chrono::milliseconds ms);
std::string ConvertToClassName(int iClass);

struct Run
{
    Run();
    Run(std::string name);
    Run(std::string name, int iClass);

    std::string m_szMapName;
    std::string m_szPlayerName;
    std::string m_szSteamId;
    int m_iRank;
    int m_iClass; // 3 -> Soldier, 4 -> Demoman
    int m_iStartTick;
    int m_iEndTick;
    int m_iDemoId;
    int m_iMapCompletions;
    float m_flDuration;
    bool m_bHasValidDemo;
    bool m_bIsValid;

    float GetRunLength();
    std::string GetDemoUrl();
};

Run::Run()
{
    m_szPlayerName = "";
    m_szMapName = "";
    m_szSteamId = "";
    m_iClass = 3;
    m_iRank = 0;
    m_iStartTick = 0;
    m_iEndTick = 0;
    m_iDemoId = 0;
    m_iMapCompletions = 0;
    m_flDuration = 0.0f;
    m_bHasValidDemo = false;
    m_bIsValid = false;
}

Run::Run(std::string name) : m_szMapName(name)
{
    m_szPlayerName = "";
    m_szSteamId = "";
    m_iClass = 3;
    m_iRank = 0;
    m_iStartTick = 0;
    m_iEndTick = 0;
    m_iDemoId = 0;
    m_iMapCompletions = 0;
    m_flDuration = 0.0f;
    m_bHasValidDemo = false;
    m_bIsValid = false;
}

Run::Run(std::string name, int iClass) : m_szMapName(name), m_iClass(iClass)
{
    m_szPlayerName = "";
    m_szSteamId = "";
    m_iRank = 0;
    m_iStartTick = 0;
    m_iEndTick = 0;
    m_iDemoId = 0;
    m_iMapCompletions = 0;
    m_flDuration = 0.0f;
    m_bHasValidDemo = false;
    m_bIsValid = false;
}

float Run::GetRunLength()
{
    if (m_bHasValidDemo)
    {
        return ConvertTicksToSeconds(m_iEndTick - m_iStartTick);
    }
    else
    {
        return m_flDuration;
    }
}

float ConvertTicksToSeconds(int iTicks)
{
    float tickInterval = 1.0f / 66.0f;
    return iTicks * tickInterval;
}

std::string ConvertMSToHHMMSSMS(std::chrono::milliseconds ms)
{
    using namespace std::chrono;
    std::stringstream ss;

    // compute h, m, s, ms
    auto secs = duration_cast<seconds>(ms);
    ms -= duration_cast<milliseconds>(secs);

    auto mins = duration_cast<minutes>(secs);
    secs -= duration_cast<seconds>(mins);

    auto hour = duration_cast<hours>(mins);
    mins -= duration_cast<minutes>(hour);

    std::string hr(std::to_string(hour.count()));
    std::string min(std::to_string(mins.count()));
    std::string s(std::to_string(secs.count()));
    std::string msecs(std::to_string(ms.count()));

    msecs = msecs.substr(0, 2);

    // add leading zero if needed
    std::string hh = std::string(4 - hr.length(), '0') + hr;
    std::string mm = std::string(2 - min.length(), '0') + min;
    std::string sec = std::string(2 - s.length(), '0') + s;
    std::string millisec = std::string(2 - msecs.length(), '0') + msecs;

    // return mm:ss if hh is 0000
    if (hh.compare("0000") != 0)
    {
        ss << hr << ":" << mm << ":" << sec << "." << millisec;
    }
    else
    {
        ss << mm << ":" << sec << "." << millisec;
    }

    return ss.str();
}

std::string ConvertToClassName(int iClass)
{
    std::string className;

    switch (iClass)
    {
        case 3:
            className = "Soldier";
            break;
        case 4:
            className = "Demoman";
            break;
        default:
            className = "error";
            break;
    }

    return className;
}

std::string Run::GetDemoUrl()
{
    std::string url("https://tempus.xyz/demos/");
    std::string finalUrl = url + std::to_string(m_iDemoId) + "/";

    return m_bHasValidDemo ? finalUrl : "no demo available";
}
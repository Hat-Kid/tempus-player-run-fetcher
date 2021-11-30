#include <iostream>
#include <tuple>
#include <fstream>
#include <string>

#include <curl/curl.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <Poco/JSON/Parser.h>

#include "include/json.hpp"
#include "include/inja.hpp"
#include "include/csv.h"

#include "run.h"
#include "steamid.h"

void FindAndReplace(std::string &data, std::string toSearch, std::string replaceStr)
{
    // Get the first occurrence
    size_t pos = data.find(toSearch);

    // Repeat until end is reached
    while (pos != std::string::npos)
    {
        // Replace this occurrence of Sub String
        data.replace(pos, toSearch.size(), replaceStr);
        // Get the next occurrence from the current position
        pos = data.find(toSearch, pos + replaceStr.size());
    }
}

std::string FetchMapData()
{
    std::string result;

    try
    {
        curlpp::Cleanup curlCleanup;
        std::ostringstream ostream;
        ostream << curlpp::options::Url("https://gist.githubusercontent.com/hexaflexahexagon/b4ccf1c5293e5416a74bf49eecdd2071/raw/90c385cadf145bb35608f810cc9c8c8fdc9ee9a0/tempusmaps.csv");
        result = ostream.str();
    }
    catch (curlpp::RuntimeError &e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (curlpp::LogicError &e)
    {
        std::cout << e.what() << std::endl;
    }

    return result;
}

std::tuple<Run, Run> FetchPlayerRunData(std::string mapName, SteamId playerId)
{
    Run soldierRun(mapName, 3);
    Run demoRun(mapName, 4);

    try
    {
        using namespace Poco::JSON;

        curlpp::Cleanup myCleanup;
        std::ostringstream jsonStream;
        Parser parser;
        Poco::Dynamic::Var parseResult;
        std::string resultStr;
        std::string runApiUrl = "https://tempus.xyz/api/maps/name/map_name/zones/typeindex/map/1/records/list?limit=10000";

        FindAndReplace(runApiUrl, "map_name", mapName);

        jsonStream << curlpp::options::Url(runApiUrl);

        resultStr = jsonStream.str();
        parseResult = parser.parse(resultStr);
        Object::Ptr body = parseResult.extract<Object::Ptr>();

        int completionsSoldier = body->getObject("completion_info")->getValue<int>("soldier");
        int completionsDemo = body->getObject("completion_info")->getValue<int>("demoman");

        if (completionsSoldier > 0)
        {
            Array::Ptr soldierRuns = body->getObject("results")->getArray("soldier");

            for (int i = 0; i < soldierRuns->size(); i++)
            {
                if (soldierRuns->getObject(i)->getValue<std::string>("steamid") == playerId.m_szSteamId)
                {
                    Object::Ptr demoinfo = soldierRuns->getObject(i)->getObject("demo_info");

                    if (demoinfo)
                    {
                        soldierRun.m_szPlayerName = soldierRuns->getObject(i)->getValue<std::string>("name");
                        soldierRun.m_iRank = i + 1;
                        soldierRun.m_iStartTick = demoinfo->getValue<int>("start_tick");
                        soldierRun.m_iEndTick = demoinfo->getValue<int>("end_tick");
                        soldierRun.m_iDemoId = demoinfo->getValue<int>("id");
                        soldierRun.m_flDuration = ConvertTicksToSeconds(soldierRun.m_iEndTick - soldierRun.m_iStartTick);
                        soldierRun.m_iMapCompletions = completionsSoldier;
                        soldierRun.m_bHasValidDemo = true;
                        soldierRun.m_bIsValid = true;
                    }
                    else
                    {
                        soldierRun.m_szPlayerName = soldierRuns->getObject(i)->getValue<std::string>("name");
                        soldierRun.m_iRank = i + 1;
                        soldierRun.m_bHasValidDemo = false;
                        soldierRun.m_iMapCompletions = completionsSoldier;
                        soldierRun.m_flDuration = soldierRuns->getObject(i)->getValue<float>("duration");
                        soldierRun.m_bIsValid = true;
                    }

                    std::cout << "Found Soldier run for " << soldierRun.m_szPlayerName << " on " << soldierRun.m_szMapName << "!\n";
                }
            }
            if (!soldierRun.m_bIsValid)
            {
                std::cout << "No Soldier run found for " <<  playerId.m_szSteamId << " on " << soldierRun.m_szMapName << "!\n";
            }
        }
        else // Special case for t0 maps.
        {
            soldierRun.m_szPlayerName = "null";
            soldierRun.m_iRank = 0;
            soldierRun.m_iStartTick = 0;
            soldierRun.m_iEndTick = 0;
            soldierRun.m_iDemoId = 0;
            soldierRun.m_flDuration = 0.0f;
            soldierRun.m_iMapCompletions = 0;
            soldierRun.m_bHasValidDemo = false;
            soldierRun.m_bIsValid = false;
        }

        Array::Ptr demoRuns = body->getObject("results")->getArray("demoman");

        for (int i = 0; i < demoRuns->size(); i++)
        {
            if (demoRuns->getObject(i)->getValue<std::string>("steamid") == playerId.m_szSteamId)
            {
                Object::Ptr demoinfo = demoRuns->getObject(i)->getObject("demo_info");

                if (demoinfo)
                {
                    demoRun.m_szPlayerName = demoRuns->getObject(i)->getValue<std::string>("name");
                    demoRun.m_iRank = i + 1;
                    demoRun.m_iStartTick = demoinfo->getValue<int>("start_tick");
                    demoRun.m_iEndTick = demoinfo->getValue<int>("end_tick");
                    demoRun.m_iDemoId = demoinfo->getValue<int>("id");
                    demoRun.m_flDuration = ConvertTicksToSeconds(demoRun.m_iEndTick - demoRun.m_iStartTick);
                    demoRun.m_iMapCompletions = completionsDemo;
                    demoRun.m_bHasValidDemo = true;
                    demoRun.m_bIsValid = true;
                }
                else
                {
                    demoRun.m_szPlayerName = demoRuns->getObject(i)->getValue<std::string>("name");
                    demoRun.m_bHasValidDemo = false;
                    demoRun.m_iRank = i + 1;
                    demoRun.m_iMapCompletions = completionsDemo;
                    demoRun.m_flDuration = demoRuns->getObject(i)->getValue<float>("duration");
                    demoRun.m_bIsValid = true;
                }

                std::cout << "Found Demo run for " << demoRun.m_szPlayerName << " on " << demoRun.m_szMapName << "!\n\n";
            }
        }
        if (!demoRun.m_bIsValid)
        {
            std::cout << "No Demo run found for " << playerId.m_szSteamId << " on " << demoRun.m_szMapName << "!\n\n";
        }
    }
    catch (curlpp::RuntimeError &e)
    {
        std::cout << e.what() << std::endl;
    }
    catch(curlpp::LogicError &e)
    {
        std::cout << e.what() << std::endl;
    }

    return {soldierRun, demoRun};
}

std::vector<Run> CreateRunList(SteamId playerId)
{
    std::string fileIn = FetchMapData();

    std::ofstream out("maps.csv");
    out << fileIn;
    out.close();

    io::CSVReader<1> in("maps.csv");
    std::string className, mapName;

    std::vector<Run> runList = {};

    while (in.read_row(mapName))
    {
        std::tuple<Run, Run> runTuple = FetchPlayerRunData(mapName, playerId);

        if (std::get<0>(runTuple).m_bIsValid)
        {
            runList.push_back(std::get<0>(runTuple));
        }
        if (std::get<1>(runTuple).m_bIsValid)
        {
            runList.push_back(std::get<1>(runTuple));
        }
    }

    return runList;
}

void ParseResults(SteamId playerId)
{
    using namespace inja;
    using json = nlohmann::json;

    json data;
    Environment env;
    Template temp = env.parse_template("template.txt");
    std::string result;

    std::vector<Run> runList = CreateRunList(playerId);
    std::vector<Run> soldierRuns = {};
    std::vector<Run> demoRuns = {};

    std::sort(runList.begin(), runList.end(),
              [](const Run &run, const Run &run1)
              {
                  return (run.m_iClass < run1.m_iClass);
              });

    int soldier = 3;
    int demo = 4;

    auto soldierRunCount = std::count_if(runList.begin(), runList.end(), [&soldier](const Run& run) { return run.m_iClass == soldier; });
    auto demoRunCount = std::count_if(runList.begin(), runList.end(), [&demo](const Run& run) { return run.m_iClass == demo; });

    for (int i = 0; i < soldierRunCount; i++)
    {
        soldierRuns.push_back(runList.at(i));

        std::sort(soldierRuns.begin(), soldierRuns.end(),
                  [](const Run& run, const Run& run1)
                  {
                      if (run.m_bHasValidDemo && run1.m_bHasValidDemo)
                      {
                          return (run.m_iEndTick - run.m_iStartTick) < (run1.m_iEndTick - run1.m_iStartTick);
                      }
                      else
                      {
                          return run.m_flDuration < run1.m_flDuration;
                      }
                  });
    }

    for (int i = soldierRunCount; i < soldierRunCount + demoRunCount; i++)
    {
        demoRuns.push_back(runList.at(i));

        std::sort(demoRuns.begin(), demoRuns.end(),
                  [](const Run& run, const Run& run1)
                  {
                      if (run.m_bHasValidDemo && run1.m_bHasValidDemo)
                      {
                          return (run.m_iEndTick - run.m_iStartTick) < (run1.m_iEndTick - run1.m_iStartTick);
                      }
                      else
                      {
                          return run.m_flDuration < run1.m_flDuration;
                      }
                  });
    }

    std::chrono::milliseconds cumulativeSoldierRunLength(0);
    std::chrono::milliseconds cumulativeDemoRunLength(0);
    std::chrono::milliseconds cumulativeRunLength(0);

    for (int i = 0; i < soldierRuns.size(); i++)
    {
        if (soldierRuns.at(i).m_szPlayerName == "null")
        {
            continue;
        }

        std::chrono::milliseconds runLength(static_cast<int>(soldierRuns.at(i).GetRunLength() * 1000));

        cumulativeSoldierRunLength += runLength;

        data["mapName"] = soldierRuns.at(i).m_szMapName;
        data["runLength"] = ConvertMSToHHMMSSMS(runLength);
        data["playerName"] = soldierRuns.at(i).m_szPlayerName;
        data["className"] = ConvertToClassName(soldierRuns.at(i).m_iClass);
        data["mapCompletions"] = soldierRuns.at(i).m_iMapCompletions;
        data["demoUrl"] = soldierRuns.at(i).GetDemoUrl();
        data["playerRank"] = soldierRuns.at(i).m_iRank;

        std::cout << "Parsing " << soldierRuns.at(i).m_szMapName << "..." << "\n";

        result += env.render(temp, data) + "\n";

        if (i == soldierRuns.size() - 1)
        {
            result += "\n";
        }
    }

    for (int i = 0; i < demoRuns.size(); i++)
    {
        if (demoRuns.at(i).m_szPlayerName == "null")
        {
            continue;
        }

        std::chrono::milliseconds runLength(static_cast<int>(demoRuns.at(i).GetRunLength() * 1000));

        cumulativeDemoRunLength += runLength;

        data["mapName"] = demoRuns.at(i).m_szMapName;
        data["runLength"] = ConvertMSToHHMMSSMS(runLength);
        data["playerName"] = demoRuns.at(i).m_szPlayerName;
        data["className"] = ConvertToClassName(demoRuns.at(i).m_iClass);
        data["mapCompletions"] = demoRuns.at(i).m_iMapCompletions;
        data["demoUrl"] = demoRuns.at(i).GetDemoUrl();
        data["playerRank"] = demoRuns.at(i).m_iRank;

        std::cout << "Parsing " << demoRuns.at(i).m_szMapName << "..." << "\n";

        result += env.render(temp, data) + "\n";
    }

    cumulativeRunLength = cumulativeSoldierRunLength + cumulativeDemoRunLength;

    result += "\n\n" + std::string("Total runs: ") + std::to_string(runList.size()) +
                       std::string(" (") + std::to_string(soldierRunCount - 1) + " Soldier, " + std::to_string(demoRunCount) + " Demo)";

    result += "\n\n" + std::string("Cumulative run length: ") + "\n"
           + std::string("Soldier: ") + ConvertMSToHHMMSSMS(cumulativeSoldierRunLength) + "\n"
           + std::string("Demoman: ") + ConvertMSToHHMMSSMS(cumulativeDemoRunLength) + "\n"
           + std::string("Total: ") + ConvertMSToHHMMSSMS(cumulativeRunLength);

    std::string fileName("runs.txt");

    std::ofstream out(fileName);
    out << result;
    out.close();
}

int main(int, char **argv)
{
    SetConsoleTitle(L"Tempus Player Run Fetcher");

    std::cout << "Enter the SteamID (format: STEAM_X:Y:Z) of the player whose runs you would like to fetch: ";

    std::string id;
    std::cin >> id;
    std::cout << "\n";

    SteamId playerId(id);

    ParseResults(playerId.m_szSteamId);

    return 0;
}
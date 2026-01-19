#pragma once

#include <vector>

#include <pugixml/pugixml.hpp>

#include "Time.hpp"

namespace statistic
{

struct GameStatistic
{
public:
    std::string stringID;
    std::string name;

    Time time{};
    unsigned int record{};
    unsigned int gameCount{};

public:
    bool loadFromStatistic(const pugi::xml_node statisticNode)
    {
        if (!statisticNode)
            return false;
        record = statisticNode.attribute("record").as_uint();
        gameCount = statisticNode.attribute("countGames").as_uint();
        std::string timeStr = statisticNode.attribute("bestTime").as_string();
        if (timeStr.size() != 5)
            return false;
        time.minuts = (timeStr[0] - '0') * 10 + (timeStr[1] - '0');
        time.seconds = (timeStr[3] - '0') * 10 + (timeStr[4] - '0');
        return time.minuts < 60 && time.seconds < 60;
    }
};

struct AllGameStatistic
{
public:
    std::vector<GameStatistic> gameStatistic;

public:
    GameStatistic *findById(const std::string_view id)
    {
        for (auto &gs : gameStatistic)
            if (gs.stringID == id)
                return &gs;
        return nullptr;
    }

    const GameStatistic *findById(const std::string_view id) const
    {
        for (const auto &gs : gameStatistic)
            if (gs.stringID == id)
                return &gs;
        return nullptr;
    }

    // Обновляет глобальную статистику результатом одной игры.
    // - минимальное время (bestTime) обновляется если `result.time` лучше
    // - рекорд очков обновляется если `result.record` больше
    // - число игр увеличивается на 1
    bool applyGameResult(const std::string_view id, const GameStatistic &result)
    {
        GameStatistic *gs = findById(id);
        if (!gs)
            return false;

        if (result.record > gs->record)
        {
            gs->time = result.time;
            gs->record = result.record;
        }
        gs->gameCount += 1;
        return true;
    }
};

} // namespace statistic

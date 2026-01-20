#pragma once

#include <vector>

#include <pugixml/pugixml.hpp>

#include "GameStatistic.hpp"

namespace statistic
{
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
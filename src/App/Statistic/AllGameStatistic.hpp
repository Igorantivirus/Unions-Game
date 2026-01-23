#pragma once

#include <vector>

#include <pugixml/pugixml.hpp>

#include "GameStatistic.hpp"

namespace statistic
{
struct AllGameStatistic
{
public:

    void resetAllStatistic()
    {
        for(auto& stat : gameStatistic_)
            stat.reset();
    }

    GameStatistic *findById(const std::string_view id)
    {
        for (auto &gs : gameStatistic_)
            if (gs.stringID == id)
                return &gs;
        return nullptr;
    }

    const GameStatistic *findById(const std::string_view id) const
    {
        for (const auto &gs : gameStatistic_)
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

        if (result.gameCount > gs->record)
        {
            gs->time = result.time;
            gs->record = result.gameCount;
        }
        gs->gameCount += 1;
        return true;
    }

    void clear()
    {
        gameStatistic_.clear();
    }

    bool empty() const
    {
        return gameStatistic_.empty();
    }

    void addGame(GameStatistic stat)
    {
        gameStatistic_.push_back(std::move(stat));
    }

    const std::vector<GameStatistic>& getAll() const
    {
        return gameStatistic_;
    }

private:
    std::vector<GameStatistic> gameStatistic_;
};
} // namespace statistic
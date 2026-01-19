#pragma once

#include "Core/BaseFolder.hpp"
#include <App/Statistic/GameStatistic.hpp>
#include <App/Statistic/GameStatisticReader.hpp>

#include <SDL3/SDL_log.h>

#include <filesystem>
#include <string>

namespace app
{

class AppState
{
public:
    explicit AppState(std::filesystem::path statPath = assets / "stat.xml")
        : statPath_(std::move(statPath))
    {
    }

    bool load()
    {
        return statistic::reader::readAllGameStatistic(stat_, statPath_.string());
    }

    bool save() const
    {
        return statistic::reader::writeAllGameStatistic(stat_, statPath_.string());
    }

    statistic::AllGameStatistic &stat()
    {
        return stat_;
    }

    const statistic::AllGameStatistic &stat() const
    {
        return stat_;
    }

    const std::string statPath() const
    {
        return statPath_.string();
    }

    const std::string& getCurrentPackageName() const
    {
        return currentPackageName_;
    }

private:
    std::filesystem::path statPath_;
    statistic::AllGameStatistic stat_{};
    std::string currentPackageName_ = "coins";
};

} // namespace app


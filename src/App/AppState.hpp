#pragma once

#include <SDLWrapper/FileWorker.hpp>
#include <App/Statistic/GameStatistic.hpp>
#include <App/IO/GameStatisticIO.hpp>

#include <SDL3/SDL_log.h>

#include <filesystem>
#include <string>

namespace app
{

class AppState
{
public:
    explicit AppState(const std::filesystem::path& workStatFile, const std::filesystem::path& assetsStatFile)
        : workStatFile_(workStatFile), assetsStatFile_(assetsStatFile)
    {
        if(!load())
            SDL_Log("Failed to load appState");
    }

    bool load()
    {
        if (!std::filesystem::exists(workStatFile_))
            IO::createAndMove(assetsStatFile_, workStatFile_);
        return IO::readAllGameStatistic(stat_, currentPackageName_, workStatFile_);
    }

    bool save() const
    {
        return IO::writeAllGameStatistic(stat_, currentPackageName_, workStatFile_);
    }

    statistic::AllGameStatistic &stat()
    {
        return stat_;
    }

    const statistic::AllGameStatistic &stat() const
    {
        return stat_;
    }

    const std::string &getCurrentPackageName() const
    {
        return currentPackageName_;
    }

    void setCurrentPackageName(std::string name)
    {
        if (!name.empty())
            currentPackageName_ = std::move(name);
    }

private:
    std::filesystem::path workStatFile_;
    std::filesystem::path assetsStatFile_;
    statistic::AllGameStatistic stat_{};
    std::string currentPackageName_;
};

using AppStatePtr = std::shared_ptr<AppState>;

} // namespace app

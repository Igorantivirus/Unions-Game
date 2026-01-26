#pragma once

#include "IO/FullFileWorker.hpp"
#include <App/IO/GameStatisticIO.hpp>
#include <Core/Managers/TextureManager.hpp>
#include <App/Statistic/GameStatistic.hpp>
#include <SDLWrapper/FileWorker.hpp>

#include <SDL3/SDL_log.h>

#include <filesystem>
#include <string>

namespace app
{

class AppState
{
public:
    void setWorkStatisticFile(const std::filesystem::path &workStatFile)
    {
        workStatFile_ = workStatFile;
    }
    void setAssetsStatisticFile(const std::filesystem::path &assetsStatFile)
    {
        assetsStatFile_ = assetsStatFile;
    }

    bool load()
    {
        if(!IO::isValidXmlFile(workStatFile_))
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

    core::managers::TextureManager &textures()
    {
        return textures_;
    }
    const core::managers::TextureManager &textures() const
    {
        return textures_;
    }

private:
    std::filesystem::path workStatFile_;
    std::filesystem::path assetsStatFile_;
    statistic::AllGameStatistic stat_{};
    std::string currentPackageName_;
    core::managers::TextureManager textures_;
};

using AppStatePtr = std::shared_ptr<AppState>;

} // namespace app

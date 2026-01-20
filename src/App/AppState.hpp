#pragma once

#include <App/IO/PathMeneger.hpp>
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
    explicit AppState(const std::string& statPath)
        : statPath_(statPath), workFolderStatPath_(IO::PathManager::workFolder() / statPath)
    {
        if(!load())
            SDL_Log("Failed to load appState");
    }


    void setStrPath(const std::string& statPath)
    {
        statPath_ = statPath;
        workFolderStatPath_ = core::PathManager::workFolder() / statPath;
        SDL_Log("%s\n%s\n", statPath_.string().c_str(), workFolderStatPath_.string().c_str());
    }
    bool load()
    {
        if (!std::filesystem::exists(workFolderStatPath_))
            createStatFile();
        return IO::readAllGameStatistic(stat_, workFolderStatPath_);
    }

    bool save() const
    {
        return IO::writeAllGameStatistic(stat_, workFolderStatPath_);
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
    std::filesystem::path statPath_;
    std::filesystem::path workFolderStatPath_;
    statistic::AllGameStatistic stat_{};
    std::string currentPackageName_ = "coins";

private:
    void createStatFile()
    {
        sdl3::FileWorker file;
        if (!file.open(core::PathManager::assets() / statPath_, sdl3::FileWorkerMode::read | sdl3::FileWorkerMode::binary))
        {
            SDL_Log("Error of open stat file from assets\n");
            return;
        }
        std::string data = file.readAll();
        file.close();
        if (!file.open(workFolderStatPath_, sdl3::FileWorkerMode::write | sdl3::FileWorkerMode::binary))
        {
            SDL_Log("Polnoy govno\n");
            return;
        }
        file.write(data);
        file.close();
    }
};

using AppStatePtr = std::shared_ptr<AppState>;

} // namespace app

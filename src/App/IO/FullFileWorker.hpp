#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include <SDL3/SDL_log.h>
#include <SDLWrapper/FileWorker.hpp>

namespace IO
{

inline std::string readAllFile(const std::filesystem::path &path)
{
    sdl3::FileWorker file;
    if (!file.open(path, sdl3::FileWorkerMode::read | sdl3::FileWorkerMode::binary))
    {
        SDL_Log("Error of open file %s\n", path.string().c_str());
        return {};
    }
    return file.readAll();
}
inline std::string readAllFile(const std::string_view path)
{
    sdl3::FileWorker file;
    if (!file.open(path, sdl3::FileWorkerMode::read | sdl3::FileWorkerMode::binary))
    {
        SDL_Log("Error of open file %s\n", path.data());
        return {};
    }
    return file.readAll();
}

inline bool writeAllFile(const std::filesystem::path &path, const std::string &str)
{
    sdl3::FileWorker file;
    if (!file.open(path, sdl3::FileWorkerMode::write | sdl3::FileWorkerMode::binary))
    {
        SDL_Log("Error of open file %s\n", path.string().c_str());
        return false;
    }
    return file.write(str);
}

inline bool writeAllFile(const std::string_view &path, const std::string &str)
{
    sdl3::FileWorker file;
    if (!file.open(path, sdl3::FileWorkerMode::write | sdl3::FileWorkerMode::binary))
    {
        SDL_Log("Error of open file %s\n", path.data());
        return false;
    }
    return file.write(str);
}

inline bool createAndMove(const std::filesystem::path &existing, const std::filesystem::path &newFile)
{
    return writeAllFile(newFile, readAllFile(existing));
}

} // namespace IO
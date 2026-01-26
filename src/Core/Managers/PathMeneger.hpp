#pragma once 

#include <filesystem>

#include <SDL3/SDL_filesystem.h>

namespace core::managers
{

// PathManager.hpp - централизованное управление путями
class PathManager
{
public:
    static void init()
    {
        #ifdef ANDROID
            assets_ = "";
            workFolder_ = SDL_GetPrefPath("igorantivirus", "unions");
        #else
            #ifdef DEBUG_BUILD_TYPE
                assets_ = "../assets";
            #else
                assets_ = "assets";
            #endif
            workFolder_ = "";
        #endif
    }

    static const std::filesystem::path &assets()
    {
        return assets_;
    }

    static const std::filesystem::path &workFolder()
    {
        return workFolder_;
    }

    static const std::string inAssets(const std::string_view path)
    {
        return (assets_ / path).string();
    }

    static const std::string inWorkFolder(const std::string_view path)
    {
        return (workFolder_ / path).string();
    }

private:
    inline static std::filesystem::path assets_;
    inline static std::filesystem::path workFolder_;
};

} // namespace code
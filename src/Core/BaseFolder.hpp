#pragma once

#include <SDL3/SDL_filesystem.h>
#include <filesystem>

inline static std::filesystem::path assets;
inline static std::filesystem::path workFolder;

static void initWolders()
{
#ifdef ANDROID

    assets = "./";
    workFolder = SDL_GetPrefPath("igorantivirus", "new_sdl");

#else
    assets = "assets";
    workFolder = "";
#endif
}
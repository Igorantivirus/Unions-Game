#pragma once

#include <SDL3/SDL_filesystem.h>
#include <filesystem>

inline static std::filesystem::path assets;

// #ifdef ANDROID
//     "./"
// #else
//     "assets"
// #endif
//     ;

inline static std::filesystem::path workFolder;
// #ifdef ANDROID
//     ""
// #else
//     "./"
// #endif
//     ;

static void initWolders()
{
#ifdef ANDROID

    assets = "./";
    workFolder = SDL_GetPrefPath("igorantivirus", "new_sdl");

#else
    assets = "../assets";
    workFolder = "./";
#endif
}
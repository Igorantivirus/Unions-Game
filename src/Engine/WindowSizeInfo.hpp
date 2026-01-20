#pragma once

#include <SDLWrapper/Names.hpp>
#include <algorithm>

namespace engine
{
struct WindowSizeInfo
{
    sdl3::Vector2i baseLandscapeLogicalSize;
    sdl3::Vector2i basePortraitLogicalSize;
    sdl3::Vector2i windowLogicslSize;
    sdl3::Vector2f centerPos;

    void init(const sdl3::Vector2i size)
    {
        baseLandscapeLogicalSize = {std::max(size.x, size.y), std::min(size.x, size.y)};
        basePortraitLogicalSize = {baseLandscapeLogicalSize.y, baseLandscapeLogicalSize.x};
        windowLogicslSize = (size.x >= size.y) ? baseLandscapeLogicalSize : basePortraitLogicalSize;

        centerPos.x = windowLogicslSize.x / 2.f;
        centerPos.y = windowLogicslSize.y / 2.f;
    }

    // true if size changed
    bool handleWindowResize(const int windowW, const int windowH)
    {
        const bool isLandscape = windowW >= windowH;
        const sdl3::Vector2i desired = isLandscape ? baseLandscapeLogicalSize : basePortraitLogicalSize;

        if (desired.x == windowLogicslSize.x && desired.y == windowLogicslSize.y)
            return false;
        windowLogicslSize = desired;
        return true;
    }
};
} // namespace engine
#pragma once

#include <cstdint>
#include <SDL3/SDL_events.h>

enum AppEventsType : std::uint32_t
{
    BEGIN_INVALID_TYPE = SDL_EVENT_USER,
    COLLISION
};
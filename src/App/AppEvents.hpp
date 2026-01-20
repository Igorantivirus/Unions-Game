#pragma once

#include <SDL3/SDL_events.h>
#include <cstdint>

namespace app
{

enum AppEventsType : std::uint32_t
{
    BEGIN_INVALID_TYPE = SDL_EVENT_USER,
    COLLISION
};

}
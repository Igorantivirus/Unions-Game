#pragma once

#include <stdexcept>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_timer.h>

namespace core
{

template <typename T>
concept PtrSizeable = sizeof(T) <= 8;
template <typename T>
concept Int32Sizeable = sizeof(T) == sizeof(int);

template <Int32Sizeable T, PtrSizeable T1, PtrSizeable T2>
void createEventCorrect(T eventType, int code, T1 data1 = nullptr, T2 data2 = nullptr)
{
    if (static_cast<int>(eventType) < SDL_EVENT_USER)
    {
        throw std::invalid_argument("Event type must be >= SDL_EVENT_USER");
    }

    SDL_Event event;
    SDL_zero(event);

    event.type = static_cast<int>(eventType); // Устанавливаем ТОЛЬКО это!
    event.user.timestamp = SDL_GetTicks();
    event.user.code = code;
    event.user.data1 = reinterpret_cast<void *>(data1);
    event.user.data2 = reinterpret_cast<void *>(data2);

    SDL_PushEvent(&event);
}

} // namespace core
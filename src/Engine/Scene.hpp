#pragma once

#include <SDLWrapper/Renders/RenderWindow.hpp>
#include <memory>

#include <SDL3/SDL_events.h>
#include <SDLWrapper/SDLWrapper.hpp>

#include "SceneAction.hpp"

namespace engine
{

class Scene
{
public:
    virtual ~Scene() = default;

    virtual void updateEvent(const SDL_Event &event) = 0;
    virtual SceneAction update(const float dt) = 0;

    virtual void draw(sdl3::RenderWindow &window) const = 0;

protected:

    SceneAction actionRes_ = SceneAction::noneAction(); 

};

using ScenePtr = std::unique_ptr<Scene>;

} // namespace engine
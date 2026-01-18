#pragma once

#include <memory>

#include <RmlUi/Core/ElementDocument.h>
#include <SDLWrapper/SDLWrapper.hpp>

#include "SceneAction.hpp"

namespace engine
{

class Scene
{
public:
    Scene() = default;
    Scene(const Scene &) = delete;
    Scene &operator=(const Scene &) = delete;
    Scene(Scene &&) = delete;
    Scene &operator=(Scene &&) = delete;

    virtual ~Scene() = default;

    virtual void updateEvent(const SDL_Event &event) = 0;
    virtual SceneAction &update(const float dt)
    {
        return actionRes_;
    }

    virtual void draw(sdl3::RenderWindow &window) const = 0;

    virtual void hide()
    {
    }
    virtual void show()
    {
    }

protected:
    SceneAction actionRes_ = SceneAction::noneAction();
};

using ScenePtr = std::unique_ptr<Scene>;

} // namespace engine
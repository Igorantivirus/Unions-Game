#pragma once

#include <SDLWrapper/Renders/RenderWindow.hpp>
#include <memory>

#include "Engine/AdvancedContext.hpp"
#include "Scene.hpp"
#include "AdvancedContext.hpp"

namespace engine
{

class SceneFabrick
{
public:
    virtual ~SceneFabrick() = default;

    virtual ScenePtr genSceneByID(const IDType id) const = 0;

    void setContext(Context& context)
    {
        context_ = &context;
    }

    Context& getContext() const
    {
        return *context_;
    }

    void setRenderWindow(sdl3::RenderWindow& window)
    {
        window_ = &window;
    }

    sdl3::RenderWindow& getRenderWindow() const
    {
        return *window_;
    }

protected:

    Context* context_;
    sdl3::RenderWindow* window_;

};

using SceneFabrickPtr = std::unique_ptr<SceneFabrick>;

} // namespace engine
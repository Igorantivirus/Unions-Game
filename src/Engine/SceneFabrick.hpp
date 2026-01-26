#pragma once

#include <SDLWrapper/Renders/RenderWindow.hpp>
#include <memory>

#include "Core/Audio/AudioDevice.hpp"
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

    void setAudioDevice(audio::AudioDevice& audio)
    {
        audio_ = &audio;
    }

    audio::AudioDevice& getAudioDevice() const
    {
        return *audio_;
    }

protected:

    Context* context_;
    sdl3::RenderWindow* window_;
    audio::AudioDevice* audio_;

};

using SceneFabrickPtr = std::unique_ptr<SceneFabrick>;

} // namespace engine
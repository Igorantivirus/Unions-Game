#pragma once

#include <SDL3/SDL_timer.h>
#include <SDLWrapper/Renders/VideoMode.hpp>
#include <vector>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>

#include <SDLWrapper/Clock.hpp>
#include <SDLWrapper/SDLWrapper.hpp>

#include "Scene.hpp"
#include "SceneAction.hpp"
#include "SceneFabrick.hpp"

namespace engine
{

class Engine
{
public:
    SDL_AppResult start(const std::string_view wName, const sdl3::Vector2i size)
    {
        sdl3::VideoMode mode = sdl3::VideoMode::getDefaultVideoMode();
        mode.width = size.x;
        mode.height = size.y;
        window_.create(wName, mode);
        window_.setPhysicalWindowSize(size);
        cl_.start();
        return SDL_APP_CONTINUE;
    }
    void close()
    {
        window_.close();
        scenes_.clear();
    }

    void registrateSceneFabrick(SceneFabrickPtr ptr)
    {
        sceneFabrick_ = std::move(ptr);
    }

    void pushScene(const IDType sceneId)
    {
        scenes_.push_back(std::move(sceneFabrick_->genSceneByID(sceneId)));
    }

    SDL_AppResult updateEvents(const SDL_Event &event)
    {
        if (event.type == SDL_EVENT_QUIT)
            return SDL_APP_SUCCESS;
        if (scenes_.empty())
            return SDL_APP_FAILURE;
        scenes_.back()->updateEvent(event);
        return SDL_APP_CONTINUE;
    }

    SDL_AppResult iterate()
    {
        if (scenes_.empty())
            return SDL_APP_FAILURE;
        const float dt = cl_.elapsedTimeS();
        SceneAction act = scenes_.back()->update(dt);
        SDL_AppResult res = processSceneAction(act);
        if (res != SDL_APP_CONTINUE)
            return res;
        safeDrawScene();
        cl_.start();
        return res;
    }

    sdl3::RenderWindow &getWindow()
    {
        return window_;
    }

    void setFps(const unsigned fps)
    {
        fps_ = fps;
        if (fps_ > 0)
            desiredFrameMS_ = 1000.f / static_cast<float>(fps_);
    }

private:
    std::vector<ScenePtr> scenes_;
    SceneFabrickPtr sceneFabrick_;

    unsigned int fps_{};
    // желаемое время кадра (мс)
    float desiredFrameMS_{};

private:
    sdl3::RenderWindow window_;
    sdl3::ClockNS cl_;

private:
    SDL_AppResult processSceneAction(const SceneAction &act)
    {
        if (act.type == SceneActionType::None)
            return SDL_APP_CONTINUE;
        else if (act.type == SceneActionType::PushScene)
            scenes_.push_back(std::move(sceneFabrick_->genSceneByID(std::get<IDType>(act.value))));
        else if (act.type == SceneActionType::PopScene)
            scenes_.pop_back();
        else if (act.type == SceneActionType::SwitchScene)
        {
            scenes_.pop_back();
            scenes_.push_back(std::move(sceneFabrick_->genSceneByID(std::get<IDType>(act.value))));
        }
        else if (act.type == SceneActionType::Exit)
            return SDL_APP_SUCCESS;
        return SDL_APP_CONTINUE;
    }

    void safeDrawScene()
    {
        window_.clear();
        scenes_.back()->draw(window_);
        window_.display();
    }

    void fpsDelay()
    {
        if (fps_ == 0)
            return;
        
        // фактическое время, затраченное на кадр (мс)
        float frameMS = cl_.elapsedTimeMS();
        
        if (frameMS < desiredFrameMS_)
            SDL_Delay(static_cast<Uint32>(desiredFrameMS_ - frameMS));
    }
};

} // namespace engine
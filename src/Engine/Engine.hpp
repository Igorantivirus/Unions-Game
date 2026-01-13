#pragma once

#include <SDL3/SDL_init.h>
#include <SDLWrapper/Clock.hpp>
#include <SDLWrapper/Renders/VideoMode.hpp>
#include <vector>

#include <SDLWrapper/SDLWrapper.hpp>

#include "Scene.hpp"
#include "SceneFabrick.hpp"
#include "SceneAction.hpp"

namespace engine
{

class Engine
{
public:
    SDL_AppResult start(const std::string_view wName)
    {
        window_.create(wName, sdl3::VideoMode::getDefaultVideoMode());
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
        if (!scenes_.empty())
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
        window_.clear();
        scenes_.back()->draw(window_);
        window_.display();
        cl_.start();
        return res;
    }

    sdl3::RenderWindow &getWindow()
    {
        return window_;
    }

private:
    std::vector<ScenePtr> scenes_;
    SceneFabrickPtr sceneFabrick_;

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
};

} // namespace engine
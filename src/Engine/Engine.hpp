#pragma once

#include <SDL3/SDL_timer.h>
#include <SDLWrapper/Renders/VideoMode.hpp>
#include <string_view>
#include <vector>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>

#include <SDLWrapper/Clock.hpp>
#include <SDLWrapper/SDLWrapper.hpp>

#include "AdvancedContext.hpp"
#include "Core/Types.hpp"
#include "Scene.hpp"
#include "SceneAction.hpp"
#include "SceneFabrick.hpp"

namespace engine
{

class Engine
{
public:
    SDL_AppResult start(const std::string_view wName, const std::string_view fontPath, const sdl3::Vector2i size)
    {
        sdl3::VideoMode mode = sdl3::VideoMode::getDefaultVideoMode();
        mode.width = size.x;
        mode.height = size.y;
        if (!window_.create(wName, mode))
            return SDL_APP_FAILURE;
        window_.setLogicalPresentation(size);

        if (!context_.init(window_.getNativeSDLWindow(), window_.getNativeSDLRenderer(), fontPath))
            return SDL_APP_FAILURE;

        cl_.start();
        return SDL_APP_CONTINUE;
    }
    void close()
    {
        scenes_.clear();
        context_.quit();
        window_.close();
    }

    void registrateSceneFabrick(SceneFabrickPtr ptr)
    {
        sceneFabrick_ = std::move(ptr);
        sceneFabrick_->setContext(context_);
    }

    void pushScene(const IDType sceneId)
    {
        if(!scenes_.empty())
            scenes_.back()->hide();
        scenes_.push_back(std::move(sceneFabrick_->genSceneByID(sceneId)));
        scenes_.back()->show();
    }

    void popScene()
    {
        if(scenes_.empty())
            return;
        scenes_.back()->hide();
        scenes_.pop_back();
        if(!scenes_.empty())
            scenes_.back()->show();
    }

    void switchScene(const IDType sceneId)
    {
        if(scenes_.empty())
            return;
        scenes_.back()->hide();
        scenes_.pop_back();
        pushScene(sceneId);
    }

    SDL_AppResult updateEvents(SDL_Event &event)
    {
        if (event.type == SDL_EVENT_QUIT)
            return SDL_APP_SUCCESS;
        if (scenes_.empty())
            return SDL_APP_FAILURE;
        window_.convertEventToRenderCoordinates(&event);
        context_.updateEvents(event);
        scenes_.back()->updateEvent(event);
        return SDL_APP_CONTINUE;
    }

    SDL_AppResult iterate()
    {
        if (scenes_.empty())
            return SDL_APP_FAILURE;
        const float dt = cl_.elapsedTimeS();
        SceneAction& act = scenes_.back()->update(dt);
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

    void setFps(const unsigned int fps)
    {
        fps_ = fps;
        if (fps_ > 0)
            desiredFrameMS_ = 1000.f / static_cast<float>(fps_);
    }
    const unsigned int getFps() const
    {
        return fps_;
    }

private:
    std::vector<ScenePtr> scenes_;
    SceneFabrickPtr sceneFabrick_;

    unsigned int fps_{};
    // желаемое время кадра (мс)
    float desiredFrameMS_{};

private:
    sdl3::RenderWindow window_;
    Context context_;
    sdl3::ClockNS cl_;

private:
    SDL_AppResult processSceneAction(SceneAction& act)
    {
        SDL_AppResult res = SDL_APP_CONTINUE;
        if (act.type == SceneActionType::None)
            return SDL_APP_CONTINUE;
        else if (act.type == SceneActionType::PushScene)
            pushScene(std::get<IDType>(act.value));
        else if (act.type == SceneActionType::PopScene)
            popScene();
        else if (act.type == SceneActionType::SwitchScene)
            switchScene(std::get<IDType>(act.value));
        else if (act.type == SceneActionType::Exit)
            res = SDL_APP_SUCCESS;
        act = SceneAction::noneAction();
        return res;
    }

    void safeDrawScene()
    {
        window_.clear();
        context_.update();
        context_.render();
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

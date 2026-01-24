#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDLWrapper/Math/Colors.hpp>
#include <SDLWrapper/Names.hpp>
#include <SDLWrapper/Renders/VideoMode.hpp>
#include <SDLWrapper/Renders/View.hpp>
#include <string_view>
#include <vector>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>

#include <SDLWrapper/Clock.hpp>
#include <SDLWrapper/SDLWrapper.hpp>

#include "AdvancedContext.hpp"
#include "Core/Types.hpp"
#include "Engine/EngineSettings.hpp"
#include "EngineSettings.hpp"
#include "Scene.hpp"
#include "SceneAction.hpp"
#include "SceneFabrick.hpp"
#include "WindowSizeInfo.hpp"

namespace engine
{

class Engine
{
public:
    void close()
    {
        scenes_.clear();
        context_.quit();
        window_.close();
    }

    SDL_AppResult start(EngineSettings setts)
    {
        winSizeInfo_.init(setts.windowSize);

        sdl3::VideoMode mode = sdl3::VideoMode::getDefaultVideoMode();
        mode.width = setts.windowSize.x;
        mode.height = setts.windowSize.y;

        if (!window_.create(std::move(setts.appName), mode))
            return SDL_APP_FAILURE;
        if (!context_.init(window_.getNativeSDLWindow(), window_.getNativeSDLRenderer(), setts.fontFile))
            return SDL_APP_FAILURE;
        if (!window_.loadIconFromFile(setts.icoFile.string()))
            SDL_Log("Error of open icon");
        if (setts.setLogicalPresentation)
        {
            window_.setLogicalPresentation(setts.windowSize, setts.mode);
             auto view = window_.getView();
             auto logicalSize = window_.getLogicSize();
             view.setCenterPosition({logicalSize.x / 2.f, logicalSize.y / 2.f});
             window_.setView(view);
        }
        mode_ = setts.mode;
        autoOrientationEnabled_ = setts.autoOrientationEnabled;

        registrateSceneFabrick(std::move(setts.scenesFabrick));
        setFps(setts.fps);
        pushScene(setts.startSceneID);

        return SDL_APP_CONTINUE;
    }

    // SET METHODS

    void registrateSceneFabrick(SceneFabrickPtr ptr)
    {
        sceneFabrick_ = std::move(ptr);
        sceneFabrick_->setContext(context_);
        sceneFabrick_->setRenderWindow(window_);
    }
    void setFps(const unsigned int fps)
    {
        fps_ = fps;
        if (fps_ > 0)
            desiredFrameMS_ = 1000.f / static_cast<float>(fps_);
    }
    void setAutoOrientationEnabled(const bool enabled)
    {
        autoOrientationEnabled_ = enabled;
    }
    bool isAutoOrientationEnabled() const
    {
        return autoOrientationEnabled_;
    }

    // ITERATE METHODS

    SDL_AppResult updateEvents(SDL_Event &event)
    {
        if (event.type == SDL_EVENT_QUIT)
            return SDL_APP_SUCCESS;
        if (autoOrientationEnabled_ && event.type == SDL_EVENT_WINDOW_RESIZED)
            handleWindowResize(event.window.data1, event.window.data2);
        if (scenes_.empty())
            return SDL_APP_FAILURE;
        window_.convertEventToRenderCoordinates(&event);
        context_.updateEvents(event);
        window_.convertEventToViewCoordinates(&event);
        scenes_.back()->updateEvent(event);
        return SDL_APP_CONTINUE;
    }

    SDL_AppResult iterate()
    {
        if (scenes_.empty())
            return SDL_APP_FAILURE;
        const float dt = cl_.elapsedTimeS();
        cl_.start();
        SceneAction act = scenes_.back()->update(dt);
        SDL_AppResult res = processSceneAction(act);
        if (res != SDL_APP_CONTINUE)
            return res;
        context_.update();
        safeDrawScene();
        fpsDelay();
        return res;
    }

    // GET METHODS

    sdl3::RenderWindow &getWindow()
    {
        return window_;
    }

    const unsigned int getFps() const
    {
        return fps_;
    }

private:
    std::vector<ScenePtr> scenes_;
    SceneFabrickPtr sceneFabrick_;

    // желаемое время кадра (мс)
    float desiredFrameMS_{};
    unsigned int fps_{};

private:
    sdl3::RenderWindow window_;
    Context context_;
    sdl3::ClockNS cl_;

    WindowSizeInfo winSizeInfo_;
    SDL_RendererLogicalPresentation mode_;
    bool autoOrientationEnabled_ = true;

private:
    void handleWindowResize(const int windowW, const int windowH)
    {
        if(!winSizeInfo_.handleWindowResize(windowH, windowW))
            return;
        
        auto view = window_.getView();
        view.setCenterPosition(winSizeInfo_.centerPos);
        window_.setLogicalPresentation(winSizeInfo_.windowLogicslSize, SDL_LOGICAL_PRESENTATION_LETTERBOX);
        window_.setView(view);
    }

    void safeDrawScene()
    {
        window_.clear(sdl3::Colors::White);
        scenes_.back()->draw(window_);
        context_.render();
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

private: // SceneActionType process
    SDL_AppResult processSceneAction(const SceneAction &act)
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
        return res;
    }

    void pushScene(const IDType sceneId)
    {
        if (!scenes_.empty())
            scenes_.back()->hide();
        scenes_.push_back(std::move(sceneFabrick_->genSceneByID(sceneId)));
        scenes_.back()->show();
    }

    void popScene()
    {
        if (scenes_.empty())
            return;
        scenes_.back()->hide();
        scenes_.pop_back();
        if (!scenes_.empty())
            scenes_.back()->show();
    }

    void switchScene(const IDType sceneId)
    {
        if (scenes_.empty())
            return;
        scenes_.back()->hide();
        scenes_.pop_back();
        pushScene(sceneId);
    }
};

} // namespace engine

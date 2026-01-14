#pragma once

#include "Engine/AdvancedContext.hpp"
#include "Engine/SceneAction.hpp"
#include <Core/Types.hpp>
#include <Engine/Scene.hpp>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/ID.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>

class GameScene : public engine::Scene
{
public:
    inline static constexpr const IDType sceneID = 1;
    inline static const std::string menuID = "game_menu";

private:
    class GameSceneListener : public Rml::EventListener
    {
    public:
        GameSceneListener(GameScene &scene) : scene_(scene)
        {
        }

        void ProcessEvent(Rml::Event &ev) override
        {
            Rml::Element *el = ev.GetTargetElement();
            const Rml::String &id = el->GetId();

            // if(id == "exit-b")
            //     scene_.actionRes_ = engine::SceneAction::exitAction();
        }

    private:
        GameScene &scene_;
    };

public:
    GameScene(engine::Context &context) : context_(context), listener_(*this)
    {
        // doc_ = context_.loadIfNoDocument("ui/GameScene.html", menuID);
        // if (!doc_)
        //     throw std::logic_error("The document cannot be empty.");
        // doc_->Show();
        // doc_->AddEventListener(Rml::EventId::Click, &listener_, true);
    }
    ~GameScene()
    {
        // doc_->RemoveEventListener(Rml::EventId::Click, &listener_, true);
        // doc_->Hide();
    }

    void updateEvent(const SDL_Event &event) override
    {
        if(event.type == SDL_EVENT_KEY_UP && event.key.key == SDLK_ESCAPE)
            actionRes_ = engine::SceneAction::popAction();
    }

    void draw(sdl3::RenderWindow &window) const override
    {
    }

    // void hide() override
    // {
    //     // doc_->Hide();
    // }
    // void show()  override
    // {
    //     // doc_->Show();
    // }

private:
    engine::Context &context_;
    Rml::ElementDocument *doc_ = nullptr;
    GameSceneListener listener_;

private:
};

#pragma once

#include "Engine/AdvancedContext.hpp"
#include "Engine/SceneAction.hpp"
#include <Core/Types.hpp>
#include <Engine/Scene.hpp>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/ID.h>
#include <stdexcept>

class MainMenuScene : public engine::Scene
{
public:
    inline static constexpr const IDType sceneID = 0;
    inline static const std::string menuID = "main_menu";

private:
    class MainMenuListener : public Rml::EventListener
    {
    public:
        MainMenuListener(MainMenuScene &scene) : scene_(scene)
        {
        }

        void ProcessEvent(Rml::Event &ev) override
        {
            Rml::Element *el = ev.GetTargetElement();
            const Rml::String &id = el->GetId();

            if(id == "exit-b")
                scene_.actionRes_ = engine::SceneAction::exitAction();
        }

    private:
        MainMenuScene &scene_;
    };

public:
    MainMenuScene(engine::Context &context) : context_(context), listener_(*this)
    {
        doc_ = context_.loadIfNoDocument("ui/MainMenu.html", menuID);
        if (!doc_)
            throw std::logic_error("The document cannot be empty.");
        doc_->Show();
        doc_->AddEventListener(Rml::EventId::Click, &listener_, true);
    }
    ~MainMenuScene()
    {
        if (!doc_)
            throw std::logic_error("The document cannot be empty.");
        doc_->RemoveEventListener(Rml::EventId::Click, &listener_, true);
        doc_->Hide();
    }

    void updateEvent(const SDL_Event &event)
    {
    }
    engine::SceneAction update(const float dt)
    {
        return actionRes_;
    }

    void draw(sdl3::RenderWindow &window) const
    {
    }

private:
    engine::Context &context_;
    Rml::ElementDocument *doc_ = nullptr;
    MainMenuListener listener_;
};

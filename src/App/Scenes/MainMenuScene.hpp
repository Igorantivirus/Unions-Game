#pragma once

#include "Engine/SceneAction.hpp"
#include "GameScene.hpp"
#include "SettingsMenu.hpp"
#include <Core/Types.hpp>
#include <Engine/OneRmlDocScene.hpp>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/ID.h>

class MainMenuScene : public engine::OneRmlDocScene
{
public:
    inline static constexpr const IDType sceneID = 0;

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

            if (id == "exit-b")
                scene_.actionRes_ = engine::SceneAction::exitAction();
            else if (id == "start-b")
                scene_.actionRes_ = engine::SceneAction::nextAction(GameScene::sceneID);
            else if (id == "setts-b")
                scene_.actionRes_ = engine::SceneAction::nextAction(SettingsMenu::sceneID);
        }

    private:
        MainMenuScene &scene_;
    };

public:
    MainMenuScene(engine::Context &context)
        : engine::OneRmlDocScene(context, assets::ui::gameMenuRmlFile), listener_(*this)
    {
        loadDocumentOrThrow();
        addEventListener(Rml::EventId::Click, &listener_, true);
    }

    void updateEvent(const SDL_Event &event) override
    {
    }

    void draw(sdl3::RenderWindow &window) const override
    {
    }

private:
    MainMenuListener listener_;
};

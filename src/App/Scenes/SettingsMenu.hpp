#pragma once

#include "Engine/SceneAction.hpp"
#include "GameScene.hpp"
#include <Core/Types.hpp>
#include <Engine/OneRmlDocScene.hpp>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/ID.h>

class SettingsMenu : public engine::OneRmlDocScene
{
public:
    inline static constexpr const IDType sceneID = 2;
    inline static const std::string menuID = "settings_menu";

private:
    class SettingsMenuListener : public Rml::EventListener
    {
    public:
        SettingsMenuListener(SettingsMenu &scene) : scene_(scene)
        {
        }

        void ProcessEvent(Rml::Event &ev) override
        {
            Rml::Element *el = ev.GetTargetElement();
            const Rml::String &id = el->GetId();

            if (id == "save-exit-b" || id == "throw-exit-b")
                scene_.actionRes_ = engine::SceneAction::popAction();
        }

    private:
        SettingsMenu &scene_;
    };

public:
    SettingsMenu(engine::Context &context)
        : engine::OneRmlDocScene(context, "ui/SettingsMenu.html", menuID), listener_(*this)
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
    SettingsMenuListener listener_;
};

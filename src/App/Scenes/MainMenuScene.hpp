#pragma once

#include <Core/Types.hpp>
#include <Engine/OneRmlDocScene.hpp>
#include <Engine/SceneAction.hpp>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/ID.h>

#include <App/HardStrings.hpp>

#include "IDs.hpp"

namespace scenes
{

class MainMenuScene : public engine::OneRmlDocScene
{
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
            while (el && el->GetId().empty())
                el = el->GetParentNode();
            if (!el)
                return;

            const Rml::String &id = el->GetId();

            if (id == ui::mainMenu::exitB)
                scene_.actionRes_ = engine::SceneAction::exitAction();
            else if (id == ui::mainMenu::startB)
                scene_.actionRes_ = engine::SceneAction::nextAction(ids::gameMenu);
            else if (id == ui::mainMenu::todevB)
                SDL_OpenURL(ui::mainMenu::githubUrl.data());
            else if (id == ui::mainMenu::settsB)
                scene_.actionRes_ = engine::SceneAction::nextAction(ids::setsMenu);
        }

    private:
        MainMenuScene &scene_;
    };

public:
    MainMenuScene(engine::Context &context)
        : engine::OneRmlDocScene(context, ui::mainMenu::file), listener_(*this)
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

} // namespace scenes
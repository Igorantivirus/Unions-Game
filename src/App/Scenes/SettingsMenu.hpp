#pragma once

#include "Core/Time.hpp"
#include "Engine/SceneAction.hpp"
#include <App/AppState.hpp>
#include <Core/Types.hpp>
#include <Engine/OneRmlDocScene.hpp>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/ID.h>

#include <App/HardStrings.hpp>
#include <string>
#include <unordered_map>

namespace scenes
{

class SettingsMenu : public engine::OneRmlDocScene
{
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
            while (el && el->GetId().empty())
                el = el->GetParentNode();
            if (!el)
                return;

            const Rml::String &id = el->GetId();
            const std::unordered_map<std::string, std::string> &buttons = scene_.getChooseButtons();

            if (id == ui::setsMenu::saveExitB)
                scene_.actionRes_ = engine::SceneAction::popAction();
            else if(id == ui::setsMenu::saveThrowB)
            {
                scene_.appState_.stat().resetAllStatistic();
                scene_.addStatisticToUi();
            }
            else if (auto found = buttons.find(id); found != buttons.end())
            {
                scene_.appState_.setCurrentPackageName(found->second);
                scene_.addStatisticToUi();
            }
        }

    private:
        SettingsMenu &scene_;
    };

public:
    SettingsMenu(engine::Context &context, app::AppState &appState)
        : engine::OneRmlDocScene(context, ui::setsMenu::file), listener_(*this), appState_(appState)
    {
        initButtons();
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
    app::AppState &appState_;

    std::unordered_map<std::string, std::string> chooseButtons_;

private:
    const std::unordered_map<std::string, std::string> &getChooseButtons() const
    {
        return chooseButtons_;
    }

    void onDocumentLoaded(Rml::ElementDocument &doc) override
    {
        addStatisticToUi();
    }

    void initButtons()
    {
        chooseButtons_.clear();
        for (const auto &gs : appState_.stat().getAll())
        {
            std::string buttonId = gs.stringID + "-choose-b";
            chooseButtons_[std::move(buttonId)] = gs.stringID;
        }
    }

    void addStatisticToUi()
    {
        Rml::ElementDocument *doc = document();

        auto label = doc->GetElementById("current-game-label");
        const std::string &currentID = appState_.getCurrentPackageName();
        if (auto id = appState_.stat().findById(currentID); id)
            label->SetInnerRML(id->name);

        Rml::ElementList gameNames;
        Rml::ElementList timeLabels;
        Rml::ElementList recordLabels;
        Rml::ElementList countLabels;
        Rml::ElementList chooseButtons;

        doc->QuerySelectorAll(gameNames, ".game-name");
        doc->QuerySelectorAll(timeLabels, ".time-stat-label");
        doc->QuerySelectorAll(recordLabels, ".record-atat-label");
        doc->QuerySelectorAll(countLabels, ".count-stat-label");
        doc->QuerySelectorAll(chooseButtons, ".choose-b");

        IDType index = 0;
        auto iter = chooseButtons_.begin();
        for (const auto &gs : appState_.stat().getAll())
        {
            if (iter == chooseButtons_.end())
                return;
            gameNames[index]->SetInnerRML(gs.name);
            timeLabels[index]->SetInnerRML(core::Time::toString(gs.time));
            recordLabels[index]->SetInnerRML(std::to_string(gs.record));
            countLabels[index]->SetInnerRML(std::to_string(gs.gameCount));
            chooseButtons[index]->SetAttribute("id", iter->first);

            ++iter;
            ++index;
        }
    }
};

} // namespace scenes
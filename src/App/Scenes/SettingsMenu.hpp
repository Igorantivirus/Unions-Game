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

            if (id == ui::setsMenu::saveExitB || id == ui::setsMenu::saveThrowB)
                scene_.actionRes_ = engine::SceneAction::popAction();

            else if (auto found = buttons.find(id); found != buttons.end())
            {
                scene_.appState_.setCurrentPackageName(found->second);
                scene_.addStatisticToUi();
                // const std::string pack = id.substr(std::string("choose-").size());
                // scene_.appState_.setCurrentPackageName(pack);
                // scene_.refreshUI();
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
        // refreshUI();
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

    void refreshUI()
    {
        Rml::ElementDocument *doc = document();
        if (!doc)
            return;

        const auto *selected = appState_.stat().findById(appState_.getCurrentPackageName());
        if (!selected && !appState_.stat().empty())
        {
            appState_.setCurrentPackageName(appState_.stat().getAll().front().stringID);
            selected = appState_.stat().findById(appState_.getCurrentPackageName());
        }

        if (Rml::Element *selName = doc->GetElementById("selected-game-name"))
            selName->SetInnerRML(selected ? selected->name : std::string("—"));

        Rml::Element *games = doc->GetElementById("games");
        if (!games)
            return;

        std::string rml;
        for (const auto &gs : appState_.stat().getAll())
        {
            const bool isSelected = (gs.stringID == appState_.getCurrentPackageName());

            rml += "<div class=\"game";
            if (isSelected)
                rml += " selected";
            rml += "\" id=\"game-";
            rml += gs.stringID;
            rml += "\">";

            rml += "<div class=\"game-header\">";
            rml += "<div class=\"name\">";
            rml += gs.name;
            rml += "</div>";
            rml += "<button class=\"choos-b\" id=\"choose-";
            rml += gs.stringID;
            rml += "\"><span>Выбрать</span></button>";
            rml += "</div>";

            rml += "<div class=\"statistic\">";
            rml += "<div class=\"stats-grid\">";

            rml += "<div class=\"stat-cell\"><div class=\"stat-label\">Время рекорда</div><div class=\"stat-value\">";
            rml += core::Time::toString(gs.time);
            rml += "</div></div>";

            rml += "<div class=\"stat-cell\"><div class=\"stat-label\">Рекорд очков</div><div class=\"stat-value\">";
            rml += std::to_string(gs.record);
            rml += "</div></div>";

            rml += "<div class=\"stat-cell\"><div class=\"stat-label\">Сыграно игр</div><div class=\"stat-value\">";
            rml += std::to_string(gs.gameCount);
            rml += "</div></div>";

            rml += "</div></div></div>";
        }

        games->SetInnerRML(rml);
    }
};

} // namespace scenes
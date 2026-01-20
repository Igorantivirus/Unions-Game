#pragma once

#include "Engine/SceneAction.hpp"
#include <App/AppState.hpp>
#include <Core/Types.hpp>
#include <Engine/OneRmlDocScene.hpp>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/ID.h>

#include <App/Assets/HardStrings.hpp>

class SettingsMenu : public engine::OneRmlDocScene
{
public:
    inline static constexpr const IDType sceneID = 2;

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

            if (id == assets::gameMenu::saveExitB || id == assets::gameMenu::saveThrowB)
                scene_.actionRes_ = engine::SceneAction::popAction();
            else if (id.rfind("choose-", 0) == 0)
            {
                const std::string pack = id.substr(std::string("choose-").size());
                scene_.appState_.setCurrentPackageName(pack);
                scene_.refreshUI();
            }
        }

    private:
        SettingsMenu &scene_;
    };

public:
    SettingsMenu(engine::Context &context, app::AppState &appState)
        : engine::OneRmlDocScene(context, assets::ui::setsMenuRmlFile), listener_(*this), appState_(appState)
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
    void onDocumentLoaded(Rml::ElementDocument &doc) override
    {
        refreshUI();
    }

    static std::string formatTime(const statistic::Time &t)
    {
        char buf[6] = {};
        buf[0] = static_cast<char>('0' + (t.minuts / 10));
        buf[1] = static_cast<char>('0' + (t.minuts % 10));
        buf[2] = ':';
        buf[3] = static_cast<char>('0' + (t.seconds / 10));
        buf[4] = static_cast<char>('0' + (t.seconds % 10));
        buf[5] = '\0';
        return std::string(buf);
    }

    void refreshUI()
    {
        Rml::ElementDocument *doc = document();
        if (!doc)
            return;

        const auto *selected = appState_.stat().findById(appState_.getCurrentPackageName());
        if (!selected && !appState_.stat().gameStatistic.empty())
        {
            appState_.setCurrentPackageName(appState_.stat().gameStatistic.front().stringID);
            selected = appState_.stat().findById(appState_.getCurrentPackageName());
        }

        if (Rml::Element *selName = doc->GetElementById("selected-game-name"))
            selName->SetInnerRML(selected ? selected->name : std::string("—"));

        Rml::Element *games = doc->GetElementById("games");
        if (!games)
            return;

        std::string rml;
        for (const auto &gs : appState_.stat().gameStatistic)
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
            rml += formatTime(gs.time);
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

    SettingsMenuListener listener_;
    app::AppState &appState_;
};

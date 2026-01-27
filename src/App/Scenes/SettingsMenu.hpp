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
#include <SDLWrapper/Audio/AudioDevice.hpp>
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

        void init(const int value)
        {
            volValue = scene_.document()->GetElementById("volume-level");
            volRange = scene_.document()->GetElementById("vol");
            if(volValue)
                volValue->SetInnerRML(std::to_string(value));
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

            if (volValue && volRange && ev.GetId() == Rml::EventId::Change && id == "vol")
            {
                int value = volRange->GetAttribute("value")->Get<int>(50);
                volValue->SetInnerRML(std::to_string(value));
            }
            else if (id == ui::setsMenu::saveExitB)
                scene_.actionRes_ = engine::SceneAction::popAction();
            else if (id == ui::setsMenu::saveThrowB)
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
        Rml::Element *volValue = nullptr;
        Rml::Element *volRange = nullptr;
    };

public:
    SettingsMenu(engine::Context &context, sdl3::audio::AudioDevice &audio, app::AppState &appState)
        : engine::OneRmlDocScene(context, ui::setsMenu::file), audio_(audio), listener_(*this), appState_(appState)
    {
        loadDocumentOrThrow();
        addEventListener(Rml::EventId::Click, &listener_, true);
        addEventListener(Rml::EventId::Change, &listener_, true);
    }
    ~SettingsMenu()
    {
        if (range_)
        {
            int value = range_->GetAttribute("value")->Get<int>(50);
            float v = static_cast<float>(value - minvalue_) / static_cast<float>(maxvalue_ - minvalue_) * 2.f;
            audio_.setVolumeLevel(v);
            appState_.setVolume(v);
        }
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
    sdl3::audio::AudioDevice &audio_;

    std::unordered_map<std::string, std::string> chooseButtons_;

    Rml::Element *range_ = nullptr;
    int minvalue_ = 0;
    int maxvalue_ = 100;

private:
    const std::unordered_map<std::string, std::string> &getChooseButtons() const
    {
        return chooseButtons_;
    }

    void onDocumentLoaded(Rml::ElementDocument &doc) override
    {
        range_ = document()->GetElementById("vol");
        if (range_)
        {
            int minvalue_ = range_->GetAttribute("min")->Get<int>(0);
            int maxvalue_ = range_->GetAttribute("max")->Get<int>(100);
        }

        addStatisticToUi();
    }

    void addStatisticToUi()
    {
        Rml::ElementDocument *doc = document();

        auto label = doc->GetElementById("current-game-label");
        const std::string &currentID = appState_.getCurrentPackageName();
        if (auto id = appState_.stat().findById(currentID); id)
            label->SetInnerRML(id->name);

        float normvalue = appState_.getVolume();
        float value = normvalue / 2 * (maxvalue_ - minvalue_) + minvalue_;
        if (range_)
            range_->SetAttribute("value", value);
        listener_.init(value);

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
        for (const auto &gs : appState_.stat().getAll())
        {
            gameNames[index]->SetInnerRML(gs.name);
            timeLabels[index]->SetInnerRML(core::Time::toString(gs.time));
            recordLabels[index]->SetInnerRML(std::to_string(gs.record));
            countLabels[index]->SetInnerRML(std::to_string(gs.gameCount));
            std::string buttonId = gs.stringID + "-choose-b";
            chooseButtons[index]->SetAttribute("id", buttonId);
            chooseButtons_[std::move(buttonId)] = gs.stringID;
            ++index;
        }
    }
};

} // namespace scenes
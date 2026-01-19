#pragma once

#include <Engine/SceneFabrick.hpp>
#include <memory>

#include <App/AppState.hpp>
#include "Scenes/MainMenuScene.hpp"
#include "Scenes/GameScene.hpp"
#include "Scenes/SettingsMenu.hpp"

class GameSceneFactory : public engine::SceneFabrick
{
public:
    explicit GameSceneFactory(std::shared_ptr<app::AppState> appState)
        : appState_(std::move(appState))
    {
    }

    app::AppState &getAppState() const
    {
        return *appState_;
    }

    engine::ScenePtr genSceneByID(const IDType id) const override
    {
        if(MainMenuScene::sceneID == id)
            return std::move(std::make_unique<MainMenuScene>(*context_));
        if(GameScene::sceneID == id)
            return std::move(std::make_unique<GameScene>(*context_, window_->getLogicSize(), *appState_));
        if(SettingsMenu::sceneID == id)
            return std::move(std::make_unique<SettingsMenu>(*context_, *appState_));
        return nullptr;
    }

private:
    std::shared_ptr<app::AppState> appState_;

};

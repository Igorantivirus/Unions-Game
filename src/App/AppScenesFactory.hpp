#pragma once

#include <memory>

#include <App/AppState.hpp>
#include <Engine/SceneFabrick.hpp>
#include <Scenes/GameScene.hpp>
#include <Scenes/MainMenuScene.hpp>
#include <Scenes/SettingsMenu.hpp>


class AppScenesFactory : public engine::SceneFabrick
{
public:
    explicit AppScenesFactory(app::AppStatePtr appState = nullptr)
        : appState_(std::move(appState))
    {
    }

    engine::ScenePtr genSceneByID(const IDType id) const override
    {
        if (MainMenuScene::sceneID == id)
            return std::move(std::make_unique<MainMenuScene>(*context_));
        if (GameScene::sceneID == id)
            return std::move(std::make_unique<GameScene>(*context_, window_->getLogicSize(), *appState_));
        if (SettingsMenu::sceneID == id)
            return std::move(std::make_unique<SettingsMenu>(*context_, *appState_));
        return nullptr;
    }

private:
    app::AppStatePtr appState_;
};

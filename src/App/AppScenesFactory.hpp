#pragma once

#include <memory>

#include <App/AppState.hpp>
#include <Engine/SceneFabrick.hpp>
#include <Scenes/GameScene.hpp>
#include <Scenes/MainMenuScene.hpp>
#include <Scenes/SettingsMenu.hpp>
#include <Scenes/IDs.hpp>

namespace app
{

class AppScenesFactory : public engine::SceneFabrick
{
public:

    void setAppState(app::AppState& appState)
    {
        appState_ = &appState;
    }

    engine::ScenePtr genSceneByID(const IDType id) const override
    {
        if (scenes::ids::mainMenu == id)
            return std::move(std::make_unique<scenes::MainMenuScene>(*context_));
        if (scenes::ids::gameMenu == id)
            return std::move(std::make_unique<scenes::GameScene>(*context_, window_->getLogicSize(), *appState_));
        if (scenes::ids::setsMenu == id)
            return std::move(std::make_unique<scenes::SettingsMenu>(*context_, *appState_));
        return nullptr;
    }

private:
    app::AppState* appState_;
};

} // namespace app
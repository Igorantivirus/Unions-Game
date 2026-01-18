#pragma once

#include <Engine/SceneFabrick.hpp>
#include <memory>

#include "Scenes/MainMenuScene.hpp"
#include "Scenes/GameScene.hpp"

class GameSceneFactory : public engine::SceneFabrick
{
public:

    engine::ScenePtr genSceneByID(const IDType id) const override
    {
        if(MainMenuScene::sceneID == id)
            return std::move(std::make_unique<MainMenuScene>(*context_));
        if(GameScene::sceneID == id)
            return std::move(std::make_unique<GameScene>(*context_, window_->getLogicSize()));
        return nullptr;
    }

};
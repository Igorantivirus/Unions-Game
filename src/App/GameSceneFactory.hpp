#pragma once

#include <Engine/SceneFabrick.hpp>
#include <memory>

#include "Scenes/MainMenuScene.hpp"

class GameSceneFactory : public engine::SceneFabrick
{
public:

    engine::ScenePtr genSceneByID(const IDType id) const override
    {
        if(MainMenuScene::sceneID == id)
            return std::move(std::make_unique<MainMenuScene>(*context_));
        return nullptr;
    }

};
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
            return std::make_unique<MainMenuScene>(MainMenuScene{});
        return nullptr;
    }

};
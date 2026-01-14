#pragma once

#include <Core/Types.hpp>
#include <Engine/Scene.hpp>

class MainMenuScene : public engine::Scene
{
public:

    inline static constexpr const IDType sceneID = 0;

public:
    
    void updateEvent(const SDL_Event &event)
    {

    }
    engine::SceneAction update(const float dt)
    {
        return engine::SceneAction::noneAction();
    }

    void draw(sdl3::RenderWindow &window) const
    {

    }

private:




};
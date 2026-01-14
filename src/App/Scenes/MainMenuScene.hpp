#pragma once

#include "Engine/AdvancedContext.hpp"
#include <Core/Types.hpp>
#include <Engine/Scene.hpp>
#include <RmlUi/Core/ElementDocument.h>

class MainMenuScene : public engine::Scene
{
public:

    inline static constexpr const IDType sceneID = 0;
    inline static const std::string menuID = "main_menu";

public:
    MainMenuScene(engine::Context& context) : context_(context)
    {
        doc_ = context_.loadIfNoDocument("ui/MainMenu.html", menuID);
        if (doc_)
            doc_->Show();

    }
    ~MainMenuScene()
    {
        if (doc_)
            doc_->Hide();
    }
    
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

    engine::Context& context_;
    Rml::ElementDocument* doc_ = nullptr;


};

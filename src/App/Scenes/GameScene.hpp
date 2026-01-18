#pragma once

#include "AppEvents.hpp"
#include "Engine/AdvancedContext.hpp"
#include "Engine/SceneAction.hpp"
#include <Core/Types.hpp>
#include <Engine/Scene.hpp>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/ID.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>

#include <GameObjects/Entity.hpp>
#include <SDL3/SDL_mouse.h>

#include <App/GameObjects/EntityFactory.hpp>
#include <App/GameObjects/GameContactCheker.hpp>
#include <App/GameObjects/GameObjectFactory.hpp>
#include <SDLWrapper/Names.hpp>
#include <memory>

class GameScene : public engine::Scene
{
public:
    inline static constexpr const IDType sceneID = 1;
    inline static const std::string menuID = "game_menu";

private:
    class GameSceneListener : public Rml::EventListener
    {
    public:
        GameSceneListener(GameScene &scene) : scene_(scene)
        {
        }

        void ProcessEvent(Rml::Event &ev) override
        {
            Rml::Element *el = ev.GetTargetElement();
            const Rml::String &id = el->GetId();

            // if(id == "exit-b")
            //     scene_.actionRes_ = engine::SceneAction::exitAction();
        }

    private:
        GameScene &scene_;
    };

public:
    GameScene(engine::Context &context) : context_(context), listener_(*this)
    {
        // doc_ = context_.loadIfNoDocument("ui/GameScene.html", menuID);
        // if (!doc_)
        //     throw std::logic_error("The document cannot be empty.");
        // doc_->Show();
        // doc_->AddEventListener(Rml::EventId::Click, &listener_, true);

        world_.SetContactListener(&contactCheker_);

        generateGlass(400, 400, 10);
    }
    ~GameScene()
    {
        // doc_->RemoveEventListener(Rml::EventId::Click, &listener_, true);
        // doc_->Hide();
    }

    void updateEvent(const SDL_Event &event) override
    {
        if (event.type == AppEventsType::COLLISION)
        {
            SDL_Log("GOYDA\n");
            updateCollision(event);
        }
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_AC_BACK)
        {
            actionRes_ = engine::SceneAction::popAction();
        }
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                pressed_ = true;
                prEntity_ = std::make_unique<GameObject>(std::move(
                    GameEntityFactory::createById(world_, 1, {event.button.x, startY_})));
                prEntity_->setEnabled(false);
            }
        }
        if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                pressed_ = false;
                prEntity_->setEnabled(true);
                objects_.push_back(std::move(*prEntity_.get()));
                prEntity_.reset();
            }
        }
        else if (event.type == SDL_EVENT_MOUSE_MOTION)
        {
            if (!pressed_ || !prEntity_)
                return;

            prEntity_->setPosition({event.motion.x, startY_});
        }
    }

    void draw(sdl3::RenderWindow &window) const override
    {
        for (const auto &i : glass_)
            window.draw(i);
        for (const auto &i : objects_)
            window.draw(i);
        if (prEntity_)
            window.draw(prEntity_.get());
    }

    engine::SceneAction &update(const float dt) override
    {
        world_.Step(dt, 8, 3);

        return actionRes_;
    }

    // void hide() override
    // {
    //     // doc_->Hide();
    // }
    // void show()  override
    // {
    //     // doc_->Show();
    // }

private:
    engine::Context &context_;
    // Rml::ElementDocument *doc_ = nullptr;
    GameSceneListener listener_;

private:
    b2World world_{b2Vec2(0.0f, 9.81f)};
    GameContactCheker contactCheker_;
    std::vector<Entity> glass_;
    std::vector<GameObject> objects_;

    std::unique_ptr<GameObject> prEntity_ = nullptr;
    bool pressed_ = false;
    float startY_;

private:
    void generateGlass(const float width, const float height, const float thikness)
    {
        sdl3::Vector2i size = {800, 800};
        glass_.clear();
        glass_.push_back(EntityFactory::createRectangle(world_, {size.x / 2.f, size.y * 1.f}, {width, thikness}, sdl3::Colors::Black, b2BodyType::b2_staticBody));
        glass_.push_back(EntityFactory::createRectangle(world_, {size.x / 2.f - width / 2.f, size.y - height / 2.f}, {thikness, height}, sdl3::Colors::Black, b2BodyType::b2_staticBody));
        glass_.push_back(EntityFactory::createRectangle(world_, {size.x / 2.f + width / 2.f, size.y - height / 2.f}, {thikness, height}, sdl3::Colors::Black, b2BodyType::b2_staticBody));

        startY_ = size.y - height * (1.f + 1 / 3.f);
    }

    std::size_t getByID(const IDType id)
    {
        for(std::size_t i = 0; i < objects_.size(); ++i)
            if (objects_[i].getID() == id)
                return i;
        return objects_.size();
    }

    void updateCollision(const SDL_Event& event)
    {
        
        std::size_t obj1Ind = getByID(static_cast<IDType>(reinterpret_cast<uintptr_t>(event.user.data1)));
        std::size_t obj2Ind = getByID(static_cast<IDType>(reinterpret_cast<uintptr_t>(event.user.data2)));
        if(obj1Ind == objects_.size() || obj2Ind == objects_.size())
            return;

        GameObject& obj1 = objects_[obj1Ind];
        GameObject& obj2 = objects_[obj2Ind];

        sdl3::Vector2f pos = (obj1.getShape().getPosition() + obj2.getShape().getPosition()) / 2.f;

        objects_.erase(objects_.begin() + std::max(obj1Ind, obj2Ind));
        objects_.erase(objects_.begin() + std::min(obj1Ind, obj2Ind));
        
        objects_.push_back(GameEntityFactory::createById(world_, event.user.code + 1, pos));

    }
};

#pragma once

#include "AppEvents.hpp"
#include "Engine/AdvancedContext.hpp"
#include "Engine/SceneAction.hpp"
#include <Core/Types.hpp>
#include <Engine/Scene.hpp>
#include <RmlUi/Config/Config.h>
#include <RmlUi/Core/DataModelHandle.h>
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
#include <App/GameObjects/ObjectFactory.hpp>
#include <SDLWrapper/Clock.hpp>
#include <SDLWrapper/Names.hpp>
#include <memory>

#include <Engine/OneRmlDocScene.hpp>
#include <Resources/ObjectLibrary.hpp>

class GameScene : public engine::OneRmlDocScene
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

            if (id == "back-b")
                scene_.actionRes_ = engine::SceneAction::popAction();
        }

    private:
        GameScene &scene_;
    };

public:
    GameScene(engine::Context &context, const sdl3::Vector2i logicSize)
        : engine::OneRmlDocScene(context, "ui/GameMenu.html", menuID), listener_(*this)
    {
        bindData();
        loadDocumentOrThrow();
        addEventListener(Rml::EventId::Click, &listener_, true);

        world_.SetContactListener(&contactCheker_);
        generateGlass(logicSize, {(float)logicSize.x, (float)logicSize.y * 0.75f}, 30);
        
        if (!objectFactory_.loadPack("conis") && !objectFactory_.loadPack("coins"))
            SDL_Log("Failed to load object pack: conis/coins");
        
        timer_.start();
    }
    ~GameScene()
    {
        if (dataHandle_)
        {
            // Освобождаем модель данных
            dataHandle_ = Rml::DataModelHandle();

            // Удаляем модель из контекста
            context_.getContext()->RemoveDataModel("game_stats");
        }
    }

    void updateEvent(const SDL_Event &event) override
    {
        if (event.type == AppEventsType::COLLISION)
        {
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

                auto created = objectFactory_.tryCreateById(world_, rand() % 3 + 1, {event.button.x, startY_});
                if (!created)
                    return;

                prEntity_ = std::make_unique<GameObject>(std::move(*created));
                prEntity_->setEnabled(false);
            }
        }
        if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                pressed_ = false;
                prEntity_->setEnabled(true);
                addPoints(prEntity_->getPoints());
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
        updateTime();

        for (auto it = objects_.begin(); it != objects_.end(); ++it)
            if (it->getPosition().y > 2000.f)
            {
                addPoints(-it->getPoints());
                it = objects_.erase(it);
            }

        return actionRes_;
    }

private:
    GameSceneListener listener_;

private:
    b2World world_{b2Vec2(0.0f, 9.81f)};
    GameContactCheker contactCheker_;
    std::vector<Entity> glass_;
    std::vector<GameObject> objects_;
    resources::ObjectLibrary objectLibrary_;
    resources::ObjectFactory objectFactory_{objectLibrary_};

    std::unique_ptr<GameObject> prEntity_ = nullptr;
    bool pressed_ = false;
    float startY_;

private:
    Rml::DataModelHandle dataHandle_;
    Rml::String time_ = "00:00";
    int points_ = 0;
    int record_ = 100;

    sdl3::Clock timer_;

private:
    void bindData()
    {
        Rml::DataModelConstructor constructor = context_.getContext()->CreateDataModel("game_stats");
        if (constructor)
        {
            constructor.Bind("game_time", &time_);
            constructor.Bind("points", &points_);
            constructor.Bind("record", &record_);
        }
        dataHandle_ = constructor.GetModelHandle();
    }

    void generateGlass(const sdl3::Vector2i logicSize, const sdl3::Vector2f glassSize, const float thikness)
    {
        glass_.clear();

        float yPos = logicSize.y;
        // float yPos = logicSize.y / 2.f + std::min(logicSize.x, logicSize.y) / 2.f;

        glass_.push_back(EntityFactory::createRectangle(world_, {logicSize.x / 2.f, yPos}, {glassSize.x, thikness}, sdl3::Colors::Black, b2BodyType::b2_staticBody));
        glass_.push_back(EntityFactory::createRectangle(world_, {logicSize.x / 2.f - glassSize.x / 2.f, yPos - glassSize.y / 2.f}, {thikness, glassSize.y}, sdl3::Colors::Black, b2BodyType::b2_staticBody));
        glass_.push_back(EntityFactory::createRectangle(world_, {logicSize.x / 2.f + glassSize.x / 2.f, yPos - glassSize.y / 2.f}, {thikness, glassSize.y}, sdl3::Colors::Black, b2BodyType::b2_staticBody));

        startY_ = (yPos - (glassSize.y)) / 2.f;
    }

    bool loadObjectPack(const std::string_view folder)
    {
        return objectFactory_.loadPack(folder);
    }

    void unloadObjectPack(const std::string_view folder)
    {
        (void)folder;
        objectFactory_.unloadPack();
    }

    std::size_t getByID(const IDType id)
    {
        for (std::size_t i = 0; i < objects_.size(); ++i)
            if (objects_[i].getID() == id)
                return i;
        return objects_.size();
    }

    void addPoints(const int points)
    {
        points_ += points;
        dataHandle_.DirtyVariable("points");
        if (points_ > record_)
        {
            record_ = points_;
            dataHandle_.DirtyVariable("record");
        }
    }

    void updateTime()
    {
        int s = static_cast<int>(timer_.elapsedTimeS());
        time_[0] = s / 60 / 10 + '0';
        time_[1] = s / 60 % 10 + '0';
        time_[2] = ':';
        time_[3] = s % 60 / 10 + '0';
        time_[4] = s % 60 % 10 + '0';
        dataHandle_.DirtyVariable("game_time");
    }

    void updateCollision(const SDL_Event &event)
    {

        std::size_t obj1Ind = getByID(static_cast<IDType>(reinterpret_cast<uintptr_t>(event.user.data1)));
        std::size_t obj2Ind = getByID(static_cast<IDType>(reinterpret_cast<uintptr_t>(event.user.data2)));
        if (obj1Ind == objects_.size() || obj2Ind == objects_.size())
            return;

        GameObject &obj1 = objects_[obj1Ind];
        GameObject &obj2 = objects_[obj2Ind];

        const IDType level1 = obj1.getLevel();
        const IDType level2 = obj2.getLevel();
        const auto mergedIdOpt = objectFactory_.getMergeResultId(level1, level2);
        if (!mergedIdOpt)
            return;

        sdl3::Vector2f pos = (obj1.getShape().getPosition() + obj2.getShape().getPosition()) / 2.f;

        objects_.erase(objects_.begin() + std::max(obj1Ind, obj2Ind));
        objects_.erase(objects_.begin() + std::min(obj1Ind, obj2Ind));

        auto created = objectFactory_.tryCreateById(world_, *mergedIdOpt, pos);
        if (!created)
            return;

        objects_.push_back(std::move(*created));
        addPoints(objects_.back().getPoints());
    }
};

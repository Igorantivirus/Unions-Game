#pragma once

#include "Resources/Types.hpp"
#include <SDL3/SDL_log.h>
#include <memory>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDLWrapper/Clock.hpp>
#include <SDLWrapper/Names.hpp>

#include <RmlUi/Config/Config.h>
#include <RmlUi/Core/DataModelHandle.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/ID.h>

#include <App/AppEvents.hpp>
#include <App/AppState.hpp>
#include <App/GameObjects/GameContactCheker.hpp>
#include <App/HardStrings.hpp>
#include <App/Physics/EntityFactory.hpp>
#include <Core/PathMeneger.hpp>
#include <Core/Types.hpp>
#include <Engine/AdvancedContext.hpp>
#include <Engine/OneRmlDocScene.hpp>
#include <Engine/Scene.hpp>
#include <Engine/SceneAction.hpp>
#include <Physics/Entity.hpp>
#include <Resources/ObjectFactory.hpp>
#include <Statistic/GameStatistic.hpp>
#include <Core/Random.hpp>

namespace scenes
{

class GameScene : public engine::OneRmlDocScene
{
private:
    class GameSceneListener : public Rml::EventListener
    {
    public:
        GameSceneListener(GameScene &scene) : scene_(scene)
        {
        }

        void init(Rml::ElementDocument *doc)
        {
            if (!doc)
                return;
            overlay = doc->GetElementById("pause-overlay");
        }

        void ProcessEvent(Rml::Event &ev) override
        {
            Rml::Element *el = ev.GetTargetElement();
            while (el && el->GetId().empty())
                el = el->GetParentNode();
            if (!el)
                return;
            const Rml::String &id = el->GetId();

            if (id == "pause-button")
            {
                overlay->SetClass("open", true);
                scene_.setPause(true);
            }
            else if (id == "btn-resume")
            {
                overlay->SetClass("open", false);
                scene_.setPause(false);
            }
            else if (id == "btn-restart")
            {
                overlay->SetClass("open", false);
                scene_.setPause(false);
                scene_.retart();
            }
            else if (id == "btn-exit")
            {
                scene_.actionRes_ = engine::SceneAction::popAction();
            }
        }

    private:
        GameScene &scene_;
        Rml::Element *overlay = nullptr;
    };

public:
    GameScene(engine::Context &context, const sdl3::Vector2i logicSize, app::AppState &appState) : engine::OneRmlDocScene(context, ui::gameMenu::file), listener_(*this), appState_(appState), packages_(core::PathManager::assets() / assets::packages), objectFactory_(packages_)
    {
        if (!objectFactory_.loadPack(appState.getCurrentPackageName()))
            SDL_Log("Failed to load object pack: coins");

        if (const auto *gs = appState_.stat().findById(objectFactory_.getActivePack()))
            stat_.record = static_cast<int>(gs->record);

        bindData();
        loadDocumentOrThrow();
        listener_.init(document());
        addEventListener(Rml::EventId::Click, &listener_, true);

        world_.SetContactListener(&contactCheker_);
        generateGlass(logicSize, {(float)logicSize.x, (float)logicSize.y * 0.75f}, 30);

        stat_.stringID = objectFactory_.getActivePack();

        
        if(auto pack = packages_.getPack(objectFactory_.getActivePack()); pack)
            settings_ =  pack->getSetings();

        SDL_Log("%d, %d\n", (int)settings_.levelRange.x, (int)settings_.levelRange.y);

        timer_.start();
    }
    ~GameScene()
    {
        appState_.stat().applyGameResult(stat_.stringID, stat_);

        if (dataHandle_)
        {
            // Освобождаем модель данных
            dataHandle_ = Rml::DataModelHandle();
            // Удаляем модель из контекста
            context_.getContext()->RemoveDataModel(ui::gameMenu::gameStats);
        }
    }

    void updateEvent(const SDL_Event &event) override
    {
        if (paused_)
            return;
        if (event.type == app::AppEventsType::COLLISION)
            updateCollision(event);
        else if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_AC_BACK)
            actionRes_ = engine::SceneAction::popAction();
        else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            if (event.button.y < startY_)
                return;
            startEntityObject(event.button.x);
        }
        else if (event.type == SDL_EVENT_MOUSE_MOTION)
        {
            if (event.button.y < startY_)
                return;
            if (prEntity_)
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
        if (paused_)
            return actionRes_;

        if (!prEntity_ && startTimer_.elapsedTimeS() >= settings_.summonTimeStepS)
            createPrEntity();

        world_.Step(dt, 8, 3);
        updateTime();

        for (auto it = objects_.begin(); it != objects_.end(); /* без инкремента здесь */)
        {
            if (it->getPosition().y > 2000.f)
            {
                addPoints(-it->getPoints());
                it = objects_.erase(it); // erase возвращает итератор на следующий элемент
            }
            else
            {
                // if(it->getPosition().y < startY_)
                // {
                //     SDL_Log("Game over\n");
                // }
                ++it;
            }
        }

        return actionRes_;
    }

private:
    GameSceneListener listener_;

private:
    bool paused_ = false;

    b2World world_{b2Vec2(0.0f, 9.81f)};
    objects::GameContactCheker contactCheker_;
    std::vector<physics::Entity> glass_;
    std::vector<objects::GameObject> objects_;

    resources::PackageContainer packages_;
    resources::ObjectFactory objectFactory_;

    std::unique_ptr<objects::GameObject> prEntity_ = nullptr;
    
    float startY_;
    float startX_;

    sdl3::Clock startTimer_;

    resources::PackageSettings settings_;
    core::Random<IDType> random_;

private:
    Rml::DataModelHandle dataHandle_;

    statistic::GameStatistic stat_;

    sdl3::Clock timer_;

    app::AppState &appState_;

private:
    void setPause(const bool pause)
    {
        paused_ = pause;
        timer_.pause(pause);
        startTimer_.pause(pause);
    }

    void retart()
    {
        timer_.start();
        stat_.gameCount = 0;
        objects_.clear();

        updateTime();
        addPoints(0);
    }

    void createPrEntity()
    {
        IDType level = random_(settings_.levelRange.x, settings_.levelRange.y);
        auto idpt = objectFactory_.getIdByLevel(level);
        if (!idpt.has_value())
        {
            SDL_Log("Error! Not found object by level 1");
            actionRes_ = engine::SceneAction::popAction();
        }
        auto created = objectFactory_.tryCreateById(world_, idpt.value(), {startX_, -startY_});
        if (!created)
            return;

        prEntity_ = std::make_unique<objects::GameObject>(std::move(*created));
        prEntity_->setEnabled(false);
    }
    void startEntityObject(const float xPos)
    {
        if(!prEntity_)
            return;
        prEntity_->setPosition({xPos, startY_});
        prEntity_->setEnabled(true);
        addPoints(prEntity_->getPoints());
        objects_.push_back(std::move(*prEntity_.get()));
        prEntity_.reset();
        startTimer_.start();
    }

    void bindData()
    {
        Rml::DataModelConstructor constructor = context_.getContext()->CreateDataModel(ui::gameMenu::gameStats);
        if (constructor)
        {
            constructor.Bind(ui::gameMenu::timeLabel, &stat_.time);
            constructor.Bind(ui::gameMenu::pointsLabel, &stat_.gameCount);
            constructor.Bind(ui::gameMenu::recordLabel, &stat_.record);
        }
        dataHandle_ = constructor.GetModelHandle();
    }

    void generateGlass(const sdl3::Vector2i logicSize, const sdl3::Vector2f glassSize, const float thikness)
    {
        glass_.clear();

        float yPos = logicSize.y;

        glass_.push_back(physics::EntityFactory::createRectangle(world_, {logicSize.x / 2.f, yPos}, {glassSize.x, thikness}, sdl3::Colors::Black, nullptr, b2BodyType::b2_staticBody));
        glass_.push_back(physics::EntityFactory::createRectangle(world_, {logicSize.x / 2.f - glassSize.x / 2.f, yPos - glassSize.y / 2.f}, {thikness, glassSize.y}, sdl3::Colors::Black, nullptr, b2BodyType::b2_staticBody));
        glass_.push_back(physics::EntityFactory::createRectangle(world_, {logicSize.x / 2.f + glassSize.x / 2.f, yPos - glassSize.y / 2.f}, {thikness, glassSize.y}, sdl3::Colors::Black, nullptr, b2BodyType::b2_staticBody));

        startY_ = (yPos - (glassSize.y)) / 2.f;
        startX_ = logicSize.x / 2.f;
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
        stat_.gameCount += points;
        dataHandle_.DirtyVariable(ui::gameMenu::pointsLabel);
    }

    void updateTime()
    {
        stat_.time = core::Time::fromSeconds(timer_.elapsedTimeS());
        dataHandle_.DirtyVariable(ui::gameMenu::timeLabel);
    }

    void updateCollision(const SDL_Event &event)
    {
        std::size_t obj1Ind = getByID(static_cast<IDType>(reinterpret_cast<uintptr_t>(event.user.data1)));
        std::size_t obj2Ind = getByID(static_cast<IDType>(reinterpret_cast<uintptr_t>(event.user.data2)));
        if (obj1Ind == objects_.size() || obj2Ind == objects_.size())
            return;

        objects::GameObject &obj1 = objects_[obj1Ind];
        objects::GameObject &obj2 = objects_[obj2Ind];

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

} // namespace scenes

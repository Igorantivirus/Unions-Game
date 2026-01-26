#pragma once

#include "Core/Types.hpp"
#include "Resources/ObjectPack.hpp"
#include "Resources/Types.hpp"
#include <memory>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
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
#include <App/Resources/ObjectFactory.hpp>
#include <App/Statistic/GameStatistic.hpp>
#include <Core/Managers/PathMeneger.hpp>
#include <Core/Random.hpp>
#include <Engine/AdvancedContext.hpp>
#include <Engine/OneRmlDocScene.hpp>

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

        void ProcessEvent(Rml::Event &ev) override
        {
            Rml::Element *el = ev.GetTargetElement();
            while (el && el->GetId().empty())
                el = el->GetParentNode();
            if (!el)
                return;

            const Rml::String &id = el->GetId();
            const std::string classes = el->GetClassNames();

            if (id == ui::gameMenu::pauseB)
            {
                scene_.setPause(true);
            }
            else if (id == ui::gameMenu::resumeWinB)
            {
                scene_.setPause(false);
                scene_.winOverlay->SetClass(ui::gameMenu::openClass, false);
            }
            else if (el->IsClassSet(ui::gameMenu::restartClass))
            {
                scene_.retart();
            }
            else if (el->IsClassSet(ui::gameMenu::exitClass))
            {
                scene_.actionRes_ = engine::SceneAction::popAction();
            }
            else if (el->IsClassSet(ui::gameMenu::resumeClass))
            {
                scene_.setPause(false);
            }
        }

    private:
        GameScene &scene_;
    };

public:
    GameScene(engine::Context &context, sdl3::audio::AudioDevice &audio, const sdl3::Vector2i logicSize, app::AppState &appState) : 
        engine::OneRmlDocScene(context, ui::gameMenu::file),
        audio_(audio),
        listener_(*this),
        appState_(appState),
        packages_(core::managers::PathManager::assets() / assets::packages, appState_.textures(), appState.audios()),
        objectFactory_(packages_)
    {
        if (!objectFactory_.loadPack(appState.getCurrentPackageName()))
            SDL_Log("Failed to load object pack: %s", appState.getCurrentPackageName().c_str());
        if (const auto *gs = appState_.stat().findById(objectFactory_.getActivePack()))
            stat_.record = static_cast<int>(gs->record);
        if (auto pack = packages_.getPack(objectFactory_.getActivePack()); pack)
            settings_ = pack->getSetings();

        bindData();
        loadDocumentOrThrow();
        addEventListener(Rml::EventId::Click, &listener_, true);
        gameOverOverlay = document()->GetElementById(ui::gameMenu::gameOverOverlayId);
        pauseOverlay = document()->GetElementById(ui::gameMenu::pauseOverlayId);
        winOverlay = document()->GetElementById(ui::gameMenu::winOverOverlayId);

        world_.SetContactListener(&contactCheker_);
        generateGlass(logicSize, {(float)logicSize.x, (float)logicSize.y * 0.75f}, 30);

        stat_.stringID = objectFactory_.getActivePack();

        timer_.start();
        startTimer_.start();
    }
    ~GameScene()
    {
        applyStatistic();
        if (dataHandle_)
        {
            dataHandle_ = Rml::DataModelHandle(); // Освобождаем модель данных

            context_.getContext()->RemoveDataModel(ui::gameMenu::gameStats); // Удаляем модель из контекста
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
            if (event.button.y < startPoss_.y)
                return;
            startEntityObject(event.button.x);
        }
        else if (event.type == SDL_EVENT_MOUSE_MOTION)
        {
            if (event.button.y < startPoss_.y)
                return;
            if (prEntity_)
                prEntity_->setPosition({event.motion.x, startPoss_.y});
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

    engine::SceneAction update(const float dt) override
    {
        if (paused_)
            return engine::OneRmlDocScene::update(dt);
        if (!prEntity_ && startTimer_.elapsedTimeS() >= settings_.summonTimeStepS)
            createPrEntity();
        world_.Step(dt, 8, 3);
        updateTime();
        updatecorrectnessElements();
        return engine::OneRmlDocScene::update(dt);
    }

private: // Сцена
    app::AppState &appState_;
    sdl3::audio::AudioDevice &audio_;
    bool paused_ = false;
    GameSceneListener listener_;
    Rml::Element *gameOverOverlay = nullptr;
    Rml::Element *pauseOverlay = nullptr;
    Rml::Element *winOverlay = nullptr;

private: // Информация на экране
    Rml::DataModelHandle dataHandle_;
    statistic::GameStatistic stat_;
    sdl3::Clock timer_;
    unsigned countDeath_ = 0;
    bool isWin_ = false;

private: // Физический мир
    b2World world_{b2Vec2(0.0f, 9.81f)};
    objects::GameContactCheker contactCheker_;
    std::vector<physics::Entity> glass_;
    std::vector<objects::GameObject> objects_;

private: // Информация о пакете
    resources::PackageContainer packages_;
    resources::ObjectFactory objectFactory_;
    resources::PackageSettings settings_;

private: // Временный объект
    sdl3::Clock startTimer_;
    std::unique_ptr<objects::GameObject> prEntity_ = nullptr;
    sdl3::Vector2f startPoss_;
    core::Random<IDType> random_;

private: // Сцена
    void setPause(const bool pause, const bool openPauseMenu = true)
    {
        if (paused_ == pause)
            return;
        paused_ = pause;
        timer_.pause(pause);
        startTimer_.pause(pause);
        startTimer_.start();
        if (openPauseMenu)
            pauseOverlay->SetClass(ui::gameMenu::openClass, pause);
    }

    void retart()
    {
        applyStatistic();
        setPause(false);

        prEntity_.reset();
        startTimer_.start();

        countDeath_ = 0;

        timer_.start();
        startTimer_.start();
        stat_.gameCount = 0;
        objects_.clear();

        updateTime();
        addPoints(0);

        dataHandle_.DirtyVariable(ui::gameMenu::deathLabel);
        gameOverOverlay->SetClass(ui::gameMenu::openClass, false);
    }

    void applyStatistic()
    {
        appState_.stat().applyGameResult(stat_.stringID, stat_);
    }

    void bindData()
    {
        Rml::DataModelConstructor constructor = context_.getContext()->CreateDataModel(ui::gameMenu::gameStats);
        if (constructor)
        {
            constructor.Bind(ui::gameMenu::timeLabel, &stat_.time);
            constructor.Bind(ui::gameMenu::pointsLabel, &stat_.gameCount);
            constructor.Bind(ui::gameMenu::recordLabel, &stat_.record);
            constructor.Bind(ui::gameMenu::deathLabel, &countDeath_);
            constructor.Bind(ui::gameMenu::maxDeathLabel, &settings_.deathCount);
        }
        dataHandle_ = constructor.GetModelHandle();
    }

    void addPoints(const int points)
    {
        stat_.gameCount += points;
        dataHandle_.DirtyVariable(ui::gameMenu::pointsLabel);
    }

    void addCountDeath()
    {
        ++countDeath_;
        dataHandle_.DirtyVariable(ui::gameMenu::deathLabel);
        if (countDeath_ >= settings_.deathCount)
        {
            gameOverOverlay->SetClass(ui::gameMenu::openClass, true);
            setPause(true, false);

            const resources::ObjectPack *pack = packages_.getPack(appState_.getCurrentPackageName());
            const resources::PackageMusic music = pack->getMusic();
            const sdl3::audio::Audio *audio = appState_.audios().get(music.loseFile);
            if (audio)
                audio_.playSound(*audio, false);
        }
    }

    void updateTime()
    {
        stat_.time = core::Time::fromSeconds(timer_.elapsedTimeS());
        dataHandle_.DirtyVariable(ui::gameMenu::timeLabel);
    }

private: // Физический мир
    void generateGlass(const sdl3::Vector2i logicSize, const sdl3::Vector2f glassSize, const float thikness)
    {
        glass_.clear();

        float yPos = logicSize.y;

        glass_.push_back(physics::EntityFactory::createRectangle(world_, {logicSize.x / 2.f, yPos}, {glassSize.x, thikness}, sdl3::Colors::Black, nullptr, b2BodyType::b2_staticBody));
        glass_.push_back(physics::EntityFactory::createRectangle(world_, {logicSize.x / 2.f - glassSize.x / 2.f, yPos - glassSize.y / 2.f}, {thikness, glassSize.y}, sdl3::Colors::Black, nullptr, b2BodyType::b2_staticBody));
        glass_.push_back(physics::EntityFactory::createRectangle(world_, {logicSize.x / 2.f + glassSize.x / 2.f, yPos - glassSize.y / 2.f}, {thikness, glassSize.y}, sdl3::Colors::Black, nullptr, b2BodyType::b2_staticBody));

        startPoss_ =
            {
                logicSize.x / 2.f,
                (yPos - (glassSize.y)) / 2.f};
    }

    std::size_t getByID(const IDType id)
    {
        for (std::size_t i = 0; i < objects_.size(); ++i)
            if (objects_[i].getID() == id)
                return i;
        return objects_.size();
    }

    void playSound(const resources::ObjectDef *def)
    {
        const sdl3::audio::Audio *audio = appState_.audios().get(def->soundFile);
        if (audio)
            audio_.playSound(*audio, false);
    }

    void checkWin(const IDType idSummonedObject)
    {
        if(isWin_)
            return;
        const resources::ObjectPack *pack = packages_.getPack(appState_.getCurrentPackageName());
        if (!pack || idSummonedObject != pack->getMaxLevel())
            return;
        const sdl3::audio::Audio *audio = appState_.audios().get(pack->getMusic().winFile);
        if (audio)
            audio_.playSound(*audio, false);

        winOverlay->SetClass(ui::gameMenu::openClass, true);
        setPause(true, false);
        isWin_ = true;
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

        checkWin(*mergedIdOpt);

        sdl3::Vector2f pos = (obj1.getShape().getPosition() + obj2.getShape().getPosition()) / 2.f;

        objects_.erase(objects_.begin() + std::max(obj1Ind, obj2Ind));
        objects_.erase(objects_.begin() + std::min(obj1Ind, obj2Ind));

        const resources::ObjectDef *def = objectFactory_.getDefById(*mergedIdOpt);
        auto created = objectFactory_.create(world_, def, pos);
        if (!created)
            return;
        playSound(def);

        objects_.push_back(std::move(*created));
        addPoints(objects_.back().getPoints());
    }

    void updatecorrectnessElements()
    {
        for (auto it = objects_.begin(); it != objects_.end();)
        {
            if (it->getPosition().y > 2000.f)
            {
                addPoints(-it->getPoints());
                it = objects_.erase(it); // erase возвращает итератор на следующий элемент
                addCountDeath();
            }
            else
                ++it;
        }
    }

private: // Временный объект
    void createPrEntity()
    {
        IDType level = random_(settings_.levelRange.x, settings_.levelRange.y);
        auto idpt = objectFactory_.getIdByLevel(level);
        if (!idpt.has_value())
        {
            SDL_Log("Error! Not found object by level %d\n", static_cast<int>(level));
            actionRes_ = engine::SceneAction::popAction();
        }
        auto created = objectFactory_.createById(world_, idpt.value(), {startPoss_.x, -startPoss_.y});
        if (!created)
            return;

        prEntity_ = std::make_unique<objects::GameObject>(std::move(*created));
        prEntity_->setEnabled(false);
    }
    void startEntityObject(const float xPos)
    {
        if (!prEntity_ || startTimer_.elapsedTimeS() < settings_.summonTimeStepS)
            return;
        prEntity_->setPosition({xPos, startPoss_.y});
        prEntity_->setEnabled(true);
        addPoints(prEntity_->getPoints());
        objects_.push_back(std::move(*prEntity_.get()));
        prEntity_.reset();
        startTimer_.start();
    }
};

} // namespace scenes

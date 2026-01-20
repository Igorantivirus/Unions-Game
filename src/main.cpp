#include "App/HardStrings.hpp"
#include "App/Scenes/IDs.hpp"
#include "Core/PathMeneger.hpp"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>

#include <App/AppScenesFactory.hpp>
#include <App/AppState.hpp>
#include <App/HardStrings.hpp>
#include <App/Scenes/MainMenuScene.hpp>
#include <Engine/Engine.hpp>


static engine::Engine game;
static app::AppStatePtr appState;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    core::PathManager::init();

    appState = std::make_shared<app::AppState>(core::PathManager::workFolder() / names::statisticFile, core::PathManager::assets() / names::statisticFile);

    engine::EngineSettings settings;

    settings.appName = names::windowName;
    settings.icoFile = core::PathManager::assets() / names::mainIco;
    settings.fontFile = core::PathManager::assets() / assets::fontPath;
    settings.windowSize = {576, 1024};
    settings.autoOrientationEnabled = false;
    settings.fps = 60;
    settings.mode = SDL_LOGICAL_PRESENTATION_LETTERBOX;
    settings.startSceneID = scenes::ids::mainMenu;
    settings.setLogicalPresentation = true;
    settings.scenesFabrick = std::make_unique<app::AppScenesFactory>(appState);

    return game.start(std::move(settings));
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    return game.updateEvents(*event);
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    return game.iterate();
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (appState)
        appState->save();
    game.close();
}
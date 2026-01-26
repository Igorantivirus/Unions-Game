#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_main.h>


#include <App/AppScenesFactory.hpp>
#include <App/AppState.hpp>
#include <App/HardStrings.hpp>
#include <App/Scenes/IDs.hpp>
#include <App/Scenes/MainMenuScene.hpp>
#include <Core/Managers/PathMeneger.hpp>
#include <Engine/Engine.hpp>


static engine::Engine game;
static app::AppState appState;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (!MIX_Init())
    {
        SDL_Log("Couldn't initialize SDL_mixer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");

    core::managers::PathManager::init();

    appState.setWorkStatisticFile(core::managers::PathManager::workFolder() / names::statisticFile);
    appState.setAssetsStatisticFile(core::managers::PathManager::assets() / names::statisticFile);

    if (!appState.load())
    {
        SDL_Log("Error! appSate not loaded");
        return SDL_APP_FAILURE;
    }
    auto fabrick = std::make_unique<app::AppScenesFactory>();
    fabrick->setAppState(appState);

    engine::EngineSettings settings;

    settings.appName = names::windowName;
    settings.icoFile = core::managers::PathManager::assets() / names::mainIco;
    settings.fontFile = core::managers::PathManager::assets() / assets::fontPath;
    settings.windowSize = {576, 1024};
    settings.autoOrientationEnabled = false;
    settings.fps = 60;
    settings.mode = SDL_LOGICAL_PRESENTATION_LETTERBOX;
    settings.startSceneID = scenes::ids::mainMenu;
    settings.setLogicalPresentation = true;
    settings.scenesFabrick = std::move(fabrick);

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
    appState.save();
    game.close();
}

#pragma once

#include <string_view>
#include <filesystem>
#include <memory>

#include "Core/Types.hpp"
#include "SceneFabrick.hpp"

namespace engine
{

struct EngineSettings
{
    std::string_view appName;
    std::filesystem::path icoFile;
    std::filesystem::path fontFile;

    sdl3::Vector2i windowSize;
    bool autoOrientationEnabled = false;

    std::unique_ptr<SceneFabrick> scenesFabrick;

    bool setLogicalPresentation = false;
    SDL_RendererLogicalPresentation mode = SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_DISABLED;

    unsigned int fps = 0;
    IDType startSceneID = 0;

};

} // namespace engine
#pragma once

#include <memory>

#include "Scene.hpp"

namespace engine
{

class SceneFabrick
{
public:
    virtual ~SceneFabrick() = default;

    virtual ScenePtr genSceneByID(const IDType id) const = 0;

};

using SceneFabrickPtr = std::unique_ptr<SceneFabrick>;

} // namespace engine
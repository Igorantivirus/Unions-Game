#pragma once

#include "Scene.hpp"
#include "Types.hpp"
#include <memory>

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
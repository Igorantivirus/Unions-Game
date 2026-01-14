#pragma once

#include <memory>

#include "Engine/AdvancedContext.hpp"
#include "Scene.hpp"
#include "AdvancedContext.hpp"

namespace engine
{

class SceneFabrick
{
public:
    virtual ~SceneFabrick() = default;

    virtual ScenePtr genSceneByID(const IDType id) const = 0;

    void setContext(Context& context)
    {
        context_ = &context;
    }

    Context& getContext() const
    {
        return *context_;
    }

protected:

    Context* context_;

};

using SceneFabrickPtr = std::unique_ptr<SceneFabrick>;

} // namespace engine
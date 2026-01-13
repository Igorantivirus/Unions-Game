#pragma once

#include <variant>

#include <Core/Types.hpp>

namespace engine
{

    enum class SceneActionType
    {
        None,
        PopScene,
        PushScene,
        SwitchScene,
        Exit
    };

    struct SceneAction
    {
        SceneActionType type = SceneActionType::None;
        std::variant<std::monostate, IDType> value = std::monostate{};
    };

}
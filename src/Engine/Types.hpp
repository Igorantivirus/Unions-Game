#pragma once

#include <variant>
namespace engine
{
    using IDType = unsigned short;

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
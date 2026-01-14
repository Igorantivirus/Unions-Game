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
public:
    SceneActionType type = SceneActionType::None;
    std::variant<std::monostate, IDType> value = std::monostate{};

    static SceneAction noneAction()
    {
        return SceneAction{};
    }
    static SceneAction exitAction()
    {
        return SceneAction{SceneActionType::Exit};
    }

private:
    SceneAction(const SceneActionType actType = SceneActionType::None) : type{actType}
    {}

};

} // namespace engine
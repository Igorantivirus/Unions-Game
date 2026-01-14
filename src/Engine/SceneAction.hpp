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
    static SceneAction nextAction(const IDType nextID)
    {
        SceneAction res{SceneActionType::PushScene};
        res.value = nextID;
        return res;
    }
    static SceneAction popAction()
    {
        return SceneAction{SceneActionType::PopScene};
    }

private:
    SceneAction(const SceneActionType actType = SceneActionType::None) : type{actType}
    {}

};

} // namespace engine
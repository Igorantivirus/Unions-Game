#pragma once

#include <string>
#include <vector>
#include <variant>

#include <Core/Types.hpp>

#include <SDLWrapper/Names.hpp>

namespace resources
{
enum class ObjectFormType
{
    Circle,
    Ellipse,
    Polygon8
};

struct ObjectFormDef
{
    ObjectFormType type = ObjectFormType::Circle;

    // float                       - Circle radius
    // sdl3::Vector2f              - Ellipse radii
    // std::vector<sdl3::Vector2f> - Polygon8
    std::variant<float, sdl3::Vector2f, std::vector<sdl3::Vector2f>> form = 0.f;
};

struct ObjectDef
{
    IDType id = 0;
    IDType level = 0;
    int points = 0;

    ObjectFormDef form;
    std::string texturePath;
};
} // namespace resources
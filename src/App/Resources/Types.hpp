#pragma once

#include <string>
#include <variant>
#include <vector>

#include <SDLWrapper/Math/Colors.hpp>
#include <SDLWrapper/Names.hpp>

#include <Core/Types.hpp>

namespace resources
{
enum class ObjectFormType : unsigned char
{
    Circle,
    Ellipse,
    Polygon,
    Rectangle
};
enum class ObjectFillerType : unsigned char
{
    Texture,
    Color
};

struct ObjectFormDef
{
public:
    ObjectFormType type = ObjectFormType::Circle;

    // float                       - Circle radius
    // sdl3::Vector2f              - Ellipse radii or Rectangle size
    // std::vector<sdl3::Vector2f> - Polygon8
    std::variant<float, sdl3::Vector2f, std::vector<sdl3::Vector2f>> form = 0.f;

public:
    float getRadius() const
    {
        if (std::holds_alternative<float>(form))
            return std::get<float>(form);
        return 0.f;
    }
    sdl3::Vector2f getRadii() const
    {
        if (std::holds_alternative<sdl3::Vector2f>(form))
            return std::get<sdl3::Vector2f>(form);
        return {0.f, 0.f};
    }
    sdl3::Vector2f getSize() const
    {
        if (std::holds_alternative<sdl3::Vector2f>(form))
            return std::get<sdl3::Vector2f>(form);
        return {0.f, 0.f};
    }
    std::vector<sdl3::Vector2f> getPolygon() const
    {
        if (std::holds_alternative<std::vector<sdl3::Vector2f>>(form))
            return std::get<std::vector<sdl3::Vector2f>>(form);
        return {};
    }
};

struct ObjectFillerDef
{
public:
    ObjectFillerType type = ObjectFillerType::Color;

    // std::string - Texture
    // sdl3::Color - Color
    std::variant<std::string, sdl3::Color> filler = sdl3::Colors::White;

public:
    std::string getTextureName() const
    {
        if (std::holds_alternative<std::string>(filler))
            return std::get<std::string>(filler);
        return {};
    }
    sdl3::Color getColor() const
    {
        if (std::holds_alternative<sdl3::Color>(filler))
            return std::get<sdl3::Color>(filler);
        return sdl3::Colors::White;
    }
};

struct ObjectDef
{
    IDType id = 0;
    IDType level = 0;
    int points = 0;

    ObjectFormDef form;
    ObjectFillerDef filler;
};

} // namespace resources
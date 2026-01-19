#pragma once

#include "GameObject.hpp"
#include <App/PhysicBase/EntityFactory.hpp>
#include <Core/Types.hpp>
#include <SDLWrapper/DrawTransformObjects/EllipseShape.hpp>
#include <SDLWrapper/Math/Colors.hpp>

namespace GameEntityFactory
{

// sdl3::EllipseShape

GameObject createById(b2World &world, const IDType id, sdl3::Vector2f pos)
{
    if (id == 1)
        return GameObject(EntityFactory::createEllipse(world, pos, {30, 40}, sdl3::Colors::Blue), id, 1);
    if (id == 2)
        return GameObject(EntityFactory::createEllipse(world, pos, {50, 60}, sdl3::Colors::Yellow), id, 3);
    if (id == 3)
        return GameObject(EntityFactory::createEllipse(world, pos, {60, 60}, sdl3::Colors::Red), id, 10);
    if (id == 4)
        return GameObject(EntityFactory::createEllipse(world, pos, {80, 80}, sdl3::Colors::Magenta), id, 20);
    return GameObject(EntityFactory::createEllipse(world, pos, {90, 95}, sdl3::Colors::Green), id, 30);
}

} // namespace GameEntityFactory
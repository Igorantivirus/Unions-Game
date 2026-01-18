#pragma once

#include <Core/Types.hpp>
#include <SDLWrapper/DrawTransformObjects/EllipseShape.hpp>
#include <SDLWrapper/Math/Colors.hpp>
#include "EntityFactory.hpp"
#include "GameObject.hpp"

namespace GameEntityFactory
{

    // sdl3::EllipseShape 

    GameObject createById(b2World &world, const IDType id, sdl3::Vector2f pos)
    {
        if(id == 1) return GameObject(EntityFactory::createEllipse(world, pos, {30, 40}, sdl3::Colors::Blue), id);
        if(id == 2) return GameObject(EntityFactory::createEllipse(world, pos, {50, 60}, sdl3::Colors::Yellow), id);
        if(id == 3) return GameObject(EntityFactory::createEllipse(world, pos, {60, 60}, sdl3::Colors::Red), id);
        if(id == 4) return GameObject(EntityFactory::createEllipse(world, pos, {80, 80}, sdl3::Colors::Magenta), id);
        return GameObject(EntityFactory::createEllipse(world, pos, {90, 95}, sdl3::Colors::Green),id);
    }

}
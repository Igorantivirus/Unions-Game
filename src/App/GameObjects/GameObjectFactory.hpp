#pragma once

#include <Core/Types.hpp>
#include <SDLWrapper/Math/Colors.hpp>
#include "Entity.hpp"
#include "EntityFactory.hpp"

namespace GameEntityFactory
{

    Entity createById(b2World &world, const IDType id, sdl3::Vector2f pos)
    {
        if(id == 1) return EntityFactory::createEllipse(world, pos, {30, 40}, sdl3::Colors::Blue);
        if(id == 2) return EntityFactory::createEllipse(world, pos, {50, 60}, sdl3::Colors::Yellow);
        if(id == 3) return EntityFactory::createEllipse(world, pos, {60, 60}, sdl3::Colors::Red);
        if(id == 4) return EntityFactory::createEllipse(world, pos, {80, 80}, sdl3::Colors::Magenta);
        return EntityFactory::createEllipse(world, pos, {90, 95}, sdl3::Colors::Green);
    }

}
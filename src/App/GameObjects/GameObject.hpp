#pragma once

#include <Core/Types.hpp>

#include "Entity.hpp"

class GameObject : public Entity
{
public:
    GameObject(b2Body *body, std::unique_ptr<sdl3::Shape> shape, const IDType level) :
        Entity(body, std::move(shape)), level_(level)
    {
    }
    GameObject(Entity &&entity, const IDType level) :
        Entity(std::move(entity)), level_(level)
    {
    }

    const IDType getLevel() const
    {
        return level_;
    }

private:
    IDType level_ = 0;
};
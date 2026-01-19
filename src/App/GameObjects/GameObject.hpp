#pragma once

#include <Core/Types.hpp>

#include <App/PhysicBase/Entity.hpp>

class GameObject : public Entity
{
public:
    GameObject(b2Body *body, std::unique_ptr<sdl3::Shape> shape, const IDType level, const int points) :
        Entity(body, std::move(shape)), level_(level), points_(points)
    {
    }
    GameObject(Entity &&entity, const IDType level, const int points) :
        Entity(std::move(entity)), level_(level), points_(points)
    {
    }

    const IDType getLevel() const
    {
        return level_;
    }
    const int getPoints() const
    {
        return points_;
    }

private:
    IDType level_ = 0;
    int points_ = 0;
};
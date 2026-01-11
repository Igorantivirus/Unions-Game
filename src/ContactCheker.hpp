#pragma once

#include <SDLWrapper/Math/Colors.hpp>
#include <box2d/b2_body.h>
#include <box2d/b2_world_callbacks.h>

#include "Entity.hpp"
#include "deleteQueue.hpp"

inline bool operator==(const sdl3::Color &left, const sdl3::Color &right)
{
    return left.r == right.r &&
           left.g == right.g &&
           left.b == right.b &&
           left.a == right.a;
}

class ContactCheker : public b2ContactListener
{
public:
    void BeginContact(b2Contact *contact) override
    {
        b2Fixture *fixtureA = contact->GetFixtureA();
        b2Fixture *fixtureB = contact->GetFixtureB();

        b2Body *bodyA = fixtureA->GetBody();
        b2Body *bodyB = fixtureB->GetBody();

        // дальше получаете user data из тел или фикстур
        Entity *objA = reinterpret_cast<Entity *>(bodyA->GetUserData().pointer);
        Entity *objB = reinterpret_cast<Entity *>(bodyB->GetUserData().pointer);
        if (!objA || !objB || objA->getBody()->GetType() == b2_staticBody || objB->getBody()->GetType() == b2_staticBody)
            return;
        if (objA->getShape().getFillColor() == objB->getShape().getFillColor())
        {
            deleteQueue.push(objA->getID());
            deleteQueue.push(objB->getID());
        }
    }

    void EndContact(b2Contact *contact) override
    {
        // контакт закончился
    }
};

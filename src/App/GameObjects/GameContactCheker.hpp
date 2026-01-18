#pragma once

#include <App/AppEvents.hpp>
#include <Core/EventAdder.hpp>

#include <box2d/b2_body.h>
#include <box2d/b2_world_callbacks.h>

#include "GameObject.hpp"

class GameContactCheker : public b2ContactListener
{
public:
    void BeginContact(b2Contact *contact) override
    {
        b2Fixture *fixtureA = contact->GetFixtureA();
        b2Fixture *fixtureB = contact->GetFixtureB();

        b2Body *bodyA = fixtureA->GetBody();
        b2Body *bodyB = fixtureB->GetBody();

        // дальше получаете user data из тел или фикстур
        GameObject *objA = reinterpret_cast<GameObject *>(bodyA->GetUserData().pointer);
        GameObject *objB = reinterpret_cast<GameObject *>(bodyB->GetUserData().pointer);
        if (!objA || !objB || !objA->isEnabled() || !objB->isEnabled() || objA->getBody()->GetType() == b2_staticBody || objB->getBody()->GetType() == b2_staticBody)
            return;
        if (objA->getLevel() == objB->getLevel())
            core::createEventCorrect(AppEventsType::COLLISION, objA->getLevel(), objA->getID(), objB->getID());
    }

    void EndContact(b2Contact *contact) override
    {
        // контакт закончился
    }
};

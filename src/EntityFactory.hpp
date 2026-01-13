#pragma once

#include <SDLWrapper/Names.hpp>

#include "Config.hpp"
#include "Entity.hpp"

namespace EntityFactory
{

// Вспомогательная функция для инициализации базовых параметров тела
inline b2Body *createBaseBody(b2World &world, const sdl3::Shape &shape, b2BodyDef bd)
{
    bd.position.Set(shape.getPosition().x * Config::MPP, shape.getPosition().y * Config::MPP);
    bd.angle = shape.getRotation() * SDL_PI_F / 180.f;
    return world.CreateBody(&bd);
}

// 1. Перегрузка для ПРЯМОУГОЛЬНИКА
inline Entity createFromShape(b2World &world, const sdl3::RectangleShape &rect, b2BodyDef bd, b2FixtureDef fd)
{
    auto shapeCopy = std::make_unique<sdl3::RectangleShape>(rect);
    sdl3::Vector2f boxSize = shapeCopy->getSize() * 0.5f * Config::MPP;
    b2Body *body = createBaseBody(world, *shapeCopy, std::move(bd));
    b2PolygonShape box;
    box.SetAsBox(boxSize.x, boxSize.y);
    fd.shape = &box;
    body->CreateFixture(&fd);
    return Entity(body, std::move(shapeCopy));
}

// 2. Перегрузка для ЭЛЛИПСА
inline Entity createFromShape(b2World &world, const sdl3::EllipseShape &ellipse, b2BodyDef bd, b2FixtureDef fd, const unsigned segments = 24)
{
    auto shapeCopy = std::make_unique<sdl3::EllipseShape>(ellipse);
    const sdl3::Vector2f bodyRadii = shapeCopy->getRadii() * Config::MPP;

    b2Body *body = createBaseBody(world, *shapeCopy, std::move(bd));

    for (int i = 0; i < segments; ++i)
    {
        float a1 = (float)i / segments * 2.0f * b2_pi;
        float a2 = (float)(i + 1) / segments * 2.0f * b2_pi;

        b2Vec2 v[3] = {{0, 0}, {bodyRadii.x * cosf(a1), bodyRadii.y * sinf(a1)}, {bodyRadii.x * cosf(a2), bodyRadii.y * sinf(a2)}};
        b2PolygonShape sector;
        sector.Set(v, 3);

        b2FixtureDef fdPr = fd;
        fdPr.shape = &sector;

        body->CreateFixture(&fdPr);
    }

    return Entity(body, std::move(shapeCopy));
}

// --- Методы для создания по параметрам ---

inline Entity createRectangle(b2World &world, sdl3::Vector2f pos, sdl3::Vector2f size, sdl3::Color color, b2BodyType type = b2_dynamicBody)
{
    sdl3::RectangleShape rect(size);
    rect.setOrigin(size / 2.f);
    rect.setPosition(pos);
    rect.setFillColor(color);

    b2BodyDef bd;
    bd.type = type;
    b2FixtureDef fd;
    fd.density = (type == b2_staticBody) ? 0.0f : Config::defaultDensity;
    fd.friction = Config::defaultFrictionRect;

    return createFromShape(world, rect, bd, fd);
}

inline Entity createEllipse(b2World &world, sdl3::Vector2f pos, sdl3::Vector2f radii, sdl3::Color color, b2BodyType type = b2_dynamicBody)
{
    sdl3::EllipseShape ell(radii);
    ell.setPosition(pos);
    ell.setFillColor(color);

    b2BodyDef bd;
    bd.type = type;
    b2FixtureDef fd;
    fd.density = (type == b2_staticBody) ? 0.0f : Config::defaultDensity;
    fd.friction = Config::defaultFrictionEllipse;

    return createFromShape(world, ell, bd, fd);
}
} // namespace EntityFactory
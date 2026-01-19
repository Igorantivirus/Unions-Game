#pragma once

#include <SDLWrapper/DrawTransformObjects/CircleShape.hpp>
#include <SDLWrapper/Names.hpp>

#include "Config.hpp"
#include "Entity.hpp"
#include "box2d/b2_circle_shape.h"

#include "PolygonShape.hpp"

#include <array>
#include <box2d/b2_polygon_shape.h>

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

// 3. Перегрузка для ОКРУЖНОСТИ
inline Entity createFromShape(b2World &world, const sdl3::CircleShape &circle, b2BodyDef bd, b2FixtureDef fd)
{
    auto shapeCopy = std::make_unique<sdl3::CircleShape>(circle);

    b2Body *body = createBaseBody(world, *shapeCopy, std::move(bd));
    b2CircleShape shape;
    shape.m_radius = circle.getRadius() * Config::MPP;
    fd.shape = &shape;
    body->CreateFixture(&fd);
    return Entity(body, std::move(shapeCopy));
}

// 4. Перегрузка для ПРОИЗВОЛЬНОГО ПОЛИГОНА (может быть невыпуклый)
// ПРИМЕЧАНИЕ: После изменений этот метод корректно работает только для выпуклых полигонов.
inline Entity createFromShape(b2World &world, const sdl3::PolygonShape &polyShape, b2BodyDef bd, b2FixtureDef fd)
{
    auto shapeCopy = std::make_unique<sdl3::PolygonShape>(polyShape);

    const auto &points = shapeCopy->getPoints();
    if (points.size() < 3)
    {
        // Недостаточно точек для создания хотя бы одного треугольника
        b2Body *body = createBaseBody(world, *shapeCopy, std::move(bd));
        return Entity(body, std::move(shapeCopy));
    }

    // Локальные -> Box2D (метры)
    std::vector<b2Vec2> verts;
    verts.reserve(points.size());
    for (const auto &p : points)
    {
        verts.emplace_back(p.x * Config::MPP, p.y * Config::MPP);
    }

    // --- НАЧАЛО ИЗМЕНЕНИЙ ---
    // Упрощенная веерная триангуляция.
    // Вместо сложного алгоритма "ear clipping" создаем треугольники,
    // используя общую вершину в локальной точке (0,0).
    std::vector<std::array<b2Vec2, 3>> triangles;
    const b2Vec2 centerPoint(0.0f, 0.0f);

    // Создаем треугольники, соединяя каждую пару соседних вершин с центром.
    for (size_t i = 0; i < verts.size(); ++i)
    {
        // Берем текущую вершину и следующую (с замыканием на первую)
        const b2Vec2 &p1 = verts[i];
        const b2Vec2 &p2 = verts[(i + 1) % verts.size()];

        // Треугольник состоит из центра и двух соседних вершин полигона.
        std::array<b2Vec2, 3> tri = {centerPoint, p1, p2};
        triangles.push_back(tri);
    }
    // --- КОНЕЦ ИЗМЕНЕНИЙ ---

    b2Body *body = createBaseBody(world, *shapeCopy, std::move(bd));

    int i = 0;
    const float minArea2 = 1e-4f;

    for (const auto &tri : triangles)
    {
        ++i;

        b2PolygonShape poly;
        poly.Set(tri.data(), 3);

        // 2) проверяем, что Box2D реально оставил >= 3 вершин
        if (poly.m_count < 3)
        {
            SDL_Log("Degenerate triangle after Box2D cleanup, skip #%d (count=%d)\n",
                    i, poly.m_count);
            continue;
        }

        b2FixtureDef fdPr = fd;
        fdPr.shape = &poly;
        body->CreateFixture(&fdPr);
    }

    return Entity(body, std::move(shapeCopy));
}

// --- Методы для создания по параметрам ---

inline Entity createRectangle(b2World &world, sdl3::Vector2f pos, sdl3::Vector2f size, sdl3::Color color, const sdl3::Texture *texture = nullptr, b2BodyType type = b2_dynamicBody)
{
    sdl3::RectangleShape rect(size);
    rect.setOrigin(size / 2.f);
    rect.setPosition(pos);
    rect.setFillColor(color);
    if (texture)
        rect.setTexture(*texture);

    b2BodyDef bd;
    bd.type = type;
    b2FixtureDef fd;
    fd.density = (type == b2_staticBody) ? 0.0f : Config::defaultDensity;
    fd.friction = Config::defaultFrictionRect;

    return createFromShape(world, rect, bd, fd);
}

inline Entity createEllipse(b2World &world, sdl3::Vector2f pos, sdl3::Vector2f radii, sdl3::Color color, const sdl3::Texture *texture = nullptr, b2BodyType type = b2_dynamicBody)
{
    sdl3::EllipseShape ell(radii);
    ell.setPosition(pos);
    ell.setFillColor(color);
    if (texture)
        ell.setTexture(*texture);

    b2BodyDef bd;
    bd.type = type;
    b2FixtureDef fd;
    fd.density = (type == b2_staticBody) ? 0.0f : Config::defaultDensity;
    fd.friction = Config::defaultFrictionEllipse;

    return createFromShape(world, ell, bd, fd);
}

inline Entity createCircle(b2World &world, sdl3::Vector2f pos, const float radius, sdl3::Color color, const sdl3::Texture *texture = nullptr, b2BodyType type = b2_dynamicBody)
{
    sdl3::CircleShape circ(radius);
    circ.setPosition(pos);
    circ.setFillColor(color);
    if (texture)
        circ.setTexture(*texture);

    b2BodyDef bd;
    bd.type = type;
    b2FixtureDef fd;
    fd.density = (type == b2_staticBody) ? 0.0f : Config::defaultDensity;
    fd.friction = Config::defaultFrictionCircle;

    return createFromShape(world, circ, bd, fd);
}

inline Entity createPolygon(b2World &world, sdl3::Vector2f pos, const std::vector<sdl3::Vector2f> &points, sdl3::Color color, const sdl3::Texture *texture = nullptr, b2BodyType type = b2_dynamicBody)
{
    sdl3::PolygonShape poly(points);
    poly.setPosition(pos);
    poly.setFillColor(color);
    if (texture)
        poly.setTexture(*texture);

    b2BodyDef bd;
    bd.type = type;

    b2FixtureDef fd;
    fd.density = (type == b2_staticBody) ? 0.0f : Config::defaultDensity;
    // можешь завести свой Config::defaultFrictionPolygon,
    // пока возьмём, например, прямоугольник:
    fd.friction = Config::defaultFrictionRect;

    return createFromShape(world, poly, bd, fd);
}

} // namespace EntityFactory
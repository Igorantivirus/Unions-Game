#pragma once

#include <cmath>

#include <SDLWrapper/DrawTransformObjects/EllipseShape.hpp>
#include <SDLWrapper/DrawTransformObjects/RectangleShape.hpp>
#include <SDLWrapper/Names.hpp>
#include <SDLWrapper/ObjectBase/Shape.hpp>
#include <SDLWrapper/ObjectBase/Transformable.hpp>

#include "../EngineConfig.hpp"


struct IPhysicsShape
{
    virtual ~IPhysicsShape() = default;
    virtual sdl3::Vector2f getCenterPosition() const = 0;
    virtual sdl3::Vector2f support(const sdl3::Vector2f &direction) const = 0;

    virtual sdl3::Shape &getShape() = 0;
    virtual const sdl3::Shape &getShape() const = 0;
};

struct EllipseCollider : public IPhysicsShape
{
public:
    sdl3::EllipseShape ellipse;

public:
    EllipseCollider() = default;
    EllipseCollider(sdl3::EllipseShape ell) : ellipse{std::move(ell)}
    {
    }

    sdl3::Vector2f getCenterPosition() const override
    {
        const auto &M = ellipse.getTransformMatrix();
        return M.transform({0, 0});
    }
    sdl3::Vector2f support(const sdl3::Vector2f &direction) const override
    {
        const float eps = engine::cfg::epsilon;
        if (direction.x * direction.x + direction.y * direction.y < eps)
            return getCenterPosition();

        const auto radii = ellipse.getRadii();
        const float a = radii.x;
        const float b = radii.y;

        const auto &M = ellipse.getTransformMatrix();

        // 1) Переводим направление из мира в локальное пространство.
        // Для этого нужна обратная матрица: invM.
        // Важно: direction — это ВЕКТОР, поэтому w = 0 (без переноса).
        const sdl3::Vector2f dLocal{
            M.a * direction.x + M.b * direction.y,
            M.c * direction.x + M.d * direction.y,
        };

        const float dl2 = dLocal.x * dLocal.x + dLocal.y * dLocal.y;
        if (dl2 < eps)
            return getCenterPosition();

        // 2) Находим крайнюю точку на локальном эллипсе (центр в 0,0):
        // p = (a^2*dx / sqrt((a*dx)^2 + (b*dy)^2),
        //      b^2*dy / sqrt((a*dx)^2 + (b*dy)^2))
        const float denom = std::sqrt((a * dLocal.x) * (a * dLocal.x) +
                                      (b * dLocal.y) * (b * dLocal.y));
        if (denom < eps)
            return getCenterPosition();

        const sdl3::Vector2f pLocal{
            (a * a) * dLocal.x / denom,
            (b * b) * dLocal.y / denom};

        // 3) Возвращаем локальную точку обратно в мир (w = 1, т.к. это ТОЧКА с переносом).
        const sdl3::Vector3f pW3 = M.transform({pLocal.x, pLocal.y, 1.f});
        return {pW3.x, pW3.y};
    }

    sdl3::Shape &getShape() override
    {
        return ellipse;
    }
    const sdl3::Shape &getShape() const override
    {
        return ellipse;
    }
};

struct RectangleCollider : public IPhysicsShape
{
public:
    sdl3::RectangleShape rect;

public:
    RectangleCollider() = default;
    RectangleCollider(sdl3::RectangleShape rct) : rect{std::move(rct)}
    {
    }

    sdl3::Vector2f getCenterPosition() const override
    {
        const auto size = rect.getSize();
        const auto &M = rect.getTransformMatrix();
        // Локальный центр (относительно top-left 0,0)
        sdl3::Vector2f localCenter = {size.x * 0.5f, size.y * 0.5f};

        // Трансформируем точку в мировые координаты
        // Используем 3D версию transform или перегрузку для точки (w=1)
        const sdl3::Vector3f centerW3 = M.transform({localCenter.x, localCenter.y, 1.f});
        return {centerW3.x, centerW3.y};
    }
    sdl3::Vector2f support(const sdl3::Vector2f &direction) const override
    {
        const auto size = rect.getSize();
        const auto &M = rect.getTransformMatrix();

        // 1. Генерируем 4 вершины прямоугольника в локальных координатах
        // Координаты (0,0) — это верхний левый угол в локальной системе Rectanglesdl3::Shape
        sdl3::Vector2f localVertices[4] = {
            {0.f, 0.f},       // Top-Left
            {size.x, 0.f},    // Top-Right
            {size.x, size.y}, // Bottom-Right
            {0.f, size.y}     // Bottom-Left
        };

        sdl3::Vector2f bestPoint;
        float maxDot = -std::numeric_limits<float>::infinity();

        // 2. Ищем вершину, максимально удаленную в направлении direction
        for (const auto &vLocal : localVertices)
        {
            // Переводим локальную вершину в мировые координаты
            sdl3::Vector3f vWorld3 = M.transform({vLocal.x, vLocal.y, 1.f});
            sdl3::Vector2f vWorld = {vWorld3.x, vWorld3.y};

            // Скалярное произведение: насколько точка "выдвинута" вдоль direction
            float dot = vWorld.x * direction.x + vWorld.y * direction.y;

            if (dot > maxDot)
            {
                maxDot = dot;
                bestPoint = vWorld;
            }
        }

        return bestPoint;
    }

    sdl3::Shape &getShape() override
    {
        return rect;
    }
    const sdl3::Shape &getShape() const override
    {
        return rect;
    }
};

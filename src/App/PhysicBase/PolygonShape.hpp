#pragma once

#include <SDLWrapper/ObjectBase/Shape.hpp>

#include <SDLWrapper/Names.hpp>

#include <cstddef>
#include <vector>

namespace sdl3
{

// Простой многоугольник для отрисовки
class PolygonShape : public Shape
{
public:
    PolygonShape() = default;
    explicit PolygonShape(std::vector<Vector2f> points) : points_(std::move(points))
    {
        updateLocalGeometry();
    }

    void setPoints(std::vector<Vector2f> points)
    {
        points_ = std::move(points);
        updateLocalGeometry();
    }

    const std::vector<Vector2f> &getPoints() const
    {
        return points_;
    }

    std::size_t getPointCount() const override
    {
        return points_.size();
    }

    Vector2f getPoint(const std::size_t index) const override
    {
        return points_[index];
    }

private:
    std::vector<Vector2f> points_;
};

} // namespace sdl3


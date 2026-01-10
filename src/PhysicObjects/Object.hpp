#pragma once

#include <memory>

#include <SDLWrapper/ObjectBase/Drawable.hpp>
#include <SDLWrapper/Renders/RenderTarget.hpp>

#include "PhysicBody.hpp"
#include "PhysicsShapes.hpp"

struct Object : public sdl3::Drawable
{
public:
    PhysicBody body;
    std::unique_ptr<IPhysicsShape> shape;

public:
    void update(const float dt)
    {
        body.update(dt, shape->getShape());
    }

private:
    void draw(sdl3::RenderTarget &target) const override
    {
        target.draw(shape->getShape());
    }
};

namespace ObjectFactory
{
Object generateEllipse(const sdl3::Vector2f &radii, const sdl3::Vector2f &position, const sdl3::Color &color, PhysicBody physic = PhysicBody{})
{
    Object res;
    res.body = std::move(physic);

    EllipseCollider el;
    el.ellipse.setRadii(radii);
    el.ellipse.setPosition(position);
    el.ellipse.setFillColor(color);
    res.shape = std::make_unique<EllipseCollider>(el);

    const float m = res.body.getMass();
    const float inertia = m * 0.25f * (radii.x * radii.x + radii.y * radii.y);
    res.body.setInertia(inertia);
    return res;
}
Object generateRectangle(const sdl3::Vector2f &size, const sdl3::Vector2f &position, const sdl3::Color &color, PhysicBody physic = PhysicBody{})
{
    Object res;
    res.body = std::move(physic);

    RectangleCollider rct;
    rct.rect.setSize(size);
    rct.rect.setPosition(position);
    rct.rect.setFillColor(color);
    res.shape = std::make_unique<RectangleCollider>(rct);

    const float m = res.body.getMass();
    const float inertia = (m / 12.f) * (size.x * size.x + size.y * size.y);
    res.body.setInertia(inertia);
    return res;
}

} // namespace ObjectFactory

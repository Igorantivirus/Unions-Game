#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

#include <SDLWrapper/Names.hpp>
#include <SDLWrapper/Math/VectorMath.hpp>
#include <SDLWrapper/ObjectBase/Transformable.hpp>
#include "../EngineConfig.hpp"

enum class PhysicBodyType : std::uint8_t
{
    Dynamic,
    Static,
    Kinematic
};

class PhysicBody
{
public:
    explicit PhysicBody(const PhysicBodyType type = PhysicBodyType::Static) : type_{type}
    {
        recomputeInvMass();
        restitution_ = engine::cfg::restitution;
        staticFriction_ = engine::cfg::staticFriction;
        dynamicFriction_ = engine::cfg::dynamicFriction;
    }

    void update(const float dt, sdl3::Transformable &tr)
    {
        if (type_ != PhysicBodyType::Dynamic)
            return;

        forceAccum_ += sdl3::Vector2f{0.f, engine::cfg::gravityY} * mass_;

        sdl3::Vector2f acceleration{forceAccum_.x / mass_, forceAccum_.y / mass_};
        velocity_ += acceleration * dt;
        velocity_ *= std::pow(engine::cfg::linearDamping, dt);

        sdl3::Vector2f pos = tr.getPosition();
        pos += velocity_ * dt;
        tr.setPosition(pos);

        const float angularAcceleration = inertia_ > 0.f ? torqueAccum_ / inertia_ : 0.f;
        angularVelocity_ += angularAcceleration * dt;
        angularVelocity_ *= std::pow(engine::cfg::angularDamping, dt);
        tr.rotate(angularVelocity_ * dt);

        forceAccum_ = {0.f, 0.f};
        torqueAccum_ = 0.f;

        if (sdl3::dot(velocity_, velocity_) < engine::cfg::sleepLinearThreshold * engine::cfg::sleepLinearThreshold &&
            std::abs(angularVelocity_) < engine::cfg::sleepAngularThreshold)
        {
            sleepTimer_ += dt;
            if (sleepTimer_ > engine::cfg::sleepTimeToSleep)
            {
                velocity_ = {0.f, 0.f};
                angularVelocity_ = 0.f;
            }
        }
        else
        {
            sleepTimer_ = 0.f;
        }
    }

    void applyImpulse(const sdl3::Vector2f &impulse)
    {
        if (type_ != PhysicBodyType::Dynamic)
            return;
        velocity_ += impulse * invMass_;
    }

    void applyImpulseAtPoint(const sdl3::Vector2f &impulse,
                             const sdl3::Vector2f &contact,
                             const sdl3::Vector2f &center)
    {
        if (type_ != PhysicBodyType::Dynamic)
            return;

        velocity_ += impulse * invMass_;
        const sdl3::Vector2f r = contact - center;
        const float crossRJ = r.x * impulse.y - r.y * impulse.x;
        angularVelocity_ += crossRJ * invInertia_;
    }

    PhysicBodyType getType() const { return type_; }
    float getMass() const { return mass_; }
    float getInvMass() const { return invMass_; }
    float getInertia() const { return inertia_; }
    float getInvInertia() const { return invInertia_; }
    sdl3::Vector2f getVelocity() const { return velocity_; }
    float getAngularVelocity() const { return angularVelocity_; }
    float getRestitution() const { return restitution_; }
    float getStaticFriction() const { return staticFriction_; }
    float getDynamicFriction() const { return dynamicFriction_; }

    void setType(const PhysicBodyType type)
    {
        type_ = type;
        recomputeInvMass();
    }

    void setMass(const float mass)
    {
        mass_ = mass;
        recomputeInvMass();
    }

    void setInertia(const float inertia)
    {
        inertia_ = inertia;
        recomputeInvMass();
    }

    void setVelocity(const sdl3::Vector2f &v)
    {
        velocity_ = v;
    }

    void setAngularVelocity(float w)
    {
        angularVelocity_ = w;
    }

    void setRestitution(float r)
    {
        restitution_ = std::clamp(r, 0.f, 1.f);
    }

    void setFriction(float staticFric, float dynamicFric)
    {
        staticFriction_ = std::max(0.f, staticFric);
        dynamicFriction_ = std::max(0.f, dynamicFric);
    }

private:
    void recomputeInvMass()
    {
        if (type_ != PhysicBodyType::Dynamic || mass_ <= 1e-8f)
            invMass_ = 0.f;
        else
            invMass_ = 1.f / mass_;

        if (type_ != PhysicBodyType::Dynamic || inertia_ <= 1e-8f)
            invInertia_ = 0.f;
        else
            invInertia_ = 1.f / inertia_;
    }

    PhysicBodyType type_ = PhysicBodyType::Static;

    float mass_ = 1.f;
    float invMass_ = 0.f;
    float inertia_ = 1.f;
    float invInertia_ = 0.f;

    float restitution_ = 0.1f;
    float staticFriction_ = 0.6f;
    float dynamicFriction_ = 0.45f;

    sdl3::Vector2f velocity_{};
    sdl3::Vector2f forceAccum_{};

    float rotation_{}; // not used directly, kept for completeness
    float angularVelocity_{};
    float torqueAccum_{};

    float sleepTimer_ = 0.f;
};

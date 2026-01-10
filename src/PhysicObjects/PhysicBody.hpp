#pragma once

#include <cstdint>

#include <SDLWrapper/Names.hpp>
#include <SDLWrapper/ObjectBase/Transformable.hpp>

enum class PhysicBodyType : std::uint8_t
{
    Dynamic,
    Static,
    Kinematic
};

class PhysicBody
{
public:
    inline static const sdl3::Vector2f GRAVITY_CONSTANT = {0.f, 980.f};

public:
    PhysicBody(const PhysicBodyType type = PhysicBodyType::Static) : type_{type}
    {
    }

    void update(const float dt, sdl3::Transformable &tr)
    {
        if (type_ != PhysicBodyType::Dynamic)
            return;

        // 1. Учитываем гравитацию (F = m * g)
        // Добавляем силу тяжести к аккумулятору сил
        forceAccum_ += GRAVITY_CONSTANT * mass_;

        // 2. Линейное движение (законы Ньютона)
        // Ускорение a = F / m
        sdl3::Vector2f acceleration = {forceAccum_.x / mass_, forceAccum_.y / mass_};

        // Интегрируем скорость: v = v + a * dt
        velocity_.x += acceleration.x * dt;
        velocity_.y += acceleration.y * dt;

        // Линейное демпфирование (необязательно, но полезно для стабильности)
        // velocity_ *= 0.999f;

        // Интегрируем позицию: p = p + v * dt
        sdl3::Vector2f currentPos = tr.getPosition();
        currentPos.x += velocity_.x * dt;
        currentPos.y += velocity_.y * dt;
        tr.setPosition(currentPos);

        // 3. Угловое движение
        // Угловое ускорение alpha = T / I
        float angularAcceleration = torqueAccum_ / inertia_;

        // Интегрируем угловую скорость: w = w + alpha * dt
        angularVelocity_ += angularAcceleration * dt;

        // Угловое демпфирование
        // angularVelocity_ *= 0.99f;

        // Интегрируем поворот: angle = angle + w * dt
        // Используем метод rotate(), который прибавляет значение к текущему углу
        tr.rotate(angularVelocity_ * dt);

        // 4. Очистка аккумуляторов сил для следующего кадра
        forceAccum_ = {0.0f, 0.0f};
        torqueAccum_ = 0.0f;
    }
    void setType(const PhysicBodyType type)
    {
        type_ = type;
    }
    const PhysicBodyType getType() const
    {
        return type_;
    }

private:
    PhysicBodyType type_ = PhysicBodyType::Static;

    float mass_ = 1;

    sdl3::Vector2f velocity_{100, -600};
    sdl3::Vector2f forceAccum_{};

    float rotation_{};
    float angularVelocity_{};
    float torqueAccum_{}; // аккумулятор моментов

    float inertia_;
};
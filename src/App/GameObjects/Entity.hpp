#pragma once

#include <SDLWrapper/Names.hpp>
#include <SDLWrapper/SDLWrapper.hpp>
#include <box2d/box2d.h>

#include "Config.hpp"
#include <Core/Types.hpp>


class Entity : public sdl3::Drawable
{
public:
    Entity(b2Body *body, std::unique_ptr<sdl3::Shape> shape)
        : m_body(body), m_shape(std::move(shape))
    {
        if (m_body)
            m_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
        ID_ = maxID_++;
    }
    Entity(const Entity &) = delete;
    Entity(Entity &&other) noexcept
        : m_body(other.m_body), m_shape(std::move(other.m_shape)), ID_{other.ID_}
    {
        other.m_body = nullptr;
        if (m_body)
            m_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
    }

    virtual ~Entity()
    {
        if (m_body)
        {
            m_body->GetUserData().pointer = 0;
            m_body->GetWorld()->DestroyBody(m_body);
        }
        m_body = nullptr;
    }

    Entity &operator=(const Entity &) = delete;
    Entity &operator=(Entity &&other) noexcept
    {
        if (this != &other)
        {
            if (m_body)
            {
                m_body->GetUserData().pointer = 0;
                m_body->GetWorld()->DestroyBody(m_body);
            }

            m_body = other.m_body;
            m_shape = std::move(other.m_shape);
            ID_ = other.ID_;
            other.m_body = nullptr;

            if (m_body)
                m_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
        }
        return *this;
    }

    void updateForNeed()
    {
        const b2Vec2 &pos = m_body->GetPosition();
        m_shape->setPosition({pos.x * Config::PPM, pos.y * Config::PPM});
        m_shape->setRotation(m_body->GetAngle() * 180.f / SDL_PI_F);
    }

    void setPosition(sdl3::Vector2f pos_px)
    {
        m_body->SetTransform({pos_px.x * Config::MPP, pos_px.y * Config::MPP}, m_body->GetAngle());
        update();
    }
    sdl3::Vector2f getPosition() const
    {
        auto p = m_body->GetTransform().p;
        return sdl3::Vector2f{p.x, p.y};
    }

    void setRotation(float degrees)
    {
        m_body->SetTransform(m_body->GetPosition(), degrees * SDL_PI_F / 180.f);
        update();
    }

    const b2Body *getBody() const
    {
        return m_body;
    }
    const sdl3::Shape &getShape() const
    {
        return *m_shape;
    }

    const unsigned short getID() const
    {
        return ID_;
    }

    void setEnabled(bool enabled)
    {
        if (!m_body)
            return;
        m_body->SetEnabled(enabled);

        // Опционально: если включили — можно разбудить
        if (enabled)
            m_body->SetAwake(true);
    }

    bool isEnabled() const
    {
        return m_body && m_body->IsEnabled();
    }

private:
    b2Body *m_body = nullptr;
    mutable std::unique_ptr<sdl3::Shape> m_shape;

    IDType ID_ = 0;

    inline static unsigned short maxID_ = 1;

private:
    void update() const
    {
        const b2Vec2 &pos = m_body->GetPosition();
        m_shape->setPosition({pos.x * Config::PPM, pos.y * Config::PPM});
        m_shape->setRotation(m_body->GetAngle() * 180.f / SDL_PI_F);
    }
    void draw(sdl3::RenderTarget &target) const override
    {
        update();
        target.draw(*m_shape.get());
    }
};

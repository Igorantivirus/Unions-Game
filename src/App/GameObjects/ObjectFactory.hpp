#pragma once

#include <App/GameObjects/Config.hpp>
#include <App/GameObjects/Entity.hpp>
#include <App/GameObjects/EntityFactory.hpp>
#include <App/GameObjects/GameObject.hpp>
#include <App/GameObjects/PolygonShape.hpp>
#include <App/Resources/ObjectLibrary.hpp>

#include <SDLWrapper/DrawTransformObjects/CircleShape.hpp>
#include <SDLWrapper/DrawTransformObjects/EllipseShape.hpp>
#include <SDLWrapper/Math/Colors.hpp>

#include <box2d/box2d.h>

#include <SDL3/SDL_log.h>

#include <algorithm>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace resources
{

class ObjectFactory
{
public:
    explicit ObjectFactory(ObjectLibrary &library) : library_(library)
    {
    }

    bool loadPack(std::string_view folder)
    {
        activePack_ = normalizeFolderName(folder);
        if (activePack_.empty())
            return false;
        return library_.loadFolder(activePack_);
    }

    void unloadPack()
    {
        if (activePack_.empty())
            return;
        library_.unloadFolder(activePack_);
        activePack_.clear();
    }

    const std::string &getActivePack() const
    {
        return activePack_;
    }

    const ObjectDef *getDefById(const IDType id) const
    {
        if (activePack_.empty())
            return nullptr;
        return library_.getObject(activePack_, id);
    }

    bool hasId(const IDType id) const
    {
        return getDefById(id) != nullptr;
    }

    // Находит id объекта по level из конфига (level может не совпадать с id).
    std::optional<IDType> getIdByLevel(const IDType level) const
    {
        if (activePack_.empty())
            return std::nullopt;

        const ObjectPack *pack = library_.getPack(activePack_);
        if (!pack)
            return std::nullopt;

        for (const auto &[id, def] : pack->getAll())
            if (def.level == level)
                return id;

        return std::nullopt;
    }

    // Простая логика: результат мержа = level + 1.
    std::optional<IDType> getMergeResultId(const IDType levelA, const IDType levelB) const
    {
        if (levelA != levelB)
            return std::nullopt;
        return getIdByLevel(levelA + 1);
    }

    std::optional<GameObject> tryCreateById(b2World &world, const IDType id, const sdl3::Vector2f pos, const b2BodyType type = b2_dynamicBody) const
    {
        const ObjectDef *def = getDefById(id);
        if (!def)
            return std::nullopt;

        try
        {
            const sdl3::Texture *tex = library_.textures().get(def->texturePath);

            switch (def->form.type)
            {
            case ObjectFormType::Circle:
                return createCircle(world, *def, tex, pos, type);
            case ObjectFormType::Ellipse:
                return createEllipse(world, *def, tex, pos, type);
            case ObjectFormType::Polygon8:
                return createPolygon(world, *def, tex, pos, type);
            default:
                return std::nullopt;
            }
        }
        catch (...)
        {
            return std::nullopt;
        }
    }

    GameObject createById(b2World &world, const IDType id, const sdl3::Vector2f pos, const b2BodyType type = b2_dynamicBody) const
    {
        const ObjectDef *def = getDefById(id);
        if (!def)
            throw std::logic_error("ObjectFactory: unknown object id");

        const sdl3::Texture *tex = library_.textures().get(def->texturePath);

        switch (def->form.type)
        {
        case ObjectFormType::Circle:
            return createCircle(world, *def, tex, pos, type);
        case ObjectFormType::Ellipse:
            return createEllipse(world, *def, tex, pos, type);
        case ObjectFormType::Polygon8:
            return createPolygon(world, *def, tex, pos, type);
        default:
            throw std::logic_error("ObjectFactory: unsupported form");
        }
    }

private:
    static std::string normalizeFolderName(std::string_view folder)
    {
        std::string s(folder);
        while (!s.empty() && (s.front() == '/' || s.front() == '\\'))
            s.erase(s.begin());
        while (!s.empty() && (s.back() == '/' || s.back() == '\\'))
            s.pop_back();
        return s;
    }

    static GameObject wrapEntity(Entity &&entity, const ObjectDef &def)
    {
        return GameObject(std::move(entity), def.level, def.points);
    }

    static GameObject createCircle(b2World &world, const ObjectDef &def, const sdl3::Texture *tex, const sdl3::Vector2f pos, const b2BodyType type)
    {
        const float radius = std::get<float>(def.form.form);

        sdl3::CircleShape shape(radius, 40);
        shape.setPosition(pos);
        shape.setFillColor(sdl3::Colors::White);
        if (tex)
            shape.setTexture(*tex);

        b2BodyDef bd;
        bd.type = type;
        bd.position.Set(shape.getPosition().x * Config::MPP, shape.getPosition().y * Config::MPP);
        bd.angle = shape.getRotation() * SDL_PI_F / 180.f;

        b2Body *body = world.CreateBody(&bd);

        b2CircleShape circle;
        circle.m_radius = radius * Config::MPP;

        b2FixtureDef fd;
        fd.shape = &circle;
        fd.density = (type == b2_staticBody) ? 0.0f : Config::defaultDensity;
        fd.friction = Config::defaultFrictionEllipse;
        body->CreateFixture(&fd);

        return wrapEntity(Entity(body, std::make_unique<sdl3::CircleShape>(shape)), def);
    }

    static GameObject createEllipse(b2World &world, const ObjectDef &def, const sdl3::Texture *tex, const sdl3::Vector2f pos, const b2BodyType type)
    {
        const sdl3::Vector2f radii = std::get<sdl3::Vector2f>(def.form.form);

        sdl3::EllipseShape shape(radii, 40);
        shape.setPosition(pos);
        shape.setFillColor(sdl3::Colors::White);
        if (tex)
            shape.setTexture(*tex);

        b2BodyDef bd;
        bd.type = type;

        b2FixtureDef fd;
        fd.density = (type == b2_staticBody) ? 0.0f : Config::defaultDensity;
        fd.friction = Config::defaultFrictionEllipse;

        // Используем уже существующую аппроксимацию через треугольные секторы.
        Entity e = EntityFactory::createFromShape(world, shape, bd, fd);
        return wrapEntity(std::move(e), def);
    }

    static GameObject createPolygon(b2World &world, const ObjectDef &def, const sdl3::Texture *tex, const sdl3::Vector2f pos, const b2BodyType type)
    {
        const auto verticesPx = std::get<std::vector<sdl3::Vector2f>>(def.form.form);
        if (verticesPx.size() < 3)
            throw std::logic_error("ObjectFactory: polygon requires >= 3 vertices");

        sdl3::PolygonShape shape(verticesPx);
        shape.setPosition(pos);
        shape.setFillColor(sdl3::Colors::White);
        if (tex)
            shape.setTexture(*tex);

        b2BodyDef bd;
        bd.type = type;
        bd.position.Set(shape.getPosition().x * Config::MPP, shape.getPosition().y * Config::MPP);
        bd.angle = shape.getRotation() * SDL_PI_F / 180.f;

        b2Body *body = world.CreateBody(&bd);

        std::vector<b2Vec2> verts;
        verts.reserve(std::min<std::size_t>(8, verticesPx.size()));
        for (std::size_t i = 0; i < verticesPx.size() && verts.size() < 8; ++i)
            verts.push_back({verticesPx[i].x * Config::MPP, verticesPx[i].y * Config::MPP});

        b2PolygonShape poly;
        poly.Set(verts.data(), static_cast<int>(verts.size()));

        b2FixtureDef fd;
        fd.shape = &poly;
        fd.density = (type == b2_staticBody) ? 0.0f : Config::defaultDensity;
        fd.friction = Config::defaultFrictionRect;
        body->CreateFixture(&fd);

        return wrapEntity(Entity(body, std::make_unique<sdl3::PolygonShape>(shape)), def);
    }

private:
    ObjectLibrary &library_;
    std::string activePack_;
};

} // namespace resources

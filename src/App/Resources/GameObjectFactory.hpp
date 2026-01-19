#pragma once

#include <App/PhysicBase/Config.hpp>
#include <App/PhysicBase/Entity.hpp>
#include <App/PhysicBase/EntityFactory.hpp>
#include <App/GameObjects/GameObject.hpp>
#include <App/PhysicBase/PolygonShape.hpp>
#include <App/Resources/PackageContainer.hpp>

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
#include <vector>

namespace resources
{

class GameObjectFactory
{
public:
    GameObjectFactory() = default;

    bool loadPack(const std::string &packName)
    {
        activePack_ = packName;
        if (activePack_.empty())
            return false;
        return packages_.loadFolder(activePack_);
    }

    void unloadPack()
    {
        if (activePack_.empty())
            return;
        packages_.unloadFolder(activePack_);
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
        return packages_.getObject(activePack_, id);
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

        const ObjectPack *pack = packages_.getPack(activePack_);
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
        {
            SDL_Log("ObjectFactory: Couldn't find object by ID\n");
            return std::nullopt;
        }

        try
        {
            const sdl3::Texture *tex = packages_.textures().get(def->filler.getTextureName());

            switch (def->form.type)
            {
            case ObjectFormType::Circle:
                return createCircle(world, *def, tex, pos, type);
            case ObjectFormType::Ellipse:
                return createEllipse(world, *def, tex, pos, type);
            case ObjectFormType::Polygon8:
                return createPolygon(world, *def, tex, pos, type);
            default:
                SDL_Log("ObjectFactory: Unsuportable ObjectFormType.\n");
                return std::nullopt;
            }
        }
        catch (...)
        {
            SDL_Log("ObjectFactory: Unknown error\n");
            return std::nullopt;
        }
    }

    GameObject createById(b2World &world, const IDType id, const sdl3::Vector2f pos, const b2BodyType type = b2_dynamicBody) const
    {
        auto opt = tryCreateById(world, id, pos, type);
        if (!opt.has_value())
            throw std::logic_error("ObjectFactory: Couldn't create an object by id");
        return std::move(opt.value());
    }

private:
    static GameObject wrapEntity(Entity &&entity, const ObjectDef &def)
    {
        return GameObject(std::move(entity), def.level, def.points);
    }

    static GameObject createCircle(b2World &world, const ObjectDef &def, const sdl3::Texture *tex, const sdl3::Vector2f pos, const b2BodyType type)
    {
        const float radius = def.form.getRadius();
        const sdl3::Color color = def.filler.getColor();
        return wrapEntity(EntityFactory::createCircle(world, pos, radius, color, tex, type), def);
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
    PackageContainer packages_;
    std::string activePack_;
};

} // namespace resources

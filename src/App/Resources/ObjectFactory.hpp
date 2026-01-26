#pragma once

#include "Core/Types.hpp"
#include <SDLWrapper/EventRegistrator.hpp>
#include <optional>
#include <string>
#include <vector>

#include <SDL3/SDL_log.h>
#include <SDLWrapper/DrawTransformObjects/CircleShape.hpp>
#include <SDLWrapper/DrawTransformObjects/EllipseShape.hpp>
#include <SDLWrapper/DrawTransformObjects/PolygonShape.hpp>
#include <SDLWrapper/Math/Colors.hpp>

#include <box2d/box2d.h>

#include <App/GameObjects/GameObject.hpp>
#include <App/Physics/Config.hpp>
#include <App/Physics/Entity.hpp>
#include <App/Physics/EntityFactory.hpp>
#include <App/Resources/PackageContainer.hpp>

namespace resources
{
// Фабрика GameObject, создаёт по PackageContainer
class ObjectFactory
{
public:
    ObjectFactory(PackageContainer &packages) : packages_(packages)
    {
    }

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

    std::optional<objects::GameObject> create(b2World &world, const ObjectDef *def, const sdl3::Vector2f pos, const b2BodyType type = b2_dynamicBody) const
    {
        if (!def)
        {
            SDL_Log("ObjectFactory: Couldn't find object by ID\n");
            return std::nullopt;
        }
        const sdl3::Texture *tex = packages_.textures().get(def->filler.getTextureName());

        switch (def->form.type)
        {
        case ObjectFormType::Circle:
            return createCircle(world, *def, tex, pos, type);
        case ObjectFormType::Ellipse:
            return createEllipse(world, *def, tex, pos, type);
        case ObjectFormType::Polygon:
            return createPolygon(world, *def, tex, pos, type);
        case ObjectFormType::Rectangle:
            return createRectangle(world, *def, tex, pos, type);
        default:
            SDL_Log("ObjectFactory: Unsuportable ObjectFormType.\n");
            return std::nullopt;
        }
    }

    std::optional<objects::GameObject> createById(b2World &world, const IDType id, const sdl3::Vector2f pos, const b2BodyType type = b2_dynamicBody) const
    {
        const ObjectDef *def = getDefById(id);
        return create(world, def, pos, type);
    }

private:
    static objects::GameObject wrapEntity(physics::Entity &&entity, const ObjectDef &def)
    {
        return objects::GameObject(std::move(entity), def.level, def.points);
    }

    static objects::GameObject createCircle(b2World &world, const ObjectDef &def, const sdl3::Texture *tex, const sdl3::Vector2f pos, const b2BodyType type)
    {
        const float radius = def.form.getRadius();
        const sdl3::Color color = def.filler.getColor();
        return wrapEntity(physics::EntityFactory::createCircle(world, pos, radius, color, tex, type), def);
    }

    static objects::GameObject createEllipse(b2World &world, const ObjectDef &def, const sdl3::Texture *tex, const sdl3::Vector2f pos, const b2BodyType type)
    {
        const sdl3::Vector2f radii = def.form.getRadii();
        const sdl3::Color color = def.filler.getColor();
        return wrapEntity(physics::EntityFactory::createEllipse(world, pos, radii, color, tex, type), def);
    }

    static objects::GameObject createRectangle(b2World &world, const ObjectDef &def, const sdl3::Texture *tex, const sdl3::Vector2f pos, const b2BodyType type)
    {
        const sdl3::Vector2f size = def.form.getSize();
        const sdl3::Color color = def.filler.getColor();
        return wrapEntity(physics::EntityFactory::createRectangle(world, pos, size, color, tex, type), def);
    }

    static objects::GameObject createPolygon(b2World &world, const ObjectDef &def, const sdl3::Texture *tex, const sdl3::Vector2f pos, const b2BodyType type)
    {

        const std::vector<sdl3::Vector2f> points = def.form.getPolygon();
        const sdl3::Color color = def.filler.getColor();
        return wrapEntity(physics::EntityFactory::createPolygon(world, pos, points, color, tex, type), def);
    }

private:
    PackageContainer &packages_;
    std::string activePack_;
};

} // namespace resources

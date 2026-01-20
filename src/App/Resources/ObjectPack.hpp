#pragma once

#include <filesystem>
#include <unordered_set>

#include <SDL3/SDL_log.h>
#include <SDLWrapper/FileWorker.hpp>
#include <SDLWrapper/Math/Colors.hpp>

#include <pugixml/pugixml.hpp>

#include "TextureManager.hpp"
#include "Types.hpp"

namespace resources
{

// Пакет одной игры (имя и определения объектов)
class ObjectPack
{
public:
    void unload(TextureManager &textures)
    {
        for (const auto &key : textureKeys_)
            textures.unload(key);
        textureKeys_.clear();
        objects_.clear();
        packName_.clear();
        folderAbs_.clear();
    }

    // GET METHODS

    bool empty() const
    {
        return objects_.empty();
    }

    const ObjectDef *getById(const IDType id) const
    {
        auto it = objects_.find(id);
        return it == objects_.end() ? nullptr : &it->second;
    }

    const std::unordered_map<IDType, ObjectDef> &getAll() const
    {
        return objects_;
    }

    const std::filesystem::path &getFolder() const
    {
        return folderAbs_;
    }

    const std::string &getName() const
    {
        return packName_;
    }

    // SET METHODS

    void setPackageName(const std::string &packName)
    {
        packName_ = packName;
    }

    void addTextureKey(const std::string &key)
    {
        textureKeys_.insert(key);
    }

    void addObject(ObjectDef def)
    {
        objects_[def.id] = std::move(def);
    }

private:
    std::string packName_;
    std::filesystem::path folderAbs_;
    std::unordered_map<IDType, ObjectDef> objects_;
    std::unordered_set<std::string> textureKeys_;
};
} // namespace resources

#pragma once

#include "ObjectPack.hpp"

#include <filesystem>
#include <string>
#include <unordered_map>

#include <App/IO/ObjectPackIO.hpp>

namespace resources
{

class PackageContainer
{
public:
    explicit PackageContainer(std::filesystem::path objectsRoot)
        : objectsRoot_(std::move(objectsRoot))
    {
    }

    TextureManager &textures()
    {
        return textures_;
    }
    const TextureManager &textures() const
    {
        return textures_;
    }

    bool loadFolder(const std::string& packName)
    {
        return loadByOtherPath(objectsRoot_ / packName, packName);
    }

    bool loadByOtherPath(const std::filesystem::path &folderAbs, const std::string packName)
    {
        auto &pack = packs_[packName];
        if (!IO::readObjectPack(pack, textures_, packName, folderAbs))
        {
            packs_.erase(packName);
            return false;
        }
        return true;
    }

    void unloadFolder(const std::string& packName)
    {
        auto it = packs_.find(packName);
        if (it == packs_.end())
            return;
        it->second.unload(textures_);
        packs_.erase(it);
    }

    void unloadAll()
    {
        for (auto &[name, pack] : packs_)
            pack.unload(textures_);
        packs_.clear();
        textures_.clear();
    }

    const ObjectPack *getPack(const std::string packName) const
    {
        auto it = packs_.find(packName);
        return it == packs_.end() ? nullptr : &it->second;
    }

    const ObjectDef *getObject(const std::string packName, const IDType id) const
    {
        const ObjectPack *pack = getPack(packName);
        return pack ? pack->getById(id) : nullptr;
    }

private:
    std::filesystem::path objectsRoot_;

    TextureManager textures_;
    std::unordered_map<std::string, ObjectPack> packs_;
};

} // namespace resources

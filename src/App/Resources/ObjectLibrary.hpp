#pragma once

#include "ObjectPack.hpp"

namespace resources
{

class ObjectLibrary
{
public:
    explicit ObjectLibrary(std::filesystem::path objectsRoot = "assets/objects")
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

    bool loadFolder(std::string_view folder)
    {
        const std::string packName = normalizeFolderName(folder);
        if (packName.empty())
            return false;

        auto &pack = packs_[packName];
        const auto folderAbs = objectsRoot_ / packName;
        if (!pack.loadFromFolder(folderAbs, textures_, packName))
        {
            packs_.erase(packName);
            return false;
        }
        return true;
    }

    void unloadFolder(std::string_view folder)
    {
        const std::string packName = normalizeFolderName(folder);
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

    const ObjectPack *getPack(std::string_view folder) const
    {
        const std::string packName = normalizeFolderName(folder);
        auto it = packs_.find(packName);
        return it == packs_.end() ? nullptr : &it->second;
    }

    const ObjectDef *getObject(std::string_view folder, const IDType id) const
    {
        const ObjectPack *pack = getPack(folder);
        return pack ? pack->getById(id) : nullptr;
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

private:
    std::filesystem::path objectsRoot_;

    TextureManager textures_;
    std::unordered_map<std::string, ObjectPack> packs_;
};

} // namespace app

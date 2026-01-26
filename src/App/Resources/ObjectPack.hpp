#pragma once

#include <filesystem>
#include <unordered_set>

#include <SDL3/SDL_log.h>
#include <SDLWrapper/FileWorker.hpp>
#include <SDLWrapper/Math/Colors.hpp>

#include <pugixml/pugixml.hpp>

#include <Core/Managers/TextureManager.hpp>
#include "Core/Managers/AudioManager.hpp"
#include "Types.hpp"

namespace resources
{

// Пакет одной игры (имя и определения объектов)
class ObjectPack
{
public:
    void unload(core::managers::TextureManager &textures, core::managers::AudioManager& audios)
    {
        for (const auto &key : textureKeys_)
            textures.unload(key);
        for (const auto &key : audioKeys_)
            audios.unload(key);
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
    const PackageSettings &getSetings() const
    {
        return settings_;
    }
    const PackageMusic &getMusic() const
    {
        return music_;
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
    void addAudioKey(const std::string &key)
    {
        if(!key.empty())
            audioKeys_.insert(key);
    }

    void addObject(ObjectDef def)
    {
        objects_[def.id] = std::move(def);
    }
    void setSettings(PackageSettings settings)
    {
        settings_ = std::move(settings);
    } 
    void setMusic(PackageMusic music)
    {
        music_ = std::move(music);
    } 

private:
    std::string packName_;
    std::filesystem::path folderAbs_;
    std::unordered_map<IDType, ObjectDef> objects_;
    PackageSettings settings_;
    PackageMusic music_;
    std::unordered_set<std::string> textureKeys_;
    std::unordered_set<std::string> audioKeys_;
};
} // namespace resources

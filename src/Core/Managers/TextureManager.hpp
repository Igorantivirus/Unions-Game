#pragma once

#include <Core/Types.hpp>
#include <SDLWrapper/Texture.hpp>

#include <filesystem>
#include <string>
#include <unordered_map>

namespace core::managers
{

class TextureManager
{
public:
    TextureManager() = default;

    bool load(const std::string& key, const std::filesystem::path &filePath)
    {
        bool res = textures_[key].loadFromFile(filePath.string().c_str());
        if(!res)
        {
            unload(key);
            return false;
        }
        return true;
    }

    const sdl3::Texture *get(const std::string& key) const
    {
        auto it = textures_.find(key);
        return it == textures_.end() ? nullptr : &it->second;
    }

    bool has(const std::string& key) const
    {
        return get(key) != nullptr;
    }

    void unload(const std::string& key)
    {
        textures_.erase(key);
    }

    void clear()
    {
        textures_.clear();
    }

private:
    std::unordered_map<std::string, sdl3::Texture> textures_;
};

} // namespace app


#pragma once

#include <Core/Types.hpp>
#include <SDLWrapper/Texture.hpp>

#include <filesystem>
#include <string>
#include <unordered_map>

#include <SDLWrapper/Audio/AudioDevice.hpp>

namespace core::managers
{

class AudioManager
{
public:
    AudioManager() = default;

    bool load(const std::string &key, const std::filesystem::path &filePath)
    {
        bool res = false;
        res = audios_[key].loadFromFile(filePath.string().c_str());

        if (!res)
        {
            unload(key);
            return false;
        }
        return true;
    }

    const sdl3::audio::Audio *get(const std::string &key) const
    {
        auto it = audios_.find(key);
        return it == audios_.end() ? nullptr : &it->second;
    }

    bool has(const std::string &key) const
    {
        return get(key) != nullptr;
    }

    void unload(const std::string &key)
    {
        audios_.erase(key);
    }

    void clear()
    {
        audios_.clear();
    }

private:
    std::unordered_map<std::string, sdl3::audio::Audio> audios_;
};

} // namespace core::managers

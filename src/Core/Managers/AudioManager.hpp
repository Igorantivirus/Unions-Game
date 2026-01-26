#pragma once

#include <Core/Types.hpp>
#include <SDLWrapper/Texture.hpp>

#include <filesystem>
#include <string>
#include <unordered_map>

#include <Core/Audio/AudioDevice.hpp>

namespace core::managers
{

class AudioManager
{
public:
    AudioManager() = default;
    AudioManager(audio::AudioDevice &device) : devicePtr_(&device)
    {
    }

    void setDevice(audio::AudioDevice &device)
    {
        devicePtr_ = &device;
    }

    bool load(const std::string &key, const std::filesystem::path &filePath)
    {
        bool res = false;
        if (devicePtr_)
            res = audios_[key].loadFromFile(filePath, devicePtr_->getSDMixer().lock());
        else
            res = audios_[key].loadFromFile(filePath);

        if (!res)
        {
            unload(key);
            return false;
        }
        return true;
    }

    const audio::Audio *get(const std::string &key) const
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
    std::unordered_map<std::string, audio::Audio> audios_;
    audio::AudioDevice *devicePtr_ = nullptr;
};

} // namespace core::managers

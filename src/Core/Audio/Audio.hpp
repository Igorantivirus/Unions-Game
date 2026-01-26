#pragma once

#include <filesystem>
#include <memory>
#include <string_view>

#include <SDL3/SDL_error.h>
#include <SDL3_mixer/SDL_mixer.h>

namespace audio
{

struct AudioBase
{
    std::shared_ptr<MIX_Audio> audio = nullptr;
    bool isRunning = false;
};

class Audio
{
public:
    Audio() = default;
    Audio(const Audio &other) = delete;
    Audio(Audio &&other) = default;
    Audio &operator=(const Audio &other) = delete;
    Audio &operator=(Audio &&other) = default;

    void stop()
    {
        isRunning_ = false;
    }

    bool loadFromFile(const std::string_view &path, std::shared_ptr<MIX_Mixer> mixer = nullptr, bool predecode = true)
    {
        MIX_Audio *audio = MIX_LoadAudio(mixer.get(), path.data(), true);
        if (!audio)
        {
            SDL_Log("%s\n", SDL_GetError());
            return false;
        }
        audio_.reset(audio, MIX_DestroyAudio);
        isRunning_ = false;
        return true;
    }
    bool loadFromFile(const std::filesystem::path &path, std::shared_ptr<MIX_Mixer> mixer = nullptr, bool predecode = true)
    {
        return loadFromFile(std::string_view(path.string()), mixer, predecode);
    }

    std::weak_ptr<const MIX_Audio> getSDLAudio() const
    {
        return audio_;
    }
    std::weak_ptr<MIX_Audio> getSDLAudio()
    {
        return audio_;
    }

    operator bool() const
    {
        return static_cast<bool>(audio_);
    }

    bool isRunnig() const
    {
        return isRunning_;
    }

private:
    std::shared_ptr<MIX_Audio> audio_ = nullptr;
    bool isRunning_ = false;
};

} // namespace audio
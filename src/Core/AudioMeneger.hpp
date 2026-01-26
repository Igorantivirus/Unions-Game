#pragma once

#include <filesystem>
#include <list>
#include <string_view>
#include <vector>

#include "MixerMemmory.hpp"

namespace core
{

class AudioMeneger
{
private:
    struct AudioGuarantor
    {
        TrackPtr track;
        AudioPtr audio;
    };

public:
    AudioMeneger()
    {
        MIX_Mixer *rawMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
        if (!rawMixer)
            throw std::runtime_error(std::string("Failed to create audio mixer: ") + SDL_GetError());
        mixer_.reset(rawMixer);
    }

    void initTracks(const std::size_t tracksCount)
    {
        freeTracks_.clear();
        usedTracks_.clear();
        freeTracks_.reserve(tracksCount);
        while (freeTracks_.size() < tracksCount)
        {
            TrackPtr track(MIX_CreateTrack(mixer_.get()), MIX_DestroyTrack);
            freeTracks_.push_back(std::move(track));
        }
    }

    bool playSouns(AudioPtr audio, bool replay = false)
    {
        if (freeTracks_.empty() || !audio)
            return false;

        TrackPtr track = freeTracks_.back();
        freeTracks_.pop_back();
        usedTracks_.push_back({track, audio}); // Гарантия, что аудио будет жить, пока воспроизводится

        MIX_SetTrackAudio(track.get(), audio.get());
        MIX_SetTrackGain(track.get(), 1.0f);
        MIX_PlayTrack(track.get(), replay ? -1 : 0);

        return true;
    }

    std::size_t update()
    {
        for (auto it = usedTracks_.begin(); it != usedTracks_.end();)
        {
            if (!MIX_TrackPlaying(it->track.get()))
            {
                MIX_SetTrackAudio(it->track.get(), nullptr); // Отсоединяем звук

                freeTracks_.push_back(it->track); // Возвращаем в пул свободных
                it = usedTracks_.erase(it);
            }
            else
                ++it;
        }
        return freeTracks_.size();
    }

    std::size_t getFreeTracksCount() const
    {
        return freeTracks_.size();
    }
    std::size_t getUsedTracksCount() const
    {
        return usedTracks_.size();
    }

    AudioPtr loadAudio(const std::filesystem::path &path, bool predecode = true)
    {
        return loadAudio(std::string_view(path.string()), predecode);
    }
    AudioPtr loadAudio(const std::string_view &path, bool predecode = true)
    {
        AudioPtr res;
        MIX_Audio *audio = MIX_LoadAudio(mixer_.get(), path.data(), true);
        res.reset(audio, MIX_DestroyAudio);
        return res;
    }

private:
    MixerUPtr mixer_;
    std::vector<TrackPtr> freeTracks_;
    std::list<AudioGuarantor> usedTracks_;
};
} // namespace core
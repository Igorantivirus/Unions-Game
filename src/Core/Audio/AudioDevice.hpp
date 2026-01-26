#pragma once

#include <list>
#include <memory>
#include <vector>

#include "Audio.hpp"

namespace audio
{

class AudioDevice
{
private:
    // Гарантия, что аудио будет жить, пока воспроизводится
    struct AudioPair
    {
        std::shared_ptr<MIX_Track> track;
        const Audio *audio;
    };

public:
    AudioDevice() = default;

    void initTracks(const std::size_t tracksCount)
    {
        MIX_Mixer *rawMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
        if (!rawMixer)
            throw std::runtime_error(std::string("Failed to create audio mixer: ") + SDL_GetError());
        mixer_.reset(rawMixer, MIX_DestroyMixer);

        freeTracks_.clear();
        usedTracks_.clear();
        freeTracks_.reserve(tracksCount);
        while (freeTracks_.size() < tracksCount)
        {
            std::shared_ptr<MIX_Track> track(MIX_CreateTrack(mixer_.get()), MIX_DestroyTrack);
            freeTracks_.push_back(std::move(track));
        }
    }

    bool playSouns(const Audio &audio, bool replay = false)
    {
        if (freeTracks_.empty() || !audio)
            return false;

        std::shared_ptr<MIX_Track> track = freeTracks_.back();
        std::shared_ptr<const MIX_Audio> audioS = audio.getSDLAudio().lock();

        AudioPair pair;
        pair.track = track;
        pair.audio = &audio;

        freeTracks_.pop_back();
        usedTracks_.push_back(std::move(pair));

        MIX_SetTrackAudio(track.get(), const_cast<MIX_Audio *>(audioS.get()));
        MIX_SetTrackGain(track.get(), 1.0f);
        MIX_PlayTrack(track.get(), replay ? -1 : 0);

        return true;
    }

    std::size_t update()
    {
        for (auto it = usedTracks_.begin(); it != usedTracks_.end();)
        {
            // Если не проигрывается или был отключен
            if (!MIX_TrackPlaying(it->track.get()) || !it->audio->isRunnig())
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

    std::weak_ptr<const MIX_Mixer> getSDMixer() const
    {
        return mixer_;
    }
    std::weak_ptr<MIX_Mixer> getSDMixer()
    {
        return mixer_;
    }

private:
    std::shared_ptr<MIX_Mixer> mixer_;
    std::vector<std::shared_ptr<MIX_Track>> freeTracks_;
    std::list<AudioPair> usedTracks_;

private:
};
} // namespace audio
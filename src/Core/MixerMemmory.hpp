#pragma once

#include <memory>

#include <SDL3_mixer/SDL_mixer.h>

namespace core
{
struct AudioDestructor
{
    void operator()(MIX_Mixer *mixer)
    {
        MIX_DestroyMixer(mixer);
    }
    void operator()(MIX_Track *track)
    {
        MIX_DestroyTrack(track);
    }
    void operator()(MIX_Audio *audio)
    {
        MIX_DestroyAudio(audio);
    }
};

using MixerUPtr = std::unique_ptr<MIX_Mixer, AudioDestructor>;
using TrackUPtr = std::unique_ptr<MIX_Track, AudioDestructor>;
using AudioUPtr = std::unique_ptr<MIX_Audio, AudioDestructor>;

using TrackPtr = std::shared_ptr<MIX_Track>;
using AudioPtr = std::shared_ptr<MIX_Audio>;
} // namespace core
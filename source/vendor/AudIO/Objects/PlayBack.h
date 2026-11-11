#pragma once

#include "VolumeAndPan.h"
#include "../3rdparty/AudioFile.h"

namespace AudIO
{

    class AudioPlayback : public Object
    {
    public:
        AudioPlayback();
        ~AudioPlayback() = default;

        enum PlayMode
        {
            FORWARD,
            REVERSE,
        };

        void reset();
        void load( const std::string& file );
        // Desktop adapter: decoding happens outside the render lock.
        std::unique_ptr<AudioFile<float>> replaceFile(std::unique_ptr<AudioFile<float>> file) {
            stop(); readIndex = 0;
            maxSamples = file ? file->getNumSamplesPerChannel() : 0;
            m_file.swap(file); return file;
        }

        // Desktop slice controls are latched on note-on by the pad adapter.
        void setSlice(double start, double end, double ratio, double fadeMilliseconds) {
            sliceStart=std::clamp(start,0.0,1.0);sliceEnd=std::clamp(end,sliceStart,1.0);
            pitchRatio=std::clamp(ratio,0.25,4.0);fadeFrames=fadeMilliseconds*.001*sampleRate();
        }
        double progress() const { return maxSamples>0?std::clamp(readIndex/maxSamples,0.0,1.0):0.0; }
        void setVolumeAndPan( float Volume, float Pan );
        void setBypass( bool Bypass ) { m_bypass.store( Bypass ); };
        void setMute( bool Mute ) { m_mute.store( Mute ); };
        void setLooping( bool Loop ) { m_loop.store( Loop ); };

        float getVolume() { return vp.getParameter( VolumeAndPan::kParamGain ); };
        float getPan() { return vp.getParameter( VolumeAndPan::kParamPan ); };

        bool isBypass() const { return m_bypass.load(); };
        bool isMuted() const { return m_mute.load(); };
        bool isPlaying() const { return m_playing.load(); };
        bool isLooping() const { return m_loop.load(); };

        void play( bool FromBegin );
        void stop();

    protected:
        void prepareToPlay( uint32_t samplerate, uint32_t blocksize ) override;
        void process( Buffer<float>& buffer ) override;

        std::unique_ptr<AudioFile<float>> m_file;
        std::atomic<bool>                 m_bypass{ false }, m_mute{ false }, m_playing{ false }, m_loop{ false };
        double readIndex{ 0 }, sliceStart{0},sliceEnd{1},pitchRatio{1},fadeFrames{96};
        int maxSamples{0};

        VolumeAndPan vp;
        PlayMode     mode{ FORWARD };
    };

    REGISTER_OBJECT( "AudioPlayback", AudioPlayback );
};

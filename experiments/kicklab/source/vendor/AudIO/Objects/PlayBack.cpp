
#include "PlayBack.h"

namespace AudIO
{
    AudioPlayback::AudioPlayback()
        : Object( 0, 2, 1 )
    {}

    void AudioPlayback::load( const std::string& File )
    {
        maxSamples = 0;
        readIndex  = 0;
        m_file     = std::make_unique<AudioFile<float>>();
        if( m_file )
        {
            stop();
            m_file->load( File );
            maxSamples = m_file->getNumSamplesPerChannel();
        }
    }

    void AudioPlayback::setVolumeAndPan( float Volume, float Pan )
    {
        vp.setParameter( VolumeAndPan::kParamGain, Volume );
        vp.setParameter( VolumeAndPan::kParamPan, Pan );
    }

    void AudioPlayback::prepareToPlay( uint32_t samplerate, uint32_t blocksize )
    {
        Object::prepareToPlay( samplerate, blocksize );
        vp.prepareToPlay( samplerate, blocksize );
    }

    // Extended desktop reader: sample-accurate boundaries, pitch by linear
    // interpolation, and symmetric fades in output-frame time.
    void AudioPlayback::process( AudIO::Buffer<float>& buffer )
    {
        buffer.clearOutputs();
        if(!m_playing || !m_file || maxSamples<=0) return;
        const int begin=std::min(maxSamples-1,int(sliceStart*maxSamples));
        const int end=std::clamp(int(std::ceil(sliceEnd*maxSamples)),begin+1,maxSamples);
        const double fade=std::min(fadeFrames,(end-begin)/pitchRatio*.5);
        for(uint32_t sample=0;sample<blockSize();++sample){
            if(readIndex>=end){if(m_loop.load())readIndex=begin;else{m_playing=false;break;}}
            readIndex=std::max(readIndex,double(begin));
            const int a=int(readIndex),b=std::min(a+1,end-1);const double f=readIndex-a;
            double envelope=1;
            if(fade>0){envelope=std::clamp(std::min((readIndex-begin)/pitchRatio,(end-1-readIndex)/pitchRatio)/fade,0.0,1.0);}
            if(!m_mute.load() && !m_bypass.load()){
                for(uint32_t ch=0;ch<buffer.getNumOutputChannels();++ch){
                    auto out=buffer.getOutputBuffer(ch);if(!out)continue;
                    const auto& data=m_file->samples[std::min<int>(ch,m_file->getNumChannels()-1)];
                    out[sample]=float((data[a]*(1-f)+data[b]*f)*envelope*vp.getGain(ch));
                }
            }
            readIndex+=pitchRatio;
        }
    }

    void AudioPlayback::reset()
    {
        m_file.reset();
        readIndex = 0;
    }

    void AudioPlayback::play( bool FromBegin )
    {
        if( FromBegin )
            readIndex = std::min(std::max(0,maxSamples-1),int(sliceStart*maxSamples));

        m_playing.store( maxSamples>0 );
    }

    void AudioPlayback::stop()
    {
        m_playing.store( false );
    }
};

// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
namespace mt {
// Interface enum values from the published AudIO/Objects/Recorder.h.
// This restores only the type contract. It does not supply the missing
// SampleRecorder or Looper implementations or pretend they are functional.
struct Recorder {
    enum Direction { FORWARD, REVERSE };
    enum Mode { L_MONO, R_MONO, LR_MONO, L_STEREO, R_STEREO, LR_STEREO };
};

#if defined( AUDIO_OBJECT_AUDIO_FILE_RECORDER ) && ( AUDIO_OBJECT_AUDIO_FILE_RECORDER > 0 )

class SampleRecorder : public AudIO::DRecorder
{
public:
    using Direction = Recorder::Direction;
    using Mode = Recorder::Mode;
    static constexpr Direction FORWARD = Recorder::FORWARD;
    static constexpr Direction REVERSE = Recorder::REVERSE;

    explicit SampleRecorder( AudIO::Design& design )
        : AudIO::DRecorder( design ) {}

    void setEnable( bool enabled ) { setBypass( ! enabled ); }
    bool isEnabled() const { return ! isBypassed(); }
    void setGainAndPan( float gain, float pan, bool ) { setVolumeAndPan( gain, pan ); }
    void setMonitorOn( bool monitor ) { setMonitor( monitor ); }
    void setGain( float gain ) { setVolume( gain ); }
    float getGain() { return getVolume(); }
    void setGainDB( float gainDB ) { setVolumeDB( gainDB ); }
    bool isForward() const { return isForwardDirection(); }
    void setDirection( Direction direction ) { AudIO::AudioRecorder::setDirection( direction == FORWARD ); }
    void setRecordingMode( Mode mode ) { setRecordMode( static_cast<AudIO::AudioRecorder::Mode>( mode ) ); }
    Mode getRecordingMode() const { return static_cast<Mode>( getRecordMode() ); }
    void startRecording( bool fromBeginning )
    {
        start( getRecordMode(), getDirection(), fromBeginning );
    }
    void stopRecording() { stop(); }
    void saveRecording( const std::string& path ) { save( path ); }

    juce::AudioBuffer<float>& getAudioBuffer()
    {
        const auto& channels = getRecorderData();
        const auto samples = channels.empty() ? 0 : static_cast<int>( channels.front().size() );
        audioView.setSize( static_cast<int>( channels.size() ), samples, false, false, true );
        for( int channel = 0; channel < audioView.getNumChannels(); ++channel )
            audioView.copyFrom( channel, 0, channels[static_cast<size_t>( channel )].data(), samples );
        return audioView;
    }

private:
    juce::AudioBuffer<float> audioView;
};

class Looper : public SampleRecorder
{
public:
    explicit Looper( AudIO::Design& design )
        : SampleRecorder( design ) {}

    void setRecord( bool record )
    {
        if( record ) startRecording( false );
        else stopRecording();
    }
    void setPlay() { startPreview( 0.0 ); }
    void setMonitorGain( float gain ) { monitorGain = gain; }
    float getMonitorGain() const { return monitorGain; }
    double getLength() { return getSampleRate() > 0.0 ? getAudioBuffer().getNumSamples() / getSampleRate() : 0.0; }
    bool isEmpty() { return getAudioBuffer().getNumSamples() == 0; }
    bool isRunning() const { return isRecording() || isPreviewing(); }
    bool isForward() const { return SampleRecorder::isForward(); }
    void start( bool fromBeginning, bool ) { startRecording( fromBeginning ); }
    void play( double fromSeconds ) { startPreview( fromSeconds ); }
    void clear() { reset(); }
    void undo() {}
    void redo() {}
    void save( const juce::String& path ) { saveRecording( path.toStdString() ); }
    double& getProgress()
    {
        progress = readProgress();
        return progress;
    }

private:
    float monitorGain = 1.0f;
    double progress = 0.0;
};

#endif
}

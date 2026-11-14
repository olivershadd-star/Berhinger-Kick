// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <array>
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace lab {
constexpr int Pads=16, Groups=8, Steps=16, Block=256, Rate=48000, InsertSlots=4, InsertParameters=16;
constexpr int Sequences=16, MaxBars=4, MaxSequenceSteps=Steps*MaxBars, MaxSongSteps=32;
enum class ProgramType { Drum=0, Keygroup=1 };
struct TrackData {
    std::string name;
    ProgramType program=ProgramType::Drum;
    int sourcePad=0;
    double level=1;
    double pan=0;
    bool mute=false;
    bool solo=false;
    bool armed=false;
};
// Values 1 and 2 are kept stable for KICKLAB 3 projects written by 0.2.3.
enum class InsertEffect {
    None=0, Lowpass=1, PurestDrive=2, Highpass=3, Isolator=4,
    Tube2=5, Mojo=6, DrumSlam=7, ButterComp2=8, SoftGate=9,
    Chorus=10, StarChild=11, Reverb=12, Loud=13, Mackity=14,
    Baxandall2=15, Biquad2=16, Pressure5=17, Pop2=18,
    AutoPan=19, ChorusEnsemble=20, PurestEcho=21, TapeDelay2=22,
    Chamber2=23, Galactic=24, DeRez2=25, BitGlitter=26, Count=27
};
struct InsertState {
    InsertEffect effect=InsertEffect::None;
    bool bypass=false;
    std::array<double,InsertParameters> parameters{};
};
struct InsertEffectInfo {
    InsertEffect effect;
    const char* name;
    const char* category;
    int parameterCount;
    std::array<const char*,InsertParameters> parameterNames;
    std::array<double,InsertParameters> defaults;
    int tailBlocks;
};
const std::array<InsertEffectInfo,26>& insertEffectCatalog();
InsertState defaultInsertState(InsertEffect effect);
struct PadEdit {
    double start=0, end=1, gain=1, semitones=0, fadeMs=2;
    double attack=0, decay=0, sustain=1, release=0;
    bool insertsBypass=false;
    std::array<InsertState,InsertSlots> inserts{};
    PadEdit(){inserts[0]=defaultInsertState(InsertEffect::Lowpass);inserts[0].bypass=true;inserts[1]=defaultInsertState(InsertEffect::PurestDrive);inserts[1].bypass=true;}
};
enum class PerformanceMode { Drum=0, Chromatic=1, Major=2, Minor=3 };
struct SequenceData {
    std::string name;
    int bars=1;
    PerformanceMode mode=PerformanceMode::Drum;
    int sourcePad=12;
    int rootNote=48;
    std::array<std::array<bool,MaxSequenceSteps>,Pads> drum{};
    std::array<int,MaxSequenceSteps> melody{};
    std::array<TrackData,Pads> tracks{};
    SequenceData(){
        melody.fill(-1);
        for(int track=0;track<Pads;++track){tracks[track].name="Track "+std::to_string(track+1);tracks[track].sourcePad=track;}
    }
};
struct SongStep {
    int sequence=0;
    int repeats=1;
};
const char* insertEffectName(InsertEffect effect);
int insertParameterCount(InsertEffect effect);
const char* insertParameterName(InsertEffect effect,int parameter);
struct Snapshot {
    std::array<std::string,Pads> names{},paths{};
    // Projection of the selected sequence and visible bar for the Windows grid.
    std::array<std::array<bool,Steps>,Pads> pattern{};
    std::array<int,Steps> melody{};
    std::array<SequenceData,Sequences> sequences{};
    std::array<SongStep,MaxSongSteps> song{};
    std::array<std::vector<float>,Pads> waveforms{};
    std::array<bool,Pads> active{};
    std::array<PadEdit,Pads> edits{};
    std::array<double,Pads> durations{}, progress{};
    double tempo=120,volume=.65;
    bool playing=false,songPlaying=false;
    int step=-1,playBar=0,viewBar=0,sequence=0,queuedSequence=-1,group=0;
    int selectedTrack=0;
    int songLength=0,songPosition=-1,songRepeat=0;
    float peak=0;
};
class Engine {
public:
    Engine(); ~Engine();
    Engine(const Engine&)=delete;
    Engine& operator=(const Engine&)=delete;
    bool load(int pad,const std::string& path,std::string& error,bool preserveEdit=false);
    bool preview(const std::string& path,std::string& error);
    void stopPreview();
    void trigger(int pad);
    void triggerPitch(int pad,int semitones);
    void release(int pad);
    void releasePerformancePad(int performancePad);
    void setEdit(int pad, const PadEdit& edit);
    void setInsertEffect(int pad,int slot,InsertEffect effect);
    void setInsertBypass(int pad,int slot,bool bypass);
    void setInsertsBypass(int pad,bool bypass);
    void setInsertParameter(int pad,int slot,int parameter,double value);
    void toggle(int pad,int step);
    void clearPattern(); void demoPattern();
    void selectSequence(int sequence);
    void queueSequence(int sequence);
    void setSequenceBars(int bars);
    void setViewBar(int bar);
    void setGroup(int group);
    void setPerformanceMode(PerformanceMode mode);
    void setMelodicSourcePad(int pad);
    void setRootNote(int note);
    void selectTrack(int track);
    void setTrackLevel(int track,double level);
    void setTrackPan(int track,double pan);
    void setTrackMute(int track,bool mute);
    void setTrackSolo(int track,bool solo);
    void setTrackArmed(int track,bool armed);
    void setTrackProgram(int track,ProgramType program);
    int performanceNote(int performancePad)const;
    // Records onto the running sequence, snapped to 1/4, 1/8 or 1/16.
    // note < 0 records a drum hit on pad; otherwise it records the MIDI note.
    bool recordHit(int pad,int note,int quantizeDivision);
    void addSongStep(int sequence,int repeats);
    void editSongStep(int position,int sequence,int repeats);
    void deleteSongStep(int position);
    void clearSong();
    void playSong();
    void setPlaying(bool); void stop();
    void setTempo(double); void setVolume(double);
    Snapshot snapshot();
    // Exactly Block stereo frames; UI waveform copies do not hold render locks.
    void render(float* interleaved);
    bool save(const std::string&,std::string& error);
    bool open(const std::string&,std::string& error);
private:
    struct Impl; std::unique_ptr<Impl> impl;
};
}

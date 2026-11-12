// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <array>
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace lab {
constexpr int Pads=16, Steps=16, Block=256, Rate=48000, InsertSlots=4, InsertParameters=16;
// Values 1 and 2 are kept stable for KICKLAB 3 projects written by 0.2.3.
enum class InsertEffect {
    None=0, Lowpass=1, PurestDrive=2, Highpass=3, Isolator=4,
    Tube2=5, Mojo=6, DrumSlam=7, ButterComp2=8, SoftGate=9,
    Chorus=10, StarChild=11, Reverb=12, Loud=13, Mackity=14
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
    std::array<const char*,3> parameterNames;
    std::array<double,3> defaults;
    int tailBlocks;
};
const std::array<InsertEffectInfo,14>& insertEffectCatalog();
InsertState defaultInsertState(InsertEffect effect);
struct PadEdit {
    double start=0, end=1, gain=1, semitones=0, fadeMs=2;
    bool insertsBypass=false;
    std::array<InsertState,InsertSlots> inserts{};
    PadEdit(){inserts[0]=defaultInsertState(InsertEffect::Lowpass);inserts[0].bypass=true;inserts[1]=defaultInsertState(InsertEffect::PurestDrive);inserts[1].bypass=true;}
};
const char* insertEffectName(InsertEffect effect);
int insertParameterCount(InsertEffect effect);
const char* insertParameterName(InsertEffect effect,int parameter);
struct Snapshot {
    std::array<std::string,Pads> names{},paths{};
    std::array<std::array<bool,Steps>,Pads> pattern{};
    std::array<std::vector<float>,Pads> waveforms{};
    std::array<bool,Pads> active{};
    std::array<PadEdit,Pads> edits{};
    std::array<double,Pads> durations{}, progress{};
    double tempo=120,volume=.65;
    bool playing=false;
    int step=-1;
    float peak=0;
};
class Engine {
public:
    Engine(); ~Engine();
    Engine(const Engine&)=delete;
    Engine& operator=(const Engine&)=delete;
    bool load(int pad,const std::string& path,std::string& error);
    void trigger(int pad);
    void setEdit(int pad, const PadEdit& edit);
    void setInsertEffect(int pad,int slot,InsertEffect effect);
    void setInsertBypass(int pad,int slot,bool bypass);
    void setInsertsBypass(int pad,bool bypass);
    void setInsertParameter(int pad,int slot,int parameter,double value);
    void toggle(int pad,int step);
    void clearPattern(); void demoPattern();
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

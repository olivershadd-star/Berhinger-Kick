// SPDX-License-Identifier: AGPL-3.0-or-later
// Original AudIO four-slot insert model with the two Airwindows processors
// already validated by the Windows prototype.
#include <set>
#include <stdexcept>
#include "airwindows/audioeffectx.h"
#define __audioeffect__
namespace awlow {
#include "airwindows/src/Lowpass/Lowpass.h"
#include "airwindows/src/Lowpass/Lowpass.cpp"
#include "airwindows/src/Lowpass/LowpassProc.cpp"
}
namespace awdrive {
#include "airwindows/src/PurestDrive/PurestDrive.h"
#include "airwindows/src/PurestDrive/PurestDrive.cpp"
#include "airwindows/src/PurestDrive/PurestDriveProc.cpp"
}
#undef __audioeffect__

static AudIO::ObjectFactoryRegistration<awlow::Lowpass> registerLowpass("Lowpass",1,"AirWindows","Plugin","Filter");
static AudIO::ObjectFactoryRegistration<awdrive::PurestDrive> registerPurestDrive("PurestDrive",1,"AirWindows","Plugin","Saturation");

struct PadEffects : public AudIO::DawInserts {
    static_assert(lab::InsertSlots==AudIO::DawInserts::MAX_INSERTS,"KICK insert count changed");
    std::array<lab::InsertEffect,lab::InsertSlots> types{};
    explicit PadEffects(const lab::PadEdit& edit){
        for(int slot=0;slot<lab::InsertSlots;++slot){
            types[slot]=edit.inserts[slot].effect;
            if(types[slot]!=lab::InsertEffect::None&&addEffect(lab::insertEffectName(types[slot]),slot))throw std::runtime_error("Effet AudIO introuvable.");
        }
        sync(edit);prepareToPlay(lab::Rate,lab::Block);
    }
    void sync(const lab::PadEdit& edit){
        setBypassAllEffects(edit.insertsBypass);
        for(int slot=0;slot<lab::InsertSlots;++slot){
            setEffectBypass(slot,edit.inserts[slot].bypass);
            int count=lab::insertParameterCount(edit.inserts[slot].effect);
            for(int parameter=0;parameter<count;++parameter)setEffectParameter(slot,parameter,float(edit.inserts[slot].parameters[parameter]));
        }
    }
    void render(AudIO::Buffer<float>& buffer){process(buffer);}
};

class PadPlayback : public AudIO::AudioPlayback {
public:
    lab::PadEdit edit;
    std::unique_ptr<PadEffects> effects=std::make_unique<PadEffects>(edit);
    void trigger(){
        setSlice(edit.start,edit.end,std::pow(2.,edit.semitones/12.),edit.fadeMs);
        play(true);tail=32;
    }
    void stop(){AudIO::AudioPlayback::stop();tail=0;}
    void resetMix(){gain=edit.gain;tail=0;}
protected:
    void process(AudIO::Buffer<float>& buffer)override {
        AudIO::AudioPlayback::process(buffer);
        const bool sounded=isPlaying();
        if(sounded)tail=32;else if(tail>0)--tail;
        if(tail>0)effects->render(buffer);
        float* out[2]={buffer.getOutputBuffer(0),buffer.getOutputBuffer(1)};
        for(int n=0;n<lab::Block;++n){gain+=(edit.gain-gain)*.004;
            for(int c=0;c<2;++c)out[c][n]=(tail>0||sounded)?float(out[c][n]*gain):0.f;}
    }
private:
    double gain=1;
    int tail=0;
};

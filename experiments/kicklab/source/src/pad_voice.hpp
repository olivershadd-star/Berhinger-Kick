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
namespace awhighpass {
#include "airwindows/src/Highpass/Highpass.h"
#include "airwindows/src/Highpass/Highpass.cpp"
#include "airwindows/src/Highpass/HighpassProc.cpp"
}
namespace awisolator {
#include "airwindows/src/Isolator/Isolator.h"
#include "airwindows/src/Isolator/Isolator.cpp"
#include "airwindows/src/Isolator/IsolatorProc.cpp"
}
namespace awtube2 {
#include "airwindows/src/Tube2/Tube2.h"
#include "airwindows/src/Tube2/Tube2.cpp"
#include "airwindows/src/Tube2/Tube2Proc.cpp"
}
namespace awmojo {
#include "airwindows/src/Mojo/Mojo.h"
#include "airwindows/src/Mojo/Mojo.cpp"
#include "airwindows/src/Mojo/MojoProc.cpp"
}
namespace awdrumslam {
#include "airwindows/src/DrumSlam/DrumSlam.h"
#include "airwindows/src/DrumSlam/DrumSlam.cpp"
#include "airwindows/src/DrumSlam/DrumSlamProc.cpp"
}
namespace awbuttercomp2 {
#include "airwindows/src/ButterComp2/ButterComp2.h"
#include "airwindows/src/ButterComp2/ButterComp2.cpp"
#include "airwindows/src/ButterComp2/ButterComp2Proc.cpp"
}
namespace awsoftgate {
#include "airwindows/src/SoftGate/SoftGate.h"
#include "airwindows/src/SoftGate/SoftGate.cpp"
#include "airwindows/src/SoftGate/SoftGateProc.cpp"
}
namespace awchorus {
#include "airwindows/src/Chorus/Chorus.h"
#include "airwindows/src/Chorus/Chorus.cpp"
#include "airwindows/src/Chorus/ChorusProc.cpp"
}
namespace awstarchild {
#include "airwindows/src/StarChild/StarChild.h"
#include "airwindows/src/StarChild/StarChild.cpp"
#include "airwindows/src/StarChild/StarChildProc.cpp"
}
namespace awreverb {
#include "airwindows/src/Reverb/Reverb.h"
#include "airwindows/src/Reverb/Reverb.cpp"
#include "airwindows/src/Reverb/ReverbProc.cpp"
}
namespace awloud {
#include "airwindows/src/Loud/Loud.h"
#include "airwindows/src/Loud/Loud.cpp"
#include "airwindows/src/Loud/LoudProc.cpp"
}
namespace awmackity {
#include "airwindows/src/Mackity/Mackity.h"
#include "airwindows/src/Mackity/Mackity.cpp"
#include "airwindows/src/Mackity/MackityProc.cpp"
}
#undef __audioeffect__

static AudIO::ObjectFactoryRegistration<awlow::Lowpass> registerLowpass("Lowpass",1,"AirWindows","Plugin","Filter");
static AudIO::ObjectFactoryRegistration<awdrive::PurestDrive> registerPurestDrive("PurestDrive",1,"AirWindows","Plugin","Saturation");
static AudIO::ObjectFactoryRegistration<awhighpass::Highpass> registerHighpass("Highpass",1,"AirWindows","Plugin","Filter");
static AudIO::ObjectFactoryRegistration<awisolator::Isolator> registerIsolator("Isolator",1,"AirWindows","Plugin","Filter");
static AudIO::ObjectFactoryRegistration<awtube2::Tube2> registerTube2("Tube2",1,"AirWindows","Plugin","Saturation");
static AudIO::ObjectFactoryRegistration<awmojo::Mojo> registerMojo("Mojo",1,"AirWindows","Plugin","Saturation");
static AudIO::ObjectFactoryRegistration<awdrumslam::DrumSlam> registerDrumSlam("DrumSlam",1,"AirWindows","Plugin","Dynamics");
static AudIO::ObjectFactoryRegistration<awbuttercomp2::ButterComp2> registerButterComp2("ButterComp2",1,"AirWindows","Plugin","Dynamics");
static AudIO::ObjectFactoryRegistration<awsoftgate::SoftGate> registerSoftGate("SoftGate",1,"AirWindows","Plugin","Dynamics");
static AudIO::ObjectFactoryRegistration<awchorus::Chorus> registerChorus("Chorus",1,"AirWindows","Plugin","Ambience");
static AudIO::ObjectFactoryRegistration<awstarchild::StarChild> registerStarChild("StarChild",1,"AirWindows","Plugin","Ambience");
static AudIO::ObjectFactoryRegistration<awreverb::Reverb> registerReverb("Reverb",1,"AirWindows","Plugin","Reverb");
static AudIO::ObjectFactoryRegistration<awloud::Loud> registerLoud("Loud",1,"AirWindows","Plugin","Distortion");
static AudIO::ObjectFactoryRegistration<awmackity::Mackity> registerMackity("Mackity",1,"AirWindows","Plugin","Distortion");

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
    int tailBlocks()const {
        int result=32;
        for(auto type:types)for(const auto& info:lab::insertEffectCatalog())if(type==info.effect)result=std::max(result,info.tailBlocks);
        return result;
    }
    void render(AudIO::Buffer<float>& buffer){process(buffer);}
};

class PadPlayback : public AudIO::AudioPlayback {
public:
    lab::PadEdit edit;
    std::unique_ptr<PadEffects> effects=std::make_unique<PadEffects>(edit);
    void trigger(){
        setSlice(edit.start,edit.end,std::pow(2.,edit.semitones/12.),edit.fadeMs);
        play(true);tail=effects->tailBlocks();
    }
    void stop(){AudIO::AudioPlayback::stop();tail=0;}
    void resetMix(){gain=edit.gain;tail=0;}
protected:
    void process(AudIO::Buffer<float>& buffer)override {
        AudIO::AudioPlayback::process(buffer);
        const bool sounded=isPlaying();
        if(sounded)tail=effects->tailBlocks();else if(tail>0)--tail;
        if(tail>0)effects->render(buffer);
        float* out[2]={buffer.getOutputBuffer(0),buffer.getOutputBuffer(1)};
        for(int n=0;n<lab::Block;++n){gain+=(edit.gain-gain)*.004;
            for(int c=0;c<2;++c)out[c][n]=(tail>0||sounded)?float(out[c][n]*gain):0.f;}
    }
private:
    double gain=1;
    int tail=0;
};

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
namespace awbaxandall2 {
#include "airwindows/src/Baxandall2/Baxandall2.h"
#include "airwindows/src/Baxandall2/Baxandall2.cpp"
#include "airwindows/src/Baxandall2/Baxandall2Proc.cpp"
}
namespace awbiquad2 {
#include "airwindows/src/Biquad2/Biquad2.h"
#include "airwindows/src/Biquad2/Biquad2.cpp"
#include "airwindows/src/Biquad2/Biquad2Proc.cpp"
}
namespace awpressure5 {
#include "airwindows/src/Pressure5/Pressure5.h"
#include "airwindows/src/Pressure5/Pressure5.cpp"
#include "airwindows/src/Pressure5/Pressure5Proc.cpp"
}
namespace awpop2 {
#include "airwindows/src/Pop2/Pop2.h"
#include "airwindows/src/Pop2/Pop2.cpp"
#include "airwindows/src/Pop2/Pop2Proc.cpp"
}
namespace awautopan {
#include "airwindows/src/AutoPan/AutoPan.h"
#include "airwindows/src/AutoPan/AutoPan.cpp"
#include "airwindows/src/AutoPan/AutoPanProc.cpp"
}
namespace awchorusensemble {
#include "airwindows/src/ChorusEnsemble/ChorusEnsemble.h"
#include "airwindows/src/ChorusEnsemble/ChorusEnsemble.cpp"
#include "airwindows/src/ChorusEnsemble/ChorusEnsembleProc.cpp"
}
namespace awpurestecho {
#include "airwindows/src/PurestEcho/PurestEcho.h"
#include "airwindows/src/PurestEcho/PurestEcho.cpp"
#include "airwindows/src/PurestEcho/PurestEchoProc.cpp"
}
namespace awtapedelay2 {
#include "airwindows/src/TapeDelay2/TapeDelay2.h"
#include "airwindows/src/TapeDelay2/TapeDelay2.cpp"
#include "airwindows/src/TapeDelay2/TapeDelay2Proc.cpp"
}
namespace awchamber2 {
#include "airwindows/src/Chamber2/Chamber2.h"
#include "airwindows/src/Chamber2/Chamber2.cpp"
#include "airwindows/src/Chamber2/Chamber2Proc.cpp"
}
namespace awgalactic {
#include "airwindows/src/Galactic/Galactic.h"
#include "airwindows/src/Galactic/Galactic.cpp"
#include "airwindows/src/Galactic/GalacticProc.cpp"
}
namespace awderez2 {
#include "airwindows/src/DeRez2/DeRez2.h"
#include "airwindows/src/DeRez2/DeRez2.cpp"
#include "airwindows/src/DeRez2/DeRez2Proc.cpp"
}
namespace awbitglitter {
#include "airwindows/src/BitGlitter/BitGlitter.h"
#include "airwindows/src/BitGlitter/BitGlitter.cpp"
#include "airwindows/src/BitGlitter/BitGlitterProc.cpp"
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
static AudIO::ObjectFactoryRegistration<awbaxandall2::Baxandall2> registerBaxandall2("Baxandall2",1,"AirWindows","Plugin","EQ");
static AudIO::ObjectFactoryRegistration<awbiquad2::Biquad2> registerBiquad2("Biquad2",1,"AirWindows","Plugin","Filter");
static AudIO::ObjectFactoryRegistration<awpressure5::Pressure5> registerPressure5("Pressure5",1,"AirWindows","Plugin","Dynamics");
static AudIO::ObjectFactoryRegistration<awpop2::Pop2> registerPop2("Pop2",1,"AirWindows","Plugin","Dynamics");
static AudIO::ObjectFactoryRegistration<awautopan::AutoPan> registerAutoPan("AutoPan",1,"AirWindows","Plugin","Modulation");
static AudIO::ObjectFactoryRegistration<awchorusensemble::ChorusEnsemble> registerChorusEnsemble("ChorusEnsemble",1,"AirWindows","Plugin","Modulation");
static AudIO::ObjectFactoryRegistration<awpurestecho::PurestEcho> registerPurestEcho("PurestEcho",1,"AirWindows","Plugin","Delay");
static AudIO::ObjectFactoryRegistration<awtapedelay2::TapeDelay2> registerTapeDelay2("TapeDelay2",1,"AirWindows","Plugin","Delay");
static AudIO::ObjectFactoryRegistration<awchamber2::Chamber2> registerChamber2("Chamber2",1,"AirWindows","Plugin","Reverb");
static AudIO::ObjectFactoryRegistration<awgalactic::Galactic> registerGalactic("Galactic",1,"AirWindows","Plugin","Reverb");
static AudIO::ObjectFactoryRegistration<awderez2::DeRez2> registerDeRez2("DeRez2",1,"AirWindows","Plugin","Lo-Fi");
static AudIO::ObjectFactoryRegistration<awbitglitter::BitGlitter> registerBitGlitter("BitGlitter",1,"AirWindows","Plugin","Lo-Fi");

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
    void trigger(double transpose=0){
        setSlice(edit.start,edit.end,std::pow(2.,(edit.semitones+transpose)/12.),edit.fadeMs);
        // Pads are one-shots. Looping must only ever be enabled by a future,
        // explicit KICK loop mode, never by importing or auditioning a WAV.
        setLooping(false);
        play(true);tail=effects->tailBlocks();
        envelope=edit.attack<=0?1.:0.;
        envelopeStage=edit.attack<=0?(edit.decay<=0?EnvelopeStage::Sustain:EnvelopeStage::Decay):EnvelopeStage::Attack;
    }
    void release(){
        if(envelopeStage==EnvelopeStage::Idle||envelopeStage==EnvelopeStage::Release)return;
        if(edit.release<=0){envelope=0;envelopeStage=EnvelopeStage::Idle;AudIO::AudioPlayback::stop();tail=effects->tailBlocks();return;}
        releaseStep=envelope/(std::max(.0001,edit.release*edit.release*4.)*lab::Rate);envelopeStage=EnvelopeStage::Release;
    }
    void stop(){AudIO::AudioPlayback::stop();tail=0;envelope=0;envelopeStage=EnvelopeStage::Idle;}
    void resetMix(){gain=edit.gain;tail=0;envelope=0;envelopeStage=EnvelopeStage::Idle;}
    void setTrackMix(double levelValue,double panValue){trackLevel=std::clamp(levelValue,0.,1.);trackPan=std::clamp(panValue,-1.,1.);}
protected:
    void process(AudIO::Buffer<float>& buffer)override {
        AudIO::AudioPlayback::process(buffer);
        const bool envelopeWasRunning=envelopeStage!=EnvelopeStage::Idle;
        float* out[2]={buffer.getOutputBuffer(0),buffer.getOutputBuffer(1)};
        for(int n=0;n<lab::Block;++n){
            advanceEnvelope();
            for(int c=0;c<2;++c)out[c][n]=float(out[c][n]*envelope);
        }
        const bool sounded=isPlaying()||envelopeWasRunning;
        if(!isPlaying()&&envelopeStage!=EnvelopeStage::Release){envelope=0;envelopeStage=EnvelopeStage::Idle;}
        if(sounded)tail=effects->tailBlocks();else if(tail>0)--tail;
        if(tail>0)effects->render(buffer);
        const double left=trackLevel*(trackPan>0?1.-trackPan:1.);
        const double right=trackLevel*(trackPan<0?1.+trackPan:1.);
        for(int n=0;n<lab::Block;++n){gain+=(edit.gain-gain)*.004;
            out[0][n]=(tail>0||sounded)?float(out[0][n]*gain*left):0.f;
            out[1][n]=(tail>0||sounded)?float(out[1][n]*gain*right):0.f;}
    }
private:
    enum class EnvelopeStage { Idle,Attack,Decay,Sustain,Release };
    void advanceEnvelope(){
        if(envelopeStage==EnvelopeStage::Attack){
            envelope+=1./(std::max(.0001,edit.attack*edit.attack*2.)*lab::Rate);
            if(envelope>=1.){envelope=1.;envelopeStage=edit.decay<=0?EnvelopeStage::Sustain:EnvelopeStage::Decay;}
        }else if(envelopeStage==EnvelopeStage::Decay){
            const double target=std::clamp(edit.sustain,0.,1.);
            envelope-=(1.-target)/(std::max(.0001,edit.decay*edit.decay*2.)*lab::Rate);
            if(envelope<=target){envelope=target;envelopeStage=EnvelopeStage::Sustain;}
        }else if(envelopeStage==EnvelopeStage::Sustain)envelope=std::clamp(edit.sustain,0.,1.);
        else if(envelopeStage==EnvelopeStage::Release){envelope-=releaseStep;if(envelope<=0){envelope=0;envelopeStage=EnvelopeStage::Idle;AudIO::AudioPlayback::stop();}}
    }
    double gain=1;
    double trackLevel=1,trackPan=0;
    double envelope=0,releaseStep=0;
    EnvelopeStage envelopeStage=EnvelopeStage::Idle;
    int tail=0;
};

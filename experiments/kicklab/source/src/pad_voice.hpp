// SPDX-License-Identifier: AGPL-3.0-or-later
// Two original Airwindows implementations from 0700-AAM, isolated exactly as
// in its airwindows2.cpp amalgamation. No plugin SDK or JUCE is required.
#include <set>
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
struct PadEffects {
    awlow::Lowpass filter{0};
    awdrive::PurestDrive drive{0};
    PadEffects(){
        static_cast<AudIO::Object&>(filter).prepareToPlay(lab::Rate,lab::Block);
        static_cast<AudIO::Object&>(drive).prepareToPlay(lab::Rate,lab::Block);
    }
};
class PadPlayback : public AudIO::AudioPlayback {
public:
    lab::PadEdit edit;
    std::unique_ptr<PadEffects> effects=std::make_unique<PadEffects>();
    void trigger(){
        setSlice(edit.start,edit.end,std::pow(2.,edit.semitones/12.),edit.fadeMs);
        play(true);tail=32;
    }
    void stop(){AudIO::AudioPlayback::stop();tail=0;}
    void resetMix(){gain=edit.gain;filterWet=driveWet=0;opening=1;intensity=0;tail=0;}
protected:
    void process(AudIO::Buffer<float>& buffer)override {
        AudIO::AudioPlayback::process(buffer);
        const bool sounded=isPlaying();
        if(sounded)tail=32;else if(tail>0)--tail;
        float* out[2]={buffer.getOutputBuffer(0),buffer.getOutputBuffer(1)};
        constexpr int segment=32;
        const double filterTarget=(!edit.bypass&&edit.filterOpen<.9999)?1:0;
        const double driveTarget=(!edit.bypass&&edit.drive>.0001)?1:0;
        for(int start=0;start<lab::Block;start+=segment){
            opening+=(edit.filterOpen-opening)*.12;
            intensity+=(edit.drive-intensity)*.12;
            float filtered[2][segment],driven[2][segment];
            float* f[2]={filtered[0],filtered[1]};float* d[2]={driven[0],driven[1]};
            for(int c=0;c<2;++c)for(int n=0;n<segment;++n)f[c][n]=out[c][start+n];
            if(tail>0&&(filterTarget||filterWet>1e-5)){
                effects->filter.setParameter(0,float(.01+opening*.99));
                effects->filter.setParameter(1,.5f);effects->filter.setParameter(2,1);
                effects->filter.processReplacing(f,f,segment);
            }
            for(int n=0;n<segment;++n){filterWet+=(filterTarget-filterWet)*.004;
                for(int c=0;c<2;++c)d[c][n]=float(out[c][start+n]*(1-filterWet)+f[c][n]*filterWet);}
            const float boost=float(1+intensity*3);
            float original[2][segment];
            for(int c=0;c<2;++c)for(int n=0;n<segment;++n){original[c][n]=d[c][n];d[c][n]*=boost;}
            if(tail>0&&(driveTarget||driveWet>1e-5)){
                effects->drive.setParameter(0,float(intensity));effects->drive.processReplacing(d,d,segment);
            }
            for(int n=0;n<segment;++n){driveWet+=(driveTarget-driveWet)*.004;gain+=(edit.gain-gain)*.004;
                for(int c=0;c<2;++c){double value=original[c][n]*(1-driveWet)+d[c][n]/boost*driveWet;
                    out[c][start+n]=(tail>0||sounded)?float(value*gain):0.f;}}
        }
    }
private:
    double gain=1,filterWet=0,driveWet=0,opening=1,intensity=0;
    int tail=0;
};

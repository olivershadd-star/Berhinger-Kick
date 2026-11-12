// SPDX-License-Identifier: AGPL-3.0-or-later
#include "engine.hpp"
#include "audio_queue.hpp"
#include "control_surface.hpp"
#include <thread>
#include <atomic>
#include <fstream>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <vector>
#include <cstdint>
#include <iomanip>
using namespace lab;
void check(bool b,const char* m){if(!b)throw std::runtime_error(m);}
void wav(const std::string& file,int rate,int channels,int frames,double frequency=440,double amplitude=8000){std::ofstream f(std::filesystem::u8path(file),std::ios::binary);auto u16=[&](unsigned n){char b[2]={char(n),char(n>>8)};f.write(b,2);};auto u32=[&](unsigned n){u16(n);u16(n>>16);};f.write("RIFF",4);u32(36+frames*channels*2);f.write("WAVEfmt ",8);u32(16);u16(1);u16(channels);u32(rate);u32(rate*channels*2);u16(channels*2);u16(16);f.write("data",4);u32(frames*channels*2);for(int n=0;n<frames;++n)for(int c=0;c<channels;++c)u16(static_cast<uint16_t>(int16_t(amplitude*std::sin(2*3.141592653589793*frequency*n/rate))));}
int main(int argc,char**argv){try{
    SurfaceState surface;
    check(surface.displays[0].parameterForKnob(3)==3,"left display Q-Link mapping failed");
    check(surface.displays[1].parameterForKnob(0)==4,"right display page mapping failed");
    surface.selectPad(11);check(surface.selectedPad==11&&surface.displays[0].pad==11&&surface.displays[1].pad==11,"display did not follow selected pad");
    surface.displays[1].followsSelectedPad=false;surface.displays[1].pad=3;surface.selectPad(5);check(surface.displays[0].pad==5&&surface.displays[1].pad==3,"independent display binding failed");
    surface.setDisplayPage(0,2);check(surface.displays[0].parameterForKnob(1)==9,"four-knob page mapping failed");
    auto dir=std::filesystem::absolute(argc>1?argv[1]:"test-output");std::filesystem::create_directories(dir);
    auto path=[&](const char* n){return (dir/n).u8string();};
    wav(path("mono.wav"),48000,1,300);wav(path("resample.wav"),44100,2,4410);wav(path("long.wav"),48000,2,48000);
    Engine e;float block[Block*2];std::string error;
    e.render(block);for(float v:block)check(v==0,"empty engine is not silent");
    check(e.load(0,path("mono.wav"),error),error.c_str());e.trigger(0);e.render(block);
    // The short clip now has 2 ms fades plus the engine startup ramp.
    double energy=0;for(int n=0;n<Block;++n){check(block[n*2]==block[n*2+1],"mono is not duplicated");energy+=block[n*2]*block[n*2];}check(energy>1e-5,"playback is silent with short fade");
    e.render(block);check(block[43*2]==0,"end fade is not silent at boundary");for(int n=44;n<Block;++n)check(block[n*2]==0&&block[n*2+1]==0,"stale samples after EOF");e.render(block);for(float v:block)check(v==0,"one-shot does not stop");
    check(e.load(1,path("resample.wav"),error),error.c_str());e.trigger(1);int active=0;for(int b=0;b<24;++b){e.render(block);for(int n=0;n<Block;++n)if(std::abs(block[n*2])>.00001)++active;}check(active>4500&&active<4801,"44.1 kHz conversion duration incorrect");
    e.trigger(0);e.setVolume(0);e.render(block);for(float v:block)check(v==0,"master mute failed");e.setVolume(.65);e.stop();
    for(int p=0;p<Pads;++p)check(e.load(p,path("long.wav"),error),error.c_str());for(int p=0;p<Pads;++p)e.trigger(p);e.render(block);for(float v:block)check(std::isfinite(v)&&std::abs(v)<=.98f,"mixing overflow");
    e.stop();e.demoPattern();e.setTempo(120);e.setPlaying(true);e.render(block);e.setPlaying(false);int paused=e.snapshot().step;for(int b=0;b<30;++b)e.render(block);check(e.snapshot().step==paused,"pause advances sequencer");e.stop();e.setPlaying(true);std::ofstream raw(std::filesystem::u8path(path("demo.f32")),std::ios::binary);
    int transitions=0,last=-1;double sum=0;for(int b=0;b<375;++b){e.render(block);raw.write(reinterpret_cast<const char*>(block),sizeof(block));for(float v:block)sum+=v*v;int s=e.snapshot().step;if(s!=last){++transitions;last=s;}}check(transitions==16,"sequencer timing failed");check(sum>1,"sequencer is silent");e.stop();
    check(e.save(path("pattern.kicklab"),error),error.c_str());e.clearPattern();check(e.open(path("pattern.kicklab"),error),error.c_str());check(e.snapshot().pattern[0][0]&&e.snapshot().pattern[1][4],"pattern roundtrip failed");
    {std::ofstream f(std::filesystem::u8path(path("broken.wav")),std::ios::binary);f<<"RIFF";}check(!e.load(0,path("broken.wav"),error),"truncated WAV accepted");
    {std::ofstream f(std::filesystem::u8path(path("bad.kicklab")));f<<"INVALID";}check(!e.open(path("bad.kicklab"),error),"invalid project accepted");
    {std::ofstream f(std::filesystem::u8path(path("sparse.kicklab")));f<<"KICKLAB 1\n120 0.65\n";for(int p=0;p<Pads;++p)f<<0<<' '<<std::quoted(p==0?path("long.wav"):std::string{})<<'\n';}
    check(e.open(path("sparse.kicklab"),error),error.c_str());
    check(e.snapshot().paths[1].empty(),"loading sparse project retained old clip");
    // Trim, pitch and fades use actual rendered samples, not UI metadata.
    PadEdit edit;edit.start=.25;edit.end=.5;edit.fadeMs=0;e.setEdit(0,edit);e.stop();e.trigger(0);
    int nonzero=0;for(int b=0;b<60;++b){e.render(block);for(int n=0;n<Block;++n){if(std::abs(block[n*2])>1e-5)++nonzero;if(b*Block+n>=12000)check(block[n*2]==0,"trim leaked beyond end");}}
    check(nonzero>11500&&nonzero<12001,"trim duration incorrect");
    edit.start=0;edit.end=1;edit.semitones=12;e.setEdit(0,edit);e.stop();e.trigger(0);
    int crossings=0;float previous=0;
    for(int b=0;b<100;++b){e.render(block);for(int n=0;n<Block;++n){float v=block[n*2];if(previous<=0&&v>1e-5)++crossings;previous=v;if(b*Block+n>=24000)check(v==0,"pitch octave duration incorrect");}}
    check(crossings>=438&&crossings<=442,"octave pitch does not double frequency");
    wav(path("high.wav"),48000,2,48000,8000,26000);
    auto measure=[&](const PadEdit& settings){
        check(e.load(0,path("high.wav"),error),error.c_str());e.setEdit(0,settings);e.stop();e.trigger(0);
        std::vector<float> result;
        for(int b=0;b<96;++b){e.render(block);if(b>=32)result.insert(result.end(),block,block+Block*2);}
        return result;
    };
    PadEdit dry;dry.fadeMs=0;auto clean=measure(dry);double dryEnergy=0;for(float v:clean)dryEnergy+=v*v;
    auto half=dry;half.gain=.5;auto halfSignal=measure(half);double halfEnergy=0;for(float v:halfSignal)halfEnergy+=v*v;
    check(std::abs(halfEnergy/dryEnergy-.25)<1e-5,"per-pad gain is incorrect");
    auto muted=dry;muted.gain=0;auto silence=measure(muted);double muteEnergy=0;for(float v:silence)muteEnergy+=v*v;
    check(muteEnergy<dryEnergy*1e-12,"per-pad mute leaves audible signal");
    auto filtered=dry;filtered.inserts[0].bypass=false;filtered.inserts[0].parameters[0]=.1;auto low=measure(filtered);double lowEnergy=0;for(float v:low)lowEnergy+=v*v;
    check(lowEnergy<dryEnergy*.2,"Airwindows lowpass does not attenuate highs");
    auto saturated=dry;saturated.inserts[1].bypass=false;saturated.inserts[1].parameters[0]=1;auto drive=measure(saturated);double changed=0;
    for(size_t i=0;i<clean.size();++i){check(std::isfinite(drive[i]),"drive output is not finite");changed+=std::pow(clean[i]-drive[i],2);}
    check(changed>dryEnergy*.0001,"Airwindows drive does not change waveform");
    auto lowThenDrive=dry;lowThenDrive.inserts[0].bypass=false;lowThenDrive.inserts[0].parameters[0]=.35;lowThenDrive.inserts[1].bypass=false;lowThenDrive.inserts[1].parameters[0]=1;auto firstOrder=measure(lowThenDrive);
    auto driveThenLow=dry;driveThenLow.inserts[0]=lowThenDrive.inserts[1];driveThenLow.inserts[1]=lowThenDrive.inserts[0];auto secondOrder=measure(driveThenLow);double orderDifference=0;
    for(size_t i=0;i<firstOrder.size();++i)orderDifference+=std::pow(firstOrder[i]-secondOrder[i],2);
    check(orderDifference>dryEnergy*1e-8,"insert slot order does not affect processing order");
    check(insertEffectCatalog().size()==14,"effect catalog is incomplete");
    for(const auto& info:insertEffectCatalog()){
        check(info.parameterCount==insertParameterCount(info.effect),"effect parameter count mismatch");
        auto settings=dry;settings.inserts={};settings.inserts[0]=defaultInsertState(info.effect);auto signal=measure(settings);double effectEnergy=0;
        for(float value:signal){check(std::isfinite(value),"catalog effect output is not finite");effectEnergy+=value*value;}
        check(effectEnergy>1e-8,"catalog effect unexpectedly silenced the test signal");
    }
    check(e.load(0,path("mono.wav"),error),error.c_str());auto reverb=dry;reverb.inserts={};reverb.inserts[0]=defaultInsertState(InsertEffect::Reverb);e.setEdit(0,reverb);e.stop();e.trigger(0);double lateTail=0;
    for(int b=0;b<80;++b){e.render(block);if(b>40)for(float value:block)lateTail+=value*value;}
    check(lateTail>1e-8,"reverb tail was cut with the sample");
    saturated.insertsBypass=true;auto bypass=measure(saturated);double bypassDifference=0;
    for(size_t i=0;i<clean.size();++i)bypassDifference+=std::pow(clean[i]-bypass[i],2);
    check(bypassDifference<dryEnergy*1e-8,"FX bypass changes dry signal");
    edit={};edit.start=.12;edit.end=.83;edit.gain=.75;edit.semitones=-7;edit.fadeMs=4.5;edit.insertsBypass=true;
    edit.inserts[0]=defaultInsertState(InsertEffect::Highpass);edit.inserts[0].parameters[0]=.4;edit.inserts[0].parameters[1]=.2;edit.inserts[0].parameters[2]=.7;
    edit.inserts[1]=defaultInsertState(InsertEffect::DrumSlam);edit.inserts[1].parameters[0]=.8;
    edit.inserts[2]=defaultInsertState(InsertEffect::Chorus);edit.inserts[2].parameters[0]=.35;
    edit.inserts[3]=defaultInsertState(InsertEffect::Reverb);edit.inserts[3].bypass=true;edit.inserts[3].parameters[0]=.65;edit.inserts[3].parameters[1]=.75;e.setEdit(0,edit);
    check(e.save(path("edited.kicklab"),error),error.c_str());e.setEdit(0,{});check(e.open(path("edited.kicklab"),error),error.c_str());auto restored=e.snapshot().edits[0];
    check(restored.start==edit.start&&restored.end==edit.end&&restored.semitones==-7&&restored.gain==.75&&restored.fadeMs==4.5&&restored.insertsBypass,"edited project roundtrip failed");
    for(int slot=0;slot<InsertSlots;++slot){check(restored.inserts[slot].effect==edit.inserts[slot].effect&&restored.inserts[slot].bypass==edit.inserts[slot].bypass,"insert identity roundtrip failed");for(int parameter=0;parameter<insertParameterCount(edit.inserts[slot].effect);++parameter)check(restored.inserts[slot].parameters[parameter]==edit.inserts[slot].parameters[parameter],"insert parameter roundtrip failed");}
    check(e.open(path("sparse.kicklab"),error),error.c_str());restored=e.snapshot().edits[0];check(restored.inserts[0].effect==InsertEffect::Lowpass&&restored.inserts[0].bypass&&restored.inserts[1].effect==InsertEffect::PurestDrive&&restored.inserts[1].bypass,"v1 project did not restore bypassed default inserts");
    {std::ofstream f(std::filesystem::u8path(path("legacy-v2.kicklab")));f<<"KICKLAB 2\n120 0.65\n";for(int p=0;p<Pads;++p)f<<0<<' '<<std::quoted(p==0?path("long.wav"):std::string{})<<" 0.1 0.9 0.75 -3 4 0.4 0.8 1\n";}
    check(e.open(path("legacy-v2.kicklab"),error),error.c_str());restored=e.snapshot().edits[0];
    check(restored.insertsBypass&&restored.inserts[0].effect==InsertEffect::Lowpass&&!restored.inserts[0].bypass&&restored.inserts[0].parameters[0]==.4&&restored.inserts[1].effect==InsertEffect::PurestDrive&&!restored.inserts[1].bypass&&restored.inserts[1].parameters[0]==.8,"v2 effect migration failed");
    e.setInsertEffect(0,2,InsertEffect::PurestDrive);e.setInsertParameter(0,2,0,.42);e.setInsertBypass(0,2,true);restored=e.snapshot().edits[0];check(restored.inserts[2].effect==InsertEffect::PurestDrive&&restored.inserts[2].parameters[0]==.42&&restored.inserts[2].bypass,"insert control API failed");
    std::cout<<"PASS: trim, octave pitch, 14 Airwindows effects, four AudIO inserts, project v3 and v1/v2 compatibility\n";
    // Batched driver completions across the ring boundary must preserve time order.
    AudioQueue<4> queue;std::array<bool,4> ready={true,true,true,false};std::vector<int> submitted;
    auto submit=[&](size_t b){submitted.push_back(int(b));ready[b]=false;return true;};
    check(queue.refill([&](size_t b){return ready[b];},submit),"initial refill failed");
    ready[3]=ready[0]=true;
    check(queue.refill([&](size_t b){return ready[b];},submit),"wrap refill failed");
    check(submitted==std::vector<int>({0,1,2,3,0}),"driver completion reordered audio buffers");
    // UI snapshots/control edits must not insert silent blocks or pause audio time.
    e.stop();e.setVolume(.65);for(int b=0;b<32;++b)e.render(block);
    constexpr int stressBlocks=1200;
    std::vector<float> reference(stressBlocks*Block*2);
    for(int b=0;b<stressBlocks;++b){if(b%32==0)e.trigger(0);e.render(reference.data()+b*Block*2);}
    e.stop();
    std::atomic<bool> go{true};std::atomic<int> snapshots{0};
    std::vector<std::thread> ui;
    for(int t=0;t<3;++t)ui.emplace_back([&]{while(go.load()){auto copy=e.snapshot();e.setTempo(120);++snapshots;}});
    while(snapshots.load()<20)std::this_thread::yield();
    bool continuous=true;
    for(int b=0;b<stressBlocks;++b){if(b%32==0)e.trigger(0);e.render(block);for(int n=0;n<Block*2;++n)if(std::abs(block[n]-reference[b*Block*2+n])>1e-6f)continuous=false;}
    go=false;for(auto& thread:ui)thread.join();
    check(continuous,"UI activity changed audio samples or dropped blocks");
    std::cout<<"PASS: queue wraparound and concurrent UI/audio continuity ("<<snapshots<<" snapshots)\n";
    std::cout<<"PASS: silence, WAV playback, mono, EOF, resampling, mute, 16-pad mixing, sequencer, save/load, invalid input\n";
    return 0;
}catch(const std::exception& e){std::cerr<<"FAIL: "<<e.what()<<"\n";return 1;}}

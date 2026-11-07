// SPDX-License-Identifier: AGPL-3.0-or-later
#include "engine.hpp"
#include "audio_queue.hpp"
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
void wav(const std::string& file,int rate,int channels,int frames){std::ofstream f(std::filesystem::u8path(file),std::ios::binary);auto u16=[&](unsigned n){char b[2]={char(n),char(n>>8)};f.write(b,2);};auto u32=[&](unsigned n){u16(n);u16(n>>16);};f.write("RIFF",4);u32(36+frames*channels*2);f.write("WAVEfmt ",8);u32(16);u16(1);u16(channels);u32(rate);u32(rate*channels*2);u16(channels*2);u16(16);f.write("data",4);u32(frames*channels*2);for(int n=0;n<frames;++n)for(int c=0;c<channels;++c)u16(static_cast<uint16_t>(int16_t(8000*std::sin(2*3.141592653589793*440*n/rate))));}
int main(int argc,char**argv){try{
    auto dir=std::filesystem::absolute(argc>1?argv[1]:"test-output");std::filesystem::create_directories(dir);
    auto path=[&](const char* n){return (dir/n).u8string();};
    wav(path("mono.wav"),48000,1,300);wav(path("resample.wav"),44100,2,4410);wav(path("long.wav"),48000,2,48000);
    Engine e;float block[Block*2];std::string error;
    e.render(block);for(float v:block)check(v==0,"empty engine is not silent");
    check(e.load(0,path("mono.wav"),error),error.c_str());e.trigger(0);e.render(block);
    double energy=0;for(int n=0;n<Block;++n){check(block[n*2]==block[n*2+1],"mono is not duplicated");energy+=block[n*2]*block[n*2];}check(energy>.001,"playback is silent");
    e.render(block);for(int n=44;n<Block;++n)check(block[n*2]==0&&block[n*2+1]==0,"stale samples after EOF");e.render(block);for(float v:block)check(v==0,"one-shot does not stop");
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

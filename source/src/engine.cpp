// SPDX-License-Identifier: AGPL-3.0-or-later
#include "engine.hpp"
#include <thread> // Missing from the original AudIO amalgamation.
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include "AudIO.cpp"
#include "Objects/PlayBack.cpp"

namespace lab {
namespace {
unsigned u16(const std::vector<uint8_t>& v,size_t i) { return v.at(i)|(unsigned(v.at(i+1))<<8); }
unsigned u32(const std::vector<uint8_t>& v,size_t i) { return u16(v,i)|(u16(v,i+2)<<16); }
std::unique_ptr<AudioFile<float>> decode(const std::string& path) {
    std::ifstream in(std::filesystem::u8path(path),std::ios::binary|std::ios::ate);
    if(!in) throw std::runtime_error("Impossible d'ouvrir ce fichier.");
    auto length=in.tellg();
    if(length<44 || length>32*1024*1024) throw std::runtime_error("WAV invalide ou trop volumineux (maximum 32 Mo).");
    std::vector<uint8_t> raw(static_cast<size_t>(length)); in.seekg(0);
    if(!in.read(reinterpret_cast<char*>(raw.data()),length)) throw std::runtime_error("Lecture du fichier impossible.");
    if(memcmp(raw.data(),"RIFF",4)||memcmp(raw.data()+8,"WAVE",4)||u32(raw,4)!=raw.size()-8)
        throw std::runtime_error("Utilise un WAV RIFF PCM 16/24/32 bits ou flottant 32 bits.");
    size_t fmt=0,data=0,bytes=0; bool seenFmt=false,seenData=false;
    for(size_t p=12;p+8<=raw.size();) {
        size_t n=u32(raw,p+4);
        if(n>raw.size()-p-8) throw std::runtime_error("WAV tronque.");
        if(!memcmp(raw.data()+p,"fmt ",4)) { if(seenFmt||n<16) throw std::runtime_error("Format WAV invalide."); fmt=p;seenFmt=true; }
        if(!memcmp(raw.data()+p,"data",4)) { if(seenData) throw std::runtime_error("Plusieurs blocs audio non pris en charge.");data=p;bytes=n;seenData=true; }
        p+=8+n+(n&1);
    }
    if(!fmt||!data) throw std::runtime_error("Blocs WAV manquants.");
    unsigned encoding=u16(raw,fmt+8),ch=u16(raw,fmt+10),rate=u32(raw,fmt+12),bits=u16(raw,fmt+22);
    if((encoding!=1 && encoding!=3)||(encoding==3 && bits!=32)||(bits!=16 && bits!=24 && bits!=32)||ch<1||ch>2||rate<8000||rate>192000)
        throw std::runtime_error("WAV mono/stereo PCM 16/24/32 ou flottant 32 requis (8 a 192 kHz).");
    unsigned align=ch*bits/8;
    if(u16(raw,fmt+20)!=align||u32(raw,fmt+16)!=rate*align||bytes%align||bytes==0||bytes/align>rate*30)
        throw std::runtime_error("WAV incoherent, vide ou de plus de 30 secondes.");
    // Feed a canonical header to the bundled decoder; omit optional metadata.
    std::vector<uint8_t> canonical(44+bytes+(bytes&1),0);
    auto put16=[&](size_t o,unsigned n){canonical[o]=n&255;canonical[o+1]=(n>>8)&255;};
    auto put32=[&](size_t o,unsigned n){put16(o,n&65535);put16(o+2,n>>16);};
    memcpy(canonical.data(),"RIFF",4);put32(4,unsigned(canonical.size()-8));memcpy(canonical.data()+8,"WAVEfmt ",8);
    put32(16,16);put16(20,encoding);put16(22,ch);put32(24,rate);put32(28,rate*align);put16(32,align);put16(34,bits);
    memcpy(canonical.data()+36,"data",4);put32(40,unsigned(bytes));memcpy(canonical.data()+44,raw.data()+data+8,bytes);
    AudioFile<float> source;
    if(!source.loadFromMemory(canonical)) throw std::runtime_error("Le decodeur AudioFile ne peut pas lire ce WAV.");
    size_t frames=source.getNumSamplesPerChannel();
    size_t target=std::max<size_t>(1,static_cast<size_t>(std::llround(double(frames)*Rate/rate)));
    auto result=std::make_unique<AudioFile<float>>(); result->setAudioBufferSize(2,int(target));result->setSampleRate(Rate);
    for(int c=0;c<2;++c) for(size_t n=0;n<target;++n) {
        double pos=double(n)*rate/Rate;size_t a=std::min<size_t>(size_t(pos),frames-1), b=std::min(a+1,frames-1);
        float value=float(source.samples[std::min<unsigned>(c,ch-1)][a]*(1-(pos-a))+source.samples[std::min<unsigned>(c,ch-1)][b]*(pos-a));
        if(!std::isfinite(value)) throw std::runtime_error("Le WAV contient des valeurs audio non finies.");
        result->samples[c][n]=std::clamp(value,-1.f,1.f);
    }
    return result;
}
}
struct Engine::Impl {
    std::vector<float> pool;
    std::unique_ptr<AudIO::Design> graph;
    std::unique_ptr<AudIO::Buffer<float>> buffer;
    std::array<AudIO::AudioPlayback*,Pads> players{};
    std::array<std::atomic<bool>,Pads> pending{};
    Snapshot state;
    std::mutex mutex;
    double untilStep=0;
    int nextStep=0;
    Impl():pool(512*Block) {
        AudIO::MemoryPool<float>::get().initialize(pool.data(),512,Block);
        graph=std::make_unique<AudIO::Design>(0,2);
        auto bus=graph->createBus();
        for(int i=0;i<Pads;++i) {
            auto player=std::make_unique<AudIO::AudioPlayback>();players[i]=player.get();
            auto id=graph->createObject(std::move(player));
            if(!graph->sendToBus(bus,id,1.f)) throw std::runtime_error("Echec du routage AudIO.");
            state.names[i]="Vide";
            pending[i].store(false);
        }
        graph->setDesignOutput(bus);graph->setBypass(false);graph->prepareToPlay(Rate,Block);
        buffer=std::make_unique<AudIO::Buffer<float>>(0,false,2,true);
    }
    ~Impl(){graph->teardown();buffer.reset();graph.reset();}
};
Engine::Engine():impl(std::make_unique<Impl>()) {} Engine::~Engine()=default;
bool Engine::load(int pad,const std::string& path,std::string& error) {
    if(pad<0||pad>=Pads){error="Pad invalide.";return false;}
    try {
        auto file=decode(path);
        std::vector<float> waveform(320,0);
        const auto& samples=file->samples[0];
        for(size_t i=0;i<samples.size();++i){size_t j=std::min<size_t>(319,i*320/samples.size());waveform[j]=std::max(waveform[j],std::abs(samples[i]));}
        {std::lock_guard<std::mutex> lock(impl->mutex);
         file=impl->players[pad]->replaceFile(std::move(file));
         impl->state.names[pad]=std::filesystem::u8path(path).stem().u8string();impl->state.paths[pad]=path;
         impl->state.waveforms[pad]=std::move(waveform);}
        return true;
    } catch(const std::exception& e){error=e.what();return false;}
}
void Engine::trigger(int pad){if(pad>=0&&pad<Pads)impl->pending[pad].store(true);}
void Engine::toggle(int p,int s){if(p<0||p>=Pads||s<0||s>=Steps)return;std::lock_guard<std::mutex> lock(impl->mutex);impl->state.pattern[p][s]=!impl->state.pattern[p][s];}
void Engine::clearPattern(){std::lock_guard<std::mutex> lock(impl->mutex);impl->state.pattern={};}
void Engine::demoPattern(){std::lock_guard<std::mutex> lock(impl->mutex);impl->state.pattern={};for(int s:{0,4,8,12})impl->state.pattern[0][s]=true;for(int s:{4,12})impl->state.pattern[1][s]=true;for(int s=0;s<16;s+=2)impl->state.pattern[2][s]=true;impl->state.pattern[3][14]=true;}
void Engine::setPlaying(bool b){std::lock_guard<std::mutex> lock(impl->mutex);impl->state.playing=b;}
void Engine::stop(){std::lock_guard<std::mutex> lock(impl->mutex);impl->state.playing=false;impl->state.step=-1;impl->untilStep=0;impl->nextStep=0;for(int p=0;p<Pads;++p){impl->players[p]->stop();impl->pending[p].store(false);}}
void Engine::setTempo(double t){if(!std::isfinite(t))return;std::lock_guard<std::mutex> lock(impl->mutex);impl->state.tempo=std::clamp(t,40.,240.);}
void Engine::setVolume(double v){if(!std::isfinite(v))return;std::lock_guard<std::mutex> lock(impl->mutex);impl->state.volume=std::clamp(v,0.,1.);}
Snapshot Engine::snapshot(){std::lock_guard<std::mutex> lock(impl->mutex);return impl->state;}
void Engine::render(float* out){
    std::fill(out,out+Block*2,0.f);
    std::unique_lock<std::mutex> lock(impl->mutex,std::try_to_lock);if(!lock)return;
    auto& s=impl->state;
    for(int p=0;p<Pads;++p)if(impl->pending[p].exchange(false)&&!s.paths[p].empty())impl->players[p]->play(true);
    if(s.playing){
        if(impl->untilStep<=0){s.step=impl->nextStep;impl->nextStep=(s.step+1)%Steps;for(int p=0;p<Pads;++p)if(s.pattern[p][s.step]&&!s.paths[p].empty())impl->players[p]->play(true);impl->untilStep+=Rate*60./s.tempo/4.;}
        impl->untilStep-=Block;
    }
    impl->graph->process(*impl->buffer);float peak=0;
    for(int n=0;n<Block;++n)for(int c=0;c<2;++c){float v=impl->buffer->getOutputBuffer(c)[n]*float(s.volume)*.5f;if(!std::isfinite(v))v=0;v=std::clamp(v,-.98f,.98f);out[n*2+c]=v;peak=std::max(peak,std::abs(v));}
    s.peak=peak;for(int p=0;p<Pads;++p)s.active[p]=impl->players[p]->isPlaying();
}
bool Engine::save(const std::string& path,std::string& error){auto s=snapshot();std::ofstream f(std::filesystem::u8path(path));if(!f){error="Impossible d'enregistrer.";return false;}f<<"KICKLAB 1\n"<<s.tempo<<' '<<s.volume<<'\n';for(int p=0;p<Pads;++p){unsigned bits=0;for(int k=0;k<Steps;++k)if(s.pattern[p][k])bits|=1u<<k;f<<bits<<' '<<std::quoted(s.paths[p])<<'\n';}if(!f){error="Erreur d'ecriture.";return false;}return true;}
bool Engine::open(const std::string& path,std::string& error){
    std::ifstream f(std::filesystem::u8path(path),std::ios::ate);if(!f||f.tellg()>65536){error="Projet introuvable ou trop volumineux.";return false;}f.seekg(0);
    std::string tag;int version;Snapshot s;
    if(!(f>>tag>>version>>s.tempo>>s.volume)||tag!="KICKLAB"||version!=1||!std::isfinite(s.tempo)||!std::isfinite(s.volume)||s.tempo<40||s.tempo>240||s.volume<0||s.volume>1){error="Projet KICKLAB invalide.";return false;}
    for(int p=0;p<Pads;++p){unsigned bits;if(!(f>>bits>>std::quoted(s.paths[p]))||bits>65535){error="Pattern invalide.";return false;}for(int k=0;k<Steps;++k)s.pattern[p][k]=(bits&(1u<<k))!=0;}
    std::array<std::unique_ptr<AudioFile<float>>,Pads> clips;
    try{for(int p=0;p<Pads;++p)if(!s.paths[p].empty())clips[p]=decode(s.paths[p]);}catch(const std::exception& e){error=std::string("Projet non charge : ")+e.what();return false;}
    std::array<std::unique_ptr<AudioFile<float>>,Pads> old;
    {std::lock_guard<std::mutex> lock(impl->mutex);
     for(int p=0;p<Pads;++p){if(clips[p]){s.names[p]=std::filesystem::u8path(s.paths[p]).stem().u8string();s.waveforms[p].assign(320,0.f);auto& a=clips[p]->samples[0];for(size_t n=0;n<a.size();++n){size_t j=std::min<size_t>(319,n*320/a.size());s.waveforms[p][j]=std::max(s.waveforms[p][j],std::abs(a[n]));}}else s.names[p]="Vide";old[p]=impl->players[p]->replaceFile(std::move(clips[p]));impl->pending[p].store(false);}
     impl->state=std::move(s);impl->untilStep=0;impl->nextStep=0;}
    return true;
}
}

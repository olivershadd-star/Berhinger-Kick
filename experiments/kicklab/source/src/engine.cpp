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
// DawInserts normally gets this shared empty value from DawEngine.cpp.  The
// Windows prototype deliberately keeps the much larger DAW core disabled.
namespace AudIO { const std::string _emptyStr{""}; }
#include "Daw/DawInserts.cpp"
#include "Objects/PlayBack.cpp"
#include "pad_voice.hpp"

namespace lab {
const std::array<InsertEffectInfo,31>& insertEffectCatalog(){
    static const std::array<InsertEffectInfo,31> catalog={{
        {InsertEffect::Lowpass,"Lowpass","Filter",3,{"Lowpass","Soft/Hard","Dry/Wet"},{1,.5,1},32},
        {InsertEffect::PurestDrive,"PurestDrive","Saturation",1,{"Drive","",""},{0,0,0},32},
        {InsertEffect::Highpass,"Highpass","Filter",3,{"Highpass","Loose/Tight","Dry/Wet"},{0,.5,1},32},
        {InsertEffect::Isolator,"Isolator","Filter",3,{"Freq","High","Low"},{1,0,1},32},
        {InsertEffect::Tube2,"Tube2","Saturation",2,{"Input","Tube",""},{.5,.5,0},32},
        {InsertEffect::Mojo,"Mojo","Saturation",1,{"Input","",""},{.5,0,0},32},
        {InsertEffect::DrumSlam,"DrumSlam","Dynamics",3,{"Drive","Output","Dry/Wet"},{0,1,1},32},
        {InsertEffect::ButterComp2,"ButterComp2","Dynamics",3,{"Compress","Output","Dry/Wet"},{0,.5,1},32},
        {InsertEffect::SoftGate,"SoftGate","Dynamics",3,{"Thresh","Darken","Silence"},{.5,.5,0},32},
        {InsertEffect::Chorus,"Chorus","Ambience",3,{"Speed","Range","Dry/Wet"},{.5,.5,.5},96},
        {InsertEffect::StarChild,"StarChild","Ambience",3,{"Sustain","Grain","Dry/Wet"},{1,.7,.2},750},
        {InsertEffect::Reverb,"Reverb","Reverb",2,{"Big","Wet",""},{.5,.5,0},750},
        {InsertEffect::Loud,"Loud","Distortion",3,{"Boost","Output Level","Dry/Wet"},{0,1,1},32},
        {InsertEffect::Mackity,"Mackity","Distortion",2,{"In Trim","Out Pad"},{.1,1},32},
        {InsertEffect::Baxandall2,"Baxandall2","EQ",2,{"Treble","Bass"},{.5,.5},32},
        {InsertEffect::Biquad2,"Biquad2","Filter",5,{"Type","Freq","Q","Output","Inv/Wet"},{1,.5,.5,1,1},32},
        {InsertEffect::Pressure5,"Pressure5","Dynamics",6,{"Pressure","Speed","Mewiness","PawClaw","Output","Dry/Wet"},{0,.25,1,.5,.5,1},32},
        {InsertEffect::Pop2,"Pop2","Dynamics",5,{"Compress","Attack","Release","Drive","Dry/Wet"},{.5,.5,.5,.5,1},32},
        {InsertEffect::AutoPan,"AutoPan","Modulation",4,{"Rate","Phase","Width","Dry/Wet"},{.1,.5,0,1},32},
        {InsertEffect::ChorusEnsemble,"ChorusEnsemble","Modulation",3,{"Speed","Range","Dry/Wet"},{.5,.5,.8},96},
        {InsertEffect::PurestEcho,"PurestEcho","Delay",5,{"Time","Tap 1","Tap 2","Tap 3","Tap 4"},{1,1,0,0,0},1500},
        {InsertEffect::TapeDelay2,"TapeDelay2","Delay",6,{"Time","Regen","Freq","Reso","Flutter","Dry/Wet"},{1,0,.5,0,0,1},1500},
        {InsertEffect::Chamber2,"Chamber2","Reverb",4,{"Delay","Regen","Thick","Wet"},{.34,.31,.28,.25},1500},
        {InsertEffect::Galactic,"Galactic","Reverb",5,{"Replace","Brightness","Detune","Bigness","Dry/Wet"},{.5,.5,.5,1,1},1500},
        {InsertEffect::DeRez2,"DeRez2","Lo-Fi",4,{"Rate","Resolution","Hard","Dry/Wet"},{1,1,1,1},32},
        {InsertEffect::BitGlitter,"BitGlitter","Lo-Fi",4,{"Input","Glitter","Output","Dry/Wet"},{.5,0,.5,1},32},
        {InsertEffect::PurestGain,"PurestGain","Utility / Gain",2,{"Gain","Slow Fade"},{.5,1},32},
        {InsertEffect::EveryTrim,"EveryTrim","Utility / Gain",5,{"Left","Right","Mid","Side","Master"},{.5,.5,.5,.5,.5},32},
        {InsertEffect::GlitchShifter,"GlitchShifter","Pitch / Glitch",5,{"Note","Trim","Tighten","Feedback","Dry/Wet"},{.5,.5,.5,0,1},256},
        {InsertEffect::TremoSquare,"TremoSquare","Rhythmic",2,{"Frequency","Dry/Wet"},{.5,1},32},
        {InsertEffect::Fracture,"Fracture","Creative",4,{"Drive","Fracture","Output","Dry/Wet"},{.25,.5,.5,1},32}
    }};
    return catalog;
}
const InsertEffectInfo* effectInfo(InsertEffect effect){for(const auto& info:insertEffectCatalog())if(info.effect==effect)return &info;return nullptr;}
const char* insertEffectName(InsertEffect effect){auto info=effectInfo(effect);return info?info->name:"";}
int insertParameterCount(InsertEffect effect){auto info=effectInfo(effect);return info?info->parameterCount:0;}
const char* insertParameterName(InsertEffect effect,int parameter){
    auto info=effectInfo(effect);return info&&parameter>=0&&parameter<info->parameterCount?info->parameterNames[parameter]:"";
}
InsertState defaultInsertState(InsertEffect effect){
    InsertState state;state.effect=effect;if(auto info=effectInfo(effect))for(int parameter=0;parameter<info->parameterCount;++parameter)state.parameters[parameter]=info->defaults[parameter];return state;
}
namespace {
unsigned u16(const std::vector<uint8_t>& v,size_t i) { return v.at(i)|(unsigned(v.at(i+1))<<8); }
unsigned u32(const std::vector<uint8_t>& v,size_t i) { return u16(v,i)|(u16(v,i+2)<<16); }
std::unique_ptr<AudioFile<float>> decode(const std::string& path) {
    std::ifstream in(std::filesystem::u8path(path),std::ios::binary|std::ios::ate);
    if(!in) throw std::runtime_error("Impossible d'ouvrir ce fichier.");
    auto length=in.tellg();
    if(length<44 || length>64*1024*1024) throw std::runtime_error("WAV invalide ou trop volumineux (maximum 64 Mo).");
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
    if(encoding==0xfffe){
        const size_t fmtBytes=u32(raw,fmt+4);if(fmtBytes<40||u16(raw,fmt+24)<22)throw std::runtime_error("En-tête WAV Extensible incomplet.");
        static const uint8_t guidTail[12]={0,0,16,0,128,0,0,170,0,56,155,113};
        if(std::memcmp(raw.data()+fmt+36,guidTail,12))throw std::runtime_error("Sous-format WAV Extensible non pris en charge.");
        encoding=u16(raw,fmt+32);
    }
    if((encoding!=1 && encoding!=3)||(encoding==3 && bits!=32)||(bits!=16 && bits!=24 && bits!=32)||ch<1||ch>2||rate<8000||rate>192000)
        throw std::runtime_error("WAV mono/stereo PCM 16/24/32 ou flottant 32 requis (8 a 192 kHz).");
    unsigned align=ch*bits/8;
    if(u16(raw,fmt+20)!=align||u32(raw,fmt+16)!=rate*align||bytes%align||bytes==0||bytes/align>rate*60)
        throw std::runtime_error("WAV incoherent, vide ou de plus de 60 secondes.");
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
    struct BankPad {
        std::string name="Vide",path;
        std::vector<float> waveform;
        double duration=0;
        PadEdit edit;
        std::unique_ptr<AudioFile<float>> file;
        bool loaded=false;
    };
    std::vector<float> pool;
    std::unique_ptr<AudIO::Design> graph;
    std::unique_ptr<AudIO::Buffer<float>> buffer;
    std::array<PadPlayback*,Pads> players{};
    PadPlayback* previewPlayer{};
    // 1000 means no trigger; otherwise the value is a per-trigger semitone offset.
    std::array<std::atomic<int>,Pads> pendingPitch{};
    std::array<std::array<BankPad,Pads>,Groups> banks{};
    Snapshot state;
    // Waveform/string copies belong to the UI, never the render lock.
    std::mutex mutex, metadataMutex, audioMutex;
    std::array<bool,Pads> loaded{};
    float currentVolume=.65f;
    double untilStep=0;
    int nextStep=0;
    Impl():pool(512*Block) {
        AudIO::MemoryPool<float>::get().initialize(pool.data(),512,Block);
        graph=std::make_unique<AudIO::Design>(0,2);
        auto bus=graph->createBus();
        for(int i=0;i<Pads;++i) {
            auto player=std::make_unique<PadPlayback>();players[i]=player.get();
            auto id=graph->createObject(std::move(player));
            if(!graph->sendToBus(bus,id,1.f)) throw std::runtime_error("Echec du routage AudIO.");
            state.names[i]="Vide";
            pendingPitch[i].store(1000);
        }
        for(int i=0;i<Sequences;++i){state.sequences[i].name="Sequence "+std::to_string(i+1);}
        auto preview=std::make_unique<PadPlayback>();previewPlayer=preview.get();
        auto previewId=graph->createObject(std::move(preview));
        if(!graph->sendToBus(bus,previewId,1.f)) throw std::runtime_error("Echec du routage de la preecoute AudIO.");
        graph->setDesignOutput(bus);graph->setBypass(false);graph->prepareToPlay(Rate,Block);
        buffer=std::make_unique<AudIO::Buffer<float>>(0,false,2,true);
    }
    ~Impl(){graph->teardown();buffer.reset();graph.reset();}
};
Engine::Engine():impl(std::make_unique<Impl>()) {} Engine::~Engine()=default;
void Engine::resetProject(){impl=std::make_unique<Impl>();}
bool Engine::load(int pad,const std::string& path,std::string& error,bool preserveEdit) {
    if(pad<0||pad>=Pads){error="Pad invalide.";return false;}
    try {
        auto file=decode(path);
        std::vector<float> waveform(320,0);
        const auto& samples=file->samples[0];
        for(size_t i=0;i<samples.size();++i){size_t j=std::min<size_t>(319,i*320/samples.size());waveform[j]=std::max(waveform[j],std::abs(samples[i]));}
        auto name=std::filesystem::u8path(path).stem().u8string();
        auto storedPath=path;
        PadEdit edit;
        if(preserveEdit){std::lock_guard<std::mutex> lock(impl->mutex);edit=impl->state.edits[pad];}
        auto effects=std::make_unique<PadEffects>(edit);
        const double duration=double(samples.size())/Rate;
        {std::scoped_lock lock(impl->mutex,impl->metadataMutex,impl->audioMutex);
         file=impl->players[pad]->replaceFile(std::move(file));
         impl->loaded[pad]=true;
         impl->state.names[pad].swap(name);impl->state.paths[pad].swap(storedPath);
         impl->state.waveforms[pad].swap(waveform);
         impl->state.durations[pad]=duration;impl->state.edits[pad]=edit;
         auto& bank=impl->banks[impl->state.group][pad];bank.name=impl->state.names[pad];bank.path=impl->state.paths[pad];bank.waveform=impl->state.waveforms[pad];bank.duration=duration;bank.edit=edit;bank.loaded=true;
         impl->players[pad]->edit=edit;impl->players[pad]->effects.swap(effects);impl->players[pad]->resetMix();}
        return true;
    } catch(const std::exception& e){error=e.what();return false;}
}
bool Engine::preview(const std::string& path,std::string& error){
    try{
        auto file=decode(path);std::unique_ptr<AudioFile<float>> retired;
        {std::lock_guard<std::mutex> lock(impl->audioMutex);impl->previewPlayer->stop();retired=impl->previewPlayer->replaceFile(std::move(file));impl->previewPlayer->edit={};impl->previewPlayer->resetMix();impl->previewPlayer->trigger();}
        return true;
    }catch(const std::exception& e){error=e.what();return false;}
}
void Engine::stopPreview(){std::lock_guard<std::mutex> lock(impl->audioMutex);impl->previewPlayer->stop();}
void Engine::trigger(int pad){triggerPitch(pad,0);}
void Engine::triggerPitch(int pad,int semitones){if(pad>=0&&pad<Pads)impl->pendingPitch[pad].store(std::clamp(semitones,-48,48));}
void Engine::release(int pad){if(pad<0||pad>=Pads)return;impl->pendingPitch[pad].store(1000);std::lock_guard<std::mutex> lock(impl->audioMutex);impl->players[pad]->release();}
void Engine::releasePerformancePad(int){int source=0;{std::lock_guard<std::mutex> lock(impl->mutex);source=impl->state.sequences[impl->state.sequence].sourcePad;}release(source);}
void Engine::setEdit(int pad,const PadEdit& requested){
    if(pad<0||pad>=Pads)return;
    const double values[]={requested.start,requested.end,requested.gain,requested.semitones,requested.fadeMs,requested.attack,requested.decay,requested.sustain,requested.release};
    for(double v:values)if(!std::isfinite(v))return;
    PadEdit edit=requested;
    edit.start=std::clamp(edit.start,0.,.999);edit.end=std::clamp(edit.end,edit.start+.001,1.);
    edit.gain=std::clamp(edit.gain,0.,2.);edit.semitones=std::clamp(edit.semitones,-24.,24.);
    edit.fadeMs=std::clamp(edit.fadeMs,0.,20.);
    edit.attack=std::clamp(edit.attack,0.,1.);edit.decay=std::clamp(edit.decay,0.,1.);edit.sustain=std::clamp(edit.sustain,0.,1.);edit.release=std::clamp(edit.release,0.,1.);
    for(auto& insert:edit.inserts){int type=int(insert.effect);if(type<int(InsertEffect::None)||type>=int(InsertEffect::Count))return;for(double& value:insert.parameters){if(!std::isfinite(value))return;value=std::clamp(value,0.,1.);}}
    bool rebuild=false;{std::lock_guard<std::mutex> lock(impl->mutex);for(int slot=0;slot<InsertSlots;++slot)if(impl->state.edits[pad].inserts[slot].effect!=edit.inserts[slot].effect)rebuild=true;}
    std::unique_ptr<PadEffects> replacement;if(rebuild)replacement=std::make_unique<PadEffects>(edit);
    std::scoped_lock lock(impl->mutex,impl->audioMutex);
    impl->state.edits[pad]=edit;impl->banks[impl->state.group][pad].edit=edit;impl->players[pad]->edit=edit;if(replacement)impl->players[pad]->effects.swap(replacement);else impl->players[pad]->effects->sync(edit);
}
void Engine::setInsertEffect(int pad,int slot,InsertEffect effect){if(pad<0||pad>=Pads||slot<0||slot>=InsertSlots)return;PadEdit edit;{std::lock_guard<std::mutex> lock(impl->mutex);edit=impl->state.edits[pad];}edit.inserts[slot]=defaultInsertState(effect);setEdit(pad,edit);}
void Engine::setInsertBypass(int pad,int slot,bool bypass){if(pad<0||pad>=Pads||slot<0||slot>=InsertSlots)return;PadEdit edit;{std::lock_guard<std::mutex> lock(impl->mutex);edit=impl->state.edits[pad];}edit.inserts[slot].bypass=bypass;setEdit(pad,edit);}
void Engine::setInsertsBypass(int pad,bool bypass){if(pad<0||pad>=Pads)return;PadEdit edit;{std::lock_guard<std::mutex> lock(impl->mutex);edit=impl->state.edits[pad];}edit.insertsBypass=bypass;setEdit(pad,edit);}
void Engine::setInsertParameter(int pad,int slot,int parameter,double value){if(pad<0||pad>=Pads||slot<0||slot>=InsertSlots||!std::isfinite(value))return;PadEdit edit;{std::lock_guard<std::mutex> lock(impl->mutex);edit=impl->state.edits[pad];}if(parameter<0||parameter>=insertParameterCount(edit.inserts[slot].effect))return;edit.inserts[slot].parameters[parameter]=std::clamp(value,0.,1.);setEdit(pad,edit);}
namespace {
int mappedPerformanceNote(PerformanceMode mode,int root,int pad){
    static constexpr int major[16]={0,2,4,5,7,9,11,12,14,16,17,19,21,23,24,26};
    static constexpr int minor[16]={0,2,3,5,7,8,10,12,14,15,17,19,20,22,24,26};
    const int offset=mode==PerformanceMode::Major?major[pad]:mode==PerformanceMode::Minor?minor[pad]:pad;
    return std::clamp(root+offset,0,127);
}
}
void Engine::toggle(int p,int step){
    if(p<0||p>=Pads||step<0||step>=Steps)return;std::lock_guard<std::mutex> lock(impl->mutex);auto& state=impl->state;auto& sequence=state.sequences[state.sequence];const int absolute=state.viewBar*Steps+step;
    if(sequence.mode==PerformanceMode::Drum)sequence.drum[p][absolute]=!sequence.drum[p][absolute];
    else {const int note=mappedPerformanceNote(sequence.mode,sequence.rootNote,p);sequence.melody[absolute]=sequence.melody[absolute]==note?-1:note;}
}
void Engine::clearPattern(){std::lock_guard<std::mutex> lock(impl->mutex);auto& sequence=impl->state.sequences[impl->state.sequence];sequence.drum={};sequence.melody.fill(-1);}
void Engine::demoPattern(){std::lock_guard<std::mutex> lock(impl->mutex);auto& sequence=impl->state.sequences[impl->state.sequence];sequence.drum={};sequence.melody.fill(-1);sequence.bars=1;impl->state.viewBar=0;for(int s:{0,4,8,12})sequence.drum[0][s]=true;for(int s:{4,12})sequence.drum[1][s]=true;for(int s=0;s<16;s+=2)sequence.drum[2][s]=true;sequence.drum[3][14]=true;sequence.sourcePad=12;sequence.rootNote=48;const int notes[8]={48,48,50,55,53,50,48,55};for(int i=0;i<8;++i)sequence.melody[i*2]=notes[i];}
void Engine::selectSequence(int requested){std::lock_guard<std::mutex> lock(impl->mutex);auto& s=impl->state;s.sequence=std::clamp(requested,0,Sequences-1);s.queuedSequence=-1;s.viewBar=0;s.playBar=0;s.step=-1;s.songPlaying=false;s.songPosition=-1;impl->nextStep=0;impl->untilStep=0;}
void Engine::queueSequence(int requested){requested=std::clamp(requested,0,Sequences-1);bool playing=false;{std::lock_guard<std::mutex> lock(impl->mutex);playing=impl->state.playing;if(playing){impl->state.queuedSequence=requested;impl->state.songPlaying=false;}}if(!playing)selectSequence(requested);}
void Engine::setSequenceBars(int bars){std::lock_guard<std::mutex> lock(impl->mutex);auto& s=impl->state;auto& sequence=s.sequences[s.sequence];sequence.bars=std::clamp(bars,1,MaxBars);s.viewBar=std::min(s.viewBar,sequence.bars-1);if(impl->nextStep>=sequence.bars*Steps)impl->nextStep=0;}
void Engine::setViewBar(int bar){std::lock_guard<std::mutex> lock(impl->mutex);auto& s=impl->state;s.viewBar=std::clamp(bar,0,s.sequences[s.sequence].bars-1);}
void Engine::setGroup(int requested){
    requested=std::clamp(requested,0,Groups-1);
    std::array<PadEdit,Pads> edits;
    {std::lock_guard<std::mutex> lock(impl->mutex);if(requested==impl->state.group)return;for(int pad=0;pad<Pads;++pad)edits[pad]=impl->banks[requested][pad].edit;}
    std::array<std::unique_ptr<PadEffects>,Pads> effects;
    for(int pad=0;pad<Pads;++pad)effects[pad]=std::make_unique<PadEffects>(edits[pad]);
    std::scoped_lock lock(impl->mutex,impl->metadataMutex,impl->audioMutex);
    auto& state=impl->state;const int previous=state.group;
    state.playing=false;state.songPlaying=false;state.step=-1;state.playBar=0;impl->nextStep=0;impl->untilStep=0;
    for(int pad=0;pad<Pads;++pad){
        impl->players[pad]->stop();impl->pendingPitch[pad].store(1000);
        auto& oldPad=impl->banks[previous][pad];oldPad.file=impl->players[pad]->replaceFile({});oldPad.loaded=impl->loaded[pad];
        auto& newPad=impl->banks[requested][pad];impl->players[pad]->replaceFile(std::move(newPad.file));impl->loaded[pad]=newPad.loaded;
        state.names[pad]=newPad.name;state.paths[pad]=newPad.path;state.waveforms[pad]=newPad.waveform;state.durations[pad]=newPad.duration;state.edits[pad]=newPad.edit;state.active[pad]=false;state.progress[pad]=0;
        impl->players[pad]->edit=newPad.edit;impl->players[pad]->effects.swap(effects[pad]);impl->players[pad]->resetMix();
    }
    state.group=requested;
}
void Engine::setPerformanceMode(PerformanceMode mode){if(int(mode)<0||int(mode)>int(PerformanceMode::Minor))return;std::lock_guard<std::mutex> lock(impl->mutex);impl->state.sequences[impl->state.sequence].mode=mode;}
void Engine::setMelodicSourcePad(int pad){if(pad<0||pad>=Pads)return;std::lock_guard<std::mutex> lock(impl->mutex);impl->state.sequences[impl->state.sequence].sourcePad=pad;}
void Engine::setRootNote(int note){std::lock_guard<std::mutex> lock(impl->mutex);impl->state.sequences[impl->state.sequence].rootNote=std::clamp(note,0,101);}
void Engine::selectTrack(int track){std::lock_guard<std::mutex> lock(impl->mutex);impl->state.selectedTrack=std::clamp(track,0,Pads-1);}
void Engine::setTrackLevel(int track,double level){if(track<0||track>=Pads||!std::isfinite(level))return;std::lock_guard<std::mutex> lock(impl->mutex);impl->state.sequences[impl->state.sequence].tracks[track].level=std::clamp(level,0.,1.);}
void Engine::setTrackPan(int track,double pan){if(track<0||track>=Pads||!std::isfinite(pan))return;std::lock_guard<std::mutex> lock(impl->mutex);impl->state.sequences[impl->state.sequence].tracks[track].pan=std::clamp(pan,-1.,1.);}
void Engine::setTrackMute(int track,bool mute){if(track<0||track>=Pads)return;std::lock_guard<std::mutex> lock(impl->mutex);impl->state.sequences[impl->state.sequence].tracks[track].mute=mute;}
void Engine::setTrackSolo(int track,bool solo){if(track<0||track>=Pads)return;std::lock_guard<std::mutex> lock(impl->mutex);impl->state.sequences[impl->state.sequence].tracks[track].solo=solo;}
void Engine::setTrackArmed(int track,bool armed){if(track<0||track>=Pads)return;std::lock_guard<std::mutex> lock(impl->mutex);impl->state.sequences[impl->state.sequence].tracks[track].armed=armed;}
void Engine::setTrackProgram(int track,ProgramType program){if(track<0||track>=Pads||int(program)<0||int(program)>int(ProgramType::Keygroup))return;std::lock_guard<std::mutex> lock(impl->mutex);impl->state.sequences[impl->state.sequence].tracks[track].program=program;}
int Engine::performanceNote(int pad)const{if(pad<0||pad>=Pads)return 0;std::lock_guard<std::mutex> lock(impl->mutex);const auto& sequence=impl->state.sequences[impl->state.sequence];return mappedPerformanceNote(sequence.mode,sequence.rootNote,pad);}
bool Engine::recordHit(int pad,int note,int quantizeDivision){
    if(pad<0||pad>=Pads||(note>=0&&(note>127)))return false;
    std::lock_guard<std::mutex> lock(impl->mutex);auto& state=impl->state;if(!state.playing||state.step<0)return false;
    auto& sequence=state.sequences[state.sequence];const int length=std::max(Steps,sequence.bars*Steps);
    const int quantum=quantizeDivision<=4?4:quantizeDivision<=8?2:1;
    const double stepSamples=Rate*60./state.tempo/4.;const double elapsed=std::clamp(1.-impl->untilStep/stepSamples,0.,1.);
    const double position=state.playBar*Steps+state.step+elapsed;int target=int(std::floor((position+quantum*.5)/quantum))*quantum;target%=length;if(target<0)target+=length;
    if(note<0)sequence.drum[pad][target]=true;else sequence.melody[target]=note;
    return true;
}
void Engine::addSongStep(int sequence,int repeats){std::lock_guard<std::mutex> lock(impl->mutex);auto& s=impl->state;if(s.songLength>=MaxSongSteps)return;s.song[s.songLength++]={std::clamp(sequence,0,Sequences-1),std::clamp(repeats,1,128)};}
void Engine::editSongStep(int position,int sequence,int repeats){std::lock_guard<std::mutex> lock(impl->mutex);auto& s=impl->state;if(position<0||position>=s.songLength)return;s.song[position]={std::clamp(sequence,0,Sequences-1),std::clamp(repeats,1,128)};}
void Engine::deleteSongStep(int position){std::lock_guard<std::mutex> lock(impl->mutex);auto& s=impl->state;if(position<0||position>=s.songLength)return;for(int i=position;i+1<s.songLength;++i)s.song[i]=s.song[i+1];s.song[--s.songLength]={};if(s.songPosition>=s.songLength)s.songPosition=s.songLength-1;}
void Engine::clearSong(){std::lock_guard<std::mutex> lock(impl->mutex);auto& s=impl->state;s.song={};s.songLength=0;s.songPlaying=false;s.songPosition=-1;s.songRepeat=0;}
void Engine::playSong(){std::lock_guard<std::mutex> lock(impl->mutex);auto& s=impl->state;if(!s.songLength)return;s.songPlaying=true;s.songPosition=0;s.songRepeat=0;s.sequence=s.song[0].sequence;s.queuedSequence=-1;s.viewBar=s.playBar=0;s.step=-1;s.playing=true;impl->nextStep=0;impl->untilStep=0;}
void Engine::setPlaying(bool b){std::lock_guard<std::mutex> lock(impl->mutex);impl->state.playing=b;}
void Engine::stop(){std::scoped_lock lock(impl->mutex,impl->audioMutex);auto& s=impl->state;s.playing=false;s.songPlaying=false;s.songPosition=-1;s.songRepeat=0;s.step=-1;s.playBar=0;impl->untilStep=0;impl->nextStep=0;for(int p=0;p<Pads;++p){impl->players[p]->stop();impl->pendingPitch[p].store(1000);}impl->previewPlayer->stop();}
void Engine::setTempo(double t){if(!std::isfinite(t))return;std::lock_guard<std::mutex> lock(impl->mutex);impl->state.tempo=std::clamp(t,40.,240.);}
void Engine::setVolume(double v){if(!std::isfinite(v))return;std::lock_guard<std::mutex> lock(impl->mutex);impl->state.volume=std::clamp(v,0.,1.);}
Snapshot Engine::snapshot(){
    Snapshot result;
    {std::lock_guard<std::mutex> lock(impl->metadataMutex);
     result.names=impl->state.names;result.paths=impl->state.paths;result.waveforms=impl->state.waveforms;result.durations=impl->state.durations;}
    {std::lock_guard<std::mutex> lock(impl->mutex);const auto& state=impl->state;result.active=state.active;result.edits=state.edits;result.progress=state.progress;result.sequences=state.sequences;result.song=state.song;
     result.tempo=state.tempo;result.volume=state.volume;result.playing=state.playing;result.songPlaying=state.songPlaying;result.step=state.step;result.playBar=state.playBar;result.viewBar=state.viewBar;result.sequence=state.sequence;result.queuedSequence=state.queuedSequence;result.group=state.group;result.selectedTrack=state.selectedTrack;result.songLength=state.songLength;result.songPosition=state.songPosition;result.songRepeat=state.songRepeat;result.peak=state.peak;
     const auto& sequence=state.sequences[state.sequence];const int first=state.viewBar*Steps;result.melody.fill(-1);for(int p=0;p<Pads;++p)for(int step=0;step<Steps;++step)result.pattern[p][step]=sequence.drum[p][first+step];for(int step=0;step<Steps;++step)result.melody[step]=sequence.melody[first+step];}
    return result;
}
void Engine::render(float* out){
    std::fill(out,out+Block*2,0.f);
    // Never drop an audio block or pause the sample clock because the UI is reading.
    std::scoped_lock lock(impl->mutex,impl->audioMutex);
    auto& s=impl->state;
    auto& activeSequence=s.sequences[s.sequence];bool anySolo=false;for(const auto& track:activeSequence.tracks)anySolo=anySolo||track.solo;
    for(int p=0;p<Pads;++p)impl->players[p]->setTrackMix(activeSequence.tracks[p].level,activeSequence.tracks[p].pan);
    for(int p=0;p<Pads;++p){const int pitch=impl->pendingPitch[p].exchange(1000);if(pitch!=1000&&impl->loaded[p])impl->players[p]->trigger(pitch);}
    if(s.playing){
        if(impl->untilStep<=0){
            auto& sequence=s.sequences[s.sequence];const int absolute=impl->nextStep;s.step=absolute%Steps;s.playBar=absolute/Steps;s.viewBar=s.playBar;
            for(int p=0;p<Pads;++p){const auto& track=sequence.tracks[p];if(sequence.drum[p][absolute]&&impl->loaded[p]&&!track.mute&&(!anySolo||track.solo))impl->players[p]->trigger();}
            const int note=sequence.melody[absolute],source=sequence.sourcePad;if(note>=0&&source>=0&&source<Pads&&impl->loaded[source])impl->players[source]->trigger(note-sequence.rootNote);
            if(++impl->nextStep>=sequence.bars*Steps){
                impl->nextStep=0;
                if(s.songPlaying){
                    ++s.songRepeat;const int repeats=s.song[s.songPosition].repeats;
                    if(s.songRepeat>=repeats){s.songRepeat=0;++s.songPosition;if(s.songPosition>=s.songLength){s.playing=false;s.songPlaying=false;s.songPosition=-1;}else{s.sequence=s.song[s.songPosition].sequence;s.viewBar=s.playBar=0;}}
                }else if(s.queuedSequence>=0){s.sequence=s.queuedSequence;s.queuedSequence=-1;s.viewBar=s.playBar=0;}
            }
            impl->untilStep+=Rate*60./s.tempo/4.;
        }
        impl->untilStep-=Block;
    }
    impl->graph->process(*impl->buffer);float peak=0;
    for(int n=0;n<Block;++n){impl->currentVolume+=(float(s.volume)-impl->currentVolume)*.004f;for(int c=0;c<2;++c){float v=impl->buffer->getOutputBuffer(c)[n]*(s.volume==0?0.f:impl->currentVolume)*.5f;if(!std::isfinite(v))v=0;peak=std::max(peak,std::abs(v));v=std::clamp(v,-.98f,.98f);out[n*2+c]=v;}}
    s.peak=peak;for(int p=0;p<Pads;++p){s.active[p]=impl->players[p]->isPlaying();s.progress[p]=impl->players[p]->progress();}
}
bool Engine::save(const std::string& path,std::string& error){
    auto s=snapshot();std::array<std::array<std::string,Pads>,Groups> paths;std::array<std::array<PadEdit,Pads>,Groups> edits;
    {std::scoped_lock lock(impl->mutex,impl->metadataMutex);for(int group=0;group<Groups;++group)for(int pad=0;pad<Pads;++pad){paths[group][pad]=impl->banks[group][pad].path;edits[group][pad]=impl->banks[group][pad].edit;}}
    std::ofstream f(std::filesystem::u8path(path));if(!f){error="Impossible d'enregistrer.";return false;}f<<std::setprecision(17)<<"KICKLAB 8\n"<<s.tempo<<' '<<s.volume<<' '<<s.group<<' '<<s.selectedTrack<<'\n';
    for(int group=0;group<Groups;++group){f<<"GROUP "<<group<<'\n';for(int p=0;p<Pads;++p){const auto& e=edits[group][p];f<<"PAD "<<p<<' '<<std::quoted(paths[group][p])<<' '<<e.start<<' '<<e.end<<' '<<e.gain<<' '<<e.semitones<<' '<<e.fadeMs<<' '<<e.attack<<' '<<e.decay<<' '<<e.sustain<<' '<<e.release<<' '<<e.insertsBypass;for(const auto& insert:e.inserts){int count=insertParameterCount(insert.effect);f<<' '<<int(insert.effect)<<' '<<insert.bypass<<' '<<count;for(int parameter=0;parameter<count;++parameter)f<<' '<<insert.parameters[parameter];}f<<'\n';}}
    f<<"SEQUENCES "<<Sequences<<' '<<s.sequence<<' '<<s.viewBar<<'\n';
    for(int index=0;index<Sequences;++index){const auto& sequence=s.sequences[index];f<<"SEQ "<<index<<' '<<std::quoted(sequence.name)<<' '<<sequence.bars<<' '<<int(sequence.mode)<<' '<<sequence.sourcePad<<' '<<sequence.rootNote<<'\n'<<"TRACKS "<<Pads<<'\n';for(int track=0;track<Pads;++track){const auto& t=sequence.tracks[track];f<<"TRACK "<<track<<' '<<std::quoted(t.name)<<' '<<int(t.program)<<' '<<t.sourcePad<<' '<<t.level<<' '<<t.pan<<' '<<t.mute<<' '<<t.solo<<' '<<t.armed<<'\n';}f<<"DRUM";for(int p=0;p<Pads;++p){unsigned long long bits=0;for(int step=0;step<MaxSequenceSteps;++step)if(sequence.drum[p][step])bits|=1ull<<step;f<<' '<<bits;}f<<"\nMELO";for(int note:sequence.melody)f<<' '<<note;f<<'\n';}
    f<<"SONG "<<s.songLength<<'\n';for(int index=0;index<s.songLength;++index)f<<"STEP "<<s.song[index].sequence<<' '<<s.song[index].repeats<<'\n';
    if(!f){error="Erreur d'ecriture.";return false;}return true;
}
bool Engine::open(const std::string& path,std::string& error){
    std::ifstream f(std::filesystem::u8path(path),std::ios::ate);if(!f||f.tellg()>1024*1024){error="Projet introuvable ou trop volumineux.";return false;}f.seekg(0);
    std::string tag;int version;Snapshot s;
    for(int index=0;index<Sequences;++index)s.sequences[index].name="Sequence "+std::to_string(index+1);
    if(!(f>>tag>>version>>s.tempo>>s.volume)||tag!="KICKLAB"||(version<1||version>8)||!std::isfinite(s.tempo)||!std::isfinite(s.volume)||s.tempo<40||s.tempo>240||s.volume<0||s.volume>1){error="Projet KICKLAB invalide.";return false;}
    std::array<std::array<std::string,Pads>,Groups> bankPaths{};std::array<std::array<PadEdit,Pads>,Groups> bankEdits{};
    if(version>=6){if(!(f>>s.group)||s.group<0||s.group>=Groups){error="Groupe actif invalide.";return false;}if(version>=7&&(!(f>>s.selectedTrack)||s.selectedTrack<0||s.selectedTrack>=Pads)){error="Piste sélectionnée invalide.";return false;}
        for(int group=0;group<Groups;++group){int parsedGroup=-1;if(!(f>>tag>>parsedGroup)||tag!="GROUP"||parsedGroup!=group){error="Banque de pads invalide.";return false;}for(int p=0;p<Pads;++p){int parsedPad=-1;if(!(f>>tag>>parsedPad>>std::quoted(bankPaths[group][p]))||tag!="PAD"||parsedPad!=p){error="Pad de banque invalide.";return false;}auto& e=bankEdits[group][p];if(!(f>>e.start>>e.end>>e.gain>>e.semitones>>e.fadeMs>>e.attack>>e.decay>>e.sustain>>e.release>>e.insertsBypass)){error="Reglages du pad invalides.";return false;}for(auto& insert:e.inserts){int effect=0,count=0;bool bypass=false;if(!(f>>effect>>bypass>>count)||effect<int(InsertEffect::None)||effect>=int(InsertEffect::Count)||count<0||count>InsertParameters||count!=insertParameterCount(InsertEffect(effect))){error="Insert invalide.";return false;}InsertState parsed;parsed.effect=InsertEffect(effect);parsed.bypass=bypass;for(int parameter=0;parameter<count;++parameter){if(!(f>>parsed.parameters[parameter])||!std::isfinite(parsed.parameters[parameter])||parsed.parameters[parameter]<0||parsed.parameters[parameter]>1){error="Parametre d'insert invalide.";return false;}}insert=parsed;}const double values[]={e.start,e.end,e.gain,e.semitones,e.fadeMs,e.attack,e.decay,e.sustain,e.release};for(double value:values)if(!std::isfinite(value)){error="Reglages non finis.";return false;}if(e.start<0||e.end>1||e.end-e.start<.000999||e.gain<0||e.gain>2||e.semitones< -24||e.semitones>24||e.fadeMs<0||e.fadeMs>20||e.attack<0||e.attack>1||e.decay<0||e.decay>1||e.sustain<0||e.sustain>1||e.release<0||e.release>1){error="Reglages hors limites.";return false;}}}
    }else for(int p=0;p<Pads;++p){unsigned bits;if(!(f>>bits>>std::quoted(s.paths[p]))||bits>65535){error="Pattern invalide.";return false;}for(int k=0;k<Steps;++k)s.pattern[p][k]=(bits&(1u<<k))!=0;
        auto& e=s.edits[p];
        if(version==2){double filterOpen=1,drive=0;bool bypass=false;if(!(f>>e.start>>e.end>>e.gain>>e.semitones>>e.fadeMs>>filterOpen>>drive>>bypass)||!std::isfinite(filterOpen)||!std::isfinite(drive)||filterOpen<0||filterOpen>1||drive<0||drive>1){error="Reglages du pad invalides.";return false;}e.insertsBypass=bypass;e.inserts[0].parameters[0]=filterOpen;e.inserts[0].bypass=filterOpen>=.9999;e.inserts[1].parameters[0]=drive;e.inserts[1].bypass=drive<=.0001;}
        if(version>=3){if(!(f>>e.start>>e.end>>e.gain>>e.semitones>>e.fadeMs)){error="Reglages du pad invalides.";return false;}if(version>=4&&!(f>>e.attack>>e.decay>>e.sustain>>e.release)){error="Enveloppe du pad invalide.";return false;}if(!(f>>e.insertsBypass)){error="Reglages du pad invalides.";return false;}for(auto& insert:e.inserts){int effect=0,count=0;bool bypass=false;if(!(f>>effect>>bypass>>count)||effect<int(InsertEffect::None)||effect>=int(InsertEffect::Count)||count<0||count>InsertParameters||count!=insertParameterCount(InsertEffect(effect))){error="Insert invalide.";return false;}InsertState parsed;parsed.effect=InsertEffect(effect);parsed.bypass=bypass;for(int parameter=0;parameter<count;++parameter){if(!(f>>parsed.parameters[parameter])||!std::isfinite(parsed.parameters[parameter])||parsed.parameters[parameter]<0||parsed.parameters[parameter]>1){error="Parametre d'insert invalide.";return false;}}insert=parsed;}
        }
        const double v[]={e.start,e.end,e.gain,e.semitones,e.fadeMs,e.attack,e.decay,e.sustain,e.release};for(double x:v)if(!std::isfinite(x)){error="Reglages non finis.";return false;}
        if(e.start<0||e.end>1||e.end-e.start<.000999||e.gain<0||e.gain>2||e.semitones< -24||e.semitones>24||e.fadeMs<0||e.fadeMs>20||e.attack<0||e.attack>1||e.decay<0||e.decay>1||e.sustain<0||e.sustain>1||e.release<0||e.release>1){error="Reglages hors limites.";return false;}bankPaths[0][p]=s.paths[p];bankEdits[0][p]=s.edits[p];}
    if(version<5){for(int p=0;p<Pads;++p)for(int step=0;step<Steps;++step)s.sequences[0].drum[p][step]=s.pattern[p][step];}
    else {
        int count=0,current=0,view=0;if(!(f>>tag>>count>>current>>view)||tag!="SEQUENCES"||count!=Sequences||current<0||current>=Sequences){error="Liste de sequences invalide.";return false;}s.sequence=current;
        for(int expected=0;expected<Sequences;++expected){int index=0,bars=0,mode=0,source=0,root=0;if(!(f>>tag>>index>>std::quoted(s.sequences[expected].name)>>bars>>mode>>source>>root)||tag!="SEQ"||index!=expected||s.sequences[expected].name.size()>128||bars<1||bars>MaxBars||mode<0||mode>int(PerformanceMode::Minor)||source<0||source>=Pads||root<0||root>101){error="Sequence invalide.";return false;}auto& sequence=s.sequences[expected];sequence.bars=bars;sequence.mode=PerformanceMode(mode);sequence.sourcePad=source;sequence.rootNote=root;
            if(version>=7){int trackCount=0;if(!(f>>tag>>trackCount)||tag!="TRACKS"||trackCount!=Pads){error="Liste de pistes invalide.";return false;}for(int expectedTrack=0;expectedTrack<Pads;++expectedTrack){int parsedTrack=-1,program=0,sourcePad=0;auto& track=sequence.tracks[expectedTrack];if(!(f>>tag>>parsedTrack>>std::quoted(track.name)>>program>>sourcePad>>track.level>>track.pan>>track.mute>>track.solo>>track.armed)||tag!="TRACK"||parsedTrack!=expectedTrack||track.name.size()>128||program<0||program>int(ProgramType::Keygroup)||sourcePad<0||sourcePad>=Pads||!std::isfinite(track.level)||!std::isfinite(track.pan)||track.level<0||track.level>1||track.pan< -1||track.pan>1){error="Piste invalide.";return false;}track.program=ProgramType(program);track.sourcePad=sourcePad;}}
            if(!(f>>tag)||tag!="DRUM"){error="Pistes de sequence manquantes.";return false;}for(int p=0;p<Pads;++p){unsigned long long bits=0;if(!(f>>bits)){error="Piste de batterie invalide.";return false;}for(int step=0;step<MaxSequenceSteps;++step)sequence.drum[p][step]=(bits&(1ull<<step))!=0;}
            if(!(f>>tag)||tag!="MELO"){error="Piste melodique manquante.";return false;}for(int& note:sequence.melody)if(!(f>>note)||note< -1||note>127){error="Note melodique invalide.";return false;}
        }
        s.viewBar=std::clamp(view,0,s.sequences[s.sequence].bars-1);if(!(f>>tag>>s.songLength)||tag!="SONG"||s.songLength<0||s.songLength>MaxSongSteps){error="Arrangement Song invalide.";return false;}for(int index=0;index<s.songLength;++index){if(!(f>>tag>>s.song[index].sequence>>s.song[index].repeats)||tag!="STEP"||s.song[index].sequence<0||s.song[index].sequence>=Sequences||s.song[index].repeats<1||s.song[index].repeats>128){error="Etape Song invalide.";return false;}}
    }
    s.queuedSequence=-1;s.songPlaying=false;s.songPosition=-1;s.songRepeat=0;s.step=-1;s.playBar=0;
    std::array<std::array<std::unique_ptr<AudioFile<float>>,Pads>,Groups> clips;
    try{for(int group=0;group<Groups;++group)for(int p=0;p<Pads;++p)if(!bankPaths[group][p].empty())clips[group][p]=decode(bankPaths[group][p]);}catch(const std::exception& e){error=std::string("Projet non charge : ")+e.what();return false;}
    std::array<std::array<Impl::BankPad,Pads>,Groups> parsedBanks;
    for(int group=0;group<Groups;++group)for(int p=0;p<Pads;++p){auto& bank=parsedBanks[group][p];bank.path=bankPaths[group][p];bank.edit=bankEdits[group][p];bank.loaded=bool(clips[group][p]);if(clips[group][p]){bank.duration=double(clips[group][p]->getNumSamplesPerChannel())/Rate;bank.name=std::filesystem::u8path(bank.path).stem().u8string();bank.waveform.assign(320,0.f);auto& audio=clips[group][p]->samples[0];for(size_t n=0;n<audio.size();++n){size_t j=std::min<size_t>(319,n*320/audio.size());bank.waveform[j]=std::max(bank.waveform[j],std::abs(audio[n]));}bank.file=std::move(clips[group][p]);}}
    for(int p=0;p<Pads;++p){const auto& bank=parsedBanks[s.group][p];s.paths[p]=bank.path;s.edits[p]=bank.edit;s.durations[p]=bank.duration;s.names[p]=bank.name;s.waveforms[p]=bank.waveform;}
    std::array<std::unique_ptr<PadEffects>,Pads> fx;for(int p=0;p<Pads;++p)fx[p]=std::make_unique<PadEffects>(s.edits[p]);
    std::array<std::unique_ptr<AudioFile<float>>,Pads> old;
    {std::scoped_lock lock(impl->mutex,impl->metadataMutex,impl->audioMutex);
     for(int p=0;p<Pads;++p){auto& active=parsedBanks[s.group][p];impl->loaded[p]=active.loaded;old[p]=impl->players[p]->replaceFile(std::move(active.file));impl->pendingPitch[p].store(1000);impl->players[p]->edit=s.edits[p];impl->players[p]->effects.swap(fx[p]);impl->players[p]->resetMix();}
     impl->banks=std::move(parsedBanks);std::swap(impl->state,s);impl->untilStep=0;impl->nextStep=0;}
    // Retired clips and metadata are destroyed here, on the loading thread.

    return true;
}
}

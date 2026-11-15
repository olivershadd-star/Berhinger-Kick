// SPDX-License-Identifier: AGPL-3.0-or-later
#include "engine.hpp"
#include <thread> // Missing from the original AudIO amalgamation.
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <cctype>
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
bool detectLoopAndBpm(const std::string& path,double duration,double projectTempo,double& bpm){
    std::string lower=path;std::transform(lower.begin(),lower.end(),lower.begin(),[](unsigned char c){return char(std::tolower(c));});
    const bool loopName=lower.find("loop")!=std::string::npos;
    bool named=false;double namedBpm=0;
    for(size_t begin=0;begin<lower.size();){
        while(begin<lower.size()&&!std::isdigit(static_cast<unsigned char>(lower[begin])))++begin;if(begin>=lower.size())break;
        size_t end=begin;while(end<lower.size()&&std::isdigit(static_cast<unsigned char>(lower[end])))++end;
        const int value=std::stoi(lower.substr(begin,end-begin));const size_t nearEnd=std::min(lower.size(),end+4);
        const bool marked=lower.substr(end,nearEnd-end).find("bpm")!=std::string::npos||(begin>=3&&lower.substr(begin-3,3)=="bpm");
        if(value>=40&&value<=240&&(marked||loopName)){named=true;namedBpm=value;if(marked)break;}begin=end;
    }
    if(named){bpm=namedBpm;return true;}
    if(!loopName||duration<=0)return false;
    double best=projectTempo,bestDistance=1e9;
    for(int bars:{1,2,4,8,16}){const double candidate=bars*4.*60./duration;if(candidate<40||candidate>240)continue;const double distance=std::abs(std::log(candidate/projectTempo));if(distance<bestDistance){bestDistance=distance;best=candidate;}}
    bpm=std::clamp(best,40.,240.);return true;
}
bool safeRelativePath(const std::filesystem::path& path){
    if(path.empty()||path.is_absolute())return false;
    for(const auto& part:path)if(part=="..")return false;
    return true;
}
void writePadSettings(std::ostream& stream,const PadEdit& edit){
    stream<<' '<<edit.start<<' '<<edit.end<<' '<<edit.gain<<' '<<edit.semitones<<' '<<edit.fadeMs
          <<' '<<edit.attack<<' '<<edit.decay<<' '<<edit.sustain<<' '<<edit.release
          <<' '<<edit.loopSync<<' '<<edit.sourceBpm<<' '<<edit.insertsBypass;
    for(const auto& insert:edit.inserts){
        const int count=insertParameterCount(insert.effect);
        stream<<' '<<int(insert.effect)<<' '<<insert.bypass<<' '<<count;
        for(int parameter=0;parameter<count;++parameter)stream<<' '<<insert.parameters[parameter];
    }
}
bool readPadSettings(std::istream& stream,PadEdit& edit,std::string& error){
    if(!(stream>>edit.start>>edit.end>>edit.gain>>edit.semitones>>edit.fadeMs
              >>edit.attack>>edit.decay>>edit.sustain>>edit.release
              >>edit.loopSync>>edit.sourceBpm>>edit.insertsBypass)){
        error="Reglages du pad incomplets.";return false;
    }
    for(auto& insert:edit.inserts){
        int effect=0,count=0;bool bypass=false;
        if(!(stream>>effect>>bypass>>count)||effect<int(InsertEffect::None)||effect>=int(InsertEffect::Count)
           ||count<0||count>InsertParameters||count!=insertParameterCount(InsertEffect(effect))){
            error="Insert de kit invalide.";return false;
        }
        InsertState parsed;parsed.effect=InsertEffect(effect);parsed.bypass=bypass;
        for(int parameter=0;parameter<count;++parameter){
            if(!(stream>>parsed.parameters[parameter])||!std::isfinite(parsed.parameters[parameter])
               ||parsed.parameters[parameter]<0||parsed.parameters[parameter]>1){
                error="Parametre d'insert de kit invalide.";return false;
            }
        }
        insert=parsed;
    }
    const double values[]={edit.start,edit.end,edit.gain,edit.semitones,edit.fadeMs,edit.attack,edit.decay,edit.sustain,edit.release,edit.sourceBpm};
    for(double value:values)if(!std::isfinite(value)){error="Reglages du kit non finis.";return false;}
    if(edit.start<0||edit.end>1||edit.end-edit.start<.000999||edit.gain<0||edit.gain>2
       ||edit.semitones< -24||edit.semitones>24||edit.fadeMs<0||edit.fadeMs>20
       ||edit.attack<0||edit.attack>1||edit.decay<0||edit.decay>1||edit.sustain<0||edit.sustain>1
       ||edit.release<0||edit.release>1||edit.sourceBpm<40||edit.sourceBpm>240){
        error="Reglages du kit hors limites.";return false;
    }
    return true;
}
}
struct Engine::Impl {
    struct PadLayer {
        std::string name="Vide",path;
        std::vector<float> waveform;
        double duration=0;
        std::unique_ptr<AudioFile<float>> file;
        bool loaded=false;
        int velocityStart=1,velocityEnd=127;
        bool reverse=false;
    };
    struct BankPad {
        std::array<PadLayer,PadLayers> layers{};
        PadEdit edit;
        std::unique_ptr<PadEffects> effects;
        int activeLayer=0,cycleLayer=-1;
    };
    std::vector<float> pool;
    std::unique_ptr<AudIO::Design> graph;
    std::unique_ptr<AudIO::Buffer<float>> buffer;
    std::array<PadPlayback*,Pads> players{};
    PadPlayback* previewPlayer{};
    // 1000 means no trigger; otherwise the value is a per-trigger semitone offset.
    std::array<std::atomic<int>,Pads> pendingPitch{};
    std::array<std::atomic<int>,Pads> pendingVelocity{};
    // Programs own the samples and pad processing. Patterns only reference a
    // Program, so the same kit can be reused by any number of patterns.
    std::array<std::array<BankPad,Pads>,Programs> banks{};
    Snapshot state;
    // Waveform/string copies belong to the UI, never the render lock.
    std::mutex mutex, metadataMutex, audioMutex;
    std::array<bool,Pads> loaded{};
    float currentVolume=.65f;
    float limiterGain=1.f;
    double untilStep=0;
    int nextStep=0;
    int preparedProgram=-1;
    std::array<std::unique_ptr<PadEffects>,Pads> preparedEffects{};
    int repeatPad=-1,repeatPitch=0,arpSource=0,arpIndex=-1;
    std::array<std::atomic<int>,Pads> heldPitch{};
    std::array<int,Pads> ratchetRemaining{},ratchetPitch{},ratchetVelocity{},gateRemaining{};
    std::array<double,Pads> ratchetUntil{},ratchetInterval{};
    uint32_t randomState=0x6d2b79f5u;
    double untilRepeat=0;
    std::array<bool,128> arpHeld{};
    Impl():pool(512*Block) {
        AudIO::MemoryPool<float>::get().initialize(pool.data(),512,Block);
        graph=std::make_unique<AudIO::Design>(0,2);
        auto bus=graph->createBus();
        for(int i=0;i<Pads;++i) {
            auto player=std::make_unique<PadPlayback>();players[i]=player.get();
            auto id=graph->createObject(std::move(player));
            if(!graph->sendToBus(bus,id,1.f)) throw std::runtime_error("Echec du routage AudIO.");
            state.names[i]="Vide";
            pendingPitch[i].store(1000);pendingVelocity[i].store(127);heldPitch[i].store(0);
            for(int program=0;program<Programs;++program)implColor(program,i);
            state.edits[i]=banks[0][i].edit;
        }
        for(int i=0;i<Programs;++i){state.programs[i].name=i<Groups?"Drum Program "+std::string(1,char('A'+i)):"Program "+std::to_string(i+1);state.programs[i].used=i<Groups;}
        for(int i=0;i<Sequences;++i){state.sequences[i].name=i<Groups?"Pattern "+std::string(1,char('A'+i)):"Sequence "+std::to_string(i+1);state.sequences[i].program=i<Groups?i:0;}
        auto preview=std::make_unique<PadPlayback>();previewPlayer=preview.get();
        auto previewId=graph->createObject(std::move(preview));
        if(!graph->sendToBus(bus,previewId,1.f)) throw std::runtime_error("Echec du routage de la preecoute AudIO.");
        // The graph is not live yet, so force its output to unity.  AudIO's
        // default non-forced route change starts at silence and ramps for
        // 12000 samples; that hidden startup fade made the mixer bus disagree
        // with the per-track meters and made short drum hits look too quiet.
        graph->setDesignOutput(bus,true);graph->setBypass(false);graph->prepareToPlay(Rate,Block);
        buffer=std::make_unique<AudIO::Buffer<float>>(0,false,2,true);
    }
    void implColor(int program,int pad){banks[program][pad].edit.color=(pad/4)%4;}
    void cancelPreparedProgram(){
        if(preparedProgram>=0&&preparedProgram<Programs)for(int pad=0;pad<Pads;++pad)if(preparedEffects[pad])banks[preparedProgram][pad].effects=std::move(preparedEffects[pad]);
        preparedProgram=-1;state.queuedProgram=-1;
    }
    void prepareProgram(int requested){
        requested=std::clamp(requested,0,Programs-1);cancelPreparedProgram();
        if(requested==state.program)return;
        preparedProgram=requested;
        for(int pad=0;pad<Pads;++pad){auto& stored=banks[requested][pad].effects;preparedEffects[pad]=stored?std::move(stored):std::make_unique<PadEffects>(banks[requested][pad].edit);}
    }
    void activatePreparedProgram(){
        if(preparedProgram<0||preparedProgram>=Programs||preparedProgram==state.program){cancelPreparedProgram();return;}
        const int requested=preparedProgram,previous=state.program;
        for(int pad=0;pad<Pads;++pad){
            players[pad]->stop();pendingPitch[pad].store(1000);ratchetRemaining[pad]=0;gateRemaining[pad]=0;
            auto& oldPad=banks[previous][pad];auto& oldLayer=oldPad.layers[oldPad.activeLayer];oldLayer.file=players[pad]->replaceFile({});oldLayer.loaded=loaded[pad];oldPad.effects=std::move(players[pad]->effects);
            auto& newPad=banks[requested][pad];newPad.activeLayer=std::clamp(newPad.activeLayer,0,PadLayers-1);auto& newLayer=newPad.layers[newPad.activeLayer];players[pad]->replaceFile(std::move(newLayer.file));loaded[pad]=newLayer.loaded;players[pad]->effects=std::move(preparedEffects[pad]);
            state.edits[pad]=newPad.edit;state.active[pad]=false;state.progress[pad]=0;players[pad]->edit=newPad.edit;players[pad]->resetMix();
        }
        state.program=requested;preparedProgram=-1;state.queuedProgram=-1;
    }
    int selectLayer(int pad,int velocity){
        auto& bank=banks[state.program][pad];std::array<int,PadLayers> valid{};int count=0;for(int layer=0;layer<PadLayers;++layer)if(bank.layers[layer].loaded)valid[count++]=layer;if(!count)return -1;
        if(bank.edit.layerPlay==LayerPlayType::Velocity){for(int i=0;i<count;++i){const auto& candidate=bank.layers[valid[i]];if(velocity>=candidate.velocityStart&&velocity<=candidate.velocityEnd)return valid[i];}return valid[0];}
        if(bank.edit.layerPlay==LayerPlayType::Cycle){bank.cycleLayer=(bank.cycleLayer+1)%count;return valid[bank.cycleLayer];}
        randomState=randomState*1664525u+1013904223u;return valid[(randomState>>16)%uint32_t(count)];
    }
    void triggerPad(int pad,int pitch,int velocity){
        auto& bank=banks[state.program][pad];const int layer=selectLayer(pad,velocity);if(layer<0)return;
        if(bank.edit.mute)return;
        if(bank.edit.chokeGroup>=0)for(int other=0;other<Pads;++other)if(other!=pad&&banks[state.program][other].edit.chokeGroup==bank.edit.chokeGroup)players[other]->stop();
        if(layer!=bank.activeLayer){auto& previous=bank.layers[bank.activeLayer];previous.file=players[pad]->replaceFile({});previous.loaded=loaded[pad];bank.activeLayer=layer;auto& selected=bank.layers[layer];players[pad]->replaceFile(std::move(selected.file));loaded[pad]=selected.loaded;}
        const auto& selected=bank.layers[layer];const double velocityGain=bank.edit.layerPlay==LayerPlayType::Velocity?1.:velocity/127.;players[pad]->trigger(pitch,velocityGain,selected.reverse);
    }
    ~Impl(){graph->teardown();buffer.reset();graph.reset();}
};
Engine::Engine():impl(std::make_unique<Impl>()) {} Engine::~Engine()=default;
void Engine::resetProject(){impl=std::make_unique<Impl>();}
bool Engine::load(int pad,const std::string& path,std::string& error,bool preserveEdit) {
    if(pad<0||pad>=Pads){error="Pad invalide.";return false;}if(!preserveEdit){PadEdit reset;{std::lock_guard<std::mutex> lock(impl->mutex);reset.color=impl->state.edits[pad].color;}setEdit(pad,reset);}return loadLayer(pad,0,path,error);
}
bool Engine::loadLayer(int pad,int layer,const std::string& path,std::string& error){
    if(pad<0||pad>=Pads||layer<0||layer>=PadLayers){error="Pad ou couche invalide.";return false;}
    try{
        auto file=decode(path);std::vector<float> waveform(320,0);const auto& samples=file->samples[0];for(size_t i=0;i<samples.size();++i){size_t j=std::min<size_t>(319,i*320/samples.size());waveform[j]=std::max(waveform[j],std::abs(samples[i]));}
        const auto name=std::filesystem::u8path(path).stem().u8string();const double duration=double(samples.size())/Rate;PadEdit edit;double projectTempo=120;
        {std::lock_guard<std::mutex> lock(impl->mutex);edit=impl->state.edits[pad];projectTempo=impl->state.tempo;}
        if(layer==0){double sourceBpm=edit.sourceBpm;edit.loopSync=detectLoopAndBpm(path,duration,projectTempo,sourceBpm);if(edit.loopSync)edit.sourceBpm=sourceBpm;}
        std::unique_ptr<AudioFile<float>> retired;
        {std::scoped_lock lock(impl->mutex,impl->metadataMutex,impl->audioMutex);auto& bank=impl->banks[impl->state.program][pad];auto& destination=bank.layers[layer];const bool wasLoaded=destination.loaded;
            destination.name=name;destination.path=path;destination.waveform=std::move(waveform);destination.duration=duration;destination.loaded=true;
            if(layer==bank.activeLayer){retired=impl->players[pad]->replaceFile(std::move(file));impl->loaded[pad]=true;}else destination.file=std::move(file);
            if(!wasLoaded){std::array<int,PadLayers> valid{};int count=0;for(int i=0;i<PadLayers;++i)if(bank.layers[i].loaded)valid[count++]=i;for(int i=0;i<count;++i){bank.layers[valid[i]].velocityStart=1+i*127/count;bank.layers[valid[i]].velocityEnd=(i+1)*127/count;}}
            if(layer==0){bank.edit=edit;impl->state.edits[pad]=edit;impl->players[pad]->edit=edit;impl->players[pad]->effects->sync(edit);impl->state.names[pad]=name;impl->state.paths[pad]=path;impl->state.waveforms[pad]=destination.waveform;impl->state.durations[pad]=duration;}
            impl->state.active[pad]=false;impl->state.progress[pad]=0;
        }return true;
    }catch(const std::exception& exception){error=exception.what();return false;}
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
void Engine::triggerPitch(int pad,int semitones){triggerVelocity(pad,semitones,127);}
void Engine::triggerVelocity(int pad,int semitones,int velocity){
    if(pad<0||pad>=Pads)return;
    semitones=std::clamp(semitones,-48,48);velocity=std::clamp(velocity,1,127);
    SamplePlayType play=SamplePlayType::OneShot;
    {std::lock_guard<std::mutex> lock(impl->mutex);play=impl->state.edits[pad].samplePlay;impl->heldPitch[pad].store(semitones);}
    impl->pendingVelocity[pad].store(velocity);if(play!=SamplePlayType::NoteOff)impl->pendingPitch[pad].store(semitones);
}
void Engine::release(int pad){
    if(pad<0||pad>=Pads)return;impl->pendingPitch[pad].store(1000);
    std::scoped_lock lock(impl->mutex,impl->audioMutex);const auto play=impl->state.edits[pad].samplePlay;
    if(play==SamplePlayType::NoteOn)impl->players[pad]->release();
    else if(play==SamplePlayType::NoteOff)impl->triggerPad(pad,impl->heldPitch[pad].load(),impl->pendingVelocity[pad].exchange(127));
}
void Engine::releasePerformancePad(int){int source=0;{std::lock_guard<std::mutex> lock(impl->mutex);source=impl->state.sequences[impl->state.sequence].sourcePad;}release(source);}
void Engine::setEdit(int pad,const PadEdit& requested){
    if(pad<0||pad>=Pads)return;
    const double values[]={requested.start,requested.end,requested.gain,requested.pan,requested.semitones,requested.fadeMs,requested.attack,requested.decay,requested.sustain,requested.release,requested.sourceBpm};
    for(double v:values)if(!std::isfinite(v))return;
    PadEdit edit=requested;
    edit.start=std::clamp(edit.start,0.,.999);edit.end=std::clamp(edit.end,edit.start+.001,1.);
    edit.gain=std::clamp(edit.gain,0.,2.);edit.pan=std::clamp(edit.pan,-1.,1.);edit.semitones=std::clamp(edit.semitones,-24.,24.);
    edit.fadeMs=std::clamp(edit.fadeMs,0.,20.);
    edit.attack=std::clamp(edit.attack,0.,1.);edit.decay=std::clamp(edit.decay,0.,1.);edit.sustain=std::clamp(edit.sustain,0.,1.);edit.release=std::clamp(edit.release,0.,1.);
    edit.sourceBpm=std::clamp(edit.sourceBpm,40.,240.);
    edit.samplePlay=SamplePlayType(std::clamp(int(edit.samplePlay),int(SamplePlayType::OneShot),int(SamplePlayType::NoteOn)));
    edit.layerPlay=LayerPlayType(std::clamp(int(edit.layerPlay),int(LayerPlayType::Cycle),int(LayerPlayType::Random)));
    edit.chokeGroup=std::clamp(edit.chokeGroup,-1,15);edit.color=std::clamp(edit.color,0,PadColors-1);
    for(auto& insert:edit.inserts){int type=int(insert.effect);if(type<int(InsertEffect::None)||type>=int(InsertEffect::Count))return;for(double& value:insert.parameters){if(!std::isfinite(value))return;value=std::clamp(value,0.,1.);}}
    bool rebuild=false;{std::lock_guard<std::mutex> lock(impl->mutex);for(int slot=0;slot<InsertSlots;++slot)if(impl->state.edits[pad].inserts[slot].effect!=edit.inserts[slot].effect)rebuild=true;}
    std::unique_ptr<PadEffects> replacement;if(rebuild)replacement=std::make_unique<PadEffects>(edit);
    std::scoped_lock lock(impl->mutex,impl->audioMutex);
    impl->state.edits[pad]=edit;impl->banks[impl->state.program][pad].edit=edit;impl->players[pad]->edit=edit;if(edit.mute)impl->players[pad]->stop();if(replacement)impl->players[pad]->effects.swap(replacement);else impl->players[pad]->effects->sync(edit);
}
void Engine::setLayerVelocityRange(int pad,int layer,int start,int end){
    if(pad<0||pad>=Pads||layer<0||layer>=PadLayers)return;start=std::clamp(start,1,127);end=std::clamp(end,start,127);
    std::scoped_lock lock(impl->mutex,impl->metadataMutex);auto& item=impl->banks[impl->state.program][pad].layers[layer];item.velocityStart=start;item.velocityEnd=end;
}
void Engine::setLayerReverse(int pad,int layer,bool reverse){
    if(pad<0||pad>=Pads||layer<0||layer>=PadLayers)return;std::scoped_lock lock(impl->mutex,impl->metadataMutex);impl->banks[impl->state.program][pad].layers[layer].reverse=reverse;
}
void Engine::setPadLevel(int pad,double level){if(pad<0||pad>=Pads||!std::isfinite(level))return;auto edit=snapshot().edits[pad];edit.gain=std::clamp(level,0.,2.);setEdit(pad,edit);}
void Engine::setPadPan(int pad,double pan){if(pad<0||pad>=Pads||!std::isfinite(pan))return;auto edit=snapshot().edits[pad];edit.pan=std::clamp(pan,-1.,1.);setEdit(pad,edit);}
void Engine::setPadMute(int pad,bool mute){
    if(pad<0||pad>=Pads)return;std::array<int,Pads> targets{};int count=0;{
        std::lock_guard<std::mutex> lock(impl->mutex);const auto& edits=impl->state.edits;const int group=edits[pad].chokeGroup;if(group<0)targets[count++]=pad;else for(int candidate=0;candidate<Pads;++candidate)if(edits[candidate].chokeGroup==group)targets[count++]=candidate;
    }
    const auto current=snapshot();for(int index=0;index<count;++index){const int target=targets[index];auto edit=current.edits[target];edit.mute=mute;setEdit(target,edit);}
}
void Engine::togglePadMute(int pad){if(pad<0||pad>=Pads)return;const auto current=snapshot();setPadMute(pad,!current.edits[pad].mute);}
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
    if(sequence.mode==PerformanceMode::Drum){sequence.drum[p][absolute]=!sequence.drum[p][absolute];if(!sequence.drum[p][absolute])sequence.drumEvents[p][absolute]={};}
    else {const int note=mappedPerformanceNote(sequence.mode,sequence.rootNote,p);sequence.melody[absolute]=sequence.melody[absolute]==note?-1:note;if(sequence.melody[absolute]<0)sequence.melodyEvents[absolute]={};}
}
void Engine::setStepEvent(int track,int absoluteStep,const StepEvent& requested,bool melodic){
    if(track<0||track>=Pads||absoluteStep<0||absoluteStep>=MaxSequenceSteps)return;StepEvent event=requested;event.velocity=std::clamp(event.velocity,1,127);event.probability=std::clamp(event.probability,0,100);event.ratchet=std::clamp(event.ratchet,1,8);event.length=std::clamp(event.length,1,16);std::lock_guard<std::mutex> lock(impl->mutex);auto& sequence=impl->state.sequences[impl->state.sequence];if(melodic)sequence.melodyEvents[absoluteStep]=event;else sequence.drumEvents[track][absoluteStep]=event;
}
bool Engine::insertStepEvent(int trackOrNote,int absoluteStep,bool melodic){
    if(absoluteStep<0||absoluteStep>=MaxSequenceSteps||trackOrNote<0||trackOrNote>(melodic?127:Pads-1))return false;
    std::lock_guard<std::mutex> lock(impl->mutex);auto& sequence=impl->state.sequences[impl->state.sequence];if(absoluteStep>=sequence.bars*Steps)return false;
    if(melodic){if(sequence.melody[absoluteStep]>=0)return false;sequence.melody[absoluteStep]=trackOrNote;sequence.melodyEvents[absoluteStep]={};}
    else {if(sequence.drum[trackOrNote][absoluteStep])return false;sequence.drum[trackOrNote][absoluteStep]=true;sequence.drumEvents[trackOrNote][absoluteStep]={};}
    return true;
}
bool Engine::moveStepEvent(int sourceTrack,int sourceStep,int destinationTrackOrNote,int destinationStep,bool melodic){
    if(sourceStep<0||sourceStep>=MaxSequenceSteps||destinationStep<0||destinationStep>=MaxSequenceSteps||sourceTrack<0||sourceTrack>=Pads||destinationTrackOrNote<0||destinationTrackOrNote>(melodic?127:Pads-1))return false;
    std::lock_guard<std::mutex> lock(impl->mutex);auto& sequence=impl->state.sequences[impl->state.sequence];if(sourceStep>=sequence.bars*Steps||destinationStep>=sequence.bars*Steps)return false;
    if(melodic){if(sequence.melody[sourceStep]<0)return false;if(destinationStep!=sourceStep&&sequence.melody[destinationStep]>=0)return false;const auto event=sequence.melodyEvents[sourceStep];sequence.melody[sourceStep]=-1;sequence.melodyEvents[sourceStep]={};sequence.melody[destinationStep]=destinationTrackOrNote;sequence.melodyEvents[destinationStep]=event;}
    else {if(!sequence.drum[sourceTrack][sourceStep])return false;if((destinationTrackOrNote!=sourceTrack||destinationStep!=sourceStep)&&sequence.drum[destinationTrackOrNote][destinationStep])return false;const auto event=sequence.drumEvents[sourceTrack][sourceStep];sequence.drum[sourceTrack][sourceStep]=false;sequence.drumEvents[sourceTrack][sourceStep]={};sequence.drum[destinationTrackOrNote][destinationStep]=true;sequence.drumEvents[destinationTrackOrNote][destinationStep]=event;}
    return true;
}
bool Engine::deleteStepEvent(int track,int absoluteStep,bool melodic){
    if(track<0||track>=Pads||absoluteStep<0||absoluteStep>=MaxSequenceSteps)return false;std::lock_guard<std::mutex> lock(impl->mutex);auto& sequence=impl->state.sequences[impl->state.sequence];
    if(melodic){if(sequence.melody[absoluteStep]<0)return false;sequence.melody[absoluteStep]=-1;sequence.melodyEvents[absoluteStep]={};}
    else {if(!sequence.drum[track][absoluteStep])return false;sequence.drum[track][absoluteStep]=false;sequence.drumEvents[track][absoluteStep]={};}
    return true;
}
void Engine::clearPattern(){std::lock_guard<std::mutex> lock(impl->mutex);auto& sequence=impl->state.sequences[impl->state.sequence];sequence.drum={};sequence.melody.fill(-1);sequence.drumEvents={};sequence.melodyEvents={};}
void Engine::demoPattern(){std::lock_guard<std::mutex> lock(impl->mutex);auto& sequence=impl->state.sequences[impl->state.sequence];sequence.drum={};sequence.melody.fill(-1);sequence.drumEvents={};sequence.melodyEvents={};sequence.bars=1;impl->state.viewBar=0;for(int s:{0,4,8,12})sequence.drum[0][s]=true;for(int s:{4,12})sequence.drum[1][s]=true;for(int s=0;s<16;s+=2)sequence.drum[2][s]=true;sequence.drum[3][14]=true;sequence.sourcePad=12;sequence.rootNote=48;const int notes[8]={48,48,50,55,53,50,48,55};for(int i=0;i<8;++i)sequence.melody[i*2]=notes[i];}
void Engine::selectSequence(int requested){
    requested=std::clamp(requested,0,Sequences-1);std::scoped_lock lock(impl->mutex,impl->audioMutex);auto& s=impl->state;
    impl->prepareProgram(s.sequences[requested].program);impl->activatePreparedProgram();
    s.sequence=requested;s.queuedSequence=-1;s.queuedProgram=-1;s.viewBar=0;s.playBar=0;s.step=-1;s.songPlaying=false;s.songPosition=-1;impl->nextStep=0;impl->untilStep=0;impl->ratchetRemaining.fill(0);impl->gateRemaining.fill(0);
}
void Engine::queueSequence(int requested){
    requested=std::clamp(requested,0,Sequences-1);bool playing=false;
    {std::scoped_lock lock(impl->mutex,impl->audioMutex);auto& s=impl->state;playing=s.playing;if(playing){impl->prepareProgram(s.sequences[requested].program);s.queuedProgram=s.sequences[requested].program;s.queuedSequence=requested;s.songPlaying=false;}}
    if(!playing)selectSequence(requested);
}
void Engine::copySequence(int source,int destination){
    if(source<0||source>=Sequences||destination<0||destination>=Sequences||source==destination)return;
    std::lock_guard<std::mutex> lock(impl->mutex);const auto name=impl->state.sequences[destination].name;const int program=impl->state.sequences[destination].program;
    impl->state.sequences[destination]=impl->state.sequences[source];impl->state.sequences[destination].name=name;impl->state.sequences[destination].program=program;
}
int Engine::createProgram(const std::string& requestedName,ProgramType type){
    if(int(type)<0||int(type)>int(ProgramType::Keygroup))return -1;std::scoped_lock lock(impl->mutex,impl->metadataMutex,impl->audioMutex);
    int slot=-1;for(int i=0;i<Programs;++i)if(!impl->state.programs[i].used){slot=i;break;}if(slot<0)return -1;
    impl->banks[slot]={};for(int pad=0;pad<Pads;++pad)impl->implColor(slot,pad);auto name=requestedName.empty()?"Program "+std::to_string(slot+1):requestedName;if(name.size()>64)name.resize(64);impl->state.programs[slot]={name,type,true};return slot;
}
int Engine::duplicateProgram(int source,const std::string& requestedName){
    if(source<0||source>=Programs)return -1;std::array<std::array<PadLayerInfo,PadLayers>,Pads> layers;std::array<PadEdit,Pads> edits;ProgramInfo info;
    {std::scoped_lock lock(impl->mutex,impl->metadataMutex);info=impl->state.programs[source];if(!info.used)return -1;for(int p=0;p<Pads;++p){edits[p]=impl->banks[source][p].edit;for(int l=0;l<PadLayers;++l){const auto& from=impl->banks[source][p].layers[l];layers[p][l]={from.name,from.path,from.duration,from.velocityStart,from.velocityEnd,from.reverse,from.loaded};}}}
    int destination=createProgram(requestedName.empty()?info.name+" Copy":requestedName,info.type);if(destination<0)return -1;
    std::array<Impl::BankPad,Pads> parsed;
    try{for(int p=0;p<Pads;++p){auto& pad=parsed[p];pad.edit=edits[p];pad.effects=std::make_unique<PadEffects>(pad.edit);for(int l=0;l<PadLayers;++l){auto& to=pad.layers[l];const auto& from=layers[p][l];to.name=from.name;to.path=from.path;to.duration=from.duration;to.velocityStart=from.velocityStart;to.velocityEnd=from.velocityEnd;to.reverse=from.reverse;to.loaded=from.loaded;if(from.path.empty())continue;to.file=decode(from.path);to.loaded=true;to.waveform.assign(320,0.f);const auto& audio=to.file->samples[0];for(size_t n=0;n<audio.size();++n){const size_t j=std::min<size_t>(319,n*320/audio.size());to.waveform[j]=std::max(to.waveform[j],std::abs(audio[n]));}}}}
    catch(...){std::scoped_lock lock(impl->mutex,impl->metadataMutex);impl->banks[destination]={};impl->state.programs[destination]={};return -1;}
    {std::scoped_lock lock(impl->mutex,impl->metadataMutex);impl->banks[destination]=std::move(parsed);impl->state.programs[destination].emptyPadColor=info.emptyPadColor;impl->state.programs[destination].padOffColor=info.padOffColor;impl->state.programs[destination].padOnColor=info.padOnColor;}return destination;
}
bool Engine::renameProgram(int program,const std::string& requestedName){if(program<0||program>=Programs||requestedName.empty()||requestedName.size()>64)return false;std::lock_guard<std::mutex> lock(impl->mutex);if(!impl->state.programs[program].used)return false;impl->state.programs[program].name=requestedName;return true;}
bool Engine::deleteProgram(int program){
    if(program<0||program>=Programs)return false;std::scoped_lock lock(impl->mutex,impl->metadataMutex,impl->audioMutex);auto& s=impl->state;if(program==s.program||!s.programs[program].used)return false;for(const auto& sequence:s.sequences)if(sequence.program==program)return false;impl->banks[program]={};s.programs[program]={};return true;
}
bool Engine::assignProgramToSequence(int sequence,int program){
    if(sequence<0||sequence>=Sequences||program<0||program>=Programs)return false;bool active=false;
    {std::lock_guard<std::mutex> lock(impl->mutex);if(!impl->state.programs[program].used)return false;impl->state.sequences[sequence].program=program;active=sequence==impl->state.sequence;}
    if(active)queueSequence(sequence);return true;
}
bool Engine::copyProgramPad(int sourcePad,int destinationPad,std::string& error){
    if(sourcePad<0||sourcePad>=Pads||destinationPad<0||destinationPad>=Pads){error="Pad source ou destination invalide.";return false;}if(sourcePad==destinationPad)return true;
    int program=0;PadEdit edit;std::array<PadLayerInfo,PadLayers> layers;
    {std::scoped_lock lock(impl->mutex,impl->metadataMutex);program=impl->state.program;const auto& source=impl->banks[program][sourcePad];edit=source.edit;for(int layer=0;layer<PadLayers;++layer){const auto& item=source.layers[layer];layers[layer]={item.name,item.path,item.duration,item.velocityStart,item.velocityEnd,item.reverse,item.loaded};}}
    Impl::BankPad parsed;parsed.edit=edit;parsed.effects=std::make_unique<PadEffects>(edit);
    try{for(int layer=0;layer<PadLayers;++layer){const auto& source=layers[layer];auto& destination=parsed.layers[layer];destination.name=source.name;destination.path=source.path;destination.duration=source.duration;destination.velocityStart=source.velocityStart;destination.velocityEnd=source.velocityEnd;destination.reverse=source.reverse;if(source.path.empty())continue;destination.file=decode(source.path);destination.loaded=true;destination.duration=double(destination.file->getNumSamplesPerChannel())/Rate;destination.waveform.assign(320,0.f);const auto& audio=destination.file->samples[0];for(size_t sample=0;sample<audio.size();++sample){const size_t point=std::min<size_t>(319,sample*320/audio.size());destination.waveform[point]=std::max(destination.waveform[point],std::abs(audio[sample]));}}}
    catch(const std::exception& exception){error=std::string("Copie du pad impossible : ")+exception.what();return false;}
    Impl::BankPad retiredBank;std::unique_ptr<AudioFile<float>> retiredFile;std::unique_ptr<PadEffects> retiredEffects=std::move(parsed.effects);
    {std::scoped_lock lock(impl->mutex,impl->metadataMutex,impl->audioMutex);if(impl->state.program!=program){error="Le Program actif a changé pendant la copie.";return false;}impl->players[destinationPad]->stop();impl->pendingPitch[destinationPad].store(1000);retiredBank=std::move(impl->banks[program][destinationPad]);parsed.activeLayer=0;retiredFile=impl->players[destinationPad]->replaceFile(std::move(parsed.layers[0].file));impl->loaded[destinationPad]=parsed.layers[0].loaded;impl->banks[program][destinationPad]=std::move(parsed);auto& bank=impl->banks[program][destinationPad];impl->players[destinationPad]->edit=bank.edit;impl->players[destinationPad]->effects.swap(retiredEffects);impl->players[destinationPad]->resetMix();const auto& primary=bank.layers[0];impl->state.names[destinationPad]=primary.name;impl->state.paths[destinationPad]=primary.path;impl->state.waveforms[destinationPad]=primary.waveform;impl->state.durations[destinationPad]=primary.duration;impl->state.edits[destinationPad]=bank.edit;impl->state.active[destinationPad]=false;impl->state.progress[destinationPad]=0;for(int layer=0;layer<PadLayers;++layer){const auto& item=bank.layers[layer];impl->state.layers[destinationPad][layer]={item.name,item.path,item.duration,item.velocityStart,item.velocityEnd,item.reverse,item.loaded};}}
    return true;
}
bool Engine::deleteProgramPad(int pad){
    if(pad<0||pad>=Pads)return false;Impl::BankPad empty,retiredBank;std::unique_ptr<AudioFile<float>> retiredFile;std::unique_ptr<PadEffects> retiredEffects;bool contained=false;
    empty.edit.color=3;{std::lock_guard<std::mutex> lock(impl->mutex);for(const auto& layer:impl->state.layers[pad])contained=contained||layer.loaded;}empty.effects=std::make_unique<PadEffects>(empty.edit);retiredEffects=std::move(empty.effects);
    {std::scoped_lock lock(impl->mutex,impl->metadataMutex,impl->audioMutex);const int program=impl->state.program;impl->players[pad]->stop();impl->pendingPitch[pad].store(1000);retiredBank=std::move(impl->banks[program][pad]);retiredFile=impl->players[pad]->replaceFile({});impl->loaded[pad]=false;impl->banks[program][pad]=std::move(empty);impl->players[pad]->edit=impl->banks[program][pad].edit;impl->players[pad]->effects.swap(retiredEffects);impl->players[pad]->resetMix();impl->state.names[pad]="Vide";impl->state.paths[pad].clear();impl->state.waveforms[pad].clear();impl->state.durations[pad]=0;impl->state.layers[pad]={};impl->state.edits[pad]=impl->banks[program][pad].edit;impl->state.active[pad]=false;impl->state.progress[pad]=0;}
    return contained;
}
int Engine::eraseNotes(int sequenceIndex,int track,int firstStep,int lastStep){
    if(sequenceIndex<0||sequenceIndex>=Sequences||track<0||track>=Pads)return 0;std::lock_guard<std::mutex> lock(impl->mutex);auto& sequence=impl->state.sequences[sequenceIndex];const int total=sequence.bars*Steps;if(total<=0)return 0;firstStep=std::clamp(firstStep,0,total-1);lastStep=std::clamp(lastStep,0,total-1);if(firstStep>lastStep)std::swap(firstStep,lastStep);int erased=0;
    if(sequence.mode==PerformanceMode::Drum){for(int step=firstStep;step<=lastStep;++step)if(sequence.drum[track][step]){sequence.drum[track][step]=false;sequence.drumEvents[track][step]={};++erased;}}
    else for(int step=firstStep;step<=lastStep;++step)if(sequence.melody[step]>=0){sequence.melody[step]=-1;sequence.melodyEvents[step]={};++erased;}
    return erased;
}
void Engine::setProgramPadColor(int pad,int color,bool allPads){
    if(pad<0||pad>=Pads||color<0||color>=PadColors)return;
    std::lock_guard<std::mutex> lock(impl->mutex);const int program=impl->state.program;
    for(int candidate=0;candidate<Pads;++candidate){if(!allPads&&candidate!=pad)continue;impl->banks[program][candidate].edit.color=color;impl->state.edits[candidate].color=color;}
}
void Engine::resetProgramPadColors(){setProgramPadColor(0,3,true);}
void Engine::setProgramColorModes(EmptyPadColorMode emptyMode,PadOffColorMode offMode,PadOnColorMode onMode){
    if(int(emptyMode)<0||int(emptyMode)>int(EmptyPadColorMode::Dim)||int(offMode)<0||int(offMode)>int(PadOffColorMode::Dim)||int(onMode)<0||int(onMode)>int(PadOnColorMode::Full))return;
    std::lock_guard<std::mutex> lock(impl->mutex);auto& program=impl->state.programs[impl->state.program];program.emptyPadColor=emptyMode;program.padOffColor=offMode;program.padOnColor=onMode;
}
void Engine::setSequenceBars(int bars){std::lock_guard<std::mutex> lock(impl->mutex);auto& s=impl->state;auto& sequence=s.sequences[s.sequence];sequence.bars=std::clamp(bars,1,MaxBars);s.viewBar=std::min(s.viewBar,sequence.bars-1);if(impl->nextStep>=sequence.bars*Steps)impl->nextStep=0;}
void Engine::setViewBar(int bar){std::lock_guard<std::mutex> lock(impl->mutex);auto& s=impl->state;s.viewBar=std::clamp(bar,0,s.sequences[s.sequence].bars-1);}
void Engine::setGroup(int requested){
    requested=std::clamp(requested,0,Groups-1);
    std::scoped_lock lock(impl->mutex,impl->audioMutex);auto& state=impl->state;const int targetProgram=state.sequences[requested].program;
    if(state.playing){impl->prepareProgram(targetProgram);state.queuedProgram=targetProgram;state.queuedGroup=requested;state.queuedSequence=requested;state.songPlaying=false;return;}
    impl->prepareProgram(targetProgram);impl->activatePreparedProgram();state.group=requested;state.queuedGroup=-1;state.sequence=requested;state.queuedSequence=-1;state.viewBar=state.playBar=0;state.step=-1;impl->nextStep=0;impl->untilStep=0;
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
bool Engine::recordHit(int pad,int note,int quantizeDivision,int velocity){
    if(pad<0||pad>=Pads||(note>=0&&(note>127)))return false;
    std::lock_guard<std::mutex> lock(impl->mutex);auto& state=impl->state;if(!state.playing||state.step<0)return false;
    auto& sequence=state.sequences[state.sequence];const int length=std::max(Steps,sequence.bars*Steps);
    const int quantum=quantizeDivision<=4?4:quantizeDivision<=8?2:1;
    const double stepSamples=Rate*60./state.tempo/4.;const double elapsed=std::clamp(1.-impl->untilStep/stepSamples,0.,1.);
    const double position=state.playBar*Steps+state.step+elapsed;int target=int(std::floor((position+quantum*.5)/quantum))*quantum;target%=length;if(target<0)target+=length;
    StepEvent event;event.velocity=std::clamp(velocity,1,127);if(note<0){sequence.drum[pad][target]=true;sequence.drumEvents[pad][target]=event;}else{sequence.melody[target]=note;sequence.melodyEvents[target]=event;}
    return true;
}
void Engine::addSongStep(int sequence,int repeats){std::lock_guard<std::mutex> lock(impl->mutex);auto& s=impl->state;if(s.songLength>=MaxSongSteps)return;s.song[s.songLength++]={std::clamp(sequence,0,Sequences-1),std::clamp(repeats,1,128)};}
void Engine::editSongStep(int position,int sequence,int repeats){std::lock_guard<std::mutex> lock(impl->mutex);auto& s=impl->state;if(position<0||position>=s.songLength)return;s.song[position]={std::clamp(sequence,0,Sequences-1),std::clamp(repeats,1,128)};}
void Engine::deleteSongStep(int position){std::lock_guard<std::mutex> lock(impl->mutex);auto& s=impl->state;if(position<0||position>=s.songLength)return;for(int i=position;i+1<s.songLength;++i)s.song[i]=s.song[i+1];s.song[--s.songLength]={};if(s.songPosition>=s.songLength)s.songPosition=s.songLength-1;}
void Engine::clearSong(){std::lock_guard<std::mutex> lock(impl->mutex);auto& s=impl->state;s.song={};s.songLength=0;s.songPlaying=false;s.songPosition=-1;s.songRepeat=0;}
void Engine::playSong(){
    int first=-1;{std::lock_guard<std::mutex> lock(impl->mutex);if(impl->state.songLength)first=impl->state.song[0].sequence;}if(first<0)return;
    selectSequence(first);std::lock_guard<std::mutex> lock(impl->mutex);auto& s=impl->state;s.songPlaying=true;s.songPosition=0;s.songRepeat=0;s.playing=true;
}
void Engine::setPlaying(bool b){std::lock_guard<std::mutex> lock(impl->mutex);impl->state.playing=b;}
void Engine::stop(){std::scoped_lock lock(impl->mutex,impl->audioMutex);auto& s=impl->state;impl->cancelPreparedProgram();s.playing=false;s.songPlaying=false;s.noteRepeat=false;s.arpeggiator=false;s.songPosition=-1;s.songRepeat=0;s.queuedSequence=-1;s.queuedGroup=-1;s.step=-1;s.playBar=0;s.mixPeak=s.peak=s.preLimiterPeak=s.limiterReductionDb=0;s.trackPeaks={};impl->limiterGain=1;impl->untilStep=impl->untilRepeat=0;impl->nextStep=0;impl->repeatPad=-1;impl->arpHeld.fill(false);impl->arpIndex=-1;impl->ratchetRemaining.fill(0);impl->gateRemaining.fill(0);for(int p=0;p<Pads;++p){impl->players[p]->stop();impl->pendingPitch[p].store(1000);}impl->previewPlayer->stop();}
void Engine::setTempo(double t){if(!std::isfinite(t))return;std::lock_guard<std::mutex> lock(impl->mutex);impl->state.tempo=std::clamp(t,40.,240.);}
void Engine::setVolume(double v){if(!std::isfinite(v))return;std::lock_guard<std::mutex> lock(impl->mutex);impl->state.volume=std::clamp(v,0.,1.);}
void Engine::setNoteRepeat(bool enabled,bool arpeggiator){std::scoped_lock lock(impl->mutex,impl->audioMutex);impl->state.noteRepeat=enabled;impl->state.arpeggiator=enabled&&arpeggiator;impl->untilRepeat=0;if(!enabled){impl->repeatPad=-1;impl->arpHeld.fill(false);impl->arpIndex=-1;}}
void Engine::setRepeatDivision(int division){std::lock_guard<std::mutex> lock(impl->mutex);impl->state.repeatDivision=division<=4?4:division<=8?8:division<=16?16:32;impl->untilRepeat=0;}
void Engine::setRepeatVoice(int pad,int semitones,bool held){if(pad<0||pad>=Pads)return;std::scoped_lock lock(impl->mutex,impl->audioMutex);if(held){impl->repeatPad=pad;impl->repeatPitch=std::clamp(semitones,-48,48);impl->untilRepeat=0;}else if(impl->repeatPad==pad)impl->repeatPad=-1;}
void Engine::holdArpeggioNote(int sourcePad,int note,bool held){if(sourcePad<0||sourcePad>=Pads||note<0||note>127)return;std::scoped_lock lock(impl->mutex,impl->audioMutex);impl->arpSource=sourcePad;impl->arpHeld[note]=held;if(held&&impl->arpIndex<0)impl->untilRepeat=0;if(!held&&note==impl->arpIndex)impl->arpIndex=-1;}
Snapshot Engine::snapshot(){
    Snapshot result;
    {std::lock_guard<std::mutex> lock(impl->mutex);const auto& state=impl->state;result.active=state.active;result.edits=state.edits;result.progress=state.progress;result.sequences=state.sequences;result.programs=state.programs;result.song=state.song;
     result.tempo=state.tempo;result.volume=state.volume;result.playing=state.playing;result.songPlaying=state.songPlaying;result.noteRepeat=state.noteRepeat;result.arpeggiator=state.arpeggiator;result.repeatDivision=state.repeatDivision;result.step=state.step;result.playBar=state.playBar;result.viewBar=state.viewBar;result.sequence=state.sequence;result.queuedSequence=state.queuedSequence;result.group=state.group;result.queuedGroup=state.queuedGroup;result.program=state.program;result.queuedProgram=state.queuedProgram;result.selectedTrack=state.selectedTrack;result.songLength=state.songLength;result.songPosition=state.songPosition;result.songRepeat=state.songRepeat;result.mixPeak=state.mixPeak;result.peak=state.peak;result.preLimiterPeak=state.preLimiterPeak;result.limiterReductionDb=state.limiterReductionDb;result.trackPeaks=state.trackPeaks;
     const auto& sequence=state.sequences[state.sequence];const int first=state.viewBar*Steps;result.melody.fill(-1);for(int p=0;p<Pads;++p)for(int step=0;step<Steps;++step)result.pattern[p][step]=sequence.drum[p][first+step];for(int step=0;step<Steps;++step)result.melody[step]=sequence.melody[first+step];}
    {std::lock_guard<std::mutex> lock(impl->metadataMutex);for(int pad=0;pad<Pads;++pad){const auto& bank=impl->banks[result.program][pad];const auto& primary=bank.layers[0];result.names[pad]=primary.name;result.paths[pad]=primary.path;result.waveforms[pad]=primary.waveform;result.durations[pad]=primary.duration;for(int layer=0;layer<PadLayers;++layer){const auto& source=bank.layers[layer];result.layers[pad][layer]={source.name,source.path,source.duration,source.velocityStart,source.velocityEnd,source.reverse,source.loaded};}}}
    return result;
}
void Engine::render(float* out){
    std::fill(out,out+Block*2,0.f);
    // Never drop an audio block or pause the sample clock because the UI is reading.
    std::scoped_lock lock(impl->mutex,impl->audioMutex);
    auto& s=impl->state;
    // The old pattern has completed. Swap its Program at the exact boundary
    // before triggering step 1 of the queued pattern; transport never stops.
    if(s.playing&&impl->untilStep<=0&&impl->nextStep==0&&s.queuedProgram>=0&&s.sequences[s.sequence].program==s.queuedProgram)impl->activatePreparedProgram();
    if(s.playing&&impl->untilStep<=0&&impl->nextStep==0&&s.queuedGroup>=0&&s.sequence==s.queuedGroup){s.group=s.queuedGroup;s.queuedGroup=-1;}
    auto& activeSequence=s.sequences[s.sequence];bool anySolo=false;for(const auto& track:activeSequence.tracks)anySolo=anySolo||track.solo;
    for(int p=0;p<Pads;++p){impl->players[p]->projectTempo=s.tempo;const auto& edit=s.edits[p];impl->players[p]->setTrackMix(activeSequence.tracks[p].level*(edit.mute?0.:1.),std::clamp(activeSequence.tracks[p].pan+edit.pan,-1.,1.));}
    for(int p=0;p<Pads;++p){const int pitch=impl->pendingPitch[p].exchange(1000);if(pitch!=1000)impl->triggerPad(p,pitch,impl->pendingVelocity[p].exchange(127));}
    for(int p=0;p<Pads;++p)if(impl->ratchetRemaining[p]>0){impl->ratchetUntil[p]-=Block;while(impl->ratchetRemaining[p]>0&&impl->ratchetUntil[p]<=0){impl->triggerPad(p,impl->ratchetPitch[p],impl->ratchetVelocity[p]);--impl->ratchetRemaining[p];impl->ratchetUntil[p]+=impl->ratchetInterval[p];}}
    if(s.noteRepeat){
        if(impl->untilRepeat<=0){
            if(s.arpeggiator){int note=-1;for(int offset=1;offset<=128;++offset){const int candidate=(impl->arpIndex+offset+128)%128;if(impl->arpHeld[candidate]){note=candidate;break;}}if(note>=0){impl->arpIndex=note;const int root=s.sequences[s.sequence].rootNote;impl->triggerPad(impl->arpSource,note-root,127);}}
            else if(impl->repeatPad>=0)impl->triggerPad(impl->repeatPad,impl->repeatPitch,127);
            impl->untilRepeat+=Rate*60./s.tempo*4./s.repeatDivision;
        }
        impl->untilRepeat-=Block;
    }else impl->untilRepeat=0;
    if(s.playing){
        if(impl->untilStep<=0){
            auto& sequence=s.sequences[s.sequence];const int absolute=impl->nextStep;s.step=absolute%Steps;s.playBar=absolute/Steps;
            const double stepSamples=Rate*60./s.tempo/4.;auto passes=[&](int probability){if(probability>=100)return true;if(probability<=0)return false;impl->randomState=impl->randomState*1664525u+1013904223u;return int((impl->randomState>>16)%100u)<probability;};
            for(int p=0;p<Pads;++p)if(impl->gateRemaining[p]>0&&--impl->gateRemaining[p]==0&&impl->banks[s.program][p].edit.samplePlay==SamplePlayType::NoteOn)impl->players[p]->release();
            for(int p=0;p<Pads;++p){const auto& track=sequence.tracks[p];const auto& event=sequence.drumEvents[p][absolute];if(sequence.drum[p][absolute]&&!track.mute&&(!anySolo||track.solo)&&passes(event.probability)){impl->triggerPad(p,0,event.velocity);impl->gateRemaining[p]=event.length;impl->ratchetRemaining[p]=event.ratchet-1;impl->ratchetPitch[p]=0;impl->ratchetVelocity[p]=event.velocity;impl->ratchetInterval[p]=stepSamples/event.ratchet;impl->ratchetUntil[p]=impl->ratchetInterval[p];}}
            const int note=sequence.melody[absolute],source=sequence.sourcePad;const auto& melodicEvent=sequence.melodyEvents[absolute];if(note>=0&&source>=0&&source<Pads&&passes(melodicEvent.probability)){const int pitch=note-sequence.rootNote;impl->triggerPad(source,pitch,melodicEvent.velocity);impl->gateRemaining[source]=melodicEvent.length;impl->ratchetRemaining[source]=melodicEvent.ratchet-1;impl->ratchetPitch[source]=pitch;impl->ratchetVelocity[source]=melodicEvent.velocity;impl->ratchetInterval[source]=stepSamples/melodicEvent.ratchet;impl->ratchetUntil[source]=impl->ratchetInterval[source];}
            if(++impl->nextStep>=sequence.bars*Steps){
                impl->nextStep=0;
                if(s.songPlaying){
                    ++s.songRepeat;const int repeats=s.song[s.songPosition].repeats;
                    if(s.songRepeat>=repeats){s.songRepeat=0;++s.songPosition;if(s.songPosition>=s.songLength){s.playing=false;s.songPlaying=false;s.songPosition=-1;}else{const int nextSequence=s.song[s.songPosition].sequence;const int nextProgram=s.sequences[nextSequence].program;impl->prepareProgram(nextProgram);s.queuedProgram=nextProgram;s.sequence=nextSequence;s.viewBar=s.playBar=0;}}
                }else if(s.queuedSequence>=0){s.sequence=s.queuedSequence;s.queuedSequence=-1;s.viewBar=s.playBar=0;}
            }
            impl->untilStep+=Rate*60./s.tempo/4.;
        }
        impl->untilStep-=Block;
    }
    impl->graph->process(*impl->buffer);float mixPeak=0,preLimiterPeak=0;
    for(int n=0;n<Block;++n){impl->currentVolume+=(float(s.volume)-impl->currentVolume)*.004f;const float master=s.volume==0?0.f:impl->currentVolume;for(int c=0;c<2;++c){float mixed=impl->buffer->getOutputBuffer(c)[n];if(!std::isfinite(mixed))mixed=0;mixPeak=std::max(mixPeak,std::abs(mixed));const float value=mixed*master;out[n*2+c]=value;preLimiterPeak=std::max(preLimiterPeak,std::abs(value));}}
    // Stereo-linked sample peak limiter.  The previous block-wide limiter
    // reduced the complete 5.3 ms block from one isolated transient and then
    // recovered for roughly a third of a second.  On drums this audibly
    // pumped and made the output weak.  Attack only at the offending sample;
    // release smoothly over 50 ms while keeping the hard ceiling as a final
    // numerical safety net.
    constexpr float limiterRelease=1.f-std::exp(-1.f/(Rate*.050f));
    float postLimiterPeak=0,minimumLimiterGain=1;
    for(int frame=0;frame<Block;++frame){
        const float framePeak=std::max(std::abs(out[frame*2]),std::abs(out[frame*2+1]));
        const float targetGain=framePeak>MasterCeiling?MasterCeiling/framePeak:1.f;
        if(targetGain<1.f)impl->limiterGain=std::min(impl->limiterGain,targetGain);
        else impl->limiterGain+=(1.f-impl->limiterGain)*limiterRelease;
        impl->limiterGain=std::clamp(impl->limiterGain,0.f,1.f);minimumLimiterGain=std::min(minimumLimiterGain,impl->limiterGain);
        for(int channel=0;channel<2;++channel){auto& sample=out[frame*2+channel];sample=std::clamp(sample*impl->limiterGain,-MasterCeiling,MasterCeiling);postLimiterPeak=std::max(postLimiterPeak,std::abs(sample));}
    }
    // Hold percussion peaks long enough for the 60 Hz Windows UI to display
    // their envelope instead of a single-frame dot.
    constexpr float meterRelease=.996f;
    s.mixPeak=std::max(mixPeak,s.mixPeak*meterRelease);s.preLimiterPeak=std::max(preLimiterPeak,s.preLimiterPeak*meterRelease);s.peak=std::max(postLimiterPeak,s.peak*meterRelease);s.limiterReductionDb=minimumLimiterGain<1.f?float(-20.*std::log10(std::max(minimumLimiterGain,1e-6f))):0.f;
    for(int p=0;p<Pads;++p){s.active[p]=impl->players[p]->isPlaying();s.progress[p]=impl->players[p]->progress();s.trackPeaks[p]=std::max(impl->players[p]->outputPeak(),s.trackPeaks[p]*meterRelease);}
}
bool Engine::save(const std::string& path,std::string& error){
    auto s=snapshot();std::array<std::array<std::array<PadLayerInfo,PadLayers>,Pads>,Programs> layers;std::array<std::array<PadEdit,Pads>,Programs> edits;
    {std::scoped_lock lock(impl->mutex,impl->metadataMutex);for(int program=0;program<Programs;++program)for(int pad=0;pad<Pads;++pad){const auto& bank=impl->banks[program][pad];edits[program][pad]=bank.edit;for(int layer=0;layer<PadLayers;++layer){const auto& source=bank.layers[layer];layers[program][pad][layer]={source.name,source.path,source.duration,source.velocityStart,source.velocityEnd,source.reverse,source.loaded};}}}
    std::ofstream f(std::filesystem::u8path(path));if(!f){error="Impossible d'enregistrer.";return false;}f<<std::setprecision(17)<<"KICKLAB 14\n"<<s.tempo<<' '<<s.volume<<' '<<s.group<<' '<<s.selectedTrack<<'\n';
    f<<"PROGRAMS "<<Programs<<'\n';for(int program=0;program<Programs;++program){const auto& info=s.programs[program];f<<"PROGRAM "<<program<<' '<<info.used<<' '<<int(info.type)<<' '<<std::quoted(info.name)<<' '<<int(info.emptyPadColor)<<' '<<int(info.padOffColor)<<' '<<int(info.padOnColor)<<'\n';for(int p=0;p<Pads;++p){const auto& e=edits[program][p];f<<"PAD "<<p<<' '<<int(e.samplePlay)<<' '<<int(e.layerPlay)<<' '<<e.chokeGroup<<' '<<e.color;writePadSettings(f,e);f<<' '<<e.pan<<' '<<e.mute<<'\n';for(int layer=0;layer<PadLayers;++layer){const auto& item=layers[program][p][layer];f<<"LAYER "<<layer<<' '<<std::quoted(item.path)<<' '<<item.velocityStart<<' '<<item.velocityEnd<<' '<<item.reverse<<'\n';}}}
    f<<"SEQUENCES "<<Sequences<<' '<<s.sequence<<' '<<s.viewBar<<'\n';
    for(int index=0;index<Sequences;++index){const auto& sequence=s.sequences[index];f<<"SEQ "<<index<<' '<<std::quoted(sequence.name)<<' '<<sequence.bars<<' '<<int(sequence.mode)<<' '<<sequence.sourcePad<<' '<<sequence.rootNote<<' '<<sequence.program<<'\n'<<"TRACKS "<<Pads<<'\n';for(int track=0;track<Pads;++track){const auto& t=sequence.tracks[track];f<<"TRACK "<<track<<' '<<std::quoted(t.name)<<' '<<int(t.program)<<' '<<t.sourcePad<<' '<<t.level<<' '<<t.pan<<' '<<t.mute<<' '<<t.solo<<' '<<t.armed<<'\n';}f<<"DRUM";for(int p=0;p<Pads;++p){unsigned long long bits=0;for(int step=0;step<MaxSequenceSteps;++step)if(sequence.drum[p][step])bits|=1ull<<step;f<<' '<<bits;}f<<"\nMELO";for(int note:sequence.melody)f<<' '<<note;f<<"\nEVENTS\n";for(int p=0;p<Pads;++p){f<<"DEVT "<<p;for(const auto& event:sequence.drumEvents[p])f<<' '<<event.velocity<<' '<<event.probability<<' '<<event.ratchet<<' '<<event.length;f<<'\n';}f<<"MEVT";for(const auto& event:sequence.melodyEvents)f<<' '<<event.velocity<<' '<<event.probability<<' '<<event.ratchet<<' '<<event.length;f<<'\n';}
    f<<"SONG "<<s.songLength<<'\n';for(int index=0;index<s.songLength;++index)f<<"STEP "<<s.song[index].sequence<<' '<<s.song[index].repeats<<'\n';
    if(!f){error="Erreur d'ecriture.";return false;}return true;
}
bool Engine::saveKit(const std::string& path,const std::string& sampleRoot,std::string& error){
    std::array<std::array<PadLayerInfo,PadLayers>,Pads> layers;std::array<PadEdit,Pads> edits;int program=0;ProgramInfo programInfo;
    {std::scoped_lock lock(impl->mutex,impl->metadataMutex);program=impl->state.program;programInfo=impl->state.programs[program];for(int pad=0;pad<Pads;++pad){const auto& bank=impl->banks[program][pad];edits[pad]=bank.edit;for(int layer=0;layer<PadLayers;++layer){const auto& source=bank.layers[layer];layers[pad][layer]={source.name,source.path,source.duration,source.velocityStart,source.velocityEnd,source.reverse,source.loaded};}}}
    const auto root=std::filesystem::absolute(std::filesystem::u8path(sampleRoot)).lexically_normal();
    std::ofstream file(std::filesystem::u8path(path),std::ios::trunc);if(!file){error="Impossible d'enregistrer le kit.";return false;}
    file<<std::setprecision(17)<<"KICKKIT 4\n";
    file<<"COLOURS "<<int(programInfo.emptyPadColor)<<' '<<int(programInfo.padOffColor)<<' '<<int(programInfo.padOnColor)<<'\n';
    for(int pad=0;pad<Pads;++pad){
        const auto& edit=edits[pad];file<<"PAD "<<pad<<' '<<int(edit.samplePlay)<<' '<<int(edit.layerPlay)<<' '<<edit.chokeGroup<<' '<<edit.color;writePadSettings(file,edit);file<<' '<<edit.pan<<' '<<edit.mute<<'\n';
        for(int layer=0;layer<PadLayers;++layer){char location='E';std::string stored;const auto& source=layers[pad][layer];if(!source.path.empty()){const auto absolute=std::filesystem::absolute(std::filesystem::u8path(source.path)).lexically_normal();const auto relative=absolute.lexically_relative(root);if(safeRelativePath(relative)){location='R';stored=relative.generic_u8string();}else{location='A';stored=absolute.u8string();}}file<<"LAYER "<<layer<<' '<<location<<' '<<std::quoted(stored)<<' '<<source.velocityStart<<' '<<source.velocityEnd<<' '<<source.reverse<<'\n';}
    }
    if(!file){error="Erreur d'ecriture du kit.";return false;}return true;
}
bool Engine::openKit(const std::string& path,const std::string& sampleRoot,std::string& error){
    std::ifstream file(std::filesystem::u8path(path),std::ios::ate);
    if(!file||file.tellg()>512*1024){error="Kit introuvable ou trop volumineux.";return false;}file.seekg(0);
    std::string tag;int version=0;if(!(file>>tag>>version)||tag!="KICKKIT"||(version<1||version>4)){error="Fichier KICKKIT invalide.";return false;}
    const auto root=std::filesystem::absolute(std::filesystem::u8path(sampleRoot)).lexically_normal();
    EmptyPadColorMode emptyMode=EmptyPadColorMode::Off;PadOffColorMode offMode=PadOffColorMode::Dim;PadOnColorMode onMode=PadOnColorMode::Velocity;
    if(version>=4){int empty=0,off=0,on=0;if(!(file>>tag>>empty>>off>>on)||tag!="COLOURS"||empty<0||empty>1||off<0||off>1||on<0||on>2){error="Modes de couleur KICKKIT invalides.";return false;}emptyMode=EmptyPadColorMode(empty);offMode=PadOffColorMode(off);onMode=PadOnColorMode(on);}
    std::array<std::array<std::string,PadLayers>,Pads> paths{};std::array<PadEdit,Pads> edits{};std::array<std::array<int,PadLayers>,Pads> velocityStart{},velocityEnd{};std::array<std::array<bool,PadLayers>,Pads> reverse{};
    for(auto& row:velocityStart)row.fill(1);for(auto& row:velocityEnd)row.fill(127);
    for(int expected=0;expected<Pads;++expected){
        int pad=-1;if(!(file>>tag>>pad)||tag!="PAD"||pad!=expected){error="Liste de pads KICKKIT invalide.";return false;}
        if(version==1){char location=0;std::string stored;if(!(file>>location>>std::quoted(stored))||(location!='E'&&location!='R'&&location!='A')){error="Chemin KICKKIT invalide.";return false;}if(location=='R'){const auto relative=std::filesystem::u8path(stored);if(!safeRelativePath(relative)){error="Chemin relatif KICKKIT invalide.";return false;}paths[pad][0]=(root/relative).lexically_normal().u8string();}else if(location=='A'){const auto absolute=std::filesystem::u8path(stored);if(!absolute.is_absolute()){error="Chemin absolu KICKKIT invalide.";return false;}paths[pad][0]=absolute.lexically_normal().u8string();}else if(!stored.empty()){error="Pad vide KICKKIT invalide.";return false;}if(!readPadSettings(file,edits[pad],error)){error="Pad "+std::to_string(pad+1)+" : "+error;return false;}}
        else{int samplePlay=0,layerPlay=0;if(!(file>>samplePlay>>layerPlay>>edits[pad].chokeGroup>>edits[pad].color)||samplePlay<0||samplePlay>2||layerPlay<0||layerPlay>2||edits[pad].chokeGroup< -1||edits[pad].chokeGroup>15||edits[pad].color<0||edits[pad].color>=PadColors){error="Comportement de pad invalide.";return false;}edits[pad].samplePlay=SamplePlayType(samplePlay);edits[pad].layerPlay=LayerPlayType(layerPlay);if(!readPadSettings(file,edits[pad],error)){error="Pad "+std::to_string(pad+1)+" : "+error;return false;}if(version>=3&&(!(file>>edits[pad].pan>>edits[pad].mute)||!std::isfinite(edits[pad].pan)||edits[pad].pan< -1||edits[pad].pan>1)){error="Mixage de pad KICKKIT invalide.";return false;}for(int expectedLayer=0;expectedLayer<PadLayers;++expectedLayer){int layer=-1,start=1,end=127;bool backwards=false;char location=0;std::string stored;if(!(file>>tag>>layer>>location>>std::quoted(stored)>>start>>end>>backwards)||tag!="LAYER"||layer!=expectedLayer||(location!='E'&&location!='R'&&location!='A')||start<1||end>127||start>end){error="Couche KICKKIT invalide.";return false;}velocityStart[pad][layer]=start;velocityEnd[pad][layer]=end;reverse[pad][layer]=backwards;if(location=='R'){const auto relative=std::filesystem::u8path(stored);if(!safeRelativePath(relative)){error="Chemin relatif KICKKIT invalide.";return false;}paths[pad][layer]=(root/relative).lexically_normal().u8string();}else if(location=='A'){const auto absolute=std::filesystem::u8path(stored);if(!absolute.is_absolute()){error="Chemin absolu KICKKIT invalide.";return false;}paths[pad][layer]=absolute.lexically_normal().u8string();}else if(!stored.empty()){error="Couche vide KICKKIT invalide.";return false;}}}
    }
    std::array<Impl::BankPad,Pads> parsed;
    for(int pad=0;pad<Pads;++pad){auto& destination=parsed[pad];destination.edit=edits[pad];for(int layer=0;layer<PadLayers;++layer){auto& item=destination.layers[layer];item.path=paths[pad][layer];item.velocityStart=velocityStart[pad][layer];item.velocityEnd=velocityEnd[pad][layer];item.reverse=reverse[pad][layer];if(item.path.empty())continue;try{item.file=decode(item.path);item.loaded=true;item.name=std::filesystem::u8path(item.path).stem().u8string();item.duration=double(item.file->getNumSamplesPerChannel())/Rate;item.waveform.assign(320,0.f);const auto& audio=item.file->samples[0];for(size_t sample=0;sample<audio.size();++sample){const size_t point=std::min<size_t>(319,sample*320/audio.size());item.waveform[point]=std::max(item.waveform[point],std::abs(audio[sample]));}}catch(const std::exception& exception){error="Kit non charge, pad "+std::to_string(pad+1)+", couche "+std::to_string(layer+1)+" : "+exception.what();return false;}}}
    std::array<std::unique_ptr<PadEffects>,Pads> effects;for(int pad=0;pad<Pads;++pad)effects[pad]=std::make_unique<PadEffects>(edits[pad]);
    std::array<std::unique_ptr<AudioFile<float>>,Pads> retired;
    {std::scoped_lock lock(impl->mutex,impl->metadataMutex,impl->audioMutex);impl->cancelPreparedProgram();const int program=impl->state.program;
        impl->state.programs[program].emptyPadColor=emptyMode;impl->state.programs[program].padOffColor=offMode;impl->state.programs[program].padOnColor=onMode;
        for(int pad=0;pad<Pads;++pad){
            impl->players[pad]->stop();impl->pendingPitch[pad].store(1000);auto& incoming=parsed[pad];incoming.activeLayer=0;retired[pad]=impl->players[pad]->replaceFile(std::move(incoming.layers[0].file));impl->loaded[pad]=incoming.layers[0].loaded;
            auto& bank=impl->banks[program][pad];bank=std::move(incoming);bank.effects.reset();const auto& primary=bank.layers[0];
            impl->state.names[pad]=primary.name;impl->state.paths[pad]=primary.path;impl->state.waveforms[pad]=primary.waveform;impl->state.durations[pad]=primary.duration;impl->state.edits[pad]=bank.edit;impl->state.active[pad]=false;impl->state.progress[pad]=0;
            impl->players[pad]->edit=bank.edit;impl->players[pad]->effects.swap(effects[pad]);impl->players[pad]->resetMix();
        }
    }
    return true;
}
bool Engine::exportCurrentPatternWav(const std::string& path,std::string& error){
    const auto before=snapshot();
    const auto& sequence=before.sequences[before.sequence];
    const uint32_t frames=uint32_t(std::max(1ll,std::llround(sequence.bars*4.*Rate*60./before.tempo)));
    const uint32_t dataBytes=frames*2*3;
    std::ofstream file(std::filesystem::u8path(path),std::ios::binary|std::ios::trunc);
    if(!file){error="Impossible de creer le fichier WAV.";return false;}
    auto write16=[&](uint32_t value){const char data[2]={char(value&255),char((value>>8)&255)};file.write(data,2);};
    auto write32=[&](uint32_t value){const char data[4]={char(value&255),char((value>>8)&255),char((value>>16)&255),char((value>>24)&255)};file.write(data,4);};
    file.write("RIFF",4);write32(36+dataBytes);file.write("WAVEfmt ",8);write32(16);
    write16(1);write16(2);write32(Rate);write32(Rate*6);write16(6);write16(24);
    file.write("data",4);write32(dataBytes);

    stop();setPlaying(true);
    std::array<float,Block*2> rendered{};
    uint32_t written=0;
    while(written<frames&&file){
        render(rendered.data());
        const int count=int(std::min<uint32_t>(Block,frames-written));
        for(int frame=0;frame<count;++frame)for(int channel=0;channel<2;++channel){
            const double sample=std::clamp(double(rendered[frame*2+channel]),-1.,1.);
            const int32_t value=int32_t(std::llround(sample*8388607.));
            const uint32_t word=static_cast<uint32_t>(value);
            const char data[3]={char(word&255),char((word>>8)&255),char((word>>16)&255)};
            file.write(data,3);
        }
        written+=count;
    }
    stop();file.flush();const bool success=bool(file)&&written==frames;file.close();
    if(!success){std::error_code ignored;std::filesystem::remove(std::filesystem::u8path(path),ignored);error="Erreur pendant l'export WAV.";return false;}
    return true;
}
bool Engine::open(const std::string& path,std::string& error){
    std::ifstream f(std::filesystem::u8path(path),std::ios::ate);if(!f||f.tellg()>4*1024*1024){error="Projet introuvable ou trop volumineux.";return false;}f.seekg(0);
    std::string tag;int version;Snapshot s;
    for(int index=0;index<Programs;++index){s.programs[index].name=index<Groups?"Drum Program "+std::string(1,char('A'+index)):"Program "+std::to_string(index+1);s.programs[index].used=index<Groups;}
    for(int index=0;index<Sequences;++index){s.sequences[index].name="Sequence "+std::to_string(index+1);s.sequences[index].program=index<Groups?index:0;}
    if(!(f>>tag>>version>>s.tempo>>s.volume)||tag!="KICKLAB"||(version<1||version>14)||!std::isfinite(s.tempo)||!std::isfinite(s.volume)||s.tempo<40||s.tempo>240||s.volume<0||s.volume>1){error="Projet KICKLAB invalide.";return false;}
    std::array<std::array<std::array<std::string,PadLayers>,Pads>,Programs> bankPaths{};std::array<std::array<PadEdit,Pads>,Programs> bankEdits{};
    std::array<std::array<std::array<int,PadLayers>,Pads>,Programs> velocityStart{},velocityEnd{};std::array<std::array<std::array<bool,PadLayers>,Pads>,Programs> reverse{};
    for(auto& program:velocityStart)for(auto& pad:program)pad.fill(1);for(auto& program:velocityEnd)for(auto& pad:program)pad.fill(127);
    if(version>=10){
        if(!(f>>s.group>>s.selectedTrack)||s.group<0||s.group>=Groups||s.selectedTrack<0||s.selectedTrack>=Pads){error="État actif invalide.";return false;}
        int programCount=0;if(!(f>>tag>>programCount)||tag!="PROGRAMS"||programCount!=Programs){error="Liste de Programs invalide.";return false;}
        for(int program=0;program<Programs;++program){int parsed=-1,type=0;bool used=false;if(!(f>>tag>>parsed>>used>>type>>std::quoted(s.programs[program].name))||tag!="PROGRAM"||parsed!=program||type<0||type>int(ProgramType::Keygroup)||s.programs[program].name.size()>64){error="Program invalide.";return false;}s.programs[program].used=used;s.programs[program].type=ProgramType(type);if(version>=14){int empty=0,off=0,on=0;if(!(f>>empty>>off>>on)||empty<0||empty>1||off<0||off>1||on<0||on>2){error="Modes de couleur de Program invalides.";return false;}s.programs[program].emptyPadColor=EmptyPadColorMode(empty);s.programs[program].padOffColor=PadOffColorMode(off);s.programs[program].padOnColor=PadOnColorMode(on);}
            for(int p=0;p<Pads;++p){int parsedPad=-1;if(!(f>>tag>>parsedPad)||tag!="PAD"||parsedPad!=p){error="Pad de Program invalide.";return false;}auto& edit=bankEdits[program][p];if(version==10){if(!(f>>std::quoted(bankPaths[program][p][0]))){error="Chemin de pad invalide.";return false;}}else{int samplePlay=0,layerPlay=0;if(!(f>>samplePlay>>layerPlay>>edit.chokeGroup>>edit.color)||samplePlay<0||samplePlay>2||layerPlay<0||layerPlay>2||edit.chokeGroup< -1||edit.chokeGroup>15||edit.color<0||edit.color>=PadColors){error="Comportement de pad invalide.";return false;}edit.samplePlay=SamplePlayType(samplePlay);edit.layerPlay=LayerPlayType(layerPlay);}if(!readPadSettings(f,edit,error)){error="Program "+std::to_string(program+1)+", pad "+std::to_string(p+1)+" : "+error;return false;}if(version>=13&&(!(f>>edit.pan>>edit.mute)||!std::isfinite(edit.pan)||edit.pan< -1||edit.pan>1)){error="Mixage de pad de Program invalide.";return false;}if(version>=11)for(int expectedLayer=0;expectedLayer<PadLayers;++expectedLayer){int layer=-1,start=1,end=127;bool backwards=false;std::string layerPath;if(!(f>>tag>>layer>>std::quoted(layerPath)>>start>>end>>backwards)||tag!="LAYER"||layer!=expectedLayer||start<1||end>127||start>end){error="Couche de Program invalide.";return false;}bankPaths[program][p][layer]=std::move(layerPath);velocityStart[program][p][layer]=start;velocityEnd[program][p][layer]=end;reverse[program][p][layer]=backwards;}}
        }
    }else if(version>=6){if(!(f>>s.group)||s.group<0||s.group>=Groups){error="Groupe actif invalide.";return false;}if(version>=7&&(!(f>>s.selectedTrack)||s.selectedTrack<0||s.selectedTrack>=Pads)){error="Piste sélectionnée invalide.";return false;}
        for(int group=0;group<Groups;++group){int parsedGroup=-1;if(!(f>>tag>>parsedGroup)||tag!="GROUP"||parsedGroup!=group){error="Banque de pads invalide.";return false;}for(int p=0;p<Pads;++p){int parsedPad=-1;if(!(f>>tag>>parsedPad>>std::quoted(bankPaths[group][p][0]))||tag!="PAD"||parsedPad!=p){error="Pad de banque invalide.";return false;}auto& e=bankEdits[group][p];if(!(f>>e.start>>e.end>>e.gain>>e.semitones>>e.fadeMs>>e.attack>>e.decay>>e.sustain>>e.release)){error="Reglages du pad invalides.";return false;}if(version>=9){if(!(f>>e.loopSync>>e.sourceBpm)){error="Synchronisation du pad invalide.";return false;}}if(!(f>>e.insertsBypass)){error="Reglages du pad invalides.";return false;}for(auto& insert:e.inserts){int effect=0,count=0;bool bypass=false;if(!(f>>effect>>bypass>>count)||effect<int(InsertEffect::None)||effect>=int(InsertEffect::Count)||count<0||count>InsertParameters||count!=insertParameterCount(InsertEffect(effect))){error="Insert invalide.";return false;}InsertState parsed;parsed.effect=InsertEffect(effect);parsed.bypass=bypass;for(int parameter=0;parameter<count;++parameter){if(!(f>>parsed.parameters[parameter])||!std::isfinite(parsed.parameters[parameter])||parsed.parameters[parameter]<0||parsed.parameters[parameter]>1){error="Parametre d'insert invalide.";return false;}}insert=parsed;}const double values[]={e.start,e.end,e.gain,e.semitones,e.fadeMs,e.attack,e.decay,e.sustain,e.release,e.sourceBpm};for(double value:values)if(!std::isfinite(value)){error="Reglages non finis.";return false;}if(e.start<0||e.end>1||e.end-e.start<.000999||e.gain<0||e.gain>2||e.semitones< -24||e.semitones>24||e.fadeMs<0||e.fadeMs>20||e.attack<0||e.attack>1||e.decay<0||e.decay>1||e.sustain<0||e.sustain>1||e.release<0||e.release>1||e.sourceBpm<40||e.sourceBpm>240){error="Reglages hors limites.";return false;}}}
    }else for(int p=0;p<Pads;++p){unsigned bits;if(!(f>>bits>>std::quoted(s.paths[p]))||bits>65535){error="Pattern invalide.";return false;}for(int k=0;k<Steps;++k)s.pattern[p][k]=(bits&(1u<<k))!=0;
        auto& e=s.edits[p];
        if(version==2){double filterOpen=1,drive=0;bool bypass=false;if(!(f>>e.start>>e.end>>e.gain>>e.semitones>>e.fadeMs>>filterOpen>>drive>>bypass)||!std::isfinite(filterOpen)||!std::isfinite(drive)||filterOpen<0||filterOpen>1||drive<0||drive>1){error="Reglages du pad invalides.";return false;}e.insertsBypass=bypass;e.inserts[0].parameters[0]=filterOpen;e.inserts[0].bypass=filterOpen>=.9999;e.inserts[1].parameters[0]=drive;e.inserts[1].bypass=drive<=.0001;}
        if(version>=3){if(!(f>>e.start>>e.end>>e.gain>>e.semitones>>e.fadeMs)){error="Reglages du pad invalides.";return false;}if(version>=4&&!(f>>e.attack>>e.decay>>e.sustain>>e.release)){error="Enveloppe du pad invalide.";return false;}if(!(f>>e.insertsBypass)){error="Reglages du pad invalides.";return false;}for(auto& insert:e.inserts){int effect=0,count=0;bool bypass=false;if(!(f>>effect>>bypass>>count)||effect<int(InsertEffect::None)||effect>=int(InsertEffect::Count)||count<0||count>InsertParameters||count!=insertParameterCount(InsertEffect(effect))){error="Insert invalide.";return false;}InsertState parsed;parsed.effect=InsertEffect(effect);parsed.bypass=bypass;for(int parameter=0;parameter<count;++parameter){if(!(f>>parsed.parameters[parameter])||!std::isfinite(parsed.parameters[parameter])||parsed.parameters[parameter]<0||parsed.parameters[parameter]>1){error="Parametre d'insert invalide.";return false;}}insert=parsed;}
        }
        const double v[]={e.start,e.end,e.gain,e.semitones,e.fadeMs,e.attack,e.decay,e.sustain,e.release};for(double x:v)if(!std::isfinite(x)){error="Reglages non finis.";return false;}
        if(e.start<0||e.end>1||e.end-e.start<.000999||e.gain<0||e.gain>2||e.semitones< -24||e.semitones>24||e.fadeMs<0||e.fadeMs>20||e.attack<0||e.attack>1||e.decay<0||e.decay>1||e.sustain<0||e.sustain>1||e.release<0||e.release>1){error="Reglages hors limites.";return false;}bankPaths[0][p][0]=s.paths[p];bankEdits[0][p]=s.edits[p];}
    if(version<5){for(int p=0;p<Pads;++p)for(int step=0;step<Steps;++step)s.sequences[0].drum[p][step]=s.pattern[p][step];}
    else {
        int count=0,current=0,view=0;if(!(f>>tag>>count>>current>>view)||tag!="SEQUENCES"||count!=Sequences||current<0||current>=Sequences){error="Liste de sequences invalide.";return false;}s.sequence=current;
        for(int expected=0;expected<Sequences;++expected){int index=0,bars=0,mode=0,source=0,root=0,program=expected<Groups?expected:0;bool read=bool(f>>tag>>index>>std::quoted(s.sequences[expected].name)>>bars>>mode>>source>>root);if(version>=10)read=read&&bool(f>>program);if(!read||tag!="SEQ"||index!=expected||s.sequences[expected].name.size()>128||bars<1||bars>MaxBars||mode<0||mode>int(PerformanceMode::Minor)||source<0||source>=Pads||root<0||root>101||program<0||program>=Programs||!s.programs[program].used){error="Sequence invalide.";return false;}auto& sequence=s.sequences[expected];sequence.bars=bars;sequence.mode=PerformanceMode(mode);sequence.sourcePad=source;sequence.rootNote=root;sequence.program=program;
            if(version>=7){int trackCount=0;if(!(f>>tag>>trackCount)||tag!="TRACKS"||trackCount!=Pads){error="Liste de pistes invalide.";return false;}for(int expectedTrack=0;expectedTrack<Pads;++expectedTrack){int parsedTrack=-1,program=0,sourcePad=0;auto& track=sequence.tracks[expectedTrack];if(!(f>>tag>>parsedTrack>>std::quoted(track.name)>>program>>sourcePad>>track.level>>track.pan>>track.mute>>track.solo>>track.armed)||tag!="TRACK"||parsedTrack!=expectedTrack||track.name.size()>128||program<0||program>int(ProgramType::Keygroup)||sourcePad<0||sourcePad>=Pads||!std::isfinite(track.level)||!std::isfinite(track.pan)||track.level<0||track.level>1||track.pan< -1||track.pan>1){error="Piste invalide.";return false;}track.program=ProgramType(program);track.sourcePad=sourcePad;}}
            if(!(f>>tag)||tag!="DRUM"){error="Pistes de sequence manquantes.";return false;}for(int p=0;p<Pads;++p){unsigned long long bits=0;if(!(f>>bits)){error="Piste de batterie invalide.";return false;}for(int step=0;step<MaxSequenceSteps;++step)sequence.drum[p][step]=(bits&(1ull<<step))!=0;}
            if(!(f>>tag)||tag!="MELO"){error="Piste melodique manquante.";return false;}for(int& note:sequence.melody)if(!(f>>note)||note< -1||note>127){error="Note melodique invalide.";return false;}if(version>=12){if(!(f>>tag)||tag!="EVENTS"){error="Données Events manquantes.";return false;}auto readEvent=[&](StepEvent& event){return bool(f>>event.velocity>>event.probability>>event.ratchet>>event.length)&&event.velocity>=1&&event.velocity<=127&&event.probability>=0&&event.probability<=100&&event.ratchet>=1&&event.ratchet<=8&&event.length>=1&&event.length<=16;};for(int expectedTrack=0;expectedTrack<Pads;++expectedTrack){int parsedTrack=-1;if(!(f>>tag>>parsedTrack)||tag!="DEVT"||parsedTrack!=expectedTrack){error="Events de piste invalides.";return false;}for(auto& event:sequence.drumEvents[expectedTrack])if(!readEvent(event)){error="Paramètres d'Event invalides.";return false;}}if(!(f>>tag)||tag!="MEVT"){error="Events mélodiques manquants.";return false;}for(auto& event:sequence.melodyEvents)if(!readEvent(event)){error="Event mélodique invalide.";return false;}}
        }
        s.viewBar=std::clamp(view,0,s.sequences[s.sequence].bars-1);if(!(f>>tag>>s.songLength)||tag!="SONG"||s.songLength<0||s.songLength>MaxSongSteps){error="Arrangement Song invalide.";return false;}for(int index=0;index<s.songLength;++index){if(!(f>>tag>>s.song[index].sequence>>s.song[index].repeats)||tag!="STEP"||s.song[index].sequence<0||s.song[index].sequence>=Sequences||s.song[index].repeats<1||s.song[index].repeats>128){error="Etape Song invalide.";return false;}}
    }
    s.queuedSequence=-1;s.queuedProgram=-1;s.queuedGroup=-1;s.songPlaying=false;s.songPosition=-1;s.songRepeat=0;s.step=-1;s.playBar=0;s.program=s.sequences[s.sequence].program;
    std::array<std::array<std::array<std::unique_ptr<AudioFile<float>>,PadLayers>,Pads>,Programs> clips;
    try{for(int program=0;program<Programs;++program)for(int p=0;p<Pads;++p)for(int layer=0;layer<PadLayers;++layer)if(!bankPaths[program][p][layer].empty())clips[program][p][layer]=decode(bankPaths[program][p][layer]);}catch(const std::exception& e){error=std::string("Projet non charge : ")+e.what();return false;}
    std::array<std::array<Impl::BankPad,Pads>,Programs> parsedBanks;
    for(int program=0;program<Programs;++program)for(int p=0;p<Pads;++p){auto& bank=parsedBanks[program][p];bank.edit=bankEdits[program][p];for(int layer=0;layer<PadLayers;++layer){auto& item=bank.layers[layer];item.path=bankPaths[program][p][layer];item.velocityStart=velocityStart[program][p][layer];item.velocityEnd=velocityEnd[program][p][layer];item.reverse=reverse[program][p][layer];item.loaded=bool(clips[program][p][layer]);if(clips[program][p][layer]){item.duration=double(clips[program][p][layer]->getNumSamplesPerChannel())/Rate;item.name=std::filesystem::u8path(item.path).stem().u8string();item.waveform.assign(320,0.f);auto& audio=clips[program][p][layer]->samples[0];for(size_t n=0;n<audio.size();++n){size_t j=std::min<size_t>(319,n*320/audio.size());item.waveform[j]=std::max(item.waveform[j],std::abs(audio[n]));}item.file=std::move(clips[program][p][layer]);}}}
    for(int p=0;p<Pads;++p){const auto& bank=parsedBanks[s.program][p];const auto& primary=bank.layers[0];s.paths[p]=primary.path;s.edits[p]=bank.edit;s.durations[p]=primary.duration;s.names[p]=primary.name;s.waveforms[p]=primary.waveform;for(int layer=0;layer<PadLayers;++layer){const auto& item=bank.layers[layer];s.layers[p][layer]={item.name,item.path,item.duration,item.velocityStart,item.velocityEnd,item.reverse,item.loaded};}}
    std::array<std::unique_ptr<PadEffects>,Pads> fx;for(int p=0;p<Pads;++p)fx[p]=std::make_unique<PadEffects>(s.edits[p]);
    std::array<std::unique_ptr<AudioFile<float>>,Pads> old;
    {std::scoped_lock lock(impl->mutex,impl->metadataMutex,impl->audioMutex);
     for(int p=0;p<Pads;++p){auto& active=parsedBanks[s.program][p];active.activeLayer=0;impl->loaded[p]=active.layers[0].loaded;old[p]=impl->players[p]->replaceFile(std::move(active.layers[0].file));impl->pendingPitch[p].store(1000);impl->players[p]->edit=s.edits[p];impl->players[p]->effects.swap(fx[p]);impl->players[p]->resetMix();}
     impl->banks=std::move(parsedBanks);std::swap(impl->state,s);impl->untilStep=0;impl->nextStep=0;}
    // Retired clips and metadata are destroyed here, on the loading thread.

    return true;
}
}

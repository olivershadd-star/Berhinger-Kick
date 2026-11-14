// SPDX-License-Identifier: AGPL-3.0-or-later
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <avrt.h>
#include <commdlg.h>
#include <shellapi.h>
#include <array>
#include <thread>
#include <atomic>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <cwchar>
#include <cwctype>
#include <fstream>
#include "engine.hpp"
#include "audio_queue.hpp"
#include "control_surface.hpp"

namespace {
std::wstring wide(const std::string& s){if(s.empty())return {};int n=MultiByteToWideChar(CP_UTF8,0,s.data(),int(s.size()),nullptr,0);std::wstring r(n,0);MultiByteToWideChar(CP_UTF8,0,s.data(),int(s.size()),r.data(),n);return r;}
std::string utf8(const std::wstring& s){if(s.empty())return {};int n=WideCharToMultiByte(CP_UTF8,0,s.data(),int(s.size()),nullptr,0,nullptr,nullptr);std::string r(n,0);WideCharToMultiByte(CP_UTF8,0,s.data(),int(s.size()),r.data(),n,nullptr,nullptr);return r;}
struct Box {int x,y,w,h;bool contains(int a,int b)const{return a>=x&&a<x+w&&b>=y&&b<y+h;} RECT rect()const{return {x,y,x+w,y+h};}};
constexpr COLORREF bg=RGB(14,17,23),panel=RGB(23,28,37),muted=RGB(144,158,178),ink=RGB(233,239,248),accent=RGB(76,217,184),line=RGB(43,53,68);
constexpr int DesignWidth=1600,DesignHeight=900;
COLORREF padColor(int i){constexpr COLORREF colors[]={RGB(255,133,103),RGB(241,184,88),RGB(76,217,184),RGB(132,161,250)};return colors[i/4];}
std::wstring number(int n){return std::to_wstring(n);}
std::wstring padName(int p){return (p<9?L"0":L"")+number(p+1);}
std::wstring noteName(int note){static const std::array<const wchar_t*,12> names={L"C",L"C#",L"D",L"D#",L"E",L"F",L"F#",L"G",L"G#",L"A",L"A#",L"B"};note=std::clamp(note,0,127);return std::wstring(names[note%12])+number(note/12-1);}
std::wstring performanceModeName(lab::PerformanceMode mode){switch(mode){case lab::PerformanceMode::Chromatic:return L"Chromatique";case lab::PerformanceMode::Major:return L"Gamme majeure";case lab::PerformanceMode::Minor:return L"Gamme mineure";default:return L"Drum";}}
const std::array<int,16> keys={'A','Z','E','R','Q','S','D','F','W','X','C','V','1','2','3','4'};
const std::array<std::wstring,16> labels={L"A",L"Z",L"E",L"R",L"Q",L"S",L"D",L"F",L"W",L"X",L"C",L"V",L"1",L"2",L"3",L"4"};
constexpr UINT SurfaceMessage=WM_APP+42;
constexpr UINT MidiMessage=WM_APP+43;

struct BrowserEntry {
    std::filesystem::path path;
    bool directory=false;
};

struct BrowserState {
    int selected=0;
    int scroll=0;
    // Mirrors the original KICK Browser: 0=OFF, 1=AUTO, 2=MAN.
    int auditionMode=1;
    std::filesystem::path directory;
    std::vector<BrowserEntry> entries;
};

class AudioOutput {
public:
    explicit AudioOutput(lab::Engine& e):engine(e){}
    ~AudioOutput(){stop();}
    std::atomic<unsigned> error{0},renderPermille{0},lateBlocks{0};std::atomic<bool> running{false};
    void start(UINT id){stop();error=0;renderPermille=0;lateBlocks=0;alive=true;thread=std::thread([this,id]{run(id);});}
    void stop(){alive=false;if(thread.joinable())thread.join();running=false;}
private:
    lab::Engine& engine;std::thread thread;std::atomic<bool> alive{false};
    void run(UINT id){
        // Windows scheduling is not a sample clock. Wait for the driver instead
        // of Sleep(1), which may take a full scheduler tick under UI load.
        HANDLE ready=CreateEventW(nullptr,FALSE,FALSE,nullptr);
        if(!ready){error=MMSYSERR_NOMEM;return;}
        DWORD taskIndex=0;
        HANDLE task=AvSetMmThreadCharacteristicsW(L"Pro Audio",&taskIndex);
        if(task)AvSetMmThreadPriority(task,AVRT_PRIORITY_HIGH);
        constexpr int Frames=lab::Block*4, Buffers=4;
        WAVEFORMATEX fmt{};fmt.wFormatTag=WAVE_FORMAT_PCM;fmt.nChannels=2;fmt.nSamplesPerSec=lab::Rate;fmt.wBitsPerSample=16;fmt.nBlockAlign=4;fmt.nAvgBytesPerSec=lab::Rate*4;
        HWAVEOUT device{};MMRESULT result=waveOutOpen(&device,id,&fmt,reinterpret_cast<DWORD_PTR>(ready),0,CALLBACK_EVENT);
        if(result){error=result;if(task)AvRevertMmThreadCharacteristics(task);CloseHandle(ready);return;}
        std::array<WAVEHDR,Buffers> headers{};
        std::array<std::array<short,Frames*2>,Buffers> data{};
        std::array<bool,Buffers> prepared{};
        auto fill=[&](int b){
            float block[lab::Block*2];
            for(int offset=0;offset<Frames;offset+=lab::Block){
                const auto started=std::chrono::steady_clock::now();
                engine.render(block);
                const auto elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-started).count();const auto load=unsigned(std::min(9999.,elapsed/(double(lab::Block)/lab::Rate)*1000.));renderPermille=(renderPermille.load()*7+load)/8;if(load>1000)++lateBlocks;
                for(int i=0;i<lab::Block*2;++i)data[b][offset*2+i]=short(std::lround(block[i]*32767));
            }
        };
        // Prime the entire queue before playback starts.
        result=waveOutPause(device);
        if(result){error=result;alive=false;}
        for(int b=0;b<Buffers&&alive;++b){
            headers[b].lpData=reinterpret_cast<char*>(data[b].data());headers[b].dwBufferLength=sizeof(data[b]);
            result=waveOutPrepareHeader(device,&headers[b],sizeof(WAVEHDR));
            if(result){error=result;alive=false;break;}prepared[b]=true;
            fill(b);result=waveOutWrite(device,&headers[b],sizeof(WAVEHDR));
            if(result){error=result;alive=false;break;}
        }
        if(alive){result=waveOutRestart(device);if(result){error=result;alive=false;}}
        running=alive.load();
        // Refill in chronological order, including when multiple buffers finish
        // across the end of the ring during a single wakeup.
        lab::AudioQueue<Buffers> queue;
        while(alive){
            if(!queue.refill([&](std::size_t index){return alive&&(headers[index].dwFlags&WHDR_DONE);},
                [&](std::size_t index){fill(int(index));result=waveOutWrite(device,&headers[index],sizeof(WAVEHDR));if(result){error=result;return false;}return true;}))alive=false;
            if(alive)WaitForSingleObject(ready,50);
        }
        waveOutReset(device);
        for(int b=0;b<Buffers;++b)if(prepared[b])waveOutUnprepareHeader(device,&headers[b],sizeof(WAVEHDR));
        waveOutClose(device);running=false;
        if(task)AvRevertMmThreadCharacteristics(task);
        CloseHandle(ready);
    }
};

class App {
public:
    lab::Engine engine;lab::SurfaceState surface;AudioOutput audio{engine};HWND window{};std::filesystem::path base,library,midiProfile,portableMidiProfile,midiDeviceSettings,midiProfileSource;
    std::array<BrowserState,lab::SurfaceDisplays> browsers{};
    int selected=0,tab=0;bool smoke=false;UINT audioId=WAVE_MAPPER;HMIDIIN midi{};int midiId=-1;
    int songChoice=0,songRepeats=1,songSelected=0;
    std::array<lab::MidiCcBinding,lab::SurfaceMidiQLinks> midiQlinks{};std::array<lab::MidiPadBinding,lab::SurfaceMidiPads> midiPads{};std::array<lab::MidiCommandBinding,lab::MidiCommandCount> midiCommands{};int midiLearnTarget=-1,midiLearnPad=-1,midiLearnCommand=-1;unsigned midiLastPacked=0;unsigned midiMessageCount=0;bool midiProfileLoaded=false;double meterPeak=0;std::wstring preferredMidiDevice;
    std::wstring status=L"Clique sur un pad pour jouer. Clic droit pour charger un WAV.";
    std::array<ULONGLONG,16> flashed{};int mouseX=-1,mouseY=-1;int width=DesignWidth,height=DesignHeight;
    // Physical layout state. 0=Pad, 1=Keyboard, 2=Chords, 3=Step.
    int padMode=0,padGroup=0,stepTrack=0,mixerBank=0,mixerParameter=0;bool selectLatch=false,soloLatch=false,muteLatch=false,recordArmed=false;int recordQuantize=16;
    App(){
        wchar_t path[32768];DWORD n=GetModuleFileNameW(nullptr,path,32768);base=std::filesystem::path(std::wstring(path,n)).parent_path();library=base/L"samples";portableMidiProfile=base/L"profiles"/L"MaschinePlus.kickmidi";
        wchar_t local[32768];DWORD localSize=GetEnvironmentVariableW(L"LOCALAPPDATA",local,32768);const auto settingsRoot=localSize&&localSize<32768?std::filesystem::path(std::wstring(local,localSize))/L"KICKLab":base;
        midiProfile=settingsRoot/L"profiles"/L"MaschinePlus.kickmidi";midiDeviceSettings=settingsRoot/L"midi-device.txt";
        std::error_code ec;std::filesystem::create_directories(library,ec);for(auto& browser:browsers)browser.directory=library;std::filesystem::create_directories(midiProfile.parent_path(),ec);std::filesystem::create_directories(portableMidiProfile.parent_path(),ec);loadMidiDevicePreference();loadMidiProfile();SetCurrentDirectoryW(base.c_str());
    }
    ~App(){if(midiProfileLoaded||midiMessageCount)saveMidiProfile();closeMidi();audio.stop();}
    void initialise(HWND h){window=h;int loaded=0;std::string error;for(int p=0;p<16;++p){const std::array<std::wstring,16> files={L"01-Kick.wav",L"02-Snare.wav",L"03-Closed-Hat.wav",L"04-Open-Hat.wav",L"05-Clap.wav",L"06-Low-Tom.wav",L"07-Mid-Tom.wav",L"08-High-Tom.wav",L"09-Rim.wav",L"10-Shaker.wav",L"11-Claves.wav",L"12-Cowbell.wav",L"13-Bass-C.wav",L"14-Bass-E.wav",L"15-Bass-G.wav",L"16-Chime.wav"};auto path=base/L"demo-kit"/files[p];if(engine.load(p,path.u8string(),error))++loaded;}if(loaded==16)status=L"Kit chargé. Pattern vide ; le bouton Démo charge le pattern d'exemple.";else status=L"Kit de démonstration incomplet : "+number(loaded)+L"/16 fichiers.";audio.start(audioId);if(!autoConnectMidi())status+=(midiProfileLoaded?L" · Profil MIDI chargé, entrée MIDI déconnectée.":L" · Aucun profil MIDI chargé.");SetTimer(h,1,33,nullptr);DragAcceptFiles(h,TRUE);}
    void closeMidi(){if(midi){midiInStop(midi);midiInReset(midi);midiInClose(midi);midi=nullptr;}midiId=-1;}
    static void CALLBACK midiCallback(HMIDIIN,UINT message,DWORD_PTR instance,DWORD_PTR packed,DWORD_PTR){if(message==MIM_DATA){auto* self=reinterpret_cast<App*>(instance);PostMessageW(self->window,MidiMessage,static_cast<WPARAM>(packed&0xffffff),0);}}
    std::wstring midiDeviceName(UINT id)const{MIDIINCAPSW cap{};return midiInGetDevCapsW(id,&cap,sizeof(cap))==MMSYSERR_NOERROR?std::wstring(cap.szPname):std::wstring();}
    void loadMidiDevicePreference(){std::ifstream file(midiDeviceSettings.u8string());std::string name;if(file&&std::getline(file,name))preferredMidiDevice=wide(name);}
    bool saveMidiDevicePreference(){std::error_code ec;std::filesystem::create_directories(midiDeviceSettings.parent_path(),ec);if(ec)return false;std::ofstream file(midiDeviceSettings.u8string(),std::ios::trunc);if(!file)return false;file<<utf8(preferredMidiDevice);return bool(file);}
    bool openMidiDevice(UINT id,bool remember){
        closeMidi();MMRESULT result=midiInOpen(&midi,id,reinterpret_cast<DWORD_PTR>(&midiCallback),reinterpret_cast<DWORD_PTR>(this),CALLBACK_FUNCTION);if(result!=MMSYSERR_NOERROR){midi=nullptr;status=L"Impossible d'ouvrir cette entrée MIDI.";return false;}
        result=midiInStart(midi);if(result!=MMSYSERR_NOERROR){closeMidi();status=L"Entrée MIDI ouverte, mais impossible de la démarrer.";return false;}midiId=int(id);const auto name=midiDeviceName(id);if(remember){preferredMidiDevice=name;saveMidiDevicePreference();}status=L"MIDI connecté automatiquement : "+name+(midiProfileLoaded?L" · profil d'assignations chargé.":L" · aucun profil d'assignations.");return true;
    }
    bool autoConnectMidi(){
        int preferred=-1,virtualMaschine=-1,onlyMaschine=-1,maschineCount=0;for(UINT id=0;id<midiInGetNumDevs();++id){const auto name=midiDeviceName(id);if(!preferredMidiDevice.empty()&&CompareStringOrdinal(name.c_str(),-1,preferredMidiDevice.c_str(),-1,TRUE)==CSTR_EQUAL)preferred=int(id);std::wstring lower=name;std::transform(lower.begin(),lower.end(),lower.begin(),[](wchar_t c){return wchar_t(std::towlower(c));});if(lower.find(L"maschine")!=std::wstring::npos){onlyMaschine=int(id);++maschineCount;if(lower.find(L"virtual")!=std::wstring::npos)virtualMaschine=int(id);}}
        const int choice=preferred>=0?preferred:preferredMidiDevice.empty()?(virtualMaschine>=0?virtualMaschine:(maschineCount==1?onlyMaschine:-1)):-1;return choice>=0&&openMidiDevice(UINT(choice),true);
    }
    bool loadMidiProfileFile(const std::filesystem::path& source){
        std::ifstream file(source.u8string());if(!file)return false;
        std::string signature;int version=0;file>>signature>>version;if(signature!="KICKMIDI"||(version<1||version>5))return false;
        std::array<lab::MidiCcBinding,lab::SurfaceMidiQLinks> loadedQlinks{};std::array<lab::MidiPadBinding,lab::SurfaceMidiPads> loadedPads{};std::array<lab::MidiCommandBinding,lab::MidiCommandCount> loadedCommands{};
        const int storedQlinks=version>=4?lab::SurfaceMidiQLinks:8;for(int expected=0;expected<storedQlinks;++expected){char record='Q';int index=-1,channel=-1,cc=-1,mode=0;if(version>=2)file>>record;if(!(file>>index>>channel>>cc>>mode)||record!='Q'||index!=expected||channel< -1||channel>15||cc< -1||cc>127||mode<0||mode>2)return false;loadedQlinks[index].channel=channel;loadedQlinks[index].cc=cc;loadedQlinks[index].mode=static_cast<lab::MidiCcMode>(mode);}
        const int storedPads=version>=4?lab::SurfaceMidiPads:version==3?lab::SurfacePads:0;for(int expected=0;expected<storedPads;++expected){char record=0;int index=-1,channel=-1,note=-1;if(!(file>>record>>index>>channel>>note)||record!='P'||index!=expected||channel< -1||channel>15||note< -1||note>127)return false;loadedPads[index]={channel,note};}
        if(version>=2){const int storedCommands=version<3?int(lab::MidiCommandTarget::Turn)+1:version<4?int(lab::MidiCommandTarget::Back)+1:version<5?int(lab::MidiCommandTarget::Soft9):lab::MidiCommandCount;for(int expected=0;expected<storedCommands;++expected){char record=0;int index=-1,kind=0,channel=-1,numberValue=-1,mode=0;if(!(file>>record>>index>>kind>>channel>>numberValue>>mode)||record!='C'||index!=expected||kind<0||kind>2||channel< -1||channel>15||numberValue< -1||numberValue>127||mode<0||mode>2)return false;auto& binding=loadedCommands[index];binding.kind=static_cast<lab::MidiMessageKind>(kind);binding.channel=channel;binding.number=numberValue;binding.mode=static_cast<lab::MidiCcMode>(mode);}}
        if(version<4&&loadedCommands[int(lab::MidiCommandTarget::Turn)].kind!=lab::MidiMessageKind::None)loadedCommands[int(lab::MidiCommandTarget::Turn)].mode=lab::MidiCcMode::Relative1;
        midiQlinks=loadedQlinks;midiPads=loadedPads;midiCommands=loadedCommands;midiProfileLoaded=true;midiProfileSource=source;return true;
    }
    bool writeMidiProfile(const std::filesystem::path& destination){
        std::error_code ec;std::filesystem::create_directories(destination.parent_path(),ec);if(ec)return false;
        auto temporary=destination;temporary+=L".tmp";std::ofstream file(temporary.u8string(),std::ios::trunc);if(!file)return false;
        file<<"KICKMIDI 5\n";for(int index=0;index<int(midiQlinks.size());++index){const auto& binding=midiQlinks[index];file<<"Q "<<index<<' '<<binding.channel<<' '<<binding.cc<<' '<<int(binding.mode)<<'\n';}for(int index=0;index<int(midiPads.size());++index){const auto& binding=midiPads[index];file<<"P "<<index<<' '<<binding.channel<<' '<<binding.note<<'\n';}for(int index=0;index<lab::MidiCommandCount;++index){const auto& binding=midiCommands[index];file<<"C "<<index<<' '<<int(binding.kind)<<' '<<binding.channel<<' '<<binding.number<<' '<<int(binding.mode)<<'\n';}file.close();if(!file)return false;
        return MoveFileExW(temporary.c_str(),destination.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=FALSE;
    }
    bool saveMidiProfile(){const bool portable=writeMidiProfile(portableMidiProfile);const bool persistent=midiProfile==portableMidiProfile?portable:writeMidiProfile(midiProfile);if(portable||persistent){midiProfileLoaded=true;midiProfileSource=portable?portableMidiProfile:midiProfile;return true;}return false;}
    bool loadMidiProfile(){
        // The file beside the executable is authoritative for a portable build.
        // AppData remains a fallback/backup when the application directory is read-only.
        if(loadMidiProfileFile(portableMidiProfile)){if(midiProfile!=portableMidiProfile)writeMidiProfile(midiProfile);return true;}
        if(portableMidiProfile!=midiProfile&&loadMidiProfileFile(midiProfile)){writeMidiProfile(portableMidiProfile);return true;}
        std::filesystem::path newest;std::filesystem::file_time_type newestTime{};std::error_code ec;const auto parent=base.parent_path();
        for(std::filesystem::directory_iterator it(parent,ec),end;!ec&&it!=end;it.increment(ec)){if(!it->is_directory(ec))continue;auto candidate=it->path()/L"profiles"/L"MaschinePlus.kickmidi";if(candidate==portableMidiProfile||!std::filesystem::is_regular_file(candidate,ec))continue;auto changed=std::filesystem::last_write_time(candidate,ec);if(!ec&&(newest.empty()||changed>newestTime)){newest=candidate;newestTime=changed;}}
        if(!newest.empty()&&loadMidiProfileFile(newest)){saveMidiProfile();return true;}return false;
    }
    void dispatchMidiCommand(int target,bool pressed){
        using T=lab::MidiCommandTarget;switch(static_cast<T>(target)){
        case T::PlayPause:if(pressed)dispatch({lab::SurfaceAction::PlayPause});break;
        case T::Stop:if(pressed)dispatch({lab::SurfaceAction::Stop});break;
        case T::Shift:dispatch({lab::SurfaceAction::Shift,0,0,0,pressed});break;
        case T::Enter:if(pressed)dispatch({lab::SurfaceAction::CenterPush});break;
        case T::Up:if(pressed)dispatch({lab::SurfaceAction::CenterUp});break;
        case T::Down:if(pressed)dispatch({lab::SurfaceAction::CenterDown});break;
        case T::Left:if(pressed)dispatch({lab::SurfaceAction::CenterLeft});break;
        case T::Right:if(pressed)dispatch({lab::SurfaceAction::CenterRight});break;
        case T::ScreenA:if(pressed)dispatch({lab::SurfaceAction::PagePrevious});break;
        case T::ScreenB:if(pressed)dispatch({lab::SurfaceAction::PageNext});break;
        case T::Back:if(pressed)dispatch({lab::SurfaceAction::Back});break;
        case T::Soft1:case T::Soft2:case T::Soft3:case T::Soft4:case T::Soft5:case T::Soft6:case T::Soft7:case T::Soft8:if(pressed){const int soft=target-int(T::Soft1);activateSurfaceSoft(soft/4,soft%4,0);}break;
        case T::Soft9:case T::Soft10:case T::Soft11:case T::Soft12:case T::Soft13:case T::Soft14:case T::Soft15:case T::Soft16:if(pressed){const int soft=target-int(T::Soft9);activateSurfaceSoft(soft/4,soft%4,1);}break;
        case T::GroupA:case T::GroupB:case T::GroupC:case T::GroupD:case T::GroupE:case T::GroupF:case T::GroupG:case T::GroupH:if(pressed){const int group=target-int(T::GroupA);engine.setGroup(group);padGroup=engine.snapshot().group;selectPad(0);stepTrack=0;status=L"MIDI — groupe "+std::wstring(1,wchar_t(L'A'+group))+L".";}break;
        case T::PadMode:if(pressed){padMode=0;engine.setTrackProgram(stepTrack,lab::ProgramType::Drum);engine.setPerformanceMode(lab::PerformanceMode::Drum);status=L"MIDI — Pad Mode.";}break;
        case T::Keyboard:if(pressed){padMode=1;selectTrack(selected);engine.setTrackProgram(stepTrack,lab::ProgramType::Keygroup);engine.setMelodicSourcePad(selected);engine.setPerformanceMode(lab::PerformanceMode::Chromatic);status=L"MIDI — Keyboard : joue une hauteur puis passe sur Step pour la séquencer.";}break;
        case T::Chords:if(pressed)status=L"MIDI — Chords reçu ; moteur d'accords pas encore porté.";break;
        case T::Step:if(pressed)enterStepMode();break;
        case T::Pattern:if(pressed)showSurfaceNextSequence(surface.focusedDisplay);break;
        case T::Select:selectLatch=pressed;if(pressed){soloLatch=muteLatch=false;selectTrack(selected);status=L"MIDI — Select maintenu ; frappe un pad pour choisir la piste Step.";}break;
        case T::Solo:if(pressed){soloLatch=!soloLatch;muteLatch=selectLatch=false;status=soloLatch?L"MIDI — Solo actif ; frappe un pad pour isoler une piste.":L"MIDI — Solo relâché.";}break;
        case T::Mute:if(pressed){muteLatch=!muteLatch;soloLatch=selectLatch=false;status=muteLatch?L"MIDI — Mute actif ; frappe un pad pour couper une piste.":L"MIDI — Mute relâché.";}break;
        case T::Perform:if(pressed)showSurfaceInserts(surface.focusedDisplay);break;
        case T::Notes:if(pressed){padMode=1;showSurfacePadPerform(surface.focusedDisplay);}break;
        case T::Channel:if(pressed){if(surface.shift)showSurfaceMidiControl(surface.focusedDisplay);else showSurfaceMixer(stepTrack/8);}break;
        case T::Plugin:if(pressed)showSurfaceInserts(surface.focusedDisplay);break;
        case T::Ideas:if(pressed)showSurfaceSong(surface.focusedDisplay);break;
        case T::Mixer:if(pressed)showSurfaceMixer(stepTrack/8);break;
        case T::Browser:if(pressed)showSurfaceBrowser(surface.focusedDisplay);break;
        case T::Sampling:if(pressed){tab=1;status=L"Sampler / Sample Editor KICK.";}break;
        case T::FileSave:if(pressed){if(surface.shift)project(true);else fileMenu();}break;
        case T::Settings:if(pressed)settingsMenu();break;
        case T::Follow:if(pressed)cycleRecordQuantize();break;
        case T::Record:if(pressed)toggleRecord();break;
        case T::Pitch:if(pressed)showSurfaceProgram(surface.focusedDisplay);break;
        case T::Restart:case T::Erase:case T::Tap:
        case T::FixedVelocity:case T::Scene:case T::Events:case T::Variation:case T::Duplicate:
        case T::Volume:case T::Swing:case T::Tempo:case T::NoteRepeat:case T::Lock:case T::Mod:
        case T::Auto:case T::Macro:if(pressed)status=L"MIDI — "+midiCommandLabels[target]+L" reçu ; module KICK pas encore porté.";break;
        default:break;
        }
    }
    void midiData(unsigned packed){
        midiLastPacked=packed&0xffffff;++midiMessageCount;
        const int midiStatus=packed&255,type=midiStatus&0xf0,channel=midiStatus&15,data1=(packed>>8)&127,data2=(packed>>16)&127;
        if(midiLearnCommand>=0&&midiLearnCommand<lab::MidiCommandCount){const bool note=type==0x90&&data2>0,cc=type==0xb0;if(note||cc){if(midiLearnCommand==int(lab::MidiCommandTarget::Turn)&&!cc){status=L"MIDI Learn rotation : utilise un encodeur envoyant un CC.";return;}auto& binding=midiCommands[midiLearnCommand];binding.kind=cc?lab::MidiMessageKind::ControlChange:lab::MidiMessageKind::Note;binding.channel=channel;binding.number=data1;binding.lastValue=-1;if(midiLearnCommand==int(lab::MidiCommandTarget::Turn))binding.mode=lab::MidiCcMode::Relative1;const int learned=midiLearnCommand;midiLearnCommand=-1;const bool saved=saveMidiProfile();status=L"MIDI Learn : commande "+number(learned+1)+L" assignée à "+(cc?L"CC ":L"note ")+number(data1)+(saved?L" · profil sauvegardé.":L" · sauvegarde impossible.");return;}}
        if(type==0x90&&data2>0&&midiLearnPad>=0&&midiLearnPad<int(midiPads.size())){const int learned=midiLearnPad;for(int pad=0;pad<int(midiPads.size());++pad)if(pad!=learned&&midiPads[pad].matches(channel,data1))midiPads[pad]={};midiPads[learned]={channel,data1};midiLearnPad=-1;const bool saved=saveMidiProfile();const std::wstring padLabel=std::wstring(1,wchar_t(L'A'+learned/lab::SurfacePads))+number(learned%lab::SurfacePads+1);status=L"MIDI Learn : note "+number(data1)+L" canal "+number(channel+1)+L" assignée au pad "+padLabel+(saved?L" · profil sauvegardé.":L" · sauvegarde impossible.");return;}
        if(type==0xb0&&midiLearnTarget>=0&&midiLearnTarget<int(midiQlinks.size())){
            auto& binding=midiQlinks[midiLearnTarget];binding.channel=channel;binding.cc=data1;
            const bool saved=saveMidiProfile();status=L"MIDI Learn : CC "+number(data1)+L" canal "+number(channel+1)+L" assigné à P"+number(midiLearnTarget+1)+(saved?L" · profil Maschine+ sauvegardé.":L" · impossible de sauvegarder le profil.");
            midiLearnTarget=-1;return;
        }
        if(type==0x90||type==0x80){const bool pressed=type==0x90&&data2>0;bool matched=false;for(int target=0;target<int(midiPads.size());++target)if(midiPads[target].matches(channel,data1)){const int group=target/lab::SurfacePads,pad=target%lab::SurfacePads;if(pressed&&group!=padGroup){engine.setGroup(group);padGroup=engine.snapshot().group;selectPad(0);stepTrack=0;}if(group==padGroup)dispatch({lab::SurfaceAction::PadPress,pad,0,0,pressed});matched=true;break;}const int fallback=data1-36;if(!matched&&fallback>=0&&fallback<lab::SurfacePads&&midiPads[padGroup*lab::SurfacePads+fallback].note<0)dispatch({lab::SurfaceAction::PadPress,fallback,0,0,pressed});}
        if(type==0x90||type==0x80){for(int target=0;target<lab::MidiCommandCount;++target){auto& binding=midiCommands[target];if(binding.kind!=lab::MidiMessageKind::Note||binding.channel!=channel||binding.number!=data1)continue;const bool pressed=type==0x90&&data2>0;if(pressed||binding.lastValue>0)dispatchMidiCommand(target,pressed);binding.lastValue=pressed?data2:0;}}
        if(type==0xb0){for(int target=0;target<lab::MidiCommandCount;++target){auto& binding=midiCommands[target];if(binding.kind!=lab::MidiMessageKind::ControlChange||binding.channel!=channel||binding.number!=data1)continue;if(target==int(lab::MidiCommandTarget::Turn)){int delta=binding.mode==lab::MidiCcMode::Absolute?lab::midiAbsoluteDelta(binding.lastValue,data2):lab::midiCcDelta(binding.mode,data2);binding.lastValue=data2;if(delta)dispatch({lab::SurfaceAction::CenterTurn,0,delta});}else{const bool pressed=data2>0;if(pressed!=(binding.lastValue>0))dispatchMidiCommand(target,pressed);binding.lastValue=data2;}}}
        if(type!=0xb0)return;
        for(int target=0;target<int(midiQlinks.size());++target){
            const auto& binding=midiQlinks[target];if(binding.channel!=channel||binding.cc!=data1)continue;
            const int bank=target/8,logical=target%8,display=logical/4,knob=logical%4;if(surface.displays[display].module==lab::DisplayModule::Mixer&&mixerBank!=bank)showSurfaceMixer(bank);
            if(binding.mode==lab::MidiCcMode::Absolute)dispatch({lab::SurfaceAction::QLinkSet,knob,data2,display});
            else {int delta=lab::midiCcDelta(binding.mode,data2);if(delta)dispatch({lab::SurfaceAction::QLinkTurn,knob,delta,display});}
        }
    }
    void midiMenu(){HMENU menu=CreatePopupMenu();AppendMenuW(menu,MF_STRING|(midiId<0?MF_CHECKED:0),500,L"Désactivé");for(UINT i=0;i<midiInGetNumDevs();++i)AppendMenuW(menu,MF_STRING|(int(i)==midiId?MF_CHECKED:0),501+i,midiDeviceName(i).c_str());POINT p;GetCursorPos(&p);int choice=TrackPopupMenu(menu,TPM_RETURNCMD|TPM_NONOTIFY,p.x,p.y,0,window,nullptr);DestroyMenu(menu);if(!choice)return;if(choice>500)openMidiDevice(UINT(choice-501),true);else{closeMidi();preferredMidiDevice.clear();saveMidiDevicePreference();status=L"MIDI désactivé.";}}
    void audioMenu(){HMENU menu=CreatePopupMenu();AppendMenuW(menu,MF_STRING|(audioId==WAVE_MAPPER?MF_CHECKED:0),600,L"Sortie Windows par défaut");for(UINT i=0;i<waveOutGetNumDevs();++i){WAVEOUTCAPSW cap{};waveOutGetDevCapsW(i,&cap,sizeof(cap));AppendMenuW(menu,MF_STRING|(i==audioId?MF_CHECKED:0),601+i,cap.szPname);}POINT p;GetCursorPos(&p);int choice=TrackPopupMenu(menu,TPM_RETURNCMD|TPM_NONOTIFY,p.x,p.y,0,window,nullptr);DestroyMenu(menu);if(choice){audioId=choice==600?WAVE_MAPPER:UINT(choice-601);audio.start(audioId);status=L"Changement de sortie audio...";}}
    std::filesystem::path chooseMidiProfile(){std::vector<wchar_t> name(32768,0);OPENFILENAMEW dialog{};dialog.lStructSize=sizeof(dialog);dialog.hwndOwner=window;dialog.lpstrFile=name.data();dialog.nMaxFile=DWORD(name.size());dialog.lpstrFilter=L"Profil MIDI KICK (*.kickmidi)\0*.kickmidi\0Tous les fichiers\0*.*\0\0";dialog.lpstrDefExt=L"kickmidi";dialog.Flags=OFN_EXPLORER|OFN_NOCHANGEDIR|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;return GetOpenFileNameW(&dialog)?std::filesystem::path(name.data()):std::filesystem::path();}
    void importMidiProfile(){const auto source=chooseMidiProfile();if(source.empty())return;if(!loadMidiProfileFile(source)){MessageBoxW(window,L"Ce fichier n'est pas un profil KICKMIDI valide (versions 1 à 5).",L"Importer le profil MIDI",MB_OK|MB_ICONINFORMATION);return;}const bool saved=saveMidiProfile();status=L"Profil MIDI importé : "+source.filename().wstring()+(saved?L" · conservé pour les prochains lancements.":L" · chargé, mais copie permanente impossible.");}
    void settingsMenu(){HMENU menu=CreatePopupMenu();AppendMenuW(menu,MF_STRING,900,L"Sortie audio Windows...");AppendMenuW(menu,MF_STRING,901,L"Entrée MIDI / Maschine+...");AppendMenuW(menu,MF_SEPARATOR,0,nullptr);AppendMenuW(menu,MF_STRING,902,L"Sauvegarder le profil MIDI maintenant");AppendMenuW(menu,MF_STRING,903,L"Ouvrir le dossier portable des profils MIDI");AppendMenuW(menu,MF_STRING,904,L"Importer un ancien profil MIDI...");AppendMenuW(menu,MF_STRING,905,L"Ouvrir la sauvegarde AppData");POINT p;GetCursorPos(&p);int choice=TrackPopupMenu(menu,TPM_RETURNCMD|TPM_NONOTIFY,p.x,p.y,0,window,nullptr);DestroyMenu(menu);if(choice==900)audioMenu();else if(choice==901)midiMenu();else if(choice==902){const bool saved=saveMidiProfile();status=saved?L"Profil MIDI sauvegardé : "+portableMidiProfile.wstring():L"Échec de sauvegarde MIDI dans le dossier portable et AppData.";}else if(choice==903)ShellExecuteW(window,L"open",portableMidiProfile.parent_path().c_str(),nullptr,nullptr,SW_SHOWNORMAL);else if(choice==904)importMidiProfile();else if(choice==905)ShellExecuteW(window,L"open",midiProfile.parent_path().c_str(),nullptr,nullptr,SW_SHOWNORMAL);}
    std::wstring choose(bool save,bool project,const std::filesystem::path& initial={},const std::wstring& suggested={}){std::vector<wchar_t> name(32768,0);if(!suggested.empty())std::copy_n(suggested.c_str(),std::min(suggested.size(),name.size()-1),name.data());auto initialText=initial.wstring();OPENFILENAMEW dialog{};dialog.lStructSize=sizeof(dialog);dialog.hwndOwner=window;dialog.lpstrFile=name.data();dialog.nMaxFile=DWORD(name.size());dialog.lpstrFilter=project?L"Projet KICK Lab (*.kicklab)\0*.kicklab\0\0":L"Audio WAV (*.wav)\0*.wav\0\0";dialog.lpstrDefExt=project?L"kicklab":L"wav";dialog.lpstrInitialDir=initialText.empty()?nullptr:initialText.c_str();dialog.Flags=OFN_EXPLORER|OFN_NOCHANGEDIR|OFN_PATHMUSTEXIST|(save?OFN_OVERWRITEPROMPT:OFN_FILEMUSTEXIST);if(save?GetSaveFileNameW(&dialog):GetOpenFileNameW(&dialog))return name.data();return {};}
    void load(const std::wstring& path){if(path.empty())return;std::string error;if(engine.load(selected,utf8(path),error)){status=L"WAV chargé sur le pad "+padName(selected)+L". Conversion à 48 kHz si nécessaire.";play(selected);}else MessageBoxW(window,wide(error).c_str(),L"Chargement WAV",MB_OK|MB_ICONINFORMATION);}
    void sampleMenu(){HMENU menu=CreatePopupMenu();AppendMenuW(menu,MF_STRING,700,L"Factory Sample Library");AppendMenuW(menu,MF_STRING,701,L"Importer depuis l'ordinateur");AppendMenuW(menu,MF_SEPARATOR,0,nullptr);AppendMenuW(menu,MF_STRING,702,L"Ouvrir le dossier de la bibliothèque");POINT p;GetCursorPos(&p);int choice=TrackPopupMenu(menu,TPM_RETURNCMD|TPM_NONOTIFY,p.x,p.y,0,window,nullptr);DestroyMenu(menu);if(choice==700)load(choose(false,false,library));else if(choice==701)load(choose(false,false));else if(choice==702)ShellExecuteW(window,L"open",library.c_str(),nullptr,nullptr,SW_SHOWNORMAL);}
    void savePadToLibrary(){auto s=engine.snapshot();if(s.paths[selected].empty()){MessageBoxW(window,L"Charge d'abord un WAV sur ce pad.",L"Factory Sample Library",MB_OK|MB_ICONINFORMATION);return;}auto source=std::filesystem::u8path(s.paths[selected]);auto folder=browsers[surface.focusedDisplay].directory;if(folder.empty()||!std::filesystem::is_directory(folder))folder=library;auto destinationText=choose(true,false,folder,source.filename().wstring());if(destinationText.empty())return;auto destination=std::filesystem::path(destinationText);std::error_code ec;if(std::filesystem::exists(destination,ec)&&std::filesystem::equivalent(source,destination,ec)){status=L"Ce WAV est déjà dans la bibliothèque : "+destination.filename().wstring();return;}ec.clear();std::filesystem::copy_file(source,destination,std::filesystem::copy_options::overwrite_existing,ec);if(ec){MessageBoxW(window,(L"Copie impossible : "+wide(ec.message())).c_str(),L"Factory Sample Library",MB_OK|MB_ICONINFORMATION);return;}std::string loadError;if(engine.load(selected,destination.u8string(),loadError,true))status=L"WAV ajouté à la bibliothèque : "+destination.filename().wstring();else status=L"WAV copié, mais impossible de rattacher le pad : "+wide(loadError);}
    void project(bool save){auto p=choose(save,true);if(p.empty())return;std::string error;bool ok=save?engine.save(utf8(p),error):engine.open(utf8(p),error);if(!ok)MessageBoxW(window,wide(error).c_str(),L"Projet KICK Lab",MB_OK|MB_ICONINFORMATION);else{if(!save){auto snapshot=engine.snapshot();padGroup=snapshot.group;selectTrack(snapshot.selectedTrack);}auto name=std::filesystem::path(p).filename().wstring();status=(save?L"Projet enregistré : ":L"Projet chargé : ")+name;}}
    void fileMenu(){HMENU menu=CreatePopupMenu();AppendMenuW(menu,MF_STRING,910,L"Ouvrir un projet KICK...");AppendMenuW(menu,MF_STRING,911,L"Enregistrer le projet KICK...");AppendMenuW(menu,MF_SEPARATOR,0,nullptr);AppendMenuW(menu,MF_STRING,912,L"Ouvrir la Factory Sample Library");POINT p;GetCursorPos(&p);int choice=TrackPopupMenu(menu,TPM_RETURNCMD|TPM_NONOTIFY,p.x,p.y,0,window,nullptr);DestroyMenu(menu);if(choice==910)project(false);else if(choice==911)project(true);else if(choice==912)ShellExecuteW(window,L"open",library.c_str(),nullptr,nullptr,SW_SHOWNORMAL);}
    void selectPad(int p){surface.selectPad(p);selected=surface.selectedPad;stepTrack=selected;engine.selectTrack(selected);}
    std::wstring recordQuantizeName()const{return L"1/"+number(recordQuantize);}
    void toggleRecord(){recordArmed=!recordArmed;status=recordArmed?L"REC armé · quantification "+recordQuantizeName()+L" · lance PLAY puis joue les pads.":L"REC désarmé.";}
    void cycleRecordQuantize(){recordQuantize=recordQuantize==16?8:recordQuantize==8?4:16;status=L"Grille d'enregistrement : "+recordQuantizeName()+L".";}
    void play(int p){
        p=std::clamp(p,0,lab::Pads-1);const auto module=surface.displays[surface.focusedDisplay].module;auto snapshot=engine.snapshot();
        if(module==lab::DisplayModule::NextSequence){engine.queueSequence(p);status=snapshot.playing?L"Next Sequence en attente : "+number(p+1):L"Séquence sélectionnée : "+number(p+1);flashed[p]=GetTickCount64()+180;return;}
        if(module==lab::DisplayModule::Song){songChoice=p;status=L"Song : séquence "+number(p+1)+L" prête à être ajoutée.";flashed[p]=GetTickCount64()+180;return;}
        const auto& sequence=snapshot.sequences[snapshot.sequence];
        if(padMode==1&&sequence.mode!=lab::PerformanceMode::Drum){stepTrack=p;const int note=engine.performanceNote(p);engine.triggerPitch(sequence.sourcePad,note-sequence.rootNote);if(recordArmed)engine.recordHit(sequence.sourcePad,note,recordQuantize);flashed[p]=GetTickCount64()+180;return;}
        selectPad(p);engine.trigger(selected);if(recordArmed)engine.recordHit(selected,-1,recordQuantize);flashed[selected]=GetTickCount64()+130;
    }
    void releasePad(int p){const auto module=surface.displays[surface.focusedDisplay].module;if(module==lab::DisplayModule::NextSequence||module==lab::DisplayModule::Song)return;auto snapshot=engine.snapshot();if(snapshot.sequences[snapshot.sequence].mode!=lab::PerformanceMode::Drum)engine.releasePerformancePad(p);else engine.release(p);}
    void dispatch(const lab::SurfaceEvent& event){
        switch(event.action){
        case lab::SurfaceAction::PadPress:{if(event.index>=0&&event.index<lab::SurfacePads){if(event.pressed&&soloLatch){selectTrack(event.index);toggleTrackSolo(event.index);status=L"Solo piste "+number(event.index+1)+L" basculé.";}else if(event.pressed&&muteLatch){selectTrack(event.index);toggleTrackMute(event.index);status=L"Mute piste "+number(event.index+1)+L" basculé.";}else if(padMode==3){if(event.pressed&&(selectLatch||surface.shift)){selectTrack(event.index);const auto snapshot=engine.snapshot();if(snapshot.sequences[snapshot.sequence].mode==lab::PerformanceMode::Drum)selectPad(event.index);if(selectLatch)selectLatch=false;status=L"Step — piste "+number(stepTrack+1)+L" sélectionnée.";}else if(event.pressed)dispatch({lab::SurfaceAction::StepToggle,stepTrack,event.index});}else if(event.pressed)play(event.index);else releasePad(event.index);}}break;
        case lab::SurfaceAction::StepToggle:if(event.index>=0&&event.index<lab::SurfacePads&&event.value>=0&&event.value<lab::SurfaceSteps){auto snapshot=engine.snapshot();if(snapshot.sequences[snapshot.sequence].mode==lab::PerformanceMode::Drum)selectPad(event.index);engine.toggle(event.index,event.value);}break;
        case lab::SurfaceAction::PlayPause:{auto s=engine.snapshot();engine.setPlaying(!s.playing);break;}
        case lab::SurfaceAction::Stop:engine.stop();recordArmed=false;break;
        case lab::SurfaceAction::TempoDelta:engine.setTempo(engine.snapshot().tempo+event.value);break;
        case lab::SurfaceAction::CenterTurn:if(tab==0)turnSurfaceMenu(event.value);break;
        case lab::SurfaceAction::CenterPush:if(tab==0)activateSurfaceMenu();break;
        case lab::SurfaceAction::CenterUp:if(tab==0)moveSurfaceMenu(0,-1);break;
        case lab::SurfaceAction::CenterDown:if(tab==0)moveSurfaceMenu(0,1);break;
        case lab::SurfaceAction::CenterLeft:if(tab==0)moveSurfaceMenu(-1,0);break;
        case lab::SurfaceAction::CenterRight:if(tab==0)moveSurfaceMenu(1,0);break;
        case lab::SurfaceAction::QLinkTurn:if(tab==0)turnSurfaceQLink(event.display,event.index,event.value);break;
        case lab::SurfaceAction::QLinkSet:if(tab==0)setSurfaceQLink(event.display,event.index,event.value/127.);break;
        case lab::SurfaceAction::FocusNext:if(tab==0)surface.focusDisplay(1-surface.focusedDisplay);break;
        case lab::SurfaceAction::PagePrevious:if(tab==0){if(surface.displays[0].module==lab::DisplayModule::Mixer||surface.displays[1].module==lab::DisplayModule::Mixer)showSurfaceMixer(0);else surface.focusDisplay(0);}break;
        case lab::SurfaceAction::PageNext:if(tab==0){if(surface.displays[0].module==lab::DisplayModule::Mixer||surface.displays[1].module==lab::DisplayModule::Mixer)showSurfaceMixer(1);else surface.focusDisplay(1);}break;
        case lab::SurfaceAction::Shift:surface.shift=event.pressed;break;
        case lab::SurfaceAction::Back:if(tab==0)closeSurfaceModule();break;
        default:break;
        }
    }
    void key(WPARAM key,LPARAM flags){if(flags&(1u<<30))return;if(tab==0){if(key==VK_UP)dispatch({lab::SurfaceAction::CenterUp});else if(key==VK_DOWN)dispatch({lab::SurfaceAction::CenterDown});else if(key==VK_LEFT)dispatch({lab::SurfaceAction::CenterLeft});else if(key==VK_RIGHT)dispatch({lab::SurfaceAction::CenterRight});else if(key==VK_RETURN)dispatch({lab::SurfaceAction::CenterPush});else if(key==VK_TAB)dispatch({lab::SurfaceAction::FocusNext});else if(key==VK_PRIOR)dispatch({lab::SurfaceAction::PagePrevious});else if(key==VK_NEXT)dispatch({lab::SurfaceAction::PageNext});else if(key==VK_ESCAPE)dispatch({lab::SurfaceAction::Back});else if(key==VK_SPACE)dispatch({lab::SurfaceAction::PlayPause});else for(int p=0;p<16;++p)if(key==WPARAM(keys[p])){dispatch({lab::SurfaceAction::PadPress,p});break;}}else{if(key==VK_SPACE)dispatch({lab::SurfaceAction::PlayPause});else if(key==VK_ESCAPE){tab=0;status=L"Retour à la surface principale KICK.";}else for(int p=0;p<16;++p)if(key==WPARAM(keys[p])){dispatch({lab::SurfaceAction::PadPress,p});break;}}InvalidateRect(window,nullptr,FALSE);}
    Box padBox(int p)const{return {700+(p%4)*132,466+(3-p/4)*92,122,84};}
    #include "editor_ui.inc"
    #include "surface_ui.inc"
    void click(int x,int y,bool right){
        if(Box{1278,26,294,94}.contains(x,y)){tab=0;showSurfaceHome();return;}
        if(tab==1&&Box{1414,16,158,34}.contains(x,y)){tab=0;status=L"Retour à la surface principale KICK.";return;}
        if(tab==1){editorClick(x,y,right);return;}
        if(maschineClick(x,y,right))return;
        if(surfaceClick(x,y,right))return;
    }
    void fill(HDC dc,Box b,COLORREF c){auto r=b.rect();HBRUSH brush=CreateSolidBrush(c);FillRect(dc,&r,brush);DeleteObject(brush);}
    void round(HDC dc,Box b,COLORREF c,COLORREF edge=line){HBRUSH brush=CreateSolidBrush(c);HPEN pen=CreatePen(PS_SOLID,1,edge);auto oldB=SelectObject(dc,brush),oldP=SelectObject(dc,pen);RoundRect(dc,b.x,b.y,b.x+b.w,b.y+b.h,12,12);SelectObject(dc,oldB);SelectObject(dc,oldP);DeleteObject(brush);DeleteObject(pen);}
    void text(HDC dc,Box b,const std::wstring& str,int size=15,COLORREF c=ink,int weight=FW_NORMAL,UINT format=DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS){HFONT font=CreateFontW(-size,0,0,0,weight,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");auto old=SelectObject(dc,font);SetTextColor(dc,c);SetBkMode(dc,TRANSPARENT);auto r=b.rect();DrawTextW(dc,str.c_str(),int(str.size()),&r,format|DT_NOPREFIX);SelectObject(dc,old);DeleteObject(font);}
    void button(HDC dc,Box b,const std::wstring& name,bool on=false){round(dc,b,on?accent:RGB(31,38,50),on?accent:line);text(dc,b,name,14,on?bg:ink,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);}
    void paint(HDC dc){
        SetMapMode(dc,MM_ANISOTROPIC);SetWindowExtEx(dc,DesignWidth,DesignHeight,nullptr);SetViewportExtEx(dc,width,height,nullptr);
        fill(dc,{0,0,DesignWidth,DesignHeight},bg);
        if(tab==1){button(dc,{1414,16,158,34},L"Retour KICK",true);const int editorDc=SaveDC(dc);SetWindowOrgEx(dc,-EditorOffsetX,0,nullptr);paintEditor(dc);RestoreDC(dc,editorDc);return;}
        auto s=engine.snapshot();
        paintSurface(dc,s);
        paintMaschineControls(dc,s);
        unsigned error=audio.error.load();
        if(error){fill(dc,{1292,298,266,86},panel);text(dc,{1292,298,266,86},L"Audio indisponible — ouvre MASTER / WINDOWS et choisis une autre sortie.",9,RGB(255,133,103),FW_SEMIBOLD,DT_LEFT|DT_TOP|DT_WORDBREAK|DT_END_ELLIPSIS);}
    }
    void paintInfo(HDC dc){
        round(dc,{28,108,1144,594},panel);text(dc,{52,129,1080,40},L"Un premier accès au code. Pas encore la groovebox complète.",25,ink,FW_SEMIBOLD);
        text(dc,{52,184,1080,30},L"CE QUI VIENT DU DÉPÔT MUSIC TRIBE",13,accent,FW_SEMIBOLD);
        text(dc,{52,221,1080,28},L"AudIO::Design → 16 lecteurs AudioPlayback → AudIO::Bus → sortie stéréo",20,ink,FW_SEMIBOLD);
        text(dc,{52,258,1080,27},L"AudIO, AudioFile et les effets Airwindows Lowpass / PurestDrive viennent de 0700-AAM.",16,muted);
        text(dc,{52,310,1080,28},L"AJOUTS DE CE PROTOTYPE",13,accent,FW_SEMIBOLD);
        text(dc,{52,344,1080,28},L"Interface Windows, éditeur de sample, séquenceur simple, MIDI et sortie WinMM.",16,ink);
        text(dc,{52,377,1080,28},L"Kit de sons de démonstration créé pour ce prototype. Aucun sample d’usine Behringer.",16,muted);
        text(dc,{52,426,1080,28},L"CE QUI RESTE À PORTER",13,RGB(241,184,88),FW_SEMIBOLD);
        text(dc,{52,460,1080,28},L"L’interface JUCE KICK, ses programmes avancés et sa banque complète d’effets.",16,ink);
        text(dc,{52,493,1080,28},L"Ce programme autonome n’est pas un VST3 et n’émule pas le firmware de la machine.",16,muted);
        text(dc,{52,542,1080,28},L"Pour chipoter : source/src/engine.cpp et windows.cpp. Détails dans LIRE-MOI.html.",16,ink);
        text(dc,{52,574,1080,28},L"Sources sous AGPL-3.0-or-later. Prototype indépendant, non officiel.",14,muted);
        button(dc,{52,624,250,42},L"Ouvrir les sources");button(dc,{322,624,250,42},L"Voir le dépôt d’origine");
        text(dc,{28,725,1144,40},L"Raccourcis : espace = lecture/pause · Échap = arrêt · clic droit sur un pad = charger un WAV",14,muted);
    }
};
LRESULT CALLBACK procedure(HWND h,UINT msg,WPARAM w,LPARAM l){auto* app=reinterpret_cast<App*>(GetWindowLongPtrW(h,GWLP_USERDATA));if(msg==WM_NCCREATE){app=static_cast<App*>(reinterpret_cast<CREATESTRUCTW*>(l)->lpCreateParams);SetWindowLongPtrW(h,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(app));}if(!app)return DefWindowProcW(h,msg,w,l);switch(msg){
case WM_CREATE:app->initialise(h);return 0;
case WM_SIZE:app->width=std::max(1,int(LOWORD(l)));app->height=std::max(1,int(HIWORD(l)));InvalidateRect(h,nullptr,FALSE);return 0;
case WM_GETMINMAXINFO:{auto* m=reinterpret_cast<MINMAXINFO*>(l);m->ptMinTrackSize={960,680};return 0;}
case WM_ERASEBKGND:return 1;
case WM_PAINT:{
    PAINTSTRUCT ps;HDC dc=BeginPaint(h,&ps);
    RECT client{};GetClientRect(h,&client);
    app->width=std::max(1,int(client.right-client.left));
    app->height=std::max(1,int(client.bottom-client.top));
    HDC buffer=CreateCompatibleDC(dc);
    HBITMAP bitmap=CreateCompatibleBitmap(dc,app->width,app->height);
    if(buffer&&bitmap){
        auto old=SelectObject(buffer,bitmap);
        // Painting maps the 1600x900 design into the full client area.
        // Restore pixel coordinates before copying: BitBlt also interprets the
        // source DC mapping, so leaving it scaled cancels/distorts the resize.
        int saved=SaveDC(buffer);
        app->paint(buffer);
        if(saved)RestoreDC(buffer,saved);else SetMapMode(buffer,MM_TEXT);
        BitBlt(dc,0,0,app->width,app->height,buffer,0,0,SRCCOPY);
        SelectObject(buffer,old);
    }
    if(bitmap)DeleteObject(bitmap);
    if(buffer)DeleteDC(buffer);
    EndPaint(h,&ps);return 0;
}
case WM_DPICHANGED:{
    auto* suggested=reinterpret_cast<RECT*>(l);
    SetWindowPos(h,nullptr,suggested->left,suggested->top,
        suggested->right-suggested->left,suggested->bottom-suggested->top,
        SWP_NOZORDER|SWP_NOACTIVATE);
    return 0;
}
case WM_TIMER:if(app->smoke&&GetTickCount64()>app->flashed[0]){DestroyWindow(h);return 0;}InvalidateRect(h,nullptr,FALSE);return 0;
case WM_LBUTTONDOWN:case WM_RBUTTONDOWN:app->click(GET_X_LPARAM(l)*DesignWidth/app->width,GET_Y_LPARAM(l)*DesignHeight/app->height,msg==WM_RBUTTONDOWN);InvalidateRect(h,nullptr,FALSE);return 0;
case WM_MOUSEMOVE:{int x=GET_X_LPARAM(l)*DesignWidth/app->width,y=GET_Y_LPARAM(l)*DesignHeight/app->height;if(app->surfaceDragDisplay>=0){app->surfacePointer(y);InvalidateRect(h,nullptr,FALSE);}else if(app->editorDrag){app->editorPointer(x,y);InvalidateRect(h,nullptr,FALSE);}return 0;}
case WM_LBUTTONUP:if(app->editorDrag||app->surfaceDragDisplay>=0){app->editorDrag=0;app->surfaceDragDisplay=-1;app->surfaceDragKnob=-1;ReleaseCapture();}return 0;
case WM_CAPTURECHANGED:app->editorDrag=0;app->surfaceDragDisplay=-1;app->surfaceDragKnob=-1;return 0;
case WM_MOUSEWHEEL:{POINT point{GET_X_LPARAM(l),GET_Y_LPARAM(l)};ScreenToClient(h,&point);int x=point.x*DesignWidth/app->width,y=point.y*DesignHeight/app->height;if(!app->surfaceWheel(x,y,GET_WHEEL_DELTA_WPARAM(w))&&!app->editorWheel(x,y,GET_WHEEL_DELTA_WPARAM(w)))app->dispatch({lab::SurfaceAction::TempoDelta,0,GET_WHEEL_DELTA_WPARAM(w)>0?1:-1});InvalidateRect(h,nullptr,FALSE);return 0;}
case WM_KEYDOWN:if(w==VK_SHIFT)app->dispatch({lab::SurfaceAction::Shift,0,0,0,true});else app->key(w,l);return 0;
case WM_KEYUP:if(w==VK_SHIFT)app->dispatch({lab::SurfaceAction::Shift,0,0,0,false});else for(int p=0;p<16;++p)if(w==WPARAM(keys[p])){app->dispatch({lab::SurfaceAction::PadPress,p,0,0,false});break;}return 0;
case WM_DROPFILES:{HDROP drop=reinterpret_cast<HDROP>(w);wchar_t path[32768];POINT point{};DragQueryPoint(drop,&point);int x=point.x*DesignWidth/app->width,y=point.y*DesignHeight/app->height;if(app->tab==0){for(int p=0;p<16;++p)if(app->padBox(p).contains(x,y))app->selectPad(p);}else if(app->tab==1){for(int p=0;p<16;++p)if(Box{app->EditorOffsetX+28+p*71,110,65,42}.contains(x,y))app->selectPad(p);}if(DragQueryFileW(drop,0,path,32768))app->load(path);DragFinish(drop);InvalidateRect(h,nullptr,FALSE);return 0;}
case SurfaceMessage:app->dispatch({static_cast<lab::SurfaceAction>(w),int(l)});InvalidateRect(h,nullptr,FALSE);return 0;
case MidiMessage:app->midiData(unsigned(w));InvalidateRect(h,nullptr,FALSE);return 0;
case WM_DESTROY:KillTimer(h,1);app->closeMidi();app->audio.stop();PostQuitMessage(0);return 0;
}return DefWindowProcW(h,msg,w,l);}
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,PWSTR command,int show){
    try{
        // Use physical client dimensions on each monitor (Windows 10/11), with
        // a fallback for systems where the newer awareness API is unavailable.
        using SetDpiContext=BOOL(WINAPI*)(HANDLE);
        auto setDpi=reinterpret_cast<SetDpiContext>(GetProcAddress(GetModuleHandleW(L"user32.dll"),"SetProcessDpiAwarenessContext"));
        if(!setDpi||!setDpi(reinterpret_cast<HANDLE>(static_cast<INT_PTR>(-4))))SetProcessDPIAware();
        App app;app.smoke=std::wstring(command).find(L"--smoke-test")!=std::wstring::npos;
        if(app.smoke)app.flashed[0]=GetTickCount64()+2500;
        WNDCLASSW c{};c.hInstance=instance;c.lpfnWndProc=procedure;c.lpszClassName=L"KickLabWindow";c.hCursor=LoadCursor(nullptr,IDC_ARROW);c.hIcon=LoadIcon(nullptr,IDI_APPLICATION);RegisterClassW(&c);
        RECT r{0,0,DesignWidth,DesignHeight};AdjustWindowRect(&r,WS_OVERLAPPEDWINDOW,FALSE);
        HWND h=CreateWindowExW(0,c.lpszClassName,L"KICK Lab 0.8.1 — Tracks / Maschine+",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,r.right-r.left,r.bottom-r.top,nullptr,nullptr,instance,&app);
        if(!h)throw std::runtime_error("Impossible de créer la fenêtre Windows.");ShowWindow(h,show);UpdateWindow(h);MSG m;BOOL result;while((result=GetMessageW(&m,nullptr,0,0))>0){TranslateMessage(&m);DispatchMessageW(&m);}return result<0?1:0;
    }catch(const std::exception& e){MessageBoxW(nullptr,wide(e.what()).c_str(),L"KICK Lab",MB_OK|MB_ICONERROR);return 1;}
}

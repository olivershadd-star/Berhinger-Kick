// SPDX-License-Identifier: AGPL-3.0-or-later
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
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
#include "engine.hpp"

namespace {
std::wstring wide(const std::string& s){if(s.empty())return {};int n=MultiByteToWideChar(CP_UTF8,0,s.data(),int(s.size()),nullptr,0);std::wstring r(n,0);MultiByteToWideChar(CP_UTF8,0,s.data(),int(s.size()),r.data(),n);return r;}
std::string utf8(const std::wstring& s){if(s.empty())return {};int n=WideCharToMultiByte(CP_UTF8,0,s.data(),int(s.size()),nullptr,0,nullptr,nullptr);std::string r(n,0);WideCharToMultiByte(CP_UTF8,0,s.data(),int(s.size()),r.data(),n,nullptr,nullptr);return r;}
struct Box {int x,y,w,h;bool contains(int a,int b)const{return a>=x&&a<x+w&&b>=y&&b<y+h;} RECT rect()const{return {x,y,x+w,y+h};}};
constexpr COLORREF bg=RGB(14,17,23),panel=RGB(23,28,37),muted=RGB(144,158,178),ink=RGB(233,239,248),accent=RGB(76,217,184),line=RGB(43,53,68);
COLORREF padColor(int i){constexpr COLORREF colors[]={RGB(255,133,103),RGB(241,184,88),RGB(76,217,184),RGB(132,161,250)};return colors[i/4];}
std::wstring number(int n){return std::to_wstring(n);}
std::wstring padName(int p){return (p<9?L"0":L"")+number(p+1);}
const std::array<int,16> keys={'A','Z','E','R','Q','S','D','F','W','X','C','V','1','2','3','4'};
const std::array<std::wstring,16> labels={L"A",L"Z",L"E",L"R",L"Q",L"S",L"D",L"F",L"W",L"X",L"C",L"V",L"1",L"2",L"3",L"4"};

class AudioOutput {
public:
    explicit AudioOutput(lab::Engine& e):engine(e){}
    ~AudioOutput(){stop();}
    std::atomic<unsigned> error{0};std::atomic<bool> running{false};
    void start(UINT id){stop();error=0;alive=true;thread=std::thread([this,id]{run(id);});}
    void stop(){alive=false;if(thread.joinable())thread.join();running=false;}
private:
    lab::Engine& engine;std::thread thread;std::atomic<bool> alive{false};
    void run(UINT id){
        WAVEFORMATEX fmt{};fmt.wFormatTag=WAVE_FORMAT_PCM;fmt.nChannels=2;fmt.nSamplesPerSec=lab::Rate;fmt.wBitsPerSample=16;fmt.nBlockAlign=4;fmt.nAvgBytesPerSec=lab::Rate*4;
        HWAVEOUT device{};MMRESULT result=waveOutOpen(&device,id,&fmt,0,0,CALLBACK_NULL);
        if(result){error=result;return;}
        std::array<WAVEHDR,4> headers{};std::array<std::array<short,lab::Block*2>,4> data{};std::array<bool,4> queued{},prepared{};
        for(int b=0;b<4;++b){headers[b].lpData=reinterpret_cast<char*>(data[b].data());headers[b].dwBufferLength=sizeof(data[b]);result=waveOutPrepareHeader(device,&headers[b],sizeof(WAVEHDR));if(result){error=result;alive=false;break;}prepared[b]=true;}
        running=alive.load();float buffer[lab::Block*2];
        while(alive){for(int b=0;b<4&&alive;++b){if(!queued[b]||(headers[b].dwFlags&WHDR_DONE)){engine.render(buffer);for(int i=0;i<lab::Block*2;++i)data[b][i]=short(std::lround(buffer[i]*32767));result=waveOutWrite(device,&headers[b],sizeof(WAVEHDR));if(result){error=result;alive=false;break;}queued[b]=true;}}Sleep(1);}
        waveOutReset(device);for(int b=0;b<4;++b)if(prepared[b])waveOutUnprepareHeader(device,&headers[b],sizeof(WAVEHDR));waveOutClose(device);running=false;
    }
};

class App {
public:
    lab::Engine engine;AudioOutput audio{engine};HWND window{};std::filesystem::path base;
    int selected=0,tab=0;bool smoke=false;UINT audioId=WAVE_MAPPER;HMIDIIN midi{};int midiId=-1;
    std::wstring status=L"Clique sur un pad pour jouer. Clic droit pour charger un WAV.";
    std::array<ULONGLONG,16> flashed{};int mouseX=-1,mouseY=-1;int width=1200,height=800;
    App(){wchar_t path[32768];DWORD n=GetModuleFileNameW(nullptr,path,32768);base=std::filesystem::path(std::wstring(path,n)).parent_path();SetCurrentDirectoryW(base.c_str());}
    ~App(){closeMidi();audio.stop();}
    void initialise(HWND h){window=h;int loaded=0;std::string error;for(int p=0;p<16;++p){const std::array<std::wstring,16> files={L"01-Kick.wav",L"02-Snare.wav",L"03-Closed-Hat.wav",L"04-Open-Hat.wav",L"05-Clap.wav",L"06-Low-Tom.wav",L"07-Mid-Tom.wav",L"08-High-Tom.wav",L"09-Rim.wav",L"10-Shaker.wav",L"11-Claves.wav",L"12-Cowbell.wav",L"13-Bass-C.wav",L"14-Bass-E.wav",L"15-Bass-G.wav",L"16-Chime.wav"};auto path=base/L"samples"/files[p];if(engine.load(p,path.u8string(),error))++loaded;}engine.demoPattern();if(loaded!=16)status=L"Kit incomplet : "+number(loaded)+L"/16 fichiers. Charger un WAV avec un clic droit.";audio.start(audioId);SetTimer(h,1,33,nullptr);DragAcceptFiles(h,TRUE);}
    void closeMidi(){if(midi){midiInStop(midi);midiInReset(midi);midiInClose(midi);midi=nullptr;}midiId=-1;}
    static void CALLBACK midiCallback(HMIDIIN,UINT message,DWORD_PTR instance,DWORD_PTR packed,DWORD_PTR){if(message==MIM_DATA){auto* self=reinterpret_cast<App*>(instance);unsigned type=packed&255,note=(packed>>8)&127,velocity=(packed>>16)&127;if((type&0xf0)==0x90&&velocity&&note>=36&&note<52)self->engine.trigger(int(note-36));}}
    void midiMenu(){HMENU menu=CreatePopupMenu();AppendMenuW(menu,MF_STRING|(midiId<0?MF_CHECKED:0),500,L"Désactivé");for(UINT i=0;i<midiInGetNumDevs();++i){MIDIINCAPSW cap{};midiInGetDevCapsW(i,&cap,sizeof(cap));AppendMenuW(menu,MF_STRING|(int(i)==midiId?MF_CHECKED:0),501+i,cap.szPname);}POINT p;GetCursorPos(&p);int choice=TrackPopupMenu(menu,TPM_RETURNCMD|TPM_NONOTIFY,p.x,p.y,0,window,nullptr);DestroyMenu(menu);if(!choice)return;closeMidi();if(choice>500){int id=choice-501;MMRESULT result=midiInOpen(&midi,id,reinterpret_cast<DWORD_PTR>(&midiCallback),reinterpret_cast<DWORD_PTR>(this),CALLBACK_FUNCTION);if(!result){midiId=id;midiInStart(midi);status=L"MIDI connecté : notes 36 à 51, tous canaux.";}else status=L"Impossible d'ouvrir cette entrée MIDI.";}else status=L"MIDI désactivé.";}
    void audioMenu(){HMENU menu=CreatePopupMenu();AppendMenuW(menu,MF_STRING|(audioId==WAVE_MAPPER?MF_CHECKED:0),600,L"Sortie Windows par défaut");for(UINT i=0;i<waveOutGetNumDevs();++i){WAVEOUTCAPSW cap{};waveOutGetDevCapsW(i,&cap,sizeof(cap));AppendMenuW(menu,MF_STRING|(i==audioId?MF_CHECKED:0),601+i,cap.szPname);}POINT p;GetCursorPos(&p);int choice=TrackPopupMenu(menu,TPM_RETURNCMD|TPM_NONOTIFY,p.x,p.y,0,window,nullptr);DestroyMenu(menu);if(choice){audioId=choice==600?WAVE_MAPPER:UINT(choice-601);audio.start(audioId);status=L"Changement de sortie audio...";}}
    std::wstring choose(bool save,bool project){std::vector<wchar_t> name(32768,0);OPENFILENAMEW dialog{};dialog.lStructSize=sizeof(dialog);dialog.hwndOwner=window;dialog.lpstrFile=name.data();dialog.nMaxFile=DWORD(name.size());dialog.lpstrFilter=project?L"Projet KICK Lab (*.kicklab)\0*.kicklab\0\0":L"Audio WAV (*.wav)\0*.wav\0\0";dialog.lpstrDefExt=project?L"kicklab":L"wav";dialog.Flags=OFN_EXPLORER|OFN_NOCHANGEDIR|OFN_PATHMUSTEXIST|(save?OFN_OVERWRITEPROMPT:OFN_FILEMUSTEXIST);if(save?GetSaveFileNameW(&dialog):GetOpenFileNameW(&dialog))return name.data();return {};}
    void load(const std::wstring& path){if(path.empty())return;std::string error;if(engine.load(selected,utf8(path),error)){status=L"WAV chargé sur le pad "+padName(selected)+L". Conversion à 48 kHz si nécessaire.";play(selected);}else MessageBoxW(window,wide(error).c_str(),L"Chargement WAV",MB_OK|MB_ICONINFORMATION);}
    void project(bool save){auto p=choose(save,true);if(p.empty())return;std::string error;bool ok=save?engine.save(utf8(p),error):engine.open(utf8(p),error);if(!ok)MessageBoxW(window,wide(error).c_str(),L"Projet KICK Lab",MB_OK|MB_ICONINFORMATION);else status=save?L"Projet enregistré. Les samples restent référencés à leur emplacement.":L"Projet chargé.";}
    void play(int p){selected=p;engine.trigger(p);flashed[p]=GetTickCount64()+130;}
    void key(WPARAM key,LPARAM flags){if(flags&(1u<<30))return;if(key==VK_SPACE){auto s=engine.snapshot();engine.setPlaying(!s.playing);}else if(key==VK_ESCAPE)engine.stop();else for(int p=0;p<16;++p)if(key==WPARAM(keys[p])){play(p);break;}InvalidateRect(window,nullptr,FALSE);}
    Box padBox(int p)const{return {44+(p%4)*88,260+(p/4)*88,78,78};}
    void click(int x,int y,bool right){
        if(Box{930,26,100,32}.contains(x,y)){tab=0;return;}if(Box{1038,26,134,32}.contains(x,y)){tab=1;return;}
        if(tab){if(Box{52,624,250,42}.contains(x,y))ShellExecuteW(window,L"open",(base/L"source").c_str(),nullptr,nullptr,SW_SHOWNORMAL);if(Box{322,624,250,42}.contains(x,y))ShellExecuteW(window,L"open",L"https://github.com/MusicTribeOpenSource/0700-AAM",nullptr,nullptr,SW_SHOWNORMAL);return;}
        if(Box{28,106,110,42}.contains(x,y)){auto s=engine.snapshot();engine.setPlaying(!s.playing);}
        else if(Box{148,106,80,42}.contains(x,y))engine.stop();
        else if(Box{245,106,36,42}.contains(x,y))engine.setTempo(engine.snapshot().tempo-1);
        else if(Box{371,106,36,42}.contains(x,y))engine.setTempo(engine.snapshot().tempo+1);
        else if(Box{430,106,112,42}.contains(x,y)){engine.demoPattern();status=L"Pattern de démonstration : 1 mesure, 16 doubles croches.";}
        else if(Box{552,106,100,42}.contains(x,y))engine.clearPattern();
        else if(Box{674,106,105,42}.contains(x,y))project(false);
        else if(Box{789,106,119,42}.contains(x,y))project(true);
        else if(Box{960,104,212,48}.contains(x,y))engine.setVolume(std::clamp((x-973)/184.,0.,1.));
        else if(Box{44,632,170,38}.contains(x,y))load(choose(false,false));
        else if(Box{228,632,166,38}.contains(x,y)){engine.stop();status=L"Toutes les voix sont arrêtées.";}
        else if(Box{830,754,150,30}.contains(x,y))midiMenu();
        else if(Box{990,754,182,30}.contains(x,y))audioMenu();
        else {
            for(int p=0;p<16;++p)if(padBox(p).contains(x,y)){selected=p;if(right)load(choose(false,false));else play(p);return;}
            if(x>=610&&x<1154&&y>=240&&y<656){int p=(y-240)/26,k=(x-610)/34;if(p<16&&k<16){selected=p;engine.toggle(p,k);}}
            else if(x>=458&&x<602&&y>=240&&y<656)selected=(y-240)/26;
        }
    }
    void fill(HDC dc,Box b,COLORREF c){auto r=b.rect();HBRUSH brush=CreateSolidBrush(c);FillRect(dc,&r,brush);DeleteObject(brush);}
    void round(HDC dc,Box b,COLORREF c,COLORREF edge=line){HBRUSH brush=CreateSolidBrush(c);HPEN pen=CreatePen(PS_SOLID,1,edge);auto oldB=SelectObject(dc,brush),oldP=SelectObject(dc,pen);RoundRect(dc,b.x,b.y,b.x+b.w,b.y+b.h,12,12);SelectObject(dc,oldB);SelectObject(dc,oldP);DeleteObject(brush);DeleteObject(pen);}
    void text(HDC dc,Box b,const std::wstring& str,int size=15,COLORREF c=ink,int weight=FW_NORMAL,UINT format=DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS){HFONT font=CreateFontW(-size,0,0,0,weight,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");auto old=SelectObject(dc,font);SetTextColor(dc,c);SetBkMode(dc,TRANSPARENT);auto r=b.rect();DrawTextW(dc,str.c_str(),int(str.size()),&r,format);SelectObject(dc,old);DeleteObject(font);}
    void button(HDC dc,Box b,const std::wstring& name,bool on=false){round(dc,b,on?accent:RGB(31,38,50),on?accent:line);text(dc,b,name,14,on?bg:ink,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);}
    void paint(HDC dc){
        SetMapMode(dc,MM_ANISOTROPIC);SetWindowExtEx(dc,1200,800,nullptr);SetViewportExtEx(dc,width,height,nullptr);
        fill(dc,{0,0,1200,800},bg);text(dc,{28,18,170,40},L"KICK / LAB",30,ink,FW_BOLD);
        text(dc,{218,20,670,24},L"EXPLORATION DU MOTEUR AudIO",12,accent,FW_SEMIBOLD);
        text(dc,{218,43,680,22},L"Prototype communautaire • interface expérimentale • Windows x64",13,muted);
        button(dc,{930,26,100,32},L"Jouer",tab==0);button(dc,{1038,26,134,32},L"Sous le capot",tab==1);
        fill(dc,{28,83,1144,1},line);
        if(tab){paintInfo(dc);return;}
        auto s=engine.snapshot();
        button(dc,{28,106,110,42},s.playing?L"Pause":L"Jouer",s.playing);button(dc,{148,106,80,42},L"Stop");
        button(dc,{245,106,36,42},L"−");text(dc,{283,106,86,24},number(int(s.tempo)),23,ink,FW_SEMIBOLD,DT_CENTER|DT_SINGLELINE);text(dc,{283,131,86,14},L"BPM",10,muted,FW_NORMAL,DT_CENTER|DT_SINGLELINE);button(dc,{371,106,36,42},L"+");
        button(dc,{430,106,112,42},L"Démo");button(dc,{552,106,100,42},L"Effacer");button(dc,{674,106,105,42},L"Ouvrir");button(dc,{789,106,119,42},L"Enregistrer");
        text(dc,{960,98,210,20},L"MASTER  "+number(int(s.volume*100))+L" %",11,muted);round(dc,{973,133,184,5},line,line);fill(dc,{973,133,int(184*s.volume),5},accent);
        round(dc,{28,174,390,510},panel);text(dc,{44,188,270,30},L"16 PADS / ONE-SHOTS",17,ink,FW_SEMIBOLD);text(dc,{44,220,340,22},L"Clavier AZERTY ou MIDI · notes 36–51",12,muted);
        ULONGLONG now=GetTickCount64();for(int p=0;p<16;++p){auto b=padBox(p);bool hot=now<flashed[p]||s.active[p];COLORREF c=padColor(p);round(dc,b,hot?c:RGB(32,39,51),p==selected?c:line);fill(dc,{b.x+10,b.y+11,5,5},c);text(dc,{b.x+20,b.y+4,45,22},padName(p),12,hot?bg:muted);text(dc,{b.x+7,b.y+27,64,21},wide(s.names[p]),12,hot?bg:ink,FW_SEMIBOLD);text(dc,{b.x+10,b.y+54,58,17},labels[p],11,hot?bg:muted);}
        button(dc,{44,632,170,38},L"Charger un WAV");button(dc,{228,632,166,38},L"Couper les voix");
        round(dc,{438,174,734,510},panel);text(dc,{458,186,400,28},L"SÉQUENCEUR / 1 MESURE",17,ink,FW_SEMIBOLD);text(dc,{925,186,225,28},L"16 pas · doubles croches",12,muted);
        for(int k=0;k<16;++k)text(dc,{610+k*34,219,28,18},number(k+1),11,k==s.step?accent:muted,FW_NORMAL,DT_CENTER|DT_SINGLELINE);
        for(int p=0;p<16;++p){if(p==selected)fill(dc,{450,240+p*26,708,24},RGB(32,41,52));text(dc,{458,240+p*26,27,22},padName(p),11,padColor(p),FW_SEMIBOLD);text(dc,{490,240+p*26,108,22},wide(s.names[p]),12,p==selected?ink:muted);for(int k=0;k<16;++k){COLORREF c=s.pattern[p][k]?padColor(p):(k/4%2?RGB(38,46,59):RGB(31,38,49));if(s.playing&&s.step==k&&!s.pattern[p][k])c=RGB(58,76,84);round(dc,{610+k*34,242+p*26,27,19},c,s.playing&&s.step==k?ink:c);}}
        round(dc,{28,697,1144,45},panel);text(dc,{44,701,170,34},L"PAD "+padName(selected)+L" / "+wide(s.names[selected]),12,padColor(selected));
        const auto& wave=s.waveforms[selected];if(wave.empty())text(dc,{234,701,800,34},L"Charge un WAV mono ou stéréo pour explorer le lecteur AudIO.",12,muted);else{HPEN pen=CreatePen(PS_SOLID,1,padColor(selected));auto old=SelectObject(dc,pen);for(int i=0;i<320;++i){int h=std::max(1,int(wave[i]*16));int x=236+i*2;MoveToEx(dc,x,720-h,nullptr);LineTo(dc,x,720+h);}SelectObject(dc,old);DeleteObject(pen);}
        fill(dc,{950,718,184,5},line);fill(dc,{950,718,int(184*std::clamp(s.peak,0.f,1.f)),5},accent);
        unsigned error=audio.error.load();std::wstring message=status;if(error){wchar_t buf[256]{};waveOutGetErrorTextW(error,buf,256);message=L"Audio indisponible : "+std::wstring(buf)+L" — choisir une autre sortie.";}
        text(dc,{28,754,785,30},message,12,error?RGB(255,133,103):muted);button(dc,{830,754,150,30},midiId<0?L"MIDI : désactivé":L"MIDI : connecté");button(dc,{990,754,182,30},audio.running?L"Audio : sortie Windows":L"Choisir sortie audio");
    }
    void paintInfo(HDC dc){
        round(dc,{28,108,1144,594},panel);text(dc,{52,129,1080,40},L"Un premier accès au code. Pas encore la groovebox complète.",25,ink,FW_SEMIBOLD);
        text(dc,{52,184,1080,30},L"CE QUI VIENT DU DÉPÔT MUSIC TRIBE",13,accent,FW_SEMIBOLD);
        text(dc,{52,221,1080,28},L"AudIO::Design → 16 lecteurs AudioPlayback → AudIO::Bus → sortie stéréo",20,ink,FW_SEMIBOLD);
        text(dc,{52,258,1080,27},L"Le graphe audio, les lecteurs, le mixage et AudioFile sont issus de l’archive 0700-AAM.",16,muted);
        text(dc,{52,310,1080,28},L"AJOUTS DE CE PROTOTYPE",13,accent,FW_SEMIBOLD);
        text(dc,{52,344,1080,28},L"Interface Windows, pads, séquenceur simple, MIDI, sortie WinMM et projets .kicklab.",16,ink);
        text(dc,{52,377,1080,28},L"Kit de sons de démonstration créé pour ce prototype. Aucun sample d’usine Behringer.",16,muted);
        text(dc,{52,426,1080,28},L"CE QUI RESTE À PORTER",13,RGB(241,184,88),FW_SEMIBOLD);
        text(dc,{52,460,1080,28},L"L’interface JUCE KICK, ses programmes avancés, son séquenceur et les effets Airwindows.",16,ink);
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
case WM_PAINT:{PAINTSTRUCT ps;HDC dc=BeginPaint(h,&ps);HDC buffer=CreateCompatibleDC(dc);HBITMAP bitmap=CreateCompatibleBitmap(dc,app->width,app->height);auto old=SelectObject(buffer,bitmap);app->paint(buffer);BitBlt(dc,0,0,app->width,app->height,buffer,0,0,SRCCOPY);SelectObject(buffer,old);DeleteObject(bitmap);DeleteDC(buffer);EndPaint(h,&ps);return 0;}
case WM_TIMER:if(app->smoke&&GetTickCount64()>app->flashed[0]){DestroyWindow(h);return 0;}InvalidateRect(h,nullptr,FALSE);return 0;
case WM_LBUTTONDOWN:case WM_RBUTTONDOWN:app->click(GET_X_LPARAM(l)*1200/app->width,GET_Y_LPARAM(l)*800/app->height,msg==WM_RBUTTONDOWN);InvalidateRect(h,nullptr,FALSE);return 0;
case WM_MOUSEWHEEL:app->engine.setTempo(app->engine.snapshot().tempo+(GET_WHEEL_DELTA_WPARAM(w)>0?1:-1));InvalidateRect(h,nullptr,FALSE);return 0;
case WM_KEYDOWN:app->key(w,l);return 0;
case WM_DROPFILES:{HDROP drop=reinterpret_cast<HDROP>(w);wchar_t path[32768];POINT point{};DragQueryPoint(drop,&point);int x=point.x*1200/app->width,y=point.y*800/app->height;for(int p=0;p<16;++p)if(app->padBox(p).contains(x,y))app->selected=p;if(DragQueryFileW(drop,0,path,32768))app->load(path);DragFinish(drop);InvalidateRect(h,nullptr,FALSE);return 0;}
case WM_DESTROY:KillTimer(h,1);app->closeMidi();app->audio.stop();PostQuitMessage(0);return 0;
}return DefWindowProcW(h,msg,w,l);}
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,PWSTR command,int show){
    try{
        SetProcessDPIAware();App app;app.smoke=std::wstring(command).find(L"--smoke-test")!=std::wstring::npos;
        if(app.smoke)app.flashed[0]=GetTickCount64()+2500;
        WNDCLASSW c{};c.hInstance=instance;c.lpfnWndProc=procedure;c.lpszClassName=L"KickLabWindow";c.hCursor=LoadCursor(nullptr,IDC_ARROW);c.hIcon=LoadIcon(nullptr,IDI_APPLICATION);RegisterClassW(&c);
        RECT r{0,0,1200,800};AdjustWindowRect(&r,WS_OVERLAPPEDWINDOW,FALSE);
        HWND h=CreateWindowExW(0,c.lpszClassName,L"KICK Lab — AudIO / Prototype Windows",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,r.right-r.left,r.bottom-r.top,nullptr,nullptr,instance,&app);
        if(!h)throw std::runtime_error("Impossible de créer la fenêtre Windows.");ShowWindow(h,show);UpdateWindow(h);MSG m;BOOL result;while((result=GetMessageW(&m,nullptr,0,0))>0){TranslateMessage(&m);DispatchMessageW(&m);}return result<0?1:0;
    }catch(const std::exception& e){MessageBoxW(nullptr,wide(e.what()).c_str(),L"KICK Lab",MB_OK|MB_ICONERROR);return 1;}
}

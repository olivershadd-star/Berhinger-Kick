// SPDX-License-Identifier: AGPL-3.0-or-later
// App member functions for the common KICK mouse/keyboard/MIDI surface.
const std::array<std::wstring,16> surfaceMenuLabels={
    L"Browser",L"Track Mixer",L"Track Mutes",L"Sequences",
    L"List Edit",L"Pad Mixer",L"Pad Mutes",L"Songs",
    L"Color Editor",L"Sampler",L"Sample Editor",L"Program Edit",
    L"Pad Notes",L"Looper",L"Q-Links",L"MIDI Control"
};
const std::array<std::wstring,lab::MidiCommandCount> midiCommandLabels={
    L"Play / Pause",L"Stop",L"Shift",L"Enter",
    L"Haut",L"Bas",L"Gauche",L"Droite",
    L"Écran A",L"Écran B",L"Rotation 4D",L"Retour / Escape",
    L"Pad Mode",L"Keyboard",L"Chords",L"Step",
    L"Browser",L"Mixer",L"Sampling",L"Next Sequence",
    L"Song",L"File / Save",L"Settings",L"Track Mute",
    L"Pad Mute",L"Select",L"Solo",L"Record",
    L"Tempo",L"Swing"
};
static constexpr int surfaceMidiQlinkPages=4;
static constexpr int surfaceMidiPadPages=4;
static constexpr int surfaceMidiCommandPages=(lab::MidiCommandCount+3)/4;
static constexpr int surfaceMidiPages=surfaceMidiQlinkPages+surfaceMidiPadPages+surfaceMidiCommandPages;

int surfaceProgramPad(const lab::DisplayBinding& binding,const lab::Snapshot& snapshot)const{return lab::programPad(snapshot.bank,binding.pad);}

Box surfaceModuleBox(int display)const{return {28+display*580,68,552,318};}
Box surfaceDisplayBox(int display)const{return {40+display*580,80,456,294};}
Box surfaceDisplayItemBox(int display,int local)const{return {54+display*580+(local%2)*211,126+(local/2)*56,199,46};}
Box surfaceKnobBox(int display,int knob)const{return {514+display*580,88+knob*67,48,48};}
Box surfaceInsertSlotBox(int display,int slot)const{return {54+display*580+slot*104,127,96,34};}
Box surfaceInsertEffectBox(int display)const{return {54+display*580,174,250,36};}
Box surfaceInsertBypassBox(int display)const{return {314+display*580,174,136,36};}
Box surfacePagePreviousBox(int display)const{return {408+display*580,88,28,24};}
Box surfacePageNextBox(int display)const{return {440+display*580,88,28,24};}
Box surfaceProgramAuditionBox(int display)const{return {54+display*580,126,396,34};}
Box surfaceMidiDeviceBox(int display)const{return {54+display*580,126,240,34};}
Box surfaceMidiPreviousBox(int display)const{return {354+display*580,126,44,34};}
Box surfaceMidiNextBox(int display)const{return {406+display*580,126,44,34};}
Box surfaceMidiBindingBox(int display,int row)const{return {54+display*580,226+row*34,396,28};}
Box surfaceBrowserPreviousBox(int display)const{return {54+display*580,126,42,34};}
Box surfaceBrowserCategoryBox(int display)const{return {102+display*580,126,300,34};}
Box surfaceBrowserNextBox(int display)const{return {408+display*580,126,42,34};}
Box surfaceBrowserFileBox(int display,int row)const{return {54+display*580,170+row*31,396,27};}
Box surfaceBrowserModeBox(int display)const{return {54+display*580,332,82,30};}
Box surfaceBrowserAuditionBox(int display)const{return {142+display*580,332,82,30};}
Box surfaceBrowserLoadBox(int display)const{return {230+display*580,332,100,30};}
Box surfaceBrowserRefreshBox(int display)const{return {336+display*580,332,114,30};}
Box surfaceSongActionBox(int display,int action)const{return {54+display*580+action*99,126,93,32};}
Box surfaceSongRowBox(int display,int row)const{return {54+display*580,174+row*43,396,36};}

int surfaceDragDisplay=-1,surfaceDragKnob=-1,surfaceDragStartY=0,surfaceDragLastStep=0;

void showSurfaceMenu(int display){
    display=std::clamp(display,0,1);
    surface.displays[display].module=lab::DisplayModule::Menu;
    const int other=1-display;
    if(surface.displays[other].module==lab::DisplayModule::Inserts)
        surface.setDisplayPage(other,0);
    surface.focusDisplay(display);
}

void showSurfaceHome(){
    for(int display=0;display<lab::SurfaceDisplays;++display){surface.displays[display].module=lab::DisplayModule::Menu;surface.displays[display].page=0;}
    surface.focusDisplay(0);status=L"Menu KICK — les deux écrans sont revenus à l'accueil.";
}

void showSurfaceMenuItem(int item){
    item=std::clamp(item,0,15);const int display=item/8;showSurfaceMenu(display);surface.menuSelections[display]=item%8;status=L"Menu KICK — "+surfaceMenuLabels[item]+L" sélectionné.";
}

void showSurfaceInserts(int display){
    display=std::clamp(display,0,1);
    auto& binding=surface.displays[display];
    binding.module=lab::DisplayModule::Inserts;
    binding.pad=selected;
    binding.slot=std::clamp(selectedInsert,0,lab::InsertSlots-1);
    binding.page=0;
    surface.focusDisplay(display);
    status=L"Écran "+std::wstring(display?L"B":L"A")+L" — paramètres réels du slot d'effet sélectionné.";
}

void showSurfaceProgram(int display){
    display=std::clamp(display,0,1);auto& binding=surface.displays[display];binding.module=lab::DisplayModule::Sample;binding.pad=selected;binding.page=0;surface.focusDisplay(display);
    status=L"Program Edit — lecture et enveloppe d'amplitude du pad "+padName(selected)+L".";
}

void showSurfacePadPerform(int display){
    display=std::clamp(display,0,1);auto snapshot=engine.snapshot();engine.setMelodicSourcePad(lab::programPad(snapshot.bank,selected));if(snapshot.sequences[snapshot.sequence].mode==lab::PerformanceMode::Drum)engine.setPerformanceMode(lab::PerformanceMode::Chromatic);auto& binding=surface.displays[display];binding.module=lab::DisplayModule::PadPerform;binding.page=0;surface.focusDisplay(display);status=L"Pad Notes — les 16 pads jouent le sample source à des hauteurs différentes.";
}

void showSurfaceNextSequence(int display){
    display=std::clamp(display,0,1);auto& binding=surface.displays[display];binding.module=lab::DisplayModule::NextSequence;binding.page=0;surface.focusDisplay(display);status=L"Next Sequence — les pads 1 à 16 sélectionnent ou mettent en attente une séquence.";
}

void showSurfaceSong(int display){
    display=std::clamp(display,0,1);auto& binding=surface.displays[display];binding.module=lab::DisplayModule::Song;binding.page=0;surface.focusDisplay(display);auto snapshot=engine.snapshot();songSelected=std::clamp(songSelected,0,std::max(0,snapshot.songLength-1));status=L"Song Editor — choisis une séquence avec les pads, règle Repeat puis ajoute-la.";
}

double surfaceProgramRatio(const lab::PadEdit& edit,int page,int knob)const{
    if(page==0){if(knob==0)return edit.gain/2.;if(knob==1)return (edit.semitones+24.)/48.;if(knob==2)return edit.start;if(knob==3)return edit.end;}
    if(page==1){if(knob==0)return edit.attack;if(knob==1)return edit.decay;if(knob==2)return edit.sustain;if(knob==3)return edit.release;}
    return 0;
}

void setSurfaceProgramRatio(int display,int knob,double ratio,int layer=0){
    if(display<0||display>=lab::SurfaceDisplays||knob<0||knob>=lab::SurfaceQLinks)return;auto& binding=surface.displays[display];if(binding.module!=lab::DisplayModule::Sample)return;
    auto snapshot=engine.snapshot();const int pad=surfaceProgramPad(binding,snapshot),page=binding.page+layer*2;if(page>1)return;auto edit=snapshot.edits[pad];ratio=std::clamp(ratio,0.,1.);
    if(page==0){if(knob==0)edit.gain=ratio*2.;else if(knob==1)edit.semitones=std::round(ratio*48.)-24.;else if(knob==2)edit.start=std::min(ratio,edit.end-.001);else if(knob==3)edit.end=std::max(ratio,edit.start+.001);}
    else if(page==1){if(knob==0)edit.attack=ratio;else if(knob==1)edit.decay=ratio;else if(knob==2)edit.sustain=ratio;else if(knob==3)edit.release=ratio;}
    engine.setEdit(pad,edit);
}

std::wstring surfaceProgramParameterName(int page,int knob)const{
    static const std::array<std::wstring,8> names={L"Volume",L"Accordage",L"Début",L"Fin",L"Attack",L"Decay",L"Sustain",L"Release"};return names[std::clamp(page*4+knob,0,7)];
}

std::wstring surfaceProgramParameterValue(const lab::PadEdit& edit,int page,int knob)const{
    if(page==0){if(knob==0)return decimal(edit.gain*100,0)+L" %";if(knob==1)return (edit.semitones>=0?L"+":L"")+decimal(edit.semitones,0)+L" st";if(knob==2)return decimal(edit.start*100,1)+L" %";return decimal(edit.end*100,1)+L" %";}
    if(knob==0)return decimal(edit.attack*edit.attack*2000,0)+L" ms";if(knob==1)return decimal(edit.decay*edit.decay*2000,0)+L" ms";if(knob==2)return decimal(edit.sustain*100,0)+L" %";return decimal(edit.release*edit.release*4000,0)+L" ms";
}

void showSurfaceMidiControl(int display){
    display=std::clamp(display,0,1);auto& binding=surface.displays[display];
    binding.module=lab::DisplayModule::MidiControl;binding.page=0;surface.focusDisplay(display);
    status=L"MIDI Control — 16 knobs logiques, huit banques de pads Maschine+ et raccourcis KICK.";
}

std::wstring surfaceBrowserModeName(int mode)const{
    return mode==1?L"AUD AUTO":mode==2?L"AUD MAN":L"AUD OFF";
}

void auditionSurfaceBrowser(int display){
    display=std::clamp(display,0,lab::SurfaceDisplays-1);auto& browser=browsers[display];
    if(browser.selected<0||browser.selected>=int(browser.files.size())){engine.stopPreview();return;}
    std::string error;
    if(engine.preview(browser.files[browser.selected].u8string(),error))status=L"Préécoute one-shot : "+browser.files[browser.selected].filename().wstring();
    else status=L"Préécoute impossible : "+wide(error);
}

void refreshSurfaceBrowser(int display,bool audition=false){
    display=std::clamp(display,0,lab::SurfaceDisplays-1);auto& browser=browsers[display];
    browser.category=std::clamp(browser.category,0,int(libraryCategories.size())-1);browser.files.clear();
    std::error_code error;const auto folder=library/libraryCategories[browser.category];
    for(std::filesystem::directory_iterator it(folder,error),end;!error&&it!=end;it.increment(error)){
        if(!it->is_regular_file(error))continue;auto extension=it->path().extension().wstring();
        std::transform(extension.begin(),extension.end(),extension.begin(),[](wchar_t c){return wchar_t(std::towlower(c));});
        if(extension==L".wav")browser.files.push_back(it->path());
    }
    std::sort(browser.files.begin(),browser.files.end(),[](const auto& a,const auto& b){return a.filename().wstring()<b.filename().wstring();});
    if(browser.files.empty()){browser.selected=browser.scroll=0;engine.stopPreview();}
    else{browser.selected=std::clamp(browser.selected,0,int(browser.files.size())-1);browser.scroll=std::clamp(browser.scroll,0,std::max(0,int(browser.files.size())-5));if(browser.selected<browser.scroll)browser.scroll=browser.selected;if(browser.selected>=browser.scroll+5)browser.scroll=browser.selected-4;if(audition&&browser.auditionMode==1)auditionSurfaceBrowser(display);}
}

void moveSurfaceBrowserSelection(int display,int delta){
    auto& browser=browsers[std::clamp(display,0,lab::SurfaceDisplays-1)];if(browser.files.empty()||!delta)return;
    const int previous=browser.selected;browser.selected=std::clamp(browser.selected+(delta>0?1:-1),0,int(browser.files.size())-1);
    if(browser.selected<browser.scroll)browser.scroll=browser.selected;if(browser.selected>=browser.scroll+5)browser.scroll=browser.selected-4;
    if(browser.selected!=previous&&browser.auditionMode==1)auditionSurfaceBrowser(display);
}

void moveSurfaceBrowserCategory(int display,int delta){
    auto& browser=browsers[std::clamp(display,0,lab::SurfaceDisplays-1)];if(!delta)return;
    const int category=std::clamp(browser.category+(delta>0?1:-1),0,int(libraryCategories.size())-1);if(category==browser.category)return;
    browser.category=category;browser.selected=browser.scroll=0;refreshSurfaceBrowser(display,true);
}

void loadSurfaceBrowserSelection(int display){
    display=std::clamp(display,0,lab::SurfaceDisplays-1);auto& browser=browsers[display];if(browser.files.empty()){status=L"Browser KICK — cette catégorie ne contient encore aucun WAV.";return;}
    auto& binding=surface.displays[display];auto snapshot=engine.snapshot();const int local=std::clamp(binding.pad,0,lab::Pads-1),pad=lab::programPad(snapshot.bank,local);std::string error;engine.stopPreview();
    if(engine.load(pad,browser.files[browser.selected].u8string(),error,true)){selectPad(local);status=L"Sample chargé sur le pad "+bankPadName(local)+L" sans modifier ses réglages : "+browser.files[browser.selected].filename().wstring();}
    else status=L"Chargement impossible : "+wide(error);
}

void showSurfaceBrowser(int display){
    display=std::clamp(display,0,lab::SurfaceDisplays-1);auto& binding=surface.displays[display];binding.module=lab::DisplayModule::Browser;binding.pad=selected;binding.page=0;surface.focusDisplay(display);refreshSurfaceBrowser(display,true);
    status=L"Browser KICK — flèches/encodeur : parcourir · Enter : charger · Échap : fermer.";
}

void closeSurfaceModule(){
    const int display=surface.focusedDisplay;if(surface.displays[display].module==lab::DisplayModule::Browser)engine.stopPreview();
    if(surface.displays[display].module!=lab::DisplayModule::Menu){showSurfaceMenu(display);status=L"Retour au menu KICK de l'écran "+std::wstring(display?L"B.":L"A.");}
    else status=L"KICK / Retour — aucun écran enfant ouvert.";
}

void setSurfaceQLink(int display,int knob,double ratio,int layer=0){
    if(display<0||display>=lab::SurfaceDisplays||knob<0||knob>=lab::SurfaceQLinks)return;
    auto& binding=surface.displays[display];ratio=std::clamp(ratio,0.,1.);if(binding.module==lab::DisplayModule::Sample){setSurfaceProgramRatio(display,knob,ratio,layer);return;}if(layer>0&&binding.module!=lab::DisplayModule::Inserts)return;
    if(binding.module==lab::DisplayModule::PadPerform){auto snapshot=engine.snapshot();if(knob==0)engine.setPerformanceMode(lab::PerformanceMode(std::clamp(int(std::lround(ratio*3)),0,3)));else if(knob==1)engine.setMelodicSourcePad(lab::programPad(snapshot.bank,std::clamp(int(std::lround(ratio*15)),0,15)));else if(knob==2)engine.setRootNote(24+std::clamp(int(std::lround(ratio*60)),0,60));else engine.setViewBar(std::clamp(int(std::lround(ratio*(snapshot.sequences[snapshot.sequence].bars-1))),0,snapshot.sequences[snapshot.sequence].bars-1));return;}
    if(binding.module==lab::DisplayModule::NextSequence){auto snapshot=engine.snapshot();if(knob==0)engine.selectSequence(std::clamp(int(std::lround(ratio*15)),0,15));else if(knob==1)engine.setSequenceBars(1+std::clamp(int(std::lround(ratio*3)),0,3));else if(knob==2)engine.setViewBar(std::clamp(int(std::lround(ratio*(snapshot.sequences[snapshot.sequence].bars-1))),0,snapshot.sequences[snapshot.sequence].bars-1));else engine.setPerformanceMode(lab::PerformanceMode(std::clamp(int(std::lround(ratio*3)),0,3)));return;}
    if(binding.module==lab::DisplayModule::Song){auto snapshot=engine.snapshot();if(knob==0)songChoice=std::clamp(int(std::lround(ratio*15)),0,15);else if(knob==1)songRepeats=1+std::clamp(int(std::lround(ratio*127)),0,127);else if(knob==2)songSelected=std::clamp(int(std::lround(ratio*std::max(0,snapshot.songLength-1))),0,std::max(0,snapshot.songLength-1));return;}
    if(binding.module!=lab::DisplayModule::Inserts)return;
    auto s=engine.snapshot();const int pad=surfaceProgramPad(binding,s),slot=std::clamp(binding.slot,0,lab::InsertSlots-1);
    const int parameter=(binding.page+layer*2)*lab::SurfaceQLinks+knob;const auto effect=s.edits[pad].inserts[slot].effect;
    if(parameter>=0&&parameter<lab::insertParameterCount(effect))engine.setInsertParameter(pad,slot,parameter,ratio);
}

void turnSurfaceQLink(int display,int knob,int delta,int layer=0){
    if(display<0||display>=lab::SurfaceDisplays||knob<0||knob>=lab::SurfaceQLinks||!delta)return;
    auto& binding=surface.displays[display];
    if(binding.module==lab::DisplayModule::Sample){auto snapshot=engine.snapshot();const int page=binding.page+layer*2;if(page>1)return;const auto& edit=snapshot.edits[surfaceProgramPad(binding,snapshot)];const double step=page==0&&knob==1?1./48.:.01;setSurfaceProgramRatio(display,knob,surfaceProgramRatio(edit,page,knob)+delta*step,layer);return;}if(layer>0&&binding.module!=lab::DisplayModule::Inserts)return;
    if(binding.module==lab::DisplayModule::PadPerform){auto snapshot=engine.snapshot();const auto& sequence=snapshot.sequences[snapshot.sequence];if(knob==0)engine.setPerformanceMode(lab::PerformanceMode(std::clamp(int(sequence.mode)+(delta>0?1:-1),0,3)));else if(knob==1){const int local=std::clamp(sequence.sourcePad%lab::Pads+(delta>0?1:-1),0,lab::Pads-1);engine.setMelodicSourcePad(lab::programPad(snapshot.bank,local));}else if(knob==2)engine.setRootNote(sequence.rootNote+(delta>0?1:-1));else engine.setViewBar(snapshot.viewBar+(delta>0?1:-1));return;}
    if(binding.module==lab::DisplayModule::NextSequence){auto snapshot=engine.snapshot();const auto& sequence=snapshot.sequences[snapshot.sequence];if(knob==0)engine.selectSequence(snapshot.sequence+(delta>0?1:-1));else if(knob==1)engine.setSequenceBars(sequence.bars+(delta>0?1:-1));else if(knob==2)engine.setViewBar(snapshot.viewBar+(delta>0?1:-1));else engine.setPerformanceMode(lab::PerformanceMode(std::clamp(int(sequence.mode)+(delta>0?1:-1),0,3)));return;}
    if(binding.module==lab::DisplayModule::Song){auto snapshot=engine.snapshot();if(knob==0)songChoice=std::clamp(songChoice+(delta>0?1:-1),0,15);else if(knob==1)songRepeats=std::clamp(songRepeats+(delta>0?1:-1),1,128);else if(knob==2)songSelected=std::clamp(songSelected+(delta>0?1:-1),0,std::max(0,snapshot.songLength-1));return;}
    if(binding.module!=lab::DisplayModule::Inserts)return;
    auto s=engine.snapshot();
    const int pad=surfaceProgramPad(binding,s),slot=std::clamp(binding.slot,0,lab::InsertSlots-1);
    const auto& insert=s.edits[pad].inserts[slot];
    const int parameter=(binding.page+layer*2)*lab::SurfaceQLinks+knob;
    if(parameter<0||parameter>=lab::insertParameterCount(insert.effect))return;
    engine.setInsertParameter(pad,slot,parameter,insert.parameters[parameter]+delta*.01);
}

std::wstring midiModeName(lab::MidiCcMode mode)const{
    if(mode==lab::MidiCcMode::Relative1)return L"REL 1/127";
    if(mode==lab::MidiCcMode::Relative65)return L"REL 63/65";
    return L"ABS";
}

std::wstring midiInputName()const{
    if(midiId<0)return L"Choisir une entrée MIDI";
    MIDIINCAPSW caps{};return midiInGetDevCapsW(UINT(midiId),&caps,sizeof(caps))==MMSYSERR_NOERROR?caps.szPname:L"Entrée MIDI active";
}

std::wstring midiLastMessage()const{
    if(!midiMessageCount)return L"Aucun message MIDI reçu";
    const int midiStatus=midiLastPacked&255,type=midiStatus&0xf0,channel=midiStatus&15,data1=(midiLastPacked>>8)&127,data2=(midiLastPacked>>16)&127;
    std::wstring kind=type==0x90?L"NOTE ON":type==0x80?L"NOTE OFF":type==0xb0?L"CC":L"MIDI";
    return kind+L"  canal "+number(channel+1)+L"  donnée "+number(data1)+L"  valeur "+number(data2)+L"  ·  #"+number(int(midiMessageCount));
}

std::wstring midiCommandBindingText(const lab::MidiCommandBinding& binding)const{
    if(binding.kind==lab::MidiMessageKind::None)return L"Non assigné";
    return std::wstring(binding.kind==lab::MidiMessageKind::Note?L"Note ":L"CC ")+number(binding.number)+L" · canal "+number(binding.channel+1);
}

std::wstring midiPadBindingText(const lab::MidiPadBinding& binding)const{
    if(binding.note<0)return L"Non assigné";
    return L"Note "+number(binding.note)+L" · canal "+number(binding.channel+1);
}

void surfacePointer(int y){
    if(surfaceDragDisplay<0)return;
    const int step=(surfaceDragStartY-y)/4;
    if(step!=surfaceDragLastStep){
        dispatch({lab::SurfaceAction::QLinkTurn,surfaceDragKnob,step-surfaceDragLastStep,surfaceDragDisplay});
        surfaceDragLastStep=step;
    }
}

void selectSurfaceMenu(int item){
    item=std::clamp(item,0,15);
    const int display=item/8;
    surface.menuSelections[display]=item%8;
    surface.focusDisplay(display);
}

void moveSurfaceMenu(int dx,int dy){
    const auto module=surface.displays[surface.focusedDisplay].module;
    if(module==lab::DisplayModule::Browser){if(dx)moveSurfaceBrowserCategory(surface.focusedDisplay,dx);if(dy)moveSurfaceBrowserSelection(surface.focusedDisplay,dy);return;}
    if(module==lab::DisplayModule::NextSequence){if(dx)turnSurfaceQLink(surface.focusedDisplay,0,dx);if(dy)turnSurfaceQLink(surface.focusedDisplay,2,dy);return;}
    if(module==lab::DisplayModule::PadPerform){if(dx)turnSurfaceQLink(surface.focusedDisplay,0,dx);if(dy)turnSurfaceQLink(surface.focusedDisplay,2,-dy);return;}
    if(module==lab::DisplayModule::Song){if(dx)turnSurfaceQLink(surface.focusedDisplay,1,dx);if(dy)turnSurfaceQLink(surface.focusedDisplay,2,dy);return;}
    if(module==lab::DisplayModule::MidiControl&&dx){auto& binding=surface.displays[surface.focusedDisplay];binding.page=std::clamp(binding.page+(dx>0?1:-1),0,surfaceMidiPages-1);return;}
    if(module==lab::DisplayModule::Sample&&dx){auto& binding=surface.displays[surface.focusedDisplay];binding.page=std::clamp(binding.page+(dx>0?1:-1),0,1);return;}
    if(module==lab::DisplayModule::Inserts&&dx){auto& binding=surface.displays[surface.focusedDisplay];auto snapshot=engine.snapshot();const auto effect=snapshot.edits[surfaceProgramPad(binding,snapshot)].inserts[binding.slot].effect;const int pages=std::max(1,(lab::insertParameterCount(effect)+3)/4);binding.page=std::clamp(binding.page+(dx>0?1:-1),0,pages-1);return;}
    if(module!=lab::DisplayModule::Menu)return;
    const int first=surface.focusedDisplay*8;
    int local=surface.menuSelections[surface.focusedDisplay];
    int row=local/2,col=local%2;
    col=std::clamp(col+dx,0,1);row=std::clamp(row+dy,0,3);
    selectSurfaceMenu(first+row*2+col);
}

void turnSurfaceMenu(int delta){
    if(!delta)return;const auto module=surface.displays[surface.focusedDisplay].module;
    if(module==lab::DisplayModule::Browser){moveSurfaceBrowserSelection(surface.focusedDisplay,delta);return;}
    if(module==lab::DisplayModule::NextSequence){turnSurfaceQLink(surface.focusedDisplay,0,delta);return;}
    if(module==lab::DisplayModule::PadPerform){turnSurfaceQLink(surface.focusedDisplay,2,delta);return;}
    if(module==lab::DisplayModule::Song){turnSurfaceQLink(surface.focusedDisplay,2,delta);return;}
    if(module==lab::DisplayModule::Inserts){auto& binding=surface.displays[surface.focusedDisplay];binding.slot=std::clamp(binding.slot+(delta>0?1:-1),0,lab::InsertSlots-1);binding.page=0;selectedInsert=binding.slot;return;}
    if(module!=lab::DisplayModule::Menu)return;
    int item=surface.focusedDisplay*8+surface.menuSelections[surface.focusedDisplay];
    selectSurfaceMenu(std::clamp(item+(delta>0?1:-1),0,15));
}

void activateSurfaceMenu(){
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::Browser){loadSurfaceBrowserSelection(surface.focusedDisplay);return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::Song){engine.addSongStep(songChoice,songRepeats);auto snapshot=engine.snapshot();songSelected=std::max(0,snapshot.songLength-1);return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::Inserts){auto& binding=surface.displays[surface.focusedDisplay];selectedInsert=binding.slot;insertMenu();binding.page=0;return;}
    if(surface.displays[surface.focusedDisplay].module!=lab::DisplayModule::Menu)return;
    const int item=surface.focusedDisplay*8+surface.menuSelections[surface.focusedDisplay];
    if(item==0){showSurfaceBrowser(surface.focusedDisplay);return;}
    if(item==3){showSurfaceNextSequence(surface.focusedDisplay);return;}
    if(item==7){showSurfaceSong(surface.focusedDisplay);return;}
    if(item==11){showSurfaceProgram(surface.focusedDisplay);return;}
    if(item==12){showSurfacePadPerform(surface.focusedDisplay);return;}
    if(item==15){showSurfaceMidiControl(surface.focusedDisplay);return;}
    if(item==9||item==10){tab=1;status=L"KICK / "+surfaceMenuLabels[item]+L" — éditeur transitoire, avant intégration complète dans les deux écrans.";return;}
    status=L"KICK / "+surfaceMenuLabels[item]+L" sélectionné — module original pas encore relié à cette base Windows.";
}

bool surfaceWheel(int x,int y,int delta){
    if(tab!=0)return false;
    for(int display=0;display<2;++display)for(int knob=0;knob<4;++knob)if(surfaceKnobBox(display,knob).contains(x,y)){
        surface.focusDisplay(display);dispatch({lab::SurfaceAction::QLinkTurn,knob,delta>0?1:-1,display});return true;
    }
    for(int display=0;display<2;++display)if(surfaceDisplayBox(display).contains(x,y)){
        surface.focusDisplay(display);
        if(surface.displays[display].module==lab::DisplayModule::Menu)turnSurfaceMenu(delta);
        else if(surface.displays[display].module==lab::DisplayModule::Browser)moveSurfaceBrowserSelection(display,delta);
        return true;
    }
    return false;
}

bool surfaceClick(int x,int y,bool right){
    for(int display=0;display<2;++display){
        for(int knob=0;knob<4;++knob)if(surfaceKnobBox(display,knob).contains(x,y)){
            surface.focusDisplay(display);
            const auto module=surface.displays[display].module;
            if(!right&&(module==lab::DisplayModule::Inserts||module==lab::DisplayModule::Sample||module==lab::DisplayModule::PadPerform||module==lab::DisplayModule::NextSequence||module==lab::DisplayModule::Song)){
                surfaceDragDisplay=display;surfaceDragKnob=knob;surfaceDragStartY=y;surfaceDragLastStep=0;SetCapture(window);
            }
            return true;
        }
        if(!surfaceDisplayBox(display).contains(x,y))continue;
        surface.focusDisplay(display);
        auto& binding=surface.displays[display];
        if(binding.module==lab::DisplayModule::Browser){
            auto& browser=browsers[display];
            if(surfaceBrowserPreviousBox(display).contains(x,y)){moveSurfaceBrowserCategory(display,-1);return true;}
            if(surfaceBrowserNextBox(display).contains(x,y)){moveSurfaceBrowserCategory(display,1);return true;}
            for(int row=0;row<5;++row)if(surfaceBrowserFileBox(display,row).contains(x,y)){const int index=browser.scroll+row;if(index<int(browser.files.size())){browser.selected=index;if(browser.auditionMode==1)auditionSurfaceBrowser(display);}return true;}
            if(surfaceBrowserModeBox(display).contains(x,y)){browser.auditionMode=(browser.auditionMode+1)%3;if(browser.auditionMode==0)engine.stopPreview();else if(browser.auditionMode==1)auditionSurfaceBrowser(display);status=L"Browser KICK — mode de préécoute "+surfaceBrowserModeName(browser.auditionMode)+L".";return true;}
            if(surfaceBrowserAuditionBox(display).contains(x,y)){if(browser.auditionMode!=0)auditionSurfaceBrowser(display);return true;}
            if(surfaceBrowserLoadBox(display).contains(x,y)){loadSurfaceBrowserSelection(display);return true;}
            if(surfaceBrowserRefreshBox(display).contains(x,y)){refreshSurfaceBrowser(display,true);status=L"Bibliothèque actualisée : "+number(int(browser.files.size()))+L" WAV dans "+libraryCategories[browser.category]+L".";return true;}
            return true;
        }
        if(binding.module==lab::DisplayModule::Inserts){
            auto current=engine.snapshot();const int programPad=surfaceProgramPad(binding,current);const int pages=std::max(1,(lab::insertParameterCount(current.edits[programPad].inserts[binding.slot].effect)+3)/4);
            if(surfacePagePreviousBox(display).contains(x,y)){binding.page=std::max(0,binding.page-1);return true;}
            if(surfacePageNextBox(display).contains(x,y)){binding.page=std::min(pages-1,binding.page+1);return true;}
            for(int slot=0;slot<lab::InsertSlots;++slot)if(surfaceInsertSlotBox(display,slot).contains(x,y)){
                binding.slot=slot;binding.page=0;selectedInsert=slot;return true;
            }
            if(surfaceInsertEffectBox(display).contains(x,y)){selectedInsert=binding.slot;insertMenu();binding.page=0;return true;}
            if(surfaceInsertBypassBox(display).contains(x,y)){
                auto s=engine.snapshot();const int pad=surfaceProgramPad(binding,s);const auto& insert=s.edits[pad].inserts[binding.slot];
                if(insert.effect!=lab::InsertEffect::None)engine.setInsertBypass(pad,binding.slot,!insert.bypass);
                return true;
            }
            return true;
        }
        if(binding.module==lab::DisplayModule::Sample){
            if(surfacePagePreviousBox(display).contains(x,y)){binding.page=std::max(0,binding.page-1);return true;}
            if(surfacePageNextBox(display).contains(x,y)){binding.page=std::min(1,binding.page+1);return true;}
            if(surfaceProgramAuditionBox(display).contains(x,y)){auto s=engine.snapshot();engine.trigger(surfaceProgramPad(binding,s));return true;}
            return true;
        }
        if(binding.module==lab::DisplayModule::Song){
            auto snapshot=engine.snapshot();
            if(surfaceSongActionBox(display,0).contains(x,y)){engine.addSongStep(songChoice,songRepeats);snapshot=engine.snapshot();songSelected=std::max(0,snapshot.songLength-1);status=L"Song : séquence "+number(songChoice+1)+L" ajoutée ×"+number(songRepeats)+L".";return true;}
            if(surfaceSongActionBox(display,1).contains(x,y)){engine.deleteSongStep(songSelected);snapshot=engine.snapshot();songSelected=std::clamp(songSelected,0,std::max(0,snapshot.songLength-1));return true;}
            if(surfaceSongActionBox(display,2).contains(x,y)){if(snapshot.songPlaying||snapshot.playing)engine.stop();else engine.playSong();return true;}
            if(surfaceSongActionBox(display,3).contains(x,y)){engine.clearSong();songSelected=0;return true;}
            const int first=std::clamp(songSelected-1,0,std::max(0,snapshot.songLength-4));for(int row=0;row<4;++row)if(surfaceSongRowBox(display,row).contains(x,y)){const int index=first+row;if(index<snapshot.songLength){songSelected=index;songChoice=snapshot.song[index].sequence;songRepeats=snapshot.song[index].repeats;}return true;}
            return true;
        }
        if(binding.module==lab::DisplayModule::PadPerform||binding.module==lab::DisplayModule::NextSequence)return true;
        if(binding.module==lab::DisplayModule::MidiControl){
            if(surfaceMidiDeviceBox(display).contains(x,y)){midiMenu();return true;}
            if(surfaceMidiPreviousBox(display).contains(x,y)){binding.page=std::max(0,binding.page-1);return true;}
            if(surfaceMidiNextBox(display).contains(x,y)){binding.page=std::min(surfaceMidiPages-1,binding.page+1);return true;}
            for(int row=0;row<4;++row)if(surfaceMidiBindingBox(display,row).contains(x,y)){
                if(binding.page<surfaceMidiQlinkPages){const int target=binding.page*4+row;auto& learned=midiQlinks[target];
                    if(right){learned.mode=learned.mode==lab::MidiCcMode::Absolute?lab::MidiCcMode::Relative1:learned.mode==lab::MidiCcMode::Relative1?lab::MidiCcMode::Relative65:lab::MidiCcMode::Absolute;const bool saved=saveMidiProfile();status=L"P"+number(target+1)+L" : mode "+midiModeName(learned.mode)+(saved?L" · profil sauvegardé.":L" · sauvegarde impossible.");}
                    else {midiLearnCommand=midiLearnPad=-1;midiLearnTarget=target;status=L"MIDI Learn armé pour P"+number(target+1)+L" — tourne maintenant le contrôle physique.";}
                }else if(binding.page<surfaceMidiQlinkPages+surfaceMidiPadPages){const int target=(binding.page-surfaceMidiQlinkPages)*4+row;auto& learned=midiPads[target];
                    if(right){learned={};const bool saved=saveMidiProfile();status=L"Pad "+number(target+1)+(saved?L" : affectation effacée.":L" : effacement non sauvegardé.");}
                    else {midiLearnTarget=midiLearnCommand=-1;midiLearnPad=target;status=L"MIDI Learn armé pour PAD "+number(target+1)+L" — frappe maintenant le pad physique.";}
                }else {const int target=(binding.page-surfaceMidiQlinkPages-surfaceMidiPadPages)*4+row;if(target>=lab::MidiCommandCount)return true;auto& learned=midiCommands[target];
                    if(right&&target==int(lab::MidiCommandTarget::Turn)){learned.mode=learned.mode==lab::MidiCcMode::Absolute?lab::MidiCcMode::Relative1:learned.mode==lab::MidiCcMode::Relative1?lab::MidiCcMode::Relative65:lab::MidiCcMode::Absolute;const bool saved=saveMidiProfile();status=L"Rotation 4D : mode "+midiModeName(learned.mode)+(saved?L" · profil sauvegardé.":L" · sauvegarde impossible.");}
                    else if(right){learned={};const bool saved=saveMidiProfile();status=midiCommandLabels[target]+(saved?L" : affectation effacée.":L" : effacement non sauvegardé.");}
                    else {midiLearnTarget=midiLearnPad=-1;midiLearnCommand=target;status=L"MIDI Learn armé pour "+midiCommandLabels[target]+L" — actionne le contrôle physique.";}
                }
                return true;
            }
            return true;
        }
        for(int local=0;local<8;++local)if(surfaceDisplayItemBox(display,local).contains(x,y)){
            selectSurfaceMenu(display*8+local);activateSurfaceMenu();return true;
        }
        return true;
    }
    return false;
}

void paintSurfaceKnob(HDC dc,int display,int knob,const lab::Snapshot& snapshot){
    const int cx=538+display*580,cy=112+knob*67;
    double ratio=.5;bool assigned=false;
    const auto& binding=surface.displays[display];
    const int layer=knobLayer,effectivePage=binding.page+layer*2;
    if(binding.module==lab::DisplayModule::Sample){if(effectivePage<2){ratio=surfaceProgramRatio(snapshot.edits[surfaceProgramPad(binding,snapshot)],effectivePage,knob);assigned=true;}}
    else if(binding.module==lab::DisplayModule::PadPerform&&layer==0){const auto& sequence=snapshot.sequences[snapshot.sequence];if(knob==0)ratio=int(sequence.mode)/3.;else if(knob==1)ratio=(sequence.sourcePad%lab::Pads)/15.;else if(knob==2)ratio=(sequence.rootNote-24)/60.;else ratio=sequence.bars>1?double(snapshot.viewBar)/(sequence.bars-1):0;assigned=true;}
    else if(binding.module==lab::DisplayModule::NextSequence&&layer==0){const auto& sequence=snapshot.sequences[snapshot.sequence];if(knob==0)ratio=snapshot.sequence/15.;else if(knob==1)ratio=(sequence.bars-1)/3.;else if(knob==2)ratio=sequence.bars>1?double(snapshot.viewBar)/(sequence.bars-1):0;else ratio=int(sequence.mode)/3.;assigned=true;}
    else if(binding.module==lab::DisplayModule::Song&&layer==0){if(knob==0)ratio=songChoice/15.;else if(knob==1)ratio=(songRepeats-1)/127.;else if(knob==2)ratio=snapshot.songLength>1?double(songSelected)/(snapshot.songLength-1):0;else ratio=0;assigned=knob<3;}
    else if(binding.module==lab::DisplayModule::Inserts){
        const auto& insert=snapshot.edits[surfaceProgramPad(binding,snapshot)].inserts[binding.slot];
        const int parameter=effectivePage*lab::SurfaceQLinks+knob;
        if(parameter>=0&&parameter<lab::insertParameterCount(insert.effect)){ratio=insert.parameters[parameter];assigned=true;}
    }
    HBRUSH brush=CreateSolidBrush(RGB(31,38,50));HPEN edge=CreatePen(PS_SOLID,2,line);
    auto oldB=SelectObject(dc,brush),oldP=SelectObject(dc,edge);Ellipse(dc,cx-19,cy-19,cx+19,cy+19);SelectObject(dc,oldB);SelectObject(dc,oldP);DeleteObject(brush);DeleteObject(edge);
    constexpr double pi=3.14159265358979323846;const double angle=(135.+270.*ratio)*pi/180.;
    HPEN mark=CreatePen(PS_SOLID,3,assigned?accent:muted);oldP=SelectObject(dc,mark);MoveToEx(dc,cx,cy,nullptr);LineTo(dc,cx+int(std::cos(angle)*12),cy+int(std::sin(angle)*12));SelectObject(dc,oldP);DeleteObject(mark);
    text(dc,{cx-24,cy+21,48,16},L"P"+number(layer*8+display*4+knob+1),9,assigned?accent:muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

void paintSurfaceDisplay(HDC dc,int display,const lab::Snapshot& snapshot){
    auto module=surfaceModuleBox(display),box=surfaceDisplayBox(display);bool focused=surface.focusedDisplay==display;
    round(dc,module,panel,focused?accent:line);
    round(dc,box,RGB(9,18,22),focused?accent:RGB(34,48,57));
    const auto& binding=surface.displays[display];
    if(binding.module==lab::DisplayModule::Browser){
        const auto& browser=browsers[display];
        text(dc,{box.x+14,box.y+9,290,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  BROWSER  ·  PAD "+bankPadName(binding.pad),12,focused?accent:muted,FW_SEMIBOLD);
        text(dc,{box.x+310,box.y+9,130,24},number(int(browser.files.size()))+L" WAV",9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
        fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        button(dc,surfaceBrowserPreviousBox(display),L"<");button(dc,surfaceBrowserCategoryBox(display),libraryCategories[browser.category],true);button(dc,surfaceBrowserNextBox(display),L">");
        for(int row=0;row<5;++row){auto b=surfaceBrowserFileBox(display,row);const int index=browser.scroll+row;const bool selectedFile=index==browser.selected&&index<int(browser.files.size());
            if(selectedFile)round(dc,b,accent,accent);else fill(dc,b,row%2?RGB(18,31,35):RGB(17,28,33));
            const std::wstring name=index<int(browser.files.size())?browser.files[index].filename().wstring():(row==0&&browser.files.empty()?L"Bibliothèque vide — ajoute des WAV via Bibliothèque":L"");
            text(dc,{b.x+10,b.y,b.w-20,b.h},name,10,selectedFile?bg:(browser.files.empty()?muted:ink),selectedFile?FW_SEMIBOLD:FW_NORMAL);
        }
        button(dc,surfaceBrowserModeBox(display),surfaceBrowserModeName(browser.auditionMode),browser.auditionMode>0);button(dc,surfaceBrowserAuditionBox(display),L"Écouter",browser.auditionMode>0);button(dc,surfaceBrowserLoadBox(display),L"Charger",!browser.files.empty());button(dc,surfaceBrowserRefreshBox(display),L"Actualiser");
        return;
    }
    if(binding.module==lab::DisplayModule::PadPerform){
        const auto& sequence=snapshot.sequences[snapshot.sequence];text(dc,{box.x+14,box.y+9,360,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  PAD NOTES / KEYGROUP",12,focused?accent:muted,FW_SEMIBOLD);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        const std::array<std::wstring,4> labels={L"Type",L"Sample source",L"Note racine",L"Mesure affichée"};const std::array<std::wstring,4> values={performanceModeName(sequence.mode),L"Pad "+std::wstring(1,wchar_t(L'A'+sequence.sourcePad/lab::Pads))+number(sequence.sourcePad%lab::Pads+1)+L" · "+wide(snapshot.names[sequence.sourcePad]),noteName(sequence.rootNote),number(snapshot.viewBar+1)+L" / "+number(sequence.bars)};
        for(int knob=0;knob<4;++knob){const int y=128+knob*55;fill(dc,{box.x+14,y,box.w-28,46},knob%2?RGB(18,31,35):RGB(17,28,33));text(dc,{box.x+24,y,42,46},L"P"+number(display*4+knob+1),10,accent,FW_SEMIBOLD);text(dc,{box.x+68,y,144,46},labels[knob],10,muted);text(dc,{box.x+210,y,216,46},values[knob],11,ink,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);}return;
    }
    if(binding.module==lab::DisplayModule::NextSequence){
        const auto& sequence=snapshot.sequences[snapshot.sequence];text(dc,{box.x+14,box.y+9,360,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  NEXT SEQUENCE",12,focused?accent:muted,FW_SEMIBOLD);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        const std::array<std::wstring,4> labels={L"Séquence",L"Longueur",L"Mesure édition",L"Vue séquenceur"};const std::array<std::wstring,4> values={number(snapshot.sequence+1)+L" · "+wide(sequence.name),number(sequence.bars)+L" mesure(s)",number(snapshot.viewBar+1)+L" / "+number(sequence.bars),performanceModeName(sequence.mode)};
        for(int knob=0;knob<4;++knob){const int y=128+knob*55;fill(dc,{box.x+14,y,box.w-28,46},knob%2?RGB(18,31,35):RGB(17,28,33));text(dc,{box.x+24,y,42,46},L"P"+number(display*4+knob+1),10,accent,FW_SEMIBOLD);text(dc,{box.x+68,y,144,46},labels[knob],10,muted);text(dc,{box.x+210,y,216,46},values[knob],11,ink,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);}text(dc,{box.x+14,348,box.w-28,18},snapshot.queuedSequence>=0?L"En attente : Sequence "+number(snapshot.queuedSequence+1):L"Pads 1–16 : sélectionner / mettre en attente",9,snapshot.queuedSequence>=0?RGB(241,184,88):muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
    }
    if(binding.module==lab::DisplayModule::Song){
        text(dc,{box.x+14,box.y+9,360,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  SONG EDITOR",12,focused?accent:muted,FW_SEMIBOLD);text(dc,{box.x+330,box.y+9,96,24},number(snapshot.songLength)+L" étapes",9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));button(dc,surfaceSongActionBox(display,0),L"Ajouter");button(dc,surfaceSongActionBox(display,1),L"Supprimer");button(dc,surfaceSongActionBox(display,2),snapshot.songPlaying?L"Stop":L"Lire",snapshot.songPlaying);button(dc,surfaceSongActionBox(display,3),L"Effacer");
        const int first=std::clamp(songSelected-1,0,std::max(0,snapshot.songLength-4));for(int row=0;row<4;++row){const int index=first+row;auto b=surfaceSongRowBox(display,row);const bool selectedRow=index==songSelected&&index<snapshot.songLength,playing=index==snapshot.songPosition&&snapshot.songPlaying;if(selectedRow)round(dc,b,accent,accent);else fill(dc,b,row%2?RGB(18,31,35):RGB(17,28,33));std::wstring value=index<snapshot.songLength?number(index+1)+L"   SEQ "+number(snapshot.song[index].sequence+1)+L"   × "+number(snapshot.song[index].repeats):(row==0&&snapshot.songLength==0?L"Song vide — choisis un pad puis Ajouter":L"");text(dc,{b.x+10,b.y,b.w-20,b.h},value,10,selectedRow?bg:(playing?accent:muted),selectedRow?FW_SEMIBOLD:FW_NORMAL);}
        text(dc,{box.x+14,350,box.w-28,16},L"P1 Seq "+number(songChoice+1)+L" · P2 Repeat ×"+number(songRepeats)+L" · P3 Étape "+number(snapshot.songLength?songSelected+1:0),9,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
    }
    if(binding.module==lab::DisplayModule::MidiControl){
        text(dc,{box.x+14,box.y+9,270,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  MIDI CONTROL",12,focused?accent:muted,FW_SEMIBOLD);
        text(dc,{box.x+326,box.y+9,114,24},L"PAGE "+number(binding.page+1)+L" / "+number(surfaceMidiPages),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
        fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        button(dc,surfaceMidiDeviceBox(display),midiInputName(),midiId>=0);button(dc,surfaceMidiPreviousBox(display),L"<");button(dc,surfaceMidiNextBox(display),L">");
        text(dc,{box.x+14,170,box.w-28,22},midiLastMessage(),9,midiMessageCount?accent:muted,FW_NORMAL,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
        const bool anyLearn=midiLearnTarget>=0||midiLearnPad>=0||midiLearnCommand>=0;std::wstring learnText=midiLearnTarget>=0?L"EN ATTENTE : bouge le contrôle pour P"+number(midiLearnTarget+1):midiLearnPad>=0?L"EN ATTENTE : frappe le PAD "+number(midiLearnPad+1):midiLearnCommand>=0?L"EN ATTENTE : actionne "+midiCommandLabels[midiLearnCommand]:(midiProfileLoaded?L"Profil Maschine+ chargé · clic : Learn · droit : option":L"Profil Maschine+ vide · clic : Learn · droit : option");
        text(dc,{box.x+14,194,box.w-28,22},learnText,9,anyLearn?RGB(241,184,88):muted,FW_SEMIBOLD);
        for(int row=0;row<4;++row){auto b=surfaceMidiBindingBox(display,row);
            if(binding.page<surfaceMidiQlinkPages){const int target=binding.page*4+row;const auto& learned=midiQlinks[target];const bool learning=midiLearnTarget==target;
                if(learning)round(dc,b,RGB(64,49,25),RGB(241,184,88));else fill(dc,b,row%2?RGB(18,31,35):RGB(17,28,33));
                text(dc,{b.x+10,b.y,52,b.h},L"P"+number(target+1),10,learning?RGB(241,184,88):accent,FW_SEMIBOLD);
                text(dc,{b.x+62,b.y,190,b.h},learned.cc<0?L"Non assigné":L"CC "+number(learned.cc)+L" · canal "+number(learned.channel+1),10,learned.cc<0?muted:ink);
                text(dc,{b.x+262,b.y,122,b.h},midiModeName(learned.mode),9,muted,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
            }else if(binding.page<surfaceMidiQlinkPages+surfaceMidiPadPages){const int target=(binding.page-surfaceMidiQlinkPages)*4+row;const auto& learned=midiPads[target];const bool learning=midiLearnPad==target;
                if(learning)round(dc,b,RGB(64,49,25),RGB(241,184,88));else fill(dc,b,row%2?RGB(18,31,35):RGB(17,28,33));
                text(dc,{b.x+10,b.y,74,b.h},L"PAD "+number(target+1),10,learning?RGB(241,184,88):accent,FW_SEMIBOLD);
                text(dc,{b.x+86,b.y,298,b.h},learning?L"EN ATTENTE — frappe le pad":midiPadBindingText(learned),10,learning?RGB(241,184,88):(learned.note<0?muted:ink));
            }else {const int target=(binding.page-surfaceMidiQlinkPages-surfaceMidiPadPages)*4+row;if(target>=lab::MidiCommandCount)continue;const auto& learned=midiCommands[target];const bool learning=midiLearnCommand==target;
                if(learning)round(dc,b,RGB(64,49,25),RGB(241,184,88));else fill(dc,b,row%2?RGB(18,31,35):RGB(17,28,33));
                text(dc,{b.x+10,b.y,104,b.h},midiCommandLabels[target],10,learning?RGB(241,184,88):accent,FW_SEMIBOLD);
                text(dc,{b.x+116,b.y,174,b.h},midiCommandBindingText(learned),10,learned.kind==lab::MidiMessageKind::None?muted:ink);
                text(dc,{b.x+296,b.y,88,b.h},target==int(lab::MidiCommandTarget::Turn)?midiModeName(learned.mode):(learned.kind==lab::MidiMessageKind::Note?L"NOTE":learned.kind==lab::MidiMessageKind::ControlChange?L"CC":L""),9,muted,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
            }
        }
        return;
    }
    if(binding.module==lab::DisplayModule::Inserts){
        const int pad=surfaceProgramPad(binding,snapshot),slot=std::clamp(binding.slot,0,lab::InsertSlots-1);
        const auto& edit=snapshot.edits[pad];const auto& insert=edit.inserts[slot];const int count=lab::insertParameterCount(insert.effect);
        const int pages=std::max(1,(count+lab::SurfaceQLinks-1)/lab::SurfaceQLinks);
        text(dc,{box.x+14,box.y+9,300,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  INSERTS  ·  PAD "+bankPadName(binding.pad),12,focused?accent:muted,FW_SEMIBOLD);
        const int firstLogical=knobLayer*8+display*4+1;text(dc,{box.x+294,box.y+9,72,24},L"P"+number(firstLogical)+L"–P"+number(firstLogical+3),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);button(dc,surfacePagePreviousBox(display),L"<");button(dc,surfacePageNextBox(display),L">");
        fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        for(int s=0;s<lab::InsertSlots;++s)button(dc,surfaceInsertSlotBox(display,s),number(s+1)+L"  "+insertName(edit.inserts[s].effect),s==slot);
        button(dc,surfaceInsertEffectBox(display),insert.effect==lab::InsertEffect::None?L"Choisir un effet":L"Effet : "+insertName(insert.effect));
        button(dc,surfaceInsertBypassBox(display),insert.bypass?L"Slot bypass":L"Slot actif",insert.effect!=lab::InsertEffect::None&&!insert.bypass);
        for(int knob=0;knob<lab::SurfaceQLinks;++knob){
            const int parameter=(binding.page+knobLayer*2)*lab::SurfaceQLinks+knob,y=222+knob*34;
            const bool valid=parameter>=0&&parameter<count;
            fill(dc,{box.x+14,y,box.w-28,28},knob%2?RGB(18,31,35):RGB(17,28,33));
            text(dc,{box.x+24,y,42,28},L"P"+number(knobLayer*8+display*lab::SurfaceQLinks+knob+1),10,valid?accent:muted,FW_SEMIBOLD);
            text(dc,{box.x+68,y,220,28},valid?wide(lab::insertParameterName(insert.effect,parameter)):L"—",10,valid?ink:muted);
            text(dc,{box.x+300,y,126,28},valid?insertValue(insert.effect,parameter,insert.parameters[parameter]):L"Non assigné",10,valid?accent:muted,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
        }
        return;
    }
    if(binding.module==lab::DisplayModule::Sample){
        const int pad=surfaceProgramPad(binding,snapshot);const auto& edit=snapshot.edits[pad];
        text(dc,{box.x+14,box.y+9,300,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  PROGRAM EDIT  ·  PAD "+bankPadName(binding.pad),12,focused?accent:muted,FW_SEMIBOLD);
        const int firstLogical=knobLayer*8+display*4+1;text(dc,{box.x+294,box.y+9,72,24},L"P"+number(firstLogical)+L"–P"+number(firstLogical+3),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);button(dc,surfacePagePreviousBox(display),L"<");button(dc,surfacePageNextBox(display),L">");fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        button(dc,surfaceProgramAuditionBox(display),L"Écouter  ·  "+wide(snapshot.names[pad]));
        for(int knob=0;knob<4;++knob){const int y=174+knob*45,page=binding.page+knobLayer*2,logical=knobLayer*8+display*4+knob;const bool valid=page<2;fill(dc,{box.x+14,y,box.w-28,38},knob%2?RGB(18,31,35):RGB(17,28,33));text(dc,{box.x+24,y,42,38},L"P"+number(logical+1),10,valid?accent:muted,FW_SEMIBOLD);text(dc,{box.x+68,y,220,38},valid?surfaceProgramParameterName(page,knob):L"—",11,valid?ink:muted);text(dc,{box.x+300,y,126,38},valid?surfaceProgramParameterValue(edit,page,knob):L"Non assigné",10,valid?accent:muted,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);}
        return;
    }
    text(dc,{box.x+14,box.y+9,250,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  MENU KICK",12,focused?accent:muted,FW_SEMIBOLD);
    text(dc,{box.x+316,box.y+9,124,24},L"PAGE "+number(display+1)+L" / 2",9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
    fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
    for(int local=0;local<8;++local){
        int item=display*8+local;auto b=surfaceDisplayItemBox(display,local);bool selected=local==surface.menuSelections[display];
        if(selected)round(dc,b,accent,accent);else fill(dc,b,local%2?RGB(18,31,35):RGB(17,28,33));
        text(dc,{b.x+10,b.y,b.w-20,b.h},number(item+1)+L"  "+surfaceMenuLabels[item],11,selected?bg:ink,selected?FW_SEMIBOLD:FW_NORMAL);
    }
}

void paintSurface(HDC dc,const lab::Snapshot& snapshot){
    paintSurfaceDisplay(dc,0,snapshot);paintSurfaceDisplay(dc,1,snapshot);
    for(int display=0;display<2;++display)for(int knob=0;knob<4;++knob)paintSurfaceKnob(dc,display,knob,snapshot);
}

void paintMasterMeter(HDC dc,double value,double peak){
    value=std::clamp(value,0.,1.);peak=std::max(0.,peak);const Box meter{42,738,236,16};
    const double db=peak>0?20.*std::log10(peak):-60.;const double ratio=std::clamp((db+60.)/60.,0.,1.);const int lit=int((meter.w-4)*ratio);
    text(dc,{42,716,118,18},L"MASTER "+number(int(value*100))+L" %",9,muted,FW_SEMIBOLD);
    const std::wstring level=peak>=1?L"CLIP":(db<=-59.9?L"−∞ dB":number(int(std::round(db)))+L" dB");text(dc,{160,716,118,18},level,9,peak>=1?RGB(255,95,90):muted,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
    round(dc,meter,RGB(20,26,34),line);if(lit>0){const int green=std::min(lit,int((meter.w-4)*.8)),amber=std::min(lit-green,int((meter.w-4)*.15)),red=lit-green-amber;if(green>0)fill(dc,{meter.x+2,meter.y+3,green,meter.h-6},accent);if(amber>0)fill(dc,{meter.x+2+green,meter.y+3,amber,meter.h-6},RGB(241,184,88));if(red>0)fill(dc,{meter.x+2+green+amber,meter.y+3,red,meter.h-6},RGB(255,95,90));}
    const int marker=meter.x+2+int((meter.w-4)*value);fill(dc,{marker-1,meter.y-2,2,meter.h+4},ink);
}

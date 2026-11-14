// SPDX-License-Identifier: AGPL-3.0-or-later
// App member functions for the common KICK mouse/keyboard/MIDI surface.
const std::array<std::wstring,16> surfaceMenuLabels={
    L"Browser",L"Track Mixer",L"Track Mutes",L"Sequences",
    L"List Edit",L"Pad Mixer",L"Pad Mutes",L"Songs",
    L"Color Editor",L"Sampler",L"Sample Editor",L"Pad Edit",
    L"Pad Notes",L"Looper",L"Q-Links",L"MIDI Control"
};
const std::array<std::wstring,lab::MidiCommandCount> midiCommandLabels={
    L"Play / Pause",L"Stop",L"Shift",L"Enter",
    L"Haut",L"Bas",L"Gauche",L"Droite",
    L"Écran A",L"Écran B",L"Rotation 4D",L"Retour / Escape",
    L"Soft 1",L"Soft 2",L"Soft 3",L"Soft 4",L"Soft 5",L"Soft 6",L"Soft 7",L"Soft 8",
    L"Groupe A",L"Groupe B",L"Groupe C",L"Groupe D",L"Groupe E",L"Groupe F",L"Groupe G",L"Groupe H",
    L"Pad Mode",L"Keyboard",L"Chords",L"Step",
    L"Restart",L"Erase",L"Tap",L"Follow",L"Record",
    L"Fixed Velocity",L"Scene",L"Pattern",L"Events",L"Variation",L"Duplicate",L"Select",L"Solo",L"Mute",
    L"Volume",L"Swing",L"Tempo",L"Note Repeat",L"Lock",L"Pitch",L"Mod",L"Perform",L"Notes",
    L"Channel / MIDI",L"Plug-in / Instance",L"Ideas / Song",L"Mixer",L"Browser",L"Sampling",L"File / Save",L"Settings",L"Auto",L"Macro / Set",
    L"Soft 9",L"Soft 10",L"Soft 11",L"Soft 12",L"Soft 13",L"Soft 14",L"Soft 15",L"Soft 16"
};

// Balanced Maschine+ grid: both displays keep their original readable width,
// the pad matrix is exactly flush with display B and the two side command
// columns mirror each other.
static constexpr int SurfaceFirstX=286,SurfaceGap=492,SurfaceY=38;
int surfaceX(int display)const{return SurfaceFirstX+display*SurfaceGap;}
Box surfaceModuleBox(int display)const{return {surfaceX(display)-12,SurfaceY-12,480,374};}
Box surfaceDisplayBox(int display)const{return {surfaceX(display),SurfaceY,456,294};}
Box surfaceDisplayItemBox(int display,int local)const{return {surfaceX(display)+14+(local%2)*211,SurfaceY+46+(local/2)*56,199,46};}
Box surfaceKnobBox(int display,int knob)const{return {surfaceX(display)+18+knob*108,SurfaceY+300,96,54};}
Box surfaceSoftButtonBox(int display,int buttonIndex)const{return {surfaceX(display)+buttonIndex*114,4,104,27};}
Box surfaceInsertSlotBox(int display,int slot)const{return {surfaceX(display)+14+slot*104,SurfaceY+47,96,34};}
Box surfaceInsertEffectBox(int display)const{return {surfaceX(display)+14,SurfaceY+94,250,36};}
Box surfaceInsertBypassBox(int display)const{return {surfaceX(display)+274,SurfaceY+94,136,36};}
Box surfacePagePreviousBox(int display)const{return {surfaceX(display)+368,SurfaceY+8,28,24};}
Box surfacePageNextBox(int display)const{return {surfaceX(display)+400,SurfaceY+8,28,24};}
Box surfaceProgramAuditionBox(int display)const{return {surfaceX(display)+14,SurfaceY+46,396,34};}
Box surfaceMidiDeviceBox(int display)const{return {surfaceX(display)+14,SurfaceY+46,240,34};}
Box surfaceMidiPreviousBox(int display)const{return {surfaceX(display)+314,SurfaceY+46,44,34};}
Box surfaceMidiNextBox(int display)const{return {surfaceX(display)+366,SurfaceY+46,44,34};}
Box surfaceMidiBindingBox(int display,int row)const{return {surfaceX(display)+14,SurfaceY+146+row*34,396,28};}
Box surfaceBrowserPreviousBox(int display)const{return {surfaceX(display)+14,SurfaceY+46,42,34};}
Box surfaceBrowserCategoryBox(int display)const{return {surfaceX(display)+62,SurfaceY+46,300,34};}
Box surfaceBrowserNextBox(int display)const{return {surfaceX(display)+368,SurfaceY+46,42,34};}
Box surfaceBrowserFileBox(int display,int row)const{return {surfaceX(display)+14,SurfaceY+90+row*31,396,27};}
Box surfaceBrowserModeBox(int display)const{return {surfaceX(display)+14,SurfaceY+252,82,30};}
Box surfaceBrowserAuditionBox(int display)const{return {surfaceX(display)+102,SurfaceY+252,82,30};}
Box surfaceBrowserLoadBox(int display)const{return {surfaceX(display)+190,SurfaceY+252,100,30};}
Box surfaceBrowserRefreshBox(int display)const{return {surfaceX(display)+296,SurfaceY+252,114,30};}
Box surfaceSongActionBox(int display,int action)const{return {surfaceX(display)+14+action*99,SurfaceY+46,93,32};}
Box surfaceSongRowBox(int display,int row)const{return {surfaceX(display)+14,SurfaceY+94+row*43,396,36};}
Box surfaceMixerTrackBox(int display,int row)const{return {surfaceX(display)+14,SurfaceY+54+row*52,396,44};}
Box surfaceTrackMuteBox(int display,int local)const{return {surfaceX(display)+14+(local%2)*211,SurfaceY+48+(local/2)*56,199,46};}

Box shortcutBox(int index)const{return {20+(index%2)*120,38+(index/2)*48,112,38};}
Box groupBox(int group)const{return {20+(group%4)*108,624+(group/4)*54,100,46};}
Box modeBox(int mode)const{return {778+mode*116,420,108,32};}
Box sideCommandBox(int command)const{return {650,466+command*41,112,34};}
Box futureCommandBox(int command)const{return {1246,466+command*41,110,34};}
Box smallControlBox(int column,int row)const{return {20+column*120,420+row*48,112,38};}
Box performanceButtonBox(int column)const{return {20+column*108,570,100,38};}
Box transportBox(int index)const{return {20+(index%4)*108,768+(index/4)*44,100,36};}
Box brandPanelBox()const{return {1368,26,204,808};}
Box brandHomeBox()const{return {1382,42,176,130};}
Box brandMasterBox()const{return {1382,674,176,144};}

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

int mixerTrackFor(int display,int control)const{
    return std::clamp(mixerBank*8+display*4+control,0,lab::Pads-1);
}

void selectTrack(int track){
    stepTrack=std::clamp(track,0,lab::Pads-1);selected=stepTrack;surface.selectPad(selected);engine.selectTrack(stepTrack);
}

void showSurfaceMixer(int bank=-1){
    if(bank>=0)mixerBank=std::clamp(bank,0,1);
    for(int display=0;display<lab::SurfaceDisplays;++display){auto& binding=surface.displays[display];binding.module=lab::DisplayModule::Mixer;binding.page=mixerBank;}
    surface.focusDisplay(stepTrack%8/4);status=L"Track Mixer KICK — page "+number(mixerBank+1)+L" : pistes "+number(mixerBank*8+1)+L" à "+number(mixerBank*8+8)+L".";
}

void toggleTrackMute(int track){auto snapshot=engine.snapshot();const auto& t=snapshot.sequences[snapshot.sequence].tracks[std::clamp(track,0,lab::Pads-1)];engine.setTrackMute(track,!t.mute);markProjectDirty();}
void toggleTrackSolo(int track){auto snapshot=engine.snapshot();const auto& t=snapshot.sequences[snapshot.sequence].tracks[std::clamp(track,0,lab::Pads-1)];engine.setTrackSolo(track,!t.solo);markProjectDirty();}
void toggleTrackArm(int track){auto snapshot=engine.snapshot();const auto& t=snapshot.sequences[snapshot.sequence].tracks[std::clamp(track,0,lab::Pads-1)];engine.setTrackArmed(track,!t.armed);markProjectDirty();}

void showSurfaceTrackMute(){
    for(int display=0;display<lab::SurfaceDisplays;++display){auto& binding=surface.displays[display];binding.module=lab::DisplayModule::TrackMute;binding.page=display;}
    surface.focusDisplay(stepTrack/8);status=L"Track Mute KICK — pads/clic : mute · Shift + pad : solo · encodeur : sélectionner · Enter : mute.";
}

void arrangeSurfaceInsertPages(int pad,int slot,int preferredDisplay){
    pad=std::clamp(pad,0,lab::Pads-1);slot=std::clamp(slot,0,lab::InsertSlots-1);preferredDisplay=std::clamp(preferredDisplay,0,lab::SurfaceDisplays-1);
    const auto snapshot=engine.snapshot();const int count=lab::insertParameterCount(snapshot.edits[pad].inserts[slot].effect);
    if(count>4){surface.displays[0]={lab::DisplayModule::Inserts,pad,slot,0,false};surface.displays[1]={lab::DisplayModule::Inserts,pad,slot,1,false};}
    else {auto& binding=surface.displays[preferredDisplay];binding={lab::DisplayModule::Inserts,pad,slot,0,false};const int other=1-preferredDisplay;if(surface.displays[other].module==lab::DisplayModule::Inserts&&surface.displays[other].pad==pad&&surface.displays[other].slot==slot)showSurfaceMenu(other);}
    surface.focusDisplay(preferredDisplay);
}

void showSurfaceInserts(int display){
    display=std::clamp(display,0,1);arrangeSurfaceInsertPages(selected,std::clamp(selectedInsert,0,lab::InsertSlots-1),display);
    const auto snapshot=engine.snapshot();const int count=lab::insertParameterCount(snapshot.edits[selected].inserts[selectedInsert].effect);
    status=count>4?L"FX KICK — paramètres 1–4 sur l'écran A et 5–8 sur l'écran B.":L"Écran "+std::wstring(display?L"B":L"A")+L" — paramètres réels du slot d'effet sélectionné.";
}

void showSurfaceProgram(int display){
    display=std::clamp(display,0,1);auto& binding=surface.displays[display];binding.module=lab::DisplayModule::Sample;binding.pad=selected;binding.page=0;surface.focusDisplay(display);
    status=L"Pad Edit — pitch, lecture et enveloppe d'amplitude du pad "+padName(selected)+L".";
}

void showSurfacePadPerform(int display){
    display=std::clamp(display,0,1);padMode=1;auto snapshot=engine.snapshot();engine.setMelodicSourcePad(selected);if(snapshot.sequences[snapshot.sequence].mode==lab::PerformanceMode::Drum)engine.setPerformanceMode(lab::PerformanceMode::Chromatic);auto& binding=surface.displays[display];binding.module=lab::DisplayModule::PadPerform;binding.page=0;surface.focusDisplay(display);status=L"Pad Notes — les 16 pads jouent le sample source à des hauteurs différentes.";
}

void enterStepMode(){
    const auto snapshot=engine.snapshot();const auto& sequence=snapshot.sequences[snapshot.sequence];const bool fromKeyboard=padMode==1&&sequence.mode!=lab::PerformanceMode::Drum;
    if(!fromKeyboard&&padMode!=3){engine.setPerformanceMode(lab::PerformanceMode::Drum);stepTrack=selected;}
    padMode=3;const auto current=engine.snapshot();const auto mode=current.sequences[current.sequence].mode;
    status=mode==lab::PerformanceMode::Drum?L"Step — les 16 pads éditent la piste du pad "+padName(stepTrack)+L".":L"Step mélodique — les 16 pads éditent la note "+noteName(engine.performanceNote(stepTrack))+L".";
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

void setSurfaceProgramRatio(int display,int knob,double ratio){
    if(display<0||display>=lab::SurfaceDisplays||knob<0||knob>=lab::SurfaceQLinks)return;auto& binding=surface.displays[display];if(binding.module!=lab::DisplayModule::Sample)return;
    auto snapshot=engine.snapshot();const int pad=std::clamp(binding.pad,0,lab::Pads-1);auto edit=snapshot.edits[pad];ratio=std::clamp(ratio,0.,1.);
    if(binding.page==0){if(knob==0)edit.gain=ratio*2.;else if(knob==1)edit.semitones=std::round(ratio*48.)-24.;else if(knob==2)edit.start=std::min(ratio,edit.end-.001);else if(knob==3)edit.end=std::max(ratio,edit.start+.001);}
    else if(binding.page==1){if(knob==0)edit.attack=ratio;else if(knob==1)edit.decay=ratio;else if(knob==2)edit.sustain=ratio;else if(knob==3)edit.release=ratio;}
    engine.setEdit(pad,edit);
}

std::wstring surfaceProgramParameterName(int page,int knob)const{
    static const std::array<std::wstring,8> names={L"Volume",L"Pitch",L"Début",L"Fin",L"Attack",L"Decay",L"Sustain",L"Release"};return names[std::clamp(page*4+knob,0,7)];
}

std::wstring surfaceProgramParameterValue(const lab::PadEdit& edit,int page,int knob)const{
    if(page==0){if(knob==0)return decimal(edit.gain*100,0)+L" %";if(knob==1)return (edit.semitones>=0?L"+":L"")+decimal(edit.semitones,0)+L" st";if(knob==2)return decimal(edit.start*100,1)+L" %";return decimal(edit.end*100,1)+L" %";}
    if(knob==0)return decimal(edit.attack*edit.attack*2000,0)+L" ms";if(knob==1)return decimal(edit.decay*edit.decay*2000,0)+L" ms";if(knob==2)return decimal(edit.sustain*100,0)+L" %";return decimal(edit.release*edit.release*4000,0)+L" ms";
}

void showSurfaceMidiControl(int display){
    display=std::clamp(display,0,1);auto& binding=surface.displays[display];
    binding.module=lab::DisplayModule::MidiControl;binding.page=0;surface.focusDisplay(display);
    status=L"MIDI Control — 16 knobs, 8×16 pads et commandes Maschine+ apprenables.";
}

std::wstring surfaceBrowserModeName(int mode)const{
    return mode==1?L"AUD AUTO":mode==2?L"AUD MAN":L"AUD OFF";
}

void auditionSurfaceBrowser(int display){
    display=std::clamp(display,0,lab::SurfaceDisplays-1);auto& browser=browsers[display];
    if(browser.selected<0||browser.selected>=int(browser.entries.size())||browser.entries[browser.selected].directory){engine.stopPreview();return;}
    std::string error;
    if(engine.preview(browser.entries[browser.selected].path.u8string(),error))status=L"Préécoute one-shot : "+browser.entries[browser.selected].path.filename().wstring();
    else status=L"Préécoute impossible : "+wide(error);
}

std::wstring surfaceBrowserPath(const BrowserState& browser)const{
    std::error_code error;auto relative=std::filesystem::relative(browser.directory,library,error);
    if(error||relative.empty()||relative==L".")return L"samples";
    return L"samples / "+relative.wstring();
}

void refreshSurfaceBrowser(int display,bool audition=false){
    display=std::clamp(display,0,lab::SurfaceDisplays-1);auto& browser=browsers[display];
    browser.entries.clear();std::error_code error;
    if(browser.directory.empty()||!std::filesystem::is_directory(browser.directory,error))browser.directory=library;
    for(std::filesystem::directory_iterator it(browser.directory,error),end;!error&&it!=end;it.increment(error)){
        const bool directory=it->is_directory(error);if(error)break;
        if(!directory){if(!it->is_regular_file(error))continue;auto extension=it->path().extension().wstring();std::transform(extension.begin(),extension.end(),extension.begin(),[](wchar_t c){return wchar_t(std::towlower(c));});if(extension!=L".wav")continue;}
        browser.entries.push_back({it->path(),directory});
    }
    std::sort(browser.entries.begin(),browser.entries.end(),[](const auto& a,const auto& b){if(a.directory!=b.directory)return a.directory>b.directory;auto an=a.path.filename().wstring(),bn=b.path.filename().wstring();std::transform(an.begin(),an.end(),an.begin(),[](wchar_t c){return wchar_t(std::towlower(c));});std::transform(bn.begin(),bn.end(),bn.begin(),[](wchar_t c){return wchar_t(std::towlower(c));});return an<bn;});
    if(browser.entries.empty()){browser.selected=browser.scroll=0;engine.stopPreview();}
    else{browser.selected=std::clamp(browser.selected,0,int(browser.entries.size())-1);browser.scroll=std::clamp(browser.scroll,0,std::max(0,int(browser.entries.size())-5));if(browser.selected<browser.scroll)browser.scroll=browser.selected;if(browser.selected>=browser.scroll+5)browser.scroll=browser.selected-4;if(audition&&browser.auditionMode==1)auditionSurfaceBrowser(display);}
}

void moveSurfaceBrowserSelection(int display,int delta){
    auto& browser=browsers[std::clamp(display,0,lab::SurfaceDisplays-1)];if(browser.entries.empty()||!delta)return;
    const int previous=browser.selected;browser.selected=std::clamp(browser.selected+(delta>0?1:-1),0,int(browser.entries.size())-1);
    if(browser.selected<browser.scroll)browser.scroll=browser.selected;if(browser.selected>=browser.scroll+5)browser.scroll=browser.selected-4;
    if(browser.selected!=previous&&browser.auditionMode==1)auditionSurfaceBrowser(display);
}

void moveSurfaceBrowserCategory(int display,int delta){
    display=std::clamp(display,0,lab::SurfaceDisplays-1);auto& browser=browsers[display];if(!delta)return;
    if(delta<0){if(browser.directory==library)return;auto parent=browser.directory.parent_path();std::error_code error;auto relative=std::filesystem::relative(parent,library,error);if(error||(!relative.empty()&&*relative.begin()==std::filesystem::path("..")))parent=library;browser.directory=parent;}
    else {if(browser.selected<0||browser.selected>=int(browser.entries.size())||!browser.entries[browser.selected].directory)return;browser.directory=browser.entries[browser.selected].path;}
    browser.selected=browser.scroll=0;engine.stopPreview();refreshSurfaceBrowser(display,true);status=L"Browser KICK — "+surfaceBrowserPath(browser)+L".";
}

void loadSurfaceBrowserSelection(int display){
    display=std::clamp(display,0,lab::SurfaceDisplays-1);auto& browser=browsers[display];if(browser.entries.empty()){status=L"Browser KICK — ce dossier est vide.";return;}
    if(browser.selected<0||browser.selected>=int(browser.entries.size()))return;if(browser.entries[browser.selected].directory){moveSurfaceBrowserCategory(display,1);return;}
    auto& binding=surface.displays[display];const int pad=std::clamp(binding.pad,0,lab::Pads-1);std::string error;engine.stopPreview();const auto file=browser.entries[browser.selected].path;
    if(engine.load(pad,file.u8string(),error,true)){selectPad(pad);markProjectDirty();status=L"Sample chargé sur le pad "+padName(pad)+L" sans modifier ses réglages : "+file.filename().wstring();}
    else status=L"Chargement impossible : "+wide(error);
}

void showSurfaceBrowser(int display){
    display=std::clamp(display,0,lab::SurfaceDisplays-1);auto& binding=surface.displays[display];binding.module=lab::DisplayModule::Browser;binding.pad=selected;binding.page=0;surface.focusDisplay(display);if(browsers[display].directory.empty())browsers[display].directory=library;refreshSurfaceBrowser(display,true);
    status=L"Browser KICK — haut/bas : parcourir · droite/Enter : ouvrir ou charger · gauche : dossier parent.";
}

void closeSurfaceModule(){
    const int display=surface.focusedDisplay;if(surface.displays[display].module==lab::DisplayModule::Browser)engine.stopPreview();
    if(surface.displays[display].module!=lab::DisplayModule::Menu){showSurfaceMenu(display);status=L"Retour au menu KICK de l'écran "+std::wstring(display?L"B.":L"A.");}
    else status=L"KICK / Retour — aucun écran enfant ouvert.";
}

void setSurfaceQLink(int display,int knob,double ratio){
    if(display<0||display>=lab::SurfaceDisplays||knob<0||knob>=lab::SurfaceQLinks)return;
    auto& binding=surface.displays[display];ratio=std::clamp(ratio,0.,1.);if(binding.module==lab::DisplayModule::Sample){setSurfaceProgramRatio(display,knob,ratio);return;}
    if(binding.module==lab::DisplayModule::Mixer){const int track=mixerTrackFor(display,knob);if(mixerParameter==0)engine.setTrackLevel(track,ratio);else engine.setTrackPan(track,ratio*2.-1.);return;}
    if(binding.module==lab::DisplayModule::PadPerform){auto snapshot=engine.snapshot();if(knob==0)engine.setPerformanceMode(lab::PerformanceMode(std::clamp(int(std::lround(ratio*3)),0,3)));else if(knob==1)engine.setMelodicSourcePad(std::clamp(int(std::lround(ratio*15)),0,15));else if(knob==2)engine.setRootNote(24+std::clamp(int(std::lround(ratio*60)),0,60));else engine.setViewBar(std::clamp(int(std::lround(ratio*(snapshot.sequences[snapshot.sequence].bars-1))),0,snapshot.sequences[snapshot.sequence].bars-1));return;}
    if(binding.module==lab::DisplayModule::NextSequence){auto snapshot=engine.snapshot();if(knob==0)engine.selectSequence(std::clamp(int(std::lround(ratio*15)),0,15));else if(knob==1)engine.setSequenceBars(1+std::clamp(int(std::lround(ratio*3)),0,3));else if(knob==2)engine.setViewBar(std::clamp(int(std::lround(ratio*(snapshot.sequences[snapshot.sequence].bars-1))),0,snapshot.sequences[snapshot.sequence].bars-1));else engine.setPerformanceMode(lab::PerformanceMode(std::clamp(int(std::lround(ratio*3)),0,3)));return;}
    if(binding.module==lab::DisplayModule::Song){auto snapshot=engine.snapshot();if(knob==0)songChoice=std::clamp(int(std::lround(ratio*15)),0,15);else if(knob==1)songRepeats=1+std::clamp(int(std::lround(ratio*127)),0,127);else if(knob==2)songSelected=std::clamp(int(std::lround(ratio*std::max(0,snapshot.songLength-1))),0,std::max(0,snapshot.songLength-1));return;}
    if(binding.module!=lab::DisplayModule::Inserts)return;
    auto s=engine.snapshot();const int pad=std::clamp(binding.pad,0,lab::Pads-1),slot=std::clamp(binding.slot,0,lab::InsertSlots-1);
    const int parameter=binding.parameterForKnob(knob);const auto effect=s.edits[pad].inserts[slot].effect;
    if(parameter>=0&&parameter<lab::insertParameterCount(effect))engine.setInsertParameter(pad,slot,parameter,ratio);
}

void turnSurfaceQLink(int display,int knob,int delta){
    if(display<0||display>=lab::SurfaceDisplays||knob<0||knob>=lab::SurfaceQLinks||!delta)return;
    auto& binding=surface.displays[display];
    if(binding.module==lab::DisplayModule::Mixer){auto snapshot=engine.snapshot();const int track=mixerTrackFor(display,knob);const auto& data=snapshot.sequences[snapshot.sequence].tracks[track];if(mixerParameter==0)engine.setTrackLevel(track,data.level+delta*.01);else engine.setTrackPan(track,data.pan+delta*.02);return;}
    if(binding.module==lab::DisplayModule::Sample){auto snapshot=engine.snapshot();const auto& edit=snapshot.edits[std::clamp(binding.pad,0,lab::Pads-1)];const double step=binding.page==0&&knob==1?1./48.:.01;setSurfaceProgramRatio(display,knob,surfaceProgramRatio(edit,binding.page,knob)+delta*step);return;}
    if(binding.module==lab::DisplayModule::PadPerform){auto snapshot=engine.snapshot();const auto& sequence=snapshot.sequences[snapshot.sequence];if(knob==0)engine.setPerformanceMode(lab::PerformanceMode(std::clamp(int(sequence.mode)+(delta>0?1:-1),0,3)));else if(knob==1)engine.setMelodicSourcePad(std::clamp(sequence.sourcePad+(delta>0?1:-1),0,15));else if(knob==2)engine.setRootNote(sequence.rootNote+(delta>0?1:-1));else engine.setViewBar(snapshot.viewBar+(delta>0?1:-1));return;}
    if(binding.module==lab::DisplayModule::NextSequence){auto snapshot=engine.snapshot();const auto& sequence=snapshot.sequences[snapshot.sequence];if(knob==0)engine.selectSequence(snapshot.sequence+(delta>0?1:-1));else if(knob==1)engine.setSequenceBars(sequence.bars+(delta>0?1:-1));else if(knob==2)engine.setViewBar(snapshot.viewBar+(delta>0?1:-1));else engine.setPerformanceMode(lab::PerformanceMode(std::clamp(int(sequence.mode)+(delta>0?1:-1),0,3)));return;}
    if(binding.module==lab::DisplayModule::Song){auto snapshot=engine.snapshot();if(knob==0)songChoice=std::clamp(songChoice+(delta>0?1:-1),0,15);else if(knob==1)songRepeats=std::clamp(songRepeats+(delta>0?1:-1),1,128);else if(knob==2)songSelected=std::clamp(songSelected+(delta>0?1:-1),0,std::max(0,snapshot.songLength-1));return;}
    if(binding.module!=lab::DisplayModule::Inserts)return;
    auto s=engine.snapshot();
    const int pad=std::clamp(binding.pad,0,lab::Pads-1),slot=std::clamp(binding.slot,0,lab::InsertSlots-1);
    const auto& insert=s.edits[pad].inserts[slot];
    const int parameter=binding.parameterForKnob(knob);
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

int midiControlPages()const{return 8+(lab::MidiCommandCount+3)/4;}

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
    if(module==lab::DisplayModule::TrackMute){const int first=surface.focusedDisplay*8;int local=std::clamp(stepTrack-first,0,7);int row=local/2,col=local%2;col=std::clamp(col+dx,0,1);row=std::clamp(row+dy,0,3);selectTrack(first+row*2+col);return;}
    if(module==lab::DisplayModule::Mixer){if(dx){mixerBank=std::clamp(mixerBank+(dx>0?1:-1),0,1);showSurfaceMixer();}if(dy){mixerParameter=std::clamp(mixerParameter+(dy>0?1:-1),0,1);}return;}
    if(module==lab::DisplayModule::NextSequence){if(dx)turnSurfaceQLink(surface.focusedDisplay,0,dx);if(dy)turnSurfaceQLink(surface.focusedDisplay,2,dy);return;}
    if(module==lab::DisplayModule::PadPerform){if(dx)turnSurfaceQLink(surface.focusedDisplay,0,dx);if(dy)turnSurfaceQLink(surface.focusedDisplay,2,-dy);return;}
    if(module==lab::DisplayModule::Song){if(dx)turnSurfaceQLink(surface.focusedDisplay,1,dx);if(dy)turnSurfaceQLink(surface.focusedDisplay,2,dy);return;}
    if(module==lab::DisplayModule::MidiControl&&dx){auto& binding=surface.displays[surface.focusedDisplay];binding.page=std::clamp(binding.page+(dx>0?1:-1),0,midiControlPages()-1);return;}
    if(module==lab::DisplayModule::Sample&&dx){auto& binding=surface.displays[surface.focusedDisplay];binding.page=std::clamp(binding.page+(dx>0?1:-1),0,1);return;}
    if(module==lab::DisplayModule::Inserts){auto& binding=surface.displays[surface.focusedDisplay];if(dy){const int slot=(binding.slot+(dy>0?1:-1)+lab::InsertSlots)%lab::InsertSlots;const int pad=binding.pad;selectedInsert=slot;arrangeSurfaceInsertPages(pad,slot,surface.focusedDisplay);status=L"FX — slot "+number(slot+1)+L" sélectionné.";}if(dx){auto& current=surface.displays[surface.focusedDisplay];auto snapshot=engine.snapshot();const auto effect=snapshot.edits[current.pad].inserts[current.slot].effect;const int pages=std::max(1,(lab::insertParameterCount(effect)+3)/4);current.page=std::clamp(current.page+(dx>0?1:-1),0,pages-1);}return;}
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
    if(module==lab::DisplayModule::TrackMute){const int first=surface.focusedDisplay*8;selectTrack(std::clamp(stepTrack+(delta>0?1:-1),first,first+7));return;}
    if(module==lab::DisplayModule::Mixer){mixerBank=std::clamp(mixerBank+(delta>0?1:-1),0,1);showSurfaceMixer();return;}
    if(module==lab::DisplayModule::NextSequence){turnSurfaceQLink(surface.focusedDisplay,0,delta);return;}
    if(module==lab::DisplayModule::PadPerform){turnSurfaceQLink(surface.focusedDisplay,2,delta);return;}
    if(module==lab::DisplayModule::Song){turnSurfaceQLink(surface.focusedDisplay,2,delta);return;}
    if(module==lab::DisplayModule::Inserts){const auto binding=surface.displays[surface.focusedDisplay];const int slot=(binding.slot+(delta>0?1:-1)+lab::InsertSlots)%lab::InsertSlots;selectedInsert=slot;arrangeSurfaceInsertPages(binding.pad,slot,surface.focusedDisplay);status=L"FX — rotation : slot "+number(slot+1)+L" sélectionné.";return;}
    if(module!=lab::DisplayModule::Menu)return;
    int item=surface.focusedDisplay*8+surface.menuSelections[surface.focusedDisplay];
    selectSurfaceMenu(std::clamp(item+(delta>0?1:-1),0,15));
}

void activateSurfaceMenu(){
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::Browser){loadSurfaceBrowserSelection(surface.focusedDisplay);return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::TrackMute){toggleTrackMute(stepTrack);return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::Mixer){selectTrack(mixerTrackFor(surface.focusedDisplay,0));return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::Song){engine.addSongStep(songChoice,songRepeats);markProjectDirty();auto snapshot=engine.snapshot();songSelected=std::max(0,snapshot.songLength-1);return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::Inserts){auto& binding=surface.displays[surface.focusedDisplay];selectedInsert=binding.slot;insertMenu();binding.page=0;return;}
    if(surface.displays[surface.focusedDisplay].module!=lab::DisplayModule::Menu)return;
    const int item=surface.focusedDisplay*8+surface.menuSelections[surface.focusedDisplay];
    if(item==0){showSurfaceBrowser(surface.focusedDisplay);return;}
    if(item==1){showSurfaceMixer(stepTrack/8);return;}
    if(item==2){showSurfaceTrackMute();return;}
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
        else if(surface.displays[display].module==lab::DisplayModule::Browser)moveSurfaceBrowserSelection(display,-delta);
        return true;
    }
    return false;
}

bool surfaceClick(int x,int y,bool right){
    for(int display=0;display<2;++display){
        for(int knob=0;knob<4;++knob)if(surfaceKnobBox(display,knob).contains(x,y)){
            surface.focusDisplay(display);
            const auto module=surface.displays[display].module;
            if(!right&&(module==lab::DisplayModule::Inserts||module==lab::DisplayModule::Sample||module==lab::DisplayModule::PadPerform||module==lab::DisplayModule::NextSequence||module==lab::DisplayModule::Song||module==lab::DisplayModule::Mixer)){
                surfaceDragDisplay=display;surfaceDragKnob=knob;surfaceDragStartY=y;surfaceDragLastStep=0;SetCapture(window);
            }
            return true;
        }
        if(!surfaceDisplayBox(display).contains(x,y))continue;
        surface.focusDisplay(display);
        auto& binding=surface.displays[display];
        if(binding.module==lab::DisplayModule::TrackMute){
            for(int local=0;local<8;++local)if(surfaceTrackMuteBox(display,local).contains(x,y)){const int track=display*8+local;selectTrack(track);if(right||surface.shift)toggleTrackSolo(track);else toggleTrackMute(track);return true;}
            return true;
        }
        if(binding.module==lab::DisplayModule::Mixer){
            for(int row=0;row<4;++row)if(surfaceMixerTrackBox(display,row).contains(x,y)){const int track=mixerTrackFor(display,row);selectTrack(track);if(right)toggleTrackMute(track);return true;}
            return true;
        }
        if(binding.module==lab::DisplayModule::Browser){
            auto& browser=browsers[display];
            if(surfaceBrowserPreviousBox(display).contains(x,y)){moveSurfaceBrowserCategory(display,-1);return true;}
            if(surfaceBrowserNextBox(display).contains(x,y)){moveSurfaceBrowserCategory(display,1);return true;}
            for(int row=0;row<5;++row)if(surfaceBrowserFileBox(display,row).contains(x,y)){const int index=browser.scroll+row;if(index<int(browser.entries.size())){browser.selected=index;if(browser.auditionMode==1)auditionSurfaceBrowser(display);}return true;}
            if(surfaceBrowserModeBox(display).contains(x,y)){browser.auditionMode=(browser.auditionMode+1)%3;if(browser.auditionMode==0)engine.stopPreview();else if(browser.auditionMode==1)auditionSurfaceBrowser(display);status=L"Browser KICK — mode de préécoute "+surfaceBrowserModeName(browser.auditionMode)+L".";return true;}
            if(surfaceBrowserAuditionBox(display).contains(x,y)){if(browser.auditionMode!=0)auditionSurfaceBrowser(display);return true;}
            if(surfaceBrowserLoadBox(display).contains(x,y)){loadSurfaceBrowserSelection(display);return true;}
            if(surfaceBrowserRefreshBox(display).contains(x,y)){refreshSurfaceBrowser(display,true);status=L"Bibliothèque actualisée : "+number(int(browser.entries.size()))+L" élément(s) dans "+surfaceBrowserPath(browser)+L".";return true;}
            return true;
        }
        if(binding.module==lab::DisplayModule::Inserts){
            auto current=engine.snapshot();const int pages=std::max(1,(lab::insertParameterCount(current.edits[binding.pad].inserts[binding.slot].effect)+3)/4);
            if(surfacePagePreviousBox(display).contains(x,y)){binding.page=std::max(0,binding.page-1);return true;}
            if(surfacePageNextBox(display).contains(x,y)){binding.page=std::min(pages-1,binding.page+1);return true;}
            for(int slot=0;slot<lab::InsertSlots;++slot)if(surfaceInsertSlotBox(display,slot).contains(x,y)){
                selectedInsert=slot;arrangeSurfaceInsertPages(binding.pad,slot,display);return true;
            }
            if(surfaceInsertEffectBox(display).contains(x,y)){selectedInsert=binding.slot;insertMenu();binding.page=0;return true;}
            if(surfaceInsertBypassBox(display).contains(x,y)){
                auto s=engine.snapshot();const auto& insert=s.edits[binding.pad].inserts[binding.slot];
                if(insert.effect!=lab::InsertEffect::None){engine.setInsertBypass(binding.pad,binding.slot,!insert.bypass);markProjectDirty();}
                return true;
            }
            return true;
        }
        if(binding.module==lab::DisplayModule::Sample){
            if(surfacePagePreviousBox(display).contains(x,y)){binding.page=std::max(0,binding.page-1);return true;}
            if(surfacePageNextBox(display).contains(x,y)){binding.page=std::min(1,binding.page+1);return true;}
            if(surfaceProgramAuditionBox(display).contains(x,y)){engine.trigger(binding.pad);return true;}
            return true;
        }
        if(binding.module==lab::DisplayModule::Song){
            auto snapshot=engine.snapshot();
            if(surfaceSongActionBox(display,0).contains(x,y)){engine.addSongStep(songChoice,songRepeats);markProjectDirty();snapshot=engine.snapshot();songSelected=std::max(0,snapshot.songLength-1);status=L"Song : séquence "+number(songChoice+1)+L" ajoutée ×"+number(songRepeats)+L".";return true;}
            if(surfaceSongActionBox(display,1).contains(x,y)){engine.deleteSongStep(songSelected);markProjectDirty();snapshot=engine.snapshot();songSelected=std::clamp(songSelected,0,std::max(0,snapshot.songLength-1));return true;}
            if(surfaceSongActionBox(display,2).contains(x,y)){if(snapshot.songPlaying||snapshot.playing)engine.stop();else engine.playSong();return true;}
            if(surfaceSongActionBox(display,3).contains(x,y)){engine.clearSong();markProjectDirty();songSelected=0;return true;}
            const int first=std::clamp(songSelected-1,0,std::max(0,snapshot.songLength-4));for(int row=0;row<4;++row)if(surfaceSongRowBox(display,row).contains(x,y)){const int index=first+row;if(index<snapshot.songLength){songSelected=index;songChoice=snapshot.song[index].sequence;songRepeats=snapshot.song[index].repeats;}return true;}
            return true;
        }
        if(binding.module==lab::DisplayModule::PadPerform||binding.module==lab::DisplayModule::NextSequence)return true;
        if(binding.module==lab::DisplayModule::MidiControl){
            if(surfaceMidiDeviceBox(display).contains(x,y)){midiMenu();return true;}
            if(surfaceMidiPreviousBox(display).contains(x,y)){binding.page=std::max(0,binding.page-1);return true;}
            if(surfaceMidiNextBox(display).contains(x,y)){binding.page=std::min(midiControlPages()-1,binding.page+1);return true;}
            for(int row=0;row<4;++row)if(surfaceMidiBindingBox(display,row).contains(x,y)){
                if(binding.page<4){const int target=binding.page*4+row;auto& learned=midiQlinks[target];
                    if(right){learned.mode=learned.mode==lab::MidiCcMode::Absolute?lab::MidiCcMode::Relative1:learned.mode==lab::MidiCcMode::Relative1?lab::MidiCcMode::Relative65:lab::MidiCcMode::Absolute;const bool saved=saveMidiProfile();status=L"P"+number(target+1)+L" : mode "+midiModeName(learned.mode)+(saved?L" · profil sauvegardé.":L" · sauvegarde impossible.");}
                    else {midiLearnCommand=midiLearnPad=-1;midiLearnTarget=target;status=L"MIDI Learn armé pour P"+number(target+1)+L" — tourne maintenant le contrôle physique.";}
                }else if(binding.page<8){const int target=padGroup*lab::SurfacePads+(binding.page-4)*4+row;auto& learned=midiPads[target];
                    const std::wstring padLabel=std::wstring(1,wchar_t(L'A'+target/lab::SurfacePads))+number(target%lab::SurfacePads+1);
                    if(right){learned={};const bool saved=saveMidiProfile();status=L"Pad "+padLabel+(saved?L" : affectation effacée.":L" : effacement non sauvegardé.");}
                    else {midiLearnTarget=midiLearnCommand=-1;midiLearnPad=target;status=L"MIDI Learn armé pour PAD "+padLabel+L" — frappe maintenant le pad physique.";}
                }else {const int target=(binding.page-8)*4+row;if(target>=lab::MidiCommandCount)return true;auto& learned=midiCommands[target];
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

bool maschineClick(int x,int y,bool right){
    for(int display=0;display<2;++display)for(int soft=0;soft<4;++soft)if(surfaceSoftButtonBox(display,soft).contains(x,y)){
        activateSurfaceSoft(display,soft,mixerBank);return true;
    }
    for(int index=0;index<12;++index)if(shortcutBox(index).contains(x,y)){
        switch(index){
        case 0: if(surface.shift)showSurfaceMidiControl(surface.focusedDisplay);else showSurfaceMixer(stepTrack/8);break;
        case 1: showSurfaceInserts(surface.focusedDisplay);break;
        case 2: showSurfaceSong(surface.focusedDisplay);break;
        case 3: showSurfaceMixer(stepTrack/8);break;
        case 4: showSurfaceBrowser(surface.focusedDisplay);break;
        case 5: tab=1;status=L"Sampler / Sample Editor KICK.";break;
        case 6: dispatch({lab::SurfaceAction::PagePrevious});break;
        case 7: dispatch({lab::SurfaceAction::PageNext});break;
        case 8: if(right||surface.shift)project(true);else fileMenu();break;
        case 9: settingsMenu();break;
        case 10: status=L"Automation / List Edit — présent dans KICK, pas encore porté.";break;
        case 11: status=L"Macro / Q-Links — utilise les huit knobs contextuels des écrans.";break;
        }
        return true;
    }
    for(int group=0;group<8;++group)if(groupBox(group).contains(x,y)){
        engine.setGroup(group);markProjectDirty();padGroup=engine.snapshot().group;selectPad(0);stepTrack=0;status=L"Groupe "+std::wstring(1,wchar_t(L'A'+group))+L" sélectionné — 16 pads et réglages propres à cette banque.";return true;
    }
    for(int mode=0;mode<4;++mode)if(modeBox(mode).contains(x,y)){
        if(mode==2){status=L"Chords — commande KICK réservée ; moteur d'accords pas encore porté.";return true;}
        if(mode==3){enterStepMode();return true;}padMode=mode;
        if(mode==0){engine.setTrackProgram(stepTrack,lab::ProgramType::Drum);engine.setPerformanceMode(lab::PerformanceMode::Drum);}
        else if(mode==1){selectTrack(selected);engine.setTrackProgram(stepTrack,lab::ProgramType::Keygroup);engine.setMelodicSourcePad(selected);engine.setPerformanceMode(lab::PerformanceMode::Chromatic);}
        status=mode==0?L"Pad Mode — les 16 pads jouent les samples du groupe.":L"Keyboard — joue une hauteur puis passe sur Step pour la séquencer.";
        return true;
    }
    for(int command=0;command<9;++command)if(sideCommandBox(command).contains(x,y)){
        if(command==2){showSurfaceNextSequence(surface.focusedDisplay);status=L"Pattern / Clip — sélection des séquences KICK.";}
        else if(command==6){selectLatch=!selectLatch;if(selectLatch)status=L"Select actif — choisis maintenant la piste avec un pad.";else status=L"Select relâché — piste "+number(stepTrack+1)+L".";}
        else if(command==7){soloLatch=!soloLatch;muteLatch=selectLatch=false;status=soloLatch?L"Solo actif — frappe un pad pour isoler sa piste.":L"Mode Solo relâché.";}
        else if(command==8){muteLatch=!muteLatch;soloLatch=selectLatch=false;status=muteLatch?L"Mute actif — frappe un pad pour couper sa piste.":L"Mode Mute relâché.";}
        else status=std::array<std::wstring,9>{L"Fixed Velocity / 16 Vel",L"Scene / Section",L"Pattern / Clip",L"Events",L"Variation",L"Duplicate / Double",L"Select",L"Solo",L"Mute / Choke"}[command]+L" — commande visible, portage KICK à venir.";
        return true;
    }
    for(int column=0;column<2;++column)for(int row=0;row<3;++row)if(smallControlBox(column,row).contains(x,y)){
        if(column==0&&row==0){auto snapshot=engine.snapshot();engine.setVolume(std::clamp(snapshot.volume+(right?-.05:.05),0.,1.));markProjectDirty();status=L"Master Volume "+number(int(engine.snapshot().volume*100))+L" %.";}
        else if(column==0&&row==2)dispatch({lab::SurfaceAction::TempoDelta,0,right?-1:1});
        else status=L"Commande Maschine+ réservée au portage KICK suivant.";
        return true;
    }
    for(int column=0;column<4;++column)if(performanceButtonBox(column).contains(x,y)){
        if(column==0)showSurfaceProgram(surface.focusedDisplay);else if(column==2)showSurfaceInserts(surface.focusedDisplay);else if(column==3){padMode=1;showSurfacePadPerform(surface.focusedDisplay);}
        else status=L"Contrôle performance visible ; routage KICK pas encore porté.";
        return true;
    }
    for(int index=0;index<8;++index)if(transportBox(index).contains(x,y)){
        if(index==4)dispatch({lab::SurfaceAction::PlayPause});
        else if(index==3)cycleRecordQuantize();
        else if(index==5)toggleRecord();
        else if(index==6)dispatch({lab::SurfaceAction::Stop});
        else if(index==7){surface.shift=!surface.shift;status=surface.shift?L"SHIFT actif.":L"SHIFT relâché.";}
        else status=L"Commande transport visible ; fonction KICK pas encore portée.";
        return true;
    }
    for(int p=0;p<16;++p)if(padBox(p).contains(x,y)){
        if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::TrackMute){selectTrack(p);if(surface.shift)toggleTrackSolo(p);else toggleTrackMute(p);return true;}
        if(soloLatch){selectTrack(p);toggleTrackSolo(p);status=L"Solo piste "+number(p+1)+L" basculé.";return true;}
        if(muteLatch){selectTrack(p);toggleTrackMute(p);status=L"Mute piste "+number(p+1)+L" basculé.";return true;}
        if(padMode==3&&(selectLatch||surface.shift)){stepTrack=p;selectPad(p);if(selectLatch)selectLatch=false;status=L"Step — piste "+number(stepTrack+1)+L" sélectionnée.";return true;}
        if(padMode==3){dispatch({lab::SurfaceAction::StepToggle,stepTrack,p});return true;}
        if(right&&padMode==0){selectPad(p);sampleMenu();return true;}
        dispatch({lab::SurfaceAction::PadPress,p});return true;
    }
    if(brandMasterBox().contains(x,y)){
        if(y<730)settingsMenu();
        else {auto snapshot=engine.snapshot();engine.setVolume(std::clamp((x-1394)/152.,0.,1.));markProjectDirty();}
        return true;
    }
    return false;
}

void paintSurfaceKnob(HDC dc,int display,int knob,const lab::Snapshot& snapshot){
    const auto knobBox=surfaceKnobBox(display,knob);const int cx=knobBox.x+knobBox.w/2,cy=knobBox.y+22;
    double ratio=.5;bool assigned=false;
    const auto& binding=surface.displays[display];
    if(binding.module==lab::DisplayModule::Mixer){const auto& track=snapshot.sequences[snapshot.sequence].tracks[mixerTrackFor(display,knob)];ratio=mixerParameter==0?track.level:(track.pan+1.)*.5;assigned=true;}
    else if(binding.module==lab::DisplayModule::Sample){ratio=surfaceProgramRatio(snapshot.edits[std::clamp(binding.pad,0,lab::Pads-1)],binding.page,knob);assigned=true;}
    else if(binding.module==lab::DisplayModule::PadPerform){const auto& sequence=snapshot.sequences[snapshot.sequence];if(knob==0)ratio=int(sequence.mode)/3.;else if(knob==1)ratio=sequence.sourcePad/15.;else if(knob==2)ratio=(sequence.rootNote-24)/60.;else ratio=sequence.bars>1?double(snapshot.viewBar)/(sequence.bars-1):0;assigned=true;}
    else if(binding.module==lab::DisplayModule::NextSequence){const auto& sequence=snapshot.sequences[snapshot.sequence];if(knob==0)ratio=snapshot.sequence/15.;else if(knob==1)ratio=(sequence.bars-1)/3.;else if(knob==2)ratio=sequence.bars>1?double(snapshot.viewBar)/(sequence.bars-1):0;else ratio=int(sequence.mode)/3.;assigned=true;}
    else if(binding.module==lab::DisplayModule::Song){if(knob==0)ratio=songChoice/15.;else if(knob==1)ratio=(songRepeats-1)/127.;else if(knob==2)ratio=snapshot.songLength>1?double(songSelected)/(snapshot.songLength-1):0;else ratio=0;assigned=knob<3;}
    else if(binding.module==lab::DisplayModule::Inserts){
        const auto& insert=snapshot.edits[binding.pad].inserts[binding.slot];
        const int parameter=binding.parameterForKnob(knob);
        if(parameter>=0&&parameter<lab::insertParameterCount(insert.effect)){ratio=insert.parameters[parameter];assigned=true;}
    }
    HBRUSH brush=CreateSolidBrush(RGB(31,38,50));HPEN edge=CreatePen(PS_SOLID,2,line);
    auto oldB=SelectObject(dc,brush),oldP=SelectObject(dc,edge);Ellipse(dc,cx-19,cy-19,cx+19,cy+19);SelectObject(dc,oldB);SelectObject(dc,oldP);DeleteObject(brush);DeleteObject(edge);
    constexpr double pi=3.14159265358979323846;const double angle=(135.+270.*ratio)*pi/180.;
    HPEN mark=CreatePen(PS_SOLID,3,assigned?accent:muted);oldP=SelectObject(dc,mark);MoveToEx(dc,cx,cy,nullptr);LineTo(dc,cx+int(std::cos(angle)*12),cy+int(std::sin(angle)*12));SelectObject(dc,oldP);DeleteObject(mark);
    text(dc,{knobBox.x,knobBox.y+41,knobBox.w,13},L"P"+number(display*4+knob+1),8,assigned?accent:muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

void paintSurfaceDisplay(HDC dc,int display,const lab::Snapshot& snapshot){
    auto module=surfaceModuleBox(display),box=surfaceDisplayBox(display);bool focused=surface.focusedDisplay==display;
    round(dc,module,panel,focused?accent:line);
    round(dc,box,RGB(9,18,22),focused?accent:RGB(34,48,57));
    const auto& binding=surface.displays[display];
    if(binding.module==lab::DisplayModule::TrackMute){
        text(dc,{box.x+14,box.y+9,300,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  TRACK MUTES",12,focused?accent:muted,FW_SEMIBOLD);
        text(dc,{box.x+316,box.y+9,124,24},L"PISTES "+number(display*8+1)+L"–"+number(display*8+8),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        for(int local=0;local<8;++local){const int track=display*8+local;const auto& data=snapshot.sequences[snapshot.sequence].tracks[track];const auto b=surfaceTrackMuteBox(display,local);const bool selectedTrack=track==stepTrack;round(dc,b,data.mute?RGB(81,31,39):(selectedTrack?RGB(31,62,59):RGB(18,31,35)),data.solo?RGB(241,184,88):(selectedTrack?accent:RGB(34,48,57)));text(dc,{b.x+9,b.y+4,b.w-18,18},number(track+1)+L"  "+wide(snapshot.names[track]),10,data.mute?RGB(255,154,135):ink,FW_SEMIBOLD);const std::wstring state=(data.mute?std::wstring(L"MUTE  "):std::wstring(L"ACTIF  "))+(data.solo?L"SOLO":L"");text(dc,{b.x+9,b.y+23,b.w-18,17},state,8,data.solo?RGB(241,184,88):muted,FW_SEMIBOLD);}
        text(dc,{box.x+14,box.y+272,box.w-28,14},L"CLIC / ENTER : MUTE  ·  SHIFT + PAD : SOLO",8,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
    }
    if(binding.module==lab::DisplayModule::Mixer){
        text(dc,{box.x+14,box.y+9,290,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  TRACK MIXER",12,focused?accent:muted,FW_SEMIBOLD);
        text(dc,{box.x+310,box.y+9,130,24},L"PISTES "+number(mixerBank*8+1)+L"–"+number(mixerBank*8+8),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        for(int row=0;row<4;++row){const int trackIndex=mixerTrackFor(display,row);const auto& track=snapshot.sequences[snapshot.sequence].tracks[trackIndex];auto b=surfaceMixerTrackBox(display,row);const bool selectedTrack=trackIndex==stepTrack;fill(dc,b,selectedTrack?RGB(31,62,59):(row%2?RGB(18,31,35):RGB(17,28,33)));const std::wstring type=track.program==lab::ProgramType::Drum?L"D":L"K";text(dc,{b.x+10,b.y,b.w-210,b.h},number(trackIndex+1)+L" ["+type+L"] "+wide(snapshot.names[trackIndex]),10,selectedTrack?accent:ink,FW_SEMIBOLD,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);std::wstring flags=(track.armed?std::wstring(L"REC "):std::wstring())+(track.solo?L"SOLO ":L"")+(track.mute?L"MUTE":L"");text(dc,{b.x+b.w-202,b.y,90,b.h},flags,8,track.mute?RGB(255,133,103):(track.solo?RGB(241,184,88):muted),FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);const std::wstring value=mixerParameter==0?number(int(std::round(track.level*100)))+L" %":(std::abs(track.pan)<.01?L"C":track.pan<0?L"L "+number(int(std::round(-track.pan*100))):L"R "+number(int(std::round(track.pan*100))));text(dc,{b.x+b.w-102,b.y,92,b.h},value,10,accent,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);}
        text(dc,{box.x+14,box.y+268,box.w-28,16},mixerParameter==0?L"KNOBS : LEVEL · bas : PAN · droite : page suivante":L"KNOBS : PAN · haut : LEVEL · gauche/droite : banques",9,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
    }
    if(binding.module==lab::DisplayModule::Browser){
        const auto& browser=browsers[display];
        text(dc,{box.x+14,box.y+9,290,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  BROWSER  ·  PAD "+padName(binding.pad),12,focused?accent:muted,FW_SEMIBOLD);
        text(dc,{box.x+310,box.y+9,130,24},number(int(browser.entries.size()))+L" élément(s)",9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
        fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        button(dc,surfaceBrowserPreviousBox(display),L"<");button(dc,surfaceBrowserCategoryBox(display),surfaceBrowserPath(browser),true);button(dc,surfaceBrowserNextBox(display),L">");
        for(int row=0;row<5;++row){auto b=surfaceBrowserFileBox(display,row);const int index=browser.scroll+row;const bool selectedFile=index==browser.selected&&index<int(browser.entries.size());
            if(selectedFile)round(dc,b,accent,accent);else fill(dc,b,row%2?RGB(18,31,35):RGB(17,28,33));
            const bool directory=index<int(browser.entries.size())&&browser.entries[index].directory;const std::wstring name=index<int(browser.entries.size())?(directory?L"[DOSSIER]  ":L"")+browser.entries[index].path.filename().wstring():(row==0&&browser.entries.empty()?L"Dossier vide":L"");
            text(dc,{b.x+10,b.y,b.w-20,b.h},name,10,selectedFile?bg:(browser.entries.empty()?muted:ink),selectedFile?FW_SEMIBOLD:FW_NORMAL);
        }
        const bool directory=browser.selected>=0&&browser.selected<int(browser.entries.size())&&browser.entries[browser.selected].directory;button(dc,surfaceBrowserModeBox(display),surfaceBrowserModeName(browser.auditionMode),browser.auditionMode>0);button(dc,surfaceBrowserAuditionBox(display),L"Écouter",browser.auditionMode>0&&!directory);button(dc,surfaceBrowserLoadBox(display),directory?L"Ouvrir":L"Charger",!browser.entries.empty());button(dc,surfaceBrowserRefreshBox(display),L"Actualiser");
        return;
    }
    if(binding.module==lab::DisplayModule::PadPerform){
        const auto& sequence=snapshot.sequences[snapshot.sequence];text(dc,{box.x+14,box.y+9,360,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  PAD NOTES / KEYGROUP",12,focused?accent:muted,FW_SEMIBOLD);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        const std::array<std::wstring,4> labels={L"Type",L"Sample source",L"Note racine",L"Mesure affichée"};const std::array<std::wstring,4> values={performanceModeName(sequence.mode),L"Pad "+padName(sequence.sourcePad)+L" · "+wide(snapshot.names[sequence.sourcePad]),noteName(sequence.rootNote),number(snapshot.viewBar+1)+L" / "+number(sequence.bars)};
        for(int knob=0;knob<4;++knob){const int y=box.y+48+knob*55;fill(dc,{box.x+14,y,box.w-28,46},knob%2?RGB(18,31,35):RGB(17,28,33));text(dc,{box.x+24,y,42,46},L"P"+number(display*4+knob+1),10,accent,FW_SEMIBOLD);text(dc,{box.x+68,y,144,46},labels[knob],10,muted);text(dc,{box.x+210,y,216,46},values[knob],11,ink,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);}return;
    }
    if(binding.module==lab::DisplayModule::NextSequence){
        const auto& sequence=snapshot.sequences[snapshot.sequence];text(dc,{box.x+14,box.y+9,360,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  NEXT SEQUENCE",12,focused?accent:muted,FW_SEMIBOLD);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        const std::array<std::wstring,4> labels={L"Séquence",L"Longueur",L"Mesure édition",L"Vue séquenceur"};const std::array<std::wstring,4> values={number(snapshot.sequence+1)+L" · "+wide(sequence.name),number(sequence.bars)+L" mesure(s)",number(snapshot.viewBar+1)+L" / "+number(sequence.bars),performanceModeName(sequence.mode)};
        for(int knob=0;knob<4;++knob){const int y=box.y+48+knob*55;fill(dc,{box.x+14,y,box.w-28,46},knob%2?RGB(18,31,35):RGB(17,28,33));text(dc,{box.x+24,y,42,46},L"P"+number(display*4+knob+1),10,accent,FW_SEMIBOLD);text(dc,{box.x+68,y,144,46},labels[knob],10,muted);text(dc,{box.x+210,y,216,46},values[knob],11,ink,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);}text(dc,{box.x+14,box.y+268,box.w-28,18},snapshot.queuedSequence>=0?L"En attente : Sequence "+number(snapshot.queuedSequence+1):L"Pads 1–16 : sélectionner / mettre en attente",9,snapshot.queuedSequence>=0?RGB(241,184,88):muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
    }
    if(binding.module==lab::DisplayModule::Song){
        text(dc,{box.x+14,box.y+9,360,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  SONG EDITOR",12,focused?accent:muted,FW_SEMIBOLD);text(dc,{box.x+330,box.y+9,96,24},number(snapshot.songLength)+L" étapes",9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));button(dc,surfaceSongActionBox(display,0),L"Ajouter");button(dc,surfaceSongActionBox(display,1),L"Supprimer");button(dc,surfaceSongActionBox(display,2),snapshot.songPlaying?L"Stop":L"Lire",snapshot.songPlaying);button(dc,surfaceSongActionBox(display,3),L"Effacer");
        const int first=std::clamp(songSelected-1,0,std::max(0,snapshot.songLength-4));for(int row=0;row<4;++row){const int index=first+row;auto b=surfaceSongRowBox(display,row);const bool selectedRow=index==songSelected&&index<snapshot.songLength,playing=index==snapshot.songPosition&&snapshot.songPlaying;if(selectedRow)round(dc,b,accent,accent);else fill(dc,b,row%2?RGB(18,31,35):RGB(17,28,33));std::wstring value=index<snapshot.songLength?number(index+1)+L"   SEQ "+number(snapshot.song[index].sequence+1)+L"   × "+number(snapshot.song[index].repeats):(row==0&&snapshot.songLength==0?L"Song vide — choisis un pad puis Ajouter":L"");text(dc,{b.x+10,b.y,b.w-20,b.h},value,10,selectedRow?bg:(playing?accent:muted),selectedRow?FW_SEMIBOLD:FW_NORMAL);}
        text(dc,{box.x+14,box.y+270,box.w-28,16},L"P1 Seq "+number(songChoice+1)+L" · P2 Repeat ×"+number(songRepeats)+L" · P3 Étape "+number(snapshot.songLength?songSelected+1:0),9,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
    }
    if(binding.module==lab::DisplayModule::MidiControl){
        text(dc,{box.x+14,box.y+9,270,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  MIDI CONTROL",12,focused?accent:muted,FW_SEMIBOLD);
        text(dc,{box.x+326,box.y+9,114,24},L"PAGE "+number(binding.page+1)+L" / "+number(midiControlPages()),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
        fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        button(dc,surfaceMidiDeviceBox(display),midiInputName(),midiId>=0);button(dc,surfaceMidiPreviousBox(display),L"<");button(dc,surfaceMidiNextBox(display),L">");
        text(dc,{box.x+14,box.y+90,box.w-28,22},midiLastMessage(),9,midiMessageCount?accent:muted,FW_NORMAL,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
        const bool anyLearn=midiLearnTarget>=0||midiLearnPad>=0||midiLearnCommand>=0;std::wstring learnText=midiLearnTarget>=0?L"EN ATTENTE : bouge le contrôle K"+number(midiLearnTarget+1):midiLearnPad>=0?L"EN ATTENTE : frappe le PAD "+std::wstring(1,wchar_t(L'A'+midiLearnPad/lab::SurfacePads))+number(midiLearnPad%lab::SurfacePads+1):midiLearnCommand>=0?L"EN ATTENTE : actionne "+midiCommandLabels[midiLearnCommand]:(midiProfileLoaded?L"Profil Maschine+ chargé · clic : Learn · droit : option":L"Profil Maschine+ vide · clic : Learn · droit : option");
        text(dc,{box.x+14,box.y+114,box.w-28,22},learnText,9,anyLearn?RGB(241,184,88):muted,FW_SEMIBOLD);
        for(int row=0;row<4;++row){auto b=surfaceMidiBindingBox(display,row);
            if(binding.page<4){const int target=binding.page*4+row;const auto& learned=midiQlinks[target];const bool learning=midiLearnTarget==target;
                if(learning)round(dc,b,RGB(64,49,25),RGB(241,184,88));else fill(dc,b,row%2?RGB(18,31,35):RGB(17,28,33));
                text(dc,{b.x+10,b.y,52,b.h},L"K"+number(target+1),10,learning?RGB(241,184,88):accent,FW_SEMIBOLD);
                text(dc,{b.x+62,b.y,190,b.h},learned.cc<0?L"Non assigné":L"CC "+number(learned.cc)+L" · canal "+number(learned.channel+1),10,learned.cc<0?muted:ink);
                text(dc,{b.x+262,b.y,122,b.h},midiModeName(learned.mode),9,muted,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
            }else if(binding.page<8){const int target=padGroup*lab::SurfacePads+(binding.page-4)*4+row;const auto& learned=midiPads[target];const bool learning=midiLearnPad==target;
                if(learning)round(dc,b,RGB(64,49,25),RGB(241,184,88));else fill(dc,b,row%2?RGB(18,31,35):RGB(17,28,33));
                text(dc,{b.x+10,b.y,74,b.h},L"PAD "+std::wstring(1,wchar_t(L'A'+padGroup))+number(target%lab::SurfacePads+1),10,learning?RGB(241,184,88):accent,FW_SEMIBOLD);
                text(dc,{b.x+86,b.y,298,b.h},learning?L"EN ATTENTE — frappe le pad":midiPadBindingText(learned),10,learning?RGB(241,184,88):(learned.note<0?muted:ink));
            }else {const int target=(binding.page-8)*4+row;if(target>=lab::MidiCommandCount)continue;const auto& learned=midiCommands[target];const bool learning=midiLearnCommand==target;
                if(learning)round(dc,b,RGB(64,49,25),RGB(241,184,88));else fill(dc,b,row%2?RGB(18,31,35):RGB(17,28,33));
                text(dc,{b.x+10,b.y,104,b.h},midiCommandLabels[target],10,learning?RGB(241,184,88):accent,FW_SEMIBOLD);
                text(dc,{b.x+116,b.y,174,b.h},midiCommandBindingText(learned),10,learned.kind==lab::MidiMessageKind::None?muted:ink);
                text(dc,{b.x+296,b.y,88,b.h},target==int(lab::MidiCommandTarget::Turn)?midiModeName(learned.mode):(learned.kind==lab::MidiMessageKind::Note?L"NOTE":learned.kind==lab::MidiMessageKind::ControlChange?L"CC":L""),9,muted,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
            }
        }
        return;
    }
    if(binding.module==lab::DisplayModule::Inserts){
        const int pad=std::clamp(binding.pad,0,lab::Pads-1),slot=std::clamp(binding.slot,0,lab::InsertSlots-1);
        const auto& edit=snapshot.edits[pad];const auto& insert=edit.inserts[slot];const int count=lab::insertParameterCount(insert.effect);
        const int pages=std::max(1,(count+lab::SurfaceQLinks-1)/lab::SurfaceQLinks);
        text(dc,{box.x+14,box.y+9,300,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  INSERTS  ·  PAD "+padName(pad),12,focused?accent:muted,FW_SEMIBOLD);
        text(dc,{box.x+318,box.y+9,48,24},number(binding.page+1)+L"/"+number(pages),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);button(dc,surfacePagePreviousBox(display),L"<");button(dc,surfacePageNextBox(display),L">");
        fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        for(int s=0;s<lab::InsertSlots;++s)button(dc,surfaceInsertSlotBox(display,s),number(s+1)+L"  "+insertName(edit.inserts[s].effect),s==slot);
        button(dc,surfaceInsertEffectBox(display),insert.effect==lab::InsertEffect::None?L"Choisir un effet":L"Effet : "+insertName(insert.effect));
        button(dc,surfaceInsertBypassBox(display),insert.bypass?L"Slot bypass":L"Slot actif",insert.effect!=lab::InsertEffect::None&&!insert.bypass);
        for(int knob=0;knob<lab::SurfaceQLinks;++knob){
            const int parameter=binding.parameterForKnob(knob),y=box.y+142+knob*34;
            const bool valid=parameter>=0&&parameter<count;
            fill(dc,{box.x+14,y,box.w-28,28},knob%2?RGB(18,31,35):RGB(17,28,33));
            text(dc,{box.x+24,y,42,28},L"P"+number(display*lab::SurfaceQLinks+knob+1),10,valid?accent:muted,FW_SEMIBOLD);
            text(dc,{box.x+68,y,220,28},valid?wide(lab::insertParameterName(insert.effect,parameter)):L"—",10,valid?ink:muted);
            text(dc,{box.x+300,y,126,28},valid?insertValue(insert.effect,parameter,insert.parameters[parameter]):L"Non assigné",10,valid?accent:muted,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
        }
        text(dc,{box.x+14,box.y+270,box.w-28,16},L"HAUT/BAS : SLOT · GAUCHE/DROITE : PAGE · ENTER : EFFET",8,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        return;
    }
    if(binding.module==lab::DisplayModule::Sample){
        const int pad=std::clamp(binding.pad,0,lab::Pads-1);const auto& edit=snapshot.edits[pad];
        text(dc,{box.x+14,box.y+9,300,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  PROGRAM EDIT  ·  PAD "+padName(pad),12,focused?accent:muted,FW_SEMIBOLD);
        text(dc,{box.x+318,box.y+9,48,24},number(binding.page+1)+L"/2",9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);button(dc,surfacePagePreviousBox(display),L"<");button(dc,surfacePageNextBox(display),L">");fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        button(dc,surfaceProgramAuditionBox(display),L"Écouter  ·  "+wide(snapshot.names[pad]));
        for(int knob=0;knob<4;++knob){const int y=box.y+94+knob*45;fill(dc,{box.x+14,y,box.w-28,38},knob%2?RGB(18,31,35):RGB(17,28,33));text(dc,{box.x+24,y,42,38},L"P"+number(display*4+knob+1),10,accent,FW_SEMIBOLD);text(dc,{box.x+68,y,220,38},surfaceProgramParameterName(binding.page,knob),11,ink);text(dc,{box.x+300,y,126,38},surfaceProgramParameterValue(edit,binding.page,knob),10,accent,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);}
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

std::wstring surfaceSoftLabel(int display,int soft,const lab::Snapshot& snapshot)const{
    const auto module=surface.displays[display].module;
    if(module==lab::DisplayModule::TrackMute){const int track=display*8+soft;const auto& data=snapshot.sequences[snapshot.sequence].tracks[track];return (data.mute?L"M ":data.solo?L"S ":L"")+number(track+1);}
    if(module==lab::DisplayModule::Mixer){const int track=mixerTrackFor(display,soft);const auto& data=snapshot.sequences[snapshot.sequence].tracks[track];return (data.mute?L"M ":data.solo?L"S ":data.armed?L"R ":L"")+number(track+1);}
    if(module==lab::DisplayModule::Browser)return std::array<std::wstring,4>{L"RETOUR",L"ÉCOUTER",L"CHARGER",L"ACTUALISER"}[soft];
    if(module==lab::DisplayModule::Inserts)return L"SLOT "+number(soft+1);
    if(module==lab::DisplayModule::Sample)return std::array<std::wstring,4>{L"SAMPLE",L"ADSR",L"ÉCOUTER",L"FX"}[soft];
    if(module==lab::DisplayModule::MidiControl)return std::array<std::wstring,4>{L"SAVE MIDI",L"DOSSIER",L"< PAGE",L"PAGE >"}[soft];
    if(module==lab::DisplayModule::Menu){static const std::array<int,8> items={0,1,2,3,5,11,12,15};return surfaceMenuLabels[items[display*4+soft]];}
    return L"SOFT "+number(mixerBank*8+display*4+soft+1);
}

void activateSurfaceSoft(int display,int soft,int bank=-1){
    display=std::clamp(display,0,1);soft=std::clamp(soft,0,3);if(bank>=0)mixerBank=std::clamp(bank,0,1);surface.focusDisplay(display);auto module=surface.displays[display].module;
    if(module==lab::DisplayModule::Mixer){showSurfaceMixer();const int track=mixerTrackFor(display,soft);selectTrack(track);if(surface.shift)toggleTrackArm(track);status=L"Track "+number(track+1)+(surface.shift?L" : armement basculé.":L" sélectionnée dans le Mixer.");return;}
    if(module==lab::DisplayModule::TrackMute){const int track=display*8+soft;selectTrack(track);if(surface.shift)toggleTrackSolo(track);else toggleTrackMute(track);return;}
    if(module==lab::DisplayModule::Browser){if(soft==0)moveSurfaceBrowserCategory(display,-1);else if(soft==1)auditionSurfaceBrowser(display);else if(soft==2)loadSurfaceBrowserSelection(display);else refreshSurfaceBrowser(display,true);return;}
    if(module==lab::DisplayModule::Inserts){selectedInsert=soft;arrangeSurfaceInsertPages(surface.displays[display].pad,soft,display);return;}
    if(module==lab::DisplayModule::Sample){if(soft<2)surface.displays[display].page=soft;else if(soft==2)engine.trigger(surface.displays[display].pad);else showSurfaceInserts(display);return;}
    if(module==lab::DisplayModule::MidiControl){auto& binding=surface.displays[display];if(soft==0){const bool saved=saveMidiProfile();status=saved?L"Profil MIDI sauvegardé : "+portableMidiProfile.wstring():L"Impossible de sauvegarder le profil MIDI.";}else if(soft==1)ShellExecuteW(window,L"open",portableMidiProfile.parent_path().c_str(),nullptr,nullptr,SW_SHOWNORMAL);else if(soft==2)binding.page=std::max(0,binding.page-1);else binding.page=std::min(midiControlPages()-1,binding.page+1);return;}
    if(module==lab::DisplayModule::Menu){static const std::array<int,8> items={0,1,2,3,5,11,12,15};selectSurfaceMenu(items[display*4+soft]);activateSurfaceMenu();return;}
    status=L"Soft contextuel "+number(mixerBank*8+display*4+soft+1)+L" reçu.";
}

void paintSurface(HDC dc,const lab::Snapshot& snapshot){
    for(int display=0;display<2;++display)for(int soft=0;soft<4;++soft)
        button(dc,surfaceSoftButtonBox(display,soft),surfaceSoftLabel(display,soft,snapshot));
    paintSurfaceDisplay(dc,0,snapshot);paintSurfaceDisplay(dc,1,snapshot);
    for(int display=0;display<2;++display)for(int knob=0;knob<4;++knob)paintSurfaceKnob(dc,display,knob,snapshot);
}

void hardwareButton(HDC dc,Box box,const std::wstring& primary,const std::wstring& shifted=L"",bool on=false,bool available=true){
    round(dc,box,on?accent:RGB(28,34,45),on?accent:(available?line:RGB(35,41,51)));
    text(dc,{box.x+6,box.y+3,box.w-12,shifted.empty()?box.h-6:17},primary,10,on?bg:(available?ink:muted),FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
    if(!shifted.empty())text(dc,{box.x+6,box.y+19,box.w-12,box.h-21},shifted,8,on?RGB(24,55,51):muted,FW_NORMAL,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
}

void paintMaschineControls(HDC dc,const lab::Snapshot& snapshot){
    static const std::array<std::pair<const wchar_t*,const wchar_t*>,12> shortcuts={{
        {L"CHANNEL",L"MIDI"},{L"PLUG-IN",L"Instance"},{L"IDEAS",L"Song"},{L"MIXER",L""},
        {L"BROWSER",L"Plug-in"},{L"SAMPLING",L""},{L"<",L"Page"},{L">",L"Page"},
        {L"FILE",L"Save"},{L"SETTINGS",L""},{L"AUTO",L""},{L"MACRO",L"Set"}
    }};
    for(int i=0;i<12;++i){const bool available=i!=10;hardwareButton(dc,shortcutBox(i),shortcuts[i].first,shortcuts[i].second,i==3&&surface.displays[0].module==lab::DisplayModule::Mixer,available);}

    static const std::array<std::pair<const wchar_t*,const wchar_t*>,5> small={{
        {L"VOLUME",L"Velocity"},{L"NOTE REPEAT",L"Arp"},{L"SWING",L"Position"},{L"LOCK",L"Ext Lock"},{L"TEMPO",L"Tune"}
    }};
    hardwareButton(dc,smallControlBox(0,0),small[0].first,small[0].second);
    hardwareButton(dc,smallControlBox(1,0),small[1].first,small[1].second,false,false);
    hardwareButton(dc,smallControlBox(0,1),small[2].first,small[2].second,false,false);
    hardwareButton(dc,smallControlBox(1,1),small[3].first,small[3].second,false,false);
    hardwareButton(dc,smallControlBox(0,2),small[4].first,small[4].second);
    text(dc,smallControlBox(1,2),number(int(snapshot.tempo))+L" BPM",15,accent,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

    static const std::array<const wchar_t*,4> performance={L"PITCH / PAD EDIT",L"MOD",L"PERFORM / FX",L"NOTES"};
    for(int i=0;i<4;++i)hardwareButton(dc,performanceButtonBox(i),performance[i],L"",false,i==0||i==2||i==3);

    for(int group=0;group<8;++group)hardwareButton(dc,groupBox(group),std::wstring(1,wchar_t(L'A'+group)),L"",group==padGroup);

    static const std::array<std::pair<const wchar_t*,const wchar_t*>,9> commands={{
        {L"FIXED VEL",L"16 Vel"},{L"SCENE",L"Section"},{L"PATTERN",L"Clip"},
        {L"EVENTS",L""},{L"VARIATION",L""},{L"DUPLICATE",L"Double"},
        {L"SELECT",L""},{L"SOLO",L""},{L"MUTE",L"Choke"}
    }};
    for(int i=0;i<9;++i){const bool available=i==2||i>=6;const bool active=i==6?selectLatch:i==7?soloLatch:i==8?muteLatch:false;hardwareButton(dc,sideCommandBox(i),commands[i].first,commands[i].second,active,available);}

    // Reserved symmetrically for the next KICK modules. They are deliberately
    // blank and have no hit handling yet.
    for(int i=0;i<9;++i)round(dc,futureCommandBox(i),RGB(18,24,34),RGB(35,47,62));

    static const std::array<const wchar_t*,4> modes={L"PAD MODE",L"KEYBOARD",L"CHORDS",L"STEP"};
    for(int mode=0;mode<4;++mode)hardwareButton(dc,modeBox(mode),modes[mode],L"",mode==padMode,mode!=2);

    const auto& sequence=snapshot.sequences[snapshot.sequence];const bool notes=padMode==1;const bool steps=padMode==3;const bool melodicSteps=steps&&sequence.mode!=lab::PerformanceMode::Drum;
    const bool playVisible=snapshot.playing&&snapshot.playBar==snapshot.viewBar;ULONGLONG now=GetTickCount64();
    for(int p=0;p<16;++p){
        const auto box=padBox(p);const bool active=steps?(melodicSteps?snapshot.melody[p]==engine.performanceNote(stepTrack):snapshot.pattern[stepTrack][p]):notes?false:snapshot.active[p];
        const bool hot=now<flashed[p]||active||(steps&&playVisible&&snapshot.step==p);const COLORREF color=steps?RGB(241,184,88):padColor(p);
        round(dc,box,hot?color:RGB(36,42,53),padMode==0&&p==selected?color:line);
        std::wstring title=steps?L"STEP "+number(p+1):notes?noteName(engine.performanceNote(p)):wide(snapshot.names[p]);
        std::wstring sub=steps?(melodicSteps?noteName(engine.performanceNote(stepTrack)):L"TRACK "+padName(stepTrack)):notes?L"PAD "+padName(sequence.sourcePad):L"PAD "+padName(p);
        text(dc,{box.x+8,box.y+7,box.w-16,25},title,12,hot?bg:ink,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
        text(dc,{box.x+8,box.y+52,box.w-16,18},sub,9,hot?RGB(40,46,50):muted,FW_NORMAL,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
    }

    hardwareButton(dc,transportBox(0),L"RESTART",L"Loop",false,false);
    hardwareButton(dc,transportBox(1),L"ERASE",L"Replace",false,false);
    hardwareButton(dc,transportBox(2),L"TAP",L"Metro",false,false);
    hardwareButton(dc,transportBox(3),L"FOLLOW",L"Grid "+recordQuantizeName(),false,true);
    hardwareButton(dc,transportBox(4),snapshot.playing?L"PAUSE":L"PLAY",L"",snapshot.playing);
    hardwareButton(dc,transportBox(5),L"REC",recordArmed?(snapshot.playing?L"Recording ":L"Armed ")+recordQuantizeName():L"Count-in",recordArmed,true);
    hardwareButton(dc,transportBox(6),L"STOP");
    hardwareButton(dc,transportBox(7),L"SHIFT",L"",surface.shift);

    paintBrandPanel(dc,snapshot);
}

void paintMasterMeter(HDC dc,double value,double peak){
    value=std::clamp(value,0.,1.);peak=std::max(0.,peak);const Box meter{1394,784,152,14};
    const double db=peak>0?20.*std::log10(peak):-60.;const double ratio=std::clamp((db+60.)/60.,0.,1.);const int lit=int((meter.w-4)*ratio);
    text(dc,{1394,746,152,16},L"MASTER  "+number(int(value*100))+L" %",9,ink,FW_SEMIBOLD);
    const std::wstring level=peak>=1?L"CLIP":(db<=-59.9?L"−∞ dB":number(int(std::round(db)))+L" dB");text(dc,{1394,764,152,16},level,9,peak>=1?RGB(255,95,90):muted,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
    round(dc,meter,RGB(20,26,34),line);if(lit>0){const int green=std::min(lit,int((meter.w-4)*.8)),amber=std::min(lit-green,int((meter.w-4)*.15)),red=lit-green-amber;if(green>0)fill(dc,{meter.x+2,meter.y+3,green,meter.h-6},accent);if(amber>0)fill(dc,{meter.x+2+green,meter.y+3,amber,meter.h-6},RGB(241,184,88));if(red>0)fill(dc,{meter.x+2+green+amber,meter.y+3,red,meter.h-6},RGB(255,95,90));}
    const int marker=meter.x+2+int((meter.w-4)*value);fill(dc,{marker-1,meter.y-2,2,meter.h+4},ink);
}

void paintBackdrop(HDC dc){
    // Quiet organic/cybernetic texture: thin traces only, kept behind every
    // control so readability remains identical on low-contrast displays.
    HPEN blue=CreatePen(PS_SOLID,1,RGB(15,42,58));HPEN red=CreatePen(PS_SOLID,1,RGB(56,22,30));
    auto old=SelectObject(dc,blue);
    for(int y=408;y<890;y+=48){MoveToEx(dc,270,y,nullptr);LineTo(dc,1360,y+((y/48)%2?18:-18));}
    SelectObject(dc,red);
    for(int x=270;x<1360;x+=92){MoveToEx(dc,x,404,nullptr);LineTo(dc,x+118,884);}
    Arc(dc,330,470,635,790,330,630,635,630);Arc(dc,1040,490,1344,820,1040,650,1344,650);
    SelectObject(dc,old);DeleteObject(blue);DeleteObject(red);
}

void paintBrandPanel(HDC dc,const lab::Snapshot& snapshot){
    const auto box=brandPanelBox();round(dc,box,RGB(11,17,27),RGB(42,69,88));
    HPEN blue=CreatePen(PS_SOLID,1,RGB(23,65,86)),red=CreatePen(PS_SOLID,1,RGB(89,30,39));auto old=SelectObject(dc,blue);
    for(int y=box.y+18;y<box.y+box.h-18;y+=42){MoveToEx(dc,box.x+12,y,nullptr);LineTo(dc,box.x+box.w-12,y+18);}
    SelectObject(dc,red);for(int y=box.y+30;y<box.y+box.h-20;y+=84)Arc(dc,box.x+22,y,box.x+box.w-22,y+122,box.x+22,y+60,box.x+box.w-22,y+60);
    SelectObject(dc,old);DeleteObject(blue);DeleteObject(red);
    fill(dc,{box.x+10,box.y+10,box.w-20,1},accent);fill(dc,{box.x+10,box.y+box.h-11,box.w-20,1},RGB(137,43,53));
    text(dc,{1382,48,176,68},L"KICK",42,ink,FW_BOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    text(dc,{1382,114,176,28},L"ANKS Labs",13,accent,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    text(dc,{1390,202,160,110},status,9,muted,FW_NORMAL,DT_CENTER|DT_TOP|DT_WORDBREAK|DT_END_ELLIPSIS);
    text(dc,{1392,626,156,18},audio.running?L"AUDIO WINDOWS · ACTIF":L"AUDIO · ARRÊT",8,audio.running?accent:RGB(255,133,103),FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    text(dc,{1392,646,156,18},midiId>=0?L"MIDI · CONNECTÉ":L"MIDI · DÉCONNECTÉ",8,midiId>=0?accent:muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    text(dc,{1392,702,156,16},L"MASTER / WINDOWS",9,ink,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    text(dc,{1392,720,156,14},L"DSP "+decimal(audio.renderPermille.load()/10.,1)+L" % · "+number(int(audio.lateBlocks.load()))+L" retards",8,audio.lateBlocks.load()?RGB(255,133,103):muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    meterPeak=std::max(double(snapshot.peak),meterPeak*.9);paintMasterMeter(dc,snapshot.volume,meterPeak);
}

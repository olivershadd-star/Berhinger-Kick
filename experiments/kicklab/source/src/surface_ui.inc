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

// The two KICK page displays use all the horizontal room up to the identity
// column. Four context knobs remain physically attached to each page.
static constexpr int SurfaceFirstX=278,SurfaceGap=566,SurfaceY=38,SurfaceDisplayWidth=552,SurfaceContentWidth=524;
int surfaceX(int display)const{return SurfaceFirstX+display*SurfaceGap;}
Box surfaceModuleBox(int display)const{return {surfaceX(display)-6,SurfaceY-12,SurfaceDisplayWidth+12,374};}
Box surfaceDisplayBox(int display)const{return {surfaceX(display),SurfaceY,SurfaceDisplayWidth,294};}
Box surfaceDisplayItemBox(int display,int local)const{return {surfaceX(display)+14+(local%2)*268,SurfaceY+46+(local/2)*56,256,46};}
Box surfaceKnobBox(int display,int knob)const{return {surfaceX(display)+25+knob*138,SurfaceY+300,88,54};}
Box surfaceSoftButtonBox(int display,int buttonIndex)const{return {surfaceX(display)+buttonIndex*138,4,128,27};}
Box surfaceInsertSlotBox(int display,int slot)const{return {surfaceX(display)+14+slot*134,SurfaceY+47,124,34};}
Box surfaceInsertEffectBox(int display)const{return {surfaceX(display)+14,SurfaceY+94,326,36};}
Box surfaceInsertBypassBox(int display)const{return {surfaceX(display)+350,SurfaceY+94,188,36};}
Box surfacePagePreviousBox(int display)const{return {surfaceX(display)+SurfaceDisplayWidth-88,SurfaceY+8,28,24};}
Box surfacePageNextBox(int display)const{return {surfaceX(display)+SurfaceDisplayWidth-54,SurfaceY+8,28,24};}
Box surfaceProgramAuditionBox(int display)const{return {surfaceX(display)+14,SurfaceY+46,SurfaceContentWidth,34};}
Box surfaceMidiDeviceBox(int display)const{return {surfaceX(display)+14,SurfaceY+46,342,34};}
Box surfaceMidiPreviousBox(int display)const{return {surfaceX(display)+406,SurfaceY+46,58,34};}
Box surfaceMidiNextBox(int display)const{return {surfaceX(display)+472,SurfaceY+46,66,34};}
Box surfaceMidiBindingBox(int display,int row)const{return {surfaceX(display)+14,SurfaceY+146+row*34,SurfaceContentWidth,28};}
Box surfaceBrowserPreviousBox(int display)const{return {surfaceX(display)+14,SurfaceY+46,42,34};}
Box surfaceBrowserCategoryBox(int display)const{return {surfaceX(display)+62,SurfaceY+46,422,34};}
Box surfaceBrowserNextBox(int display)const{return {surfaceX(display)+496,SurfaceY+46,42,34};}
Box surfaceBrowserFileBox(int display,int row)const{return {surfaceX(display)+14,SurfaceY+90+row*31,SurfaceContentWidth,27};}
Box surfaceBrowserModeBox(int display)const{return {surfaceX(display)+14,SurfaceY+252,105,30};}
Box surfaceBrowserAuditionBox(int display)const{return {surfaceX(display)+127,SurfaceY+252,105,30};}
Box surfaceBrowserLoadBox(int display)const{return {surfaceX(display)+240,SurfaceY+252,134,30};}
Box surfaceBrowserRefreshBox(int display)const{return {surfaceX(display)+382,SurfaceY+252,156,30};}
Box surfaceSongActionBox(int display,int action)const{return {surfaceX(display)+14+action*132,SurfaceY+46,124,32};}
Box surfaceSongRowBox(int display,int row)const{return {surfaceX(display)+14,SurfaceY+94+row*43,SurfaceContentWidth,36};}
Box surfaceMixerTrackBox(int display,int row)const{return {surfaceX(display)+14,SurfaceY+54+row*52,SurfaceContentWidth,44};}
Box surfaceTrackMuteBox(int display,int local)const{return {surfaceX(display)+14+(local%2)*268,SurfaceY+48+(local/2)*56,256,46};}
Box surfaceColorBox(int color)const{return {surfaceX(0)+14+(color%6)*87,SurfaceY+60+(color/6)*62,79,52};}
Box surfaceColorPadBox(int pad)const{return {surfaceX(1)+14+(pad%4)*130,SurfaceY+58+(pad/4)*52,122,44};}
Box surfaceEditPadBox(int pad)const{return {surfaceX(0)+14+(pad%4)*130,SurfaceY+58+(pad/4)*52,122,44};}

Box shortcutBox(int index)const{return {20+(index%2)*120,38+(index/2)*48,112,38};}
Box utilityButtonBox(int column)const{return {20+column*120,326,112,38};}
Box groupBox(int group)const{return {20+(group%4)*108,604+(group/4)*54,100,46};}
Box modeBox(int mode)const{return {778+mode*116,420,108,32};}
Box sideCommandBox(int command)const{return {650,466+command*41,112,34};}
Box futureCommandBox(int command)const{return {1246,466+command*41,110,34};}
Box smallControlBox(int column,int row)const{return {20+column*120,374+row*48,112,38};}
Box performanceButtonBox(int column)const{return {20+column*108,526,100,38};}
Box transportBox(int index)const{return {20+(index%4)*108,768+(index/4)*44,100,36};}
Box brandPanelBox()const{return {1412,8,176,884};}
Box brandHomeBox()const{return {1418,16,164,108};}
Box brandControlBox()const{return {1418,138,164,494};}
Box brandMacroBox(int macro)const{return {1427+(macro%2)*76,154+(macro/2)*67,66,58};}
Box brandPushBox(int buttonIndex)const{return {1427+(buttonIndex%2)*76,438+(buttonIndex/2)*69,66,54};}
Box brandMasterBox()const{return {1418,650,164,234};}

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
    for(auto& binding:surface.displays)if(binding.module==lab::DisplayModule::Events)binding.pad=stepTrack;
}

void selectGroupBlock(int group){
    group=std::clamp(group,0,lab::Groups-1);const auto before=engine.snapshot();std::wstring prefix;
    if(duplicateSourceSequence>=0){engine.copySequence(duplicateSourceSequence,group);markProjectDirty();prefix=L"Pattern "+number(duplicateSourceSequence+1)+L" copié vers "+std::wstring(1,wchar_t(L'A'+group))+L". ";duplicateSourceSequence=-1;}
    engine.setGroup(group);markProjectDirty();const auto after=engine.snapshot();padGroup=after.group;programSelected=after.sequences[group].program;selectPad(0);stepTrack=0;
    if(before.playing&&after.group!=group)status=prefix+L"Bloc "+std::wstring(1,wchar_t(L'A'+group))+L" en attente — changement à la fin du pattern courant.";
    else status=prefix+L"Pattern "+std::wstring(1,wchar_t(L'A'+group))+L" actif — Program : "+wide(after.programs[after.program].name)+L".";
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

int padMixerPadFor(int display,int control)const{return std::clamp(padMixerBank*8+display*4+control,0,lab::Pads-1);}

void showSurfacePadMixer(int bank=-1){
    if(bank>=0)padMixerBank=std::clamp(bank,0,1);
    for(int display=0;display<lab::SurfaceDisplays;++display){auto& binding=surface.displays[display];binding.module=lab::DisplayModule::PadMixer;binding.page=padMixerBank;binding.followsSelectedPad=false;}
    surface.focusDisplay((selected%8)/4);status=L"Pad Mixer KICK — Program actif, page "+number(padMixerBank+1)+L" : P1–P4 règlent Level ou Pan ; clic droit mute le pad.";
}

void showSurfacePadMute(){
    for(int display=0;display<lab::SurfaceDisplays;++display){auto& binding=surface.displays[display];binding.module=lab::DisplayModule::PadMute;binding.page=display;binding.followsSelectedPad=false;}
    surface.focusDisplay(selected/8);status=L"Pad Mutes KICK — pads/clic : mute du son dans le Program actif ; les pads d'un même Choke Group suivent ensemble.";
}

void toggleProgramPadMute(int pad){engine.togglePadMute(std::clamp(pad,0,lab::Pads-1));markProjectDirty();}

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

void showSurfaceTempo(int display){
    display=std::clamp(display,0,1);auto& binding=surface.displays[display];binding.module=lab::DisplayModule::Tempo;binding.page=0;surface.focusDisplay(display);
    status=L"Tempo — P1 règle 40–240 BPM · P2 règle par dixième · touches −/＋ sous l'écran.";
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

void showSurfacePrograms(int display=0){
    const auto snapshot=engine.snapshot();programSelected=std::clamp(snapshot.sequences[snapshot.sequence].program,0,lab::Programs-1);
    for(int screen=0;screen<lab::SurfaceDisplays;++screen){auto& binding=surface.displays[screen];binding.module=lab::DisplayModule::Programs;binding.page=screen;binding.followsSelectedPad=false;}
    surface.focusDisplay(programSelected/8);
    status=L"Program Manager — un Program contient les 16 samples et leurs réglages ; le Pattern ne contient plus que les événements.";
}

void assignSelectedProgram(){
    const auto snapshot=engine.snapshot();if(programSelected<0||programSelected>=lab::Programs||!snapshot.programs[programSelected].used){status=L"Program vide — crée ou duplique d'abord un Program.";return;}
    if(engine.assignProgramToSequence(snapshot.sequence,programSelected)){markProjectDirty();status=L"Pattern "+number(snapshot.sequence+1)+L" relié à "+wide(snapshot.programs[programSelected].name)+L".";}
}

void createSurfaceProgram(bool duplicate){
    const auto snapshot=engine.snapshot();int created=duplicate?engine.duplicateProgram(programSelected):engine.createProgram("Drum Program "+std::to_string(programSelected+1));
    if(created<0){status=duplicate?L"Duplication impossible : aucun slot libre ou sample source introuvable.":L"Les 16 slots de Programs sont occupés.";return;}
    programSelected=created;engine.assignProgramToSequence(snapshot.sequence,created);markProjectDirty();surface.focusDisplay(created/8);
    const auto updated=engine.snapshot();status=(duplicate?L"Program dupliqué et assigné : ":L"Nouveau Program assigné : ")+wide(updated.programs[created].name)+L".";
}

void deleteSurfaceProgram(){
    const auto snapshot=engine.snapshot();if(engine.deleteProgram(programSelected)){markProjectDirty();status=L"Program supprimé.";for(int i=0;i<lab::Programs;++i)if(engine.snapshot().programs[i].used){programSelected=i;break;}}
    else status=L"Suppression refusée : ce Program est actif ou encore référencé par un Pattern.";
}

void showSurfaceSong(int display){
    display=std::clamp(display,0,1);auto& binding=surface.displays[display];binding.module=lab::DisplayModule::Song;binding.page=0;surface.focusDisplay(display);auto snapshot=engine.snapshot();songSelected=std::clamp(songSelected,0,std::max(0,snapshot.songLength-1));status=L"Song Editor — choisis une séquence avec les pads, règle Repeat puis ajoute-la.";
}

void showSurfaceEvents(int display){
    display=std::clamp(display,0,1);padMode=3;auto& binding=surface.displays[display];binding.module=lab::DisplayModule::Events;binding.pad=stepTrack;binding.slot=eventStep;binding.page=0;binding.followsSelectedPad=false;surface.focusDisplay(display);status=L"Events KICK — choisis un step avec les pads, puis règle Velocity, Probability, Ratchet et Length avec P1–P4.";
}

lab::StepEvent surfaceStepEvent(const lab::Snapshot& snapshot,const lab::DisplayBinding& binding)const{
    const auto& sequence=snapshot.sequences[snapshot.sequence];const int absolute=snapshot.viewBar*lab::Steps+std::clamp(binding.slot,0,lab::Steps-1);return sequence.mode==lab::PerformanceMode::Drum?sequence.drumEvents[std::clamp(binding.pad,0,lab::Pads-1)][absolute]:sequence.melodyEvents[absolute];
}

double surfaceEventRatio(const lab::StepEvent& event,int knob)const{
    if(knob==0)return (event.velocity-1)/126.;if(knob==1)return event.probability/100.;if(knob==2)return (event.ratchet-1)/7.;return (event.length-1)/15.;
}

void setSurfaceEventRatio(int display,int knob,double ratio){
    if(display<0||display>=lab::SurfaceDisplays||knob<0||knob>=lab::SurfaceQLinks)return;auto& binding=surface.displays[display];if(binding.module!=lab::DisplayModule::Events)return;auto snapshot=engine.snapshot();auto event=surfaceStepEvent(snapshot,binding);ratio=std::clamp(ratio,0.,1.);if(knob==0)event.velocity=1+int(std::lround(ratio*126));else if(knob==1)event.probability=int(std::lround(ratio*100));else if(knob==2)event.ratchet=1+int(std::lround(ratio*7));else event.length=1+int(std::lround(ratio*15));const bool melodic=snapshot.sequences[snapshot.sequence].mode!=lab::PerformanceMode::Drum;engine.setStepEvent(std::clamp(binding.pad,0,lab::Pads-1),snapshot.viewBar*lab::Steps+std::clamp(binding.slot,0,lab::Steps-1),event,melodic);markProjectDirty();
}

struct ListEditItem {bool melodic=false;int track=0,step=0,note=-1;lab::StepEvent event{};};

std::wstring listEditTimeText(int step)const{return number(step/lab::Steps+1)+L":"+number((step%lab::Steps)/4+1)+L":"+number(step%4+1);}

std::vector<ListEditItem> listEditItems(const lab::Snapshot& snapshot)const{
    std::vector<ListEditItem> items;const auto& sequence=snapshot.sequences[snapshot.sequence];const int total=sequence.bars*lab::Steps;
    for(int step=0;step<total;++step){
        for(int track=0;track<lab::Pads;++track)if(sequence.drum[track][step])items.push_back({false,track,step,-1,sequence.drumEvents[track][step]});
        if(sequence.melody[step]>=0)items.push_back({true,sequence.sourcePad,step,sequence.melody[step],sequence.melodyEvents[step]});
    }
    return items;
}

bool currentListEditItem(const lab::Snapshot& snapshot,ListEditItem& item)const{
    const auto items=listEditItems(snapshot);if(items.empty())return false;item=items[std::clamp(listEditRow,0,int(items.size())-1)];return true;
}

void selectListEditEvent(bool melodic,int track,int step){
    const auto items=listEditItems(engine.snapshot());for(int index=0;index<int(items.size());++index)if(items[index].melodic==melodic&&items[index].track==track&&items[index].step==step){listEditRow=index;return;}listEditRow=std::clamp(listEditRow,0,std::max(0,int(items.size())-1));
}

void moveListEditSelection(int delta){const auto items=listEditItems(engine.snapshot());if(items.empty())listEditRow=0;else listEditRow=std::clamp(listEditRow+(delta>0?1:-1),0,int(items.size())-1);}

void showSurfaceListEdit(){
    for(int display=0;display<lab::SurfaceDisplays;++display){auto& binding=surface.displays[display];binding.module=lab::DisplayModule::ListEdit;binding.page=display;binding.followsSelectedPad=false;}
    const auto items=listEditItems(engine.snapshot());listEditRow=std::clamp(listEditRow,0,std::max(0,int(items.size())-1));surface.focusDisplay(0);status=L"List Edit KICK — tous les événements du pattern ; écran A : Time/Pad-Note, écran B : Length/Velocity/Probability/Ratchet.";
}

void selectListEditTrack(int track){
    const auto items=listEditItems(engine.snapshot());for(int index=0;index<int(items.size());++index)if(!items[index].melodic&&items[index].track==track){listEditRow=index;selectTrack(track);status=L"List Edit — premier événement de la piste "+number(track+1)+L" sélectionné.";return;}status=L"List Edit — aucun événement sur la piste "+number(track+1)+L".";
}

void insertListEditEvent(){
    const auto snapshot=engine.snapshot();const auto& sequence=snapshot.sequences[snapshot.sequence];ListEditItem current;const bool has=currentListEditItem(snapshot,current);const bool melodic=has?current.melodic:sequence.mode!=lab::PerformanceMode::Drum;const int trackOrNote=has?(melodic?current.note:current.track):(melodic?engine.performanceNote(stepTrack):stepTrack);const int total=sequence.bars*lab::Steps,start=has?(current.step+1)%total:snapshot.viewBar*lab::Steps+eventStep;
    for(int offset=0;offset<total;++offset){const int step=(start+offset)%total;if(engine.insertStepEvent(trackOrNote,step,melodic)){markProjectDirty();selectListEditEvent(melodic,melodic?sequence.sourcePad:trackOrNote,step);status=L"List Edit — événement inséré au step "+number(step+1)+L".";return;}}
    status=L"List Edit — aucun emplacement libre pour cet événement.";
}

void deleteListEditEvent(){
    const auto snapshot=engine.snapshot();ListEditItem item;if(!currentListEditItem(snapshot,item)){status=L"List Edit — aucun événement à supprimer.";return;}if(engine.deleteStepEvent(item.track,item.step,item.melodic)){markProjectDirty();const auto remaining=listEditItems(engine.snapshot());listEditRow=std::clamp(listEditRow,0,std::max(0,int(remaining.size())-1));status=L"List Edit — événement supprimé.";}
}

double listEditRatio(const lab::Snapshot& snapshot,int display,int knob)const{
    ListEditItem item;if(!currentListEditItem(snapshot,item))return 0.;const auto& sequence=snapshot.sequences[snapshot.sequence];if(display==0){if(knob==0)return item.step/double(std::max(1,sequence.bars*lab::Steps-1));if(knob==1)return (item.melodic?item.note:item.track)/double(item.melodic?127:lab::Pads-1);if(knob==2){const auto count=listEditItems(snapshot).size();return count>1?listEditRow/double(count-1):0.;}return 0.;}if(knob==0)return (item.event.length-1)/15.;if(knob==1)return (item.event.velocity-1)/126.;if(knob==2)return item.event.probability/100.;return (item.event.ratchet-1)/7.;
}

void setListEditRatio(int display,int knob,double ratio){
    auto snapshot=engine.snapshot();ListEditItem item;if(!currentListEditItem(snapshot,item))return;ratio=std::clamp(ratio,0.,1.);const auto& sequence=snapshot.sequences[snapshot.sequence];
    if(display==0){if(knob==2){const auto count=listEditItems(snapshot).size();listEditRow=count?std::clamp(int(std::lround(ratio*(count-1))),0,int(count)-1):0;return;}if(knob>1)return;const int destinationStep=knob==0?std::clamp(int(std::lround(ratio*(sequence.bars*lab::Steps-1))),0,sequence.bars*lab::Steps-1):item.step;const int destination=knob==1?std::clamp(int(std::lround(ratio*(item.melodic?127:lab::Pads-1))),0,item.melodic?127:lab::Pads-1):(item.melodic?item.note:item.track);if(engine.moveStepEvent(item.track,item.step,destination,destinationStep,item.melodic)){markProjectDirty();selectListEditEvent(item.melodic,item.melodic?item.track:destination,destinationStep);}else status=L"List Edit — destination déjà occupée.";return;}
    auto event=item.event;if(knob==0)event.length=1+int(std::lround(ratio*15));else if(knob==1)event.velocity=1+int(std::lround(ratio*126));else if(knob==2)event.probability=int(std::lround(ratio*100));else event.ratchet=1+int(std::lround(ratio*7));engine.setStepEvent(item.track,item.step,event,item.melodic);markProjectDirty();
}

void showSurfaceColorEditor(){
    const auto snapshot=engine.snapshot();colorSelection=snapshot.edits[std::clamp(selected,0,lab::Pads-1)].color;
    for(int display=0;display<lab::SurfaceDisplays;++display){auto& binding=surface.displays[display];binding.module=lab::DisplayModule::ColorEditor;binding.page=display;binding.followsSelectedPad=false;}
    surface.focusDisplay(0);status=L"Color Editor KICK — choisis une couleur puis un pad ; les modes Empty, Pad Off et Pad On appartiennent au Program actif.";
}

void applySurfaceColor(int pad){
    pad=std::clamp(pad,0,lab::Pads-1);selectPad(pad);engine.setProgramPadColor(pad,colorSelection,colorAllPads);markProjectDirty();status=colorAllPads?L"Color Editor — couleur appliquée aux seize pads.":L"Color Editor — couleur appliquée au pad "+padName(pad)+L".";
}

void showSurfaceEditOperations(){
    const auto snapshot=engine.snapshot();editSourcePad=std::clamp(selected,0,lab::Pads-1);editDestinationPad=editSourcePad<lab::Pads-1?editSourcePad+1:0;editPadRole=0;
    eraseSequence=snapshot.sequence;eraseTrack=std::clamp(stepTrack,0,lab::Pads-1);eraseStartStep=snapshot.viewBar*lab::Steps;eraseEndStep=std::min(snapshot.sequences[eraseSequence].bars*lab::Steps-1,eraseStartStep+lab::Steps-1);
    for(int display=0;display<lab::SurfaceDisplays;++display){auto& binding=surface.displays[display];binding.module=lab::DisplayModule::EditOperations;binding.page=display;binding.followsSelectedPad=false;}
    surface.focusDisplay(0);status=L"Édition KICK — écran A : Copy/Delete Pad · écran B : Erase Notes par séquence, piste et plage.";
}

void selectEditOperationPad(int pad){
    pad=std::clamp(pad,0,lab::Pads-1);if(editPadRole==0)editSourcePad=pad;else editDestinationPad=pad;selectPad(pad);
    status=L"Édition pad — "+std::wstring(editPadRole?L"destination ":L"source ")+padName(pad)+L" sélectionnée.";
}

double editOperationRatio(const lab::Snapshot& snapshot,int display,int knob)const{
    if(display==0){if(knob==0)return editSourcePad/double(lab::Pads-1);if(knob==1)return editDestinationPad/double(lab::Pads-1);if(knob==2)return editPadRole?1.:0.;return 0.;}
    const int sequence=std::clamp(eraseSequence,0,lab::Sequences-1),total=std::max(1,snapshot.sequences[sequence].bars*lab::Steps);
    if(knob==0)return sequence/double(lab::Sequences-1);if(knob==1)return eraseTrack/double(lab::Pads-1);if(knob==2)return eraseStartStep/double(std::max(1,total-1));return eraseEndStep/double(std::max(1,total-1));
}

void setEditOperationRatio(int display,int knob,double ratio){
    ratio=std::clamp(ratio,0.,1.);const auto snapshot=engine.snapshot();
    if(display==0){if(knob==0)editSourcePad=std::clamp(int(std::lround(ratio*(lab::Pads-1))),0,lab::Pads-1);else if(knob==1)editDestinationPad=std::clamp(int(std::lround(ratio*(lab::Pads-1))),0,lab::Pads-1);else if(knob==2)editPadRole=ratio>=.5?1:0;selectPad(editPadRole?editDestinationPad:editSourcePad);return;}
    if(knob==0)eraseSequence=std::clamp(int(std::lround(ratio*(lab::Sequences-1))),0,lab::Sequences-1);else if(knob==1)eraseTrack=std::clamp(int(std::lround(ratio*(lab::Pads-1))),0,lab::Pads-1);
    const int total=std::max(1,snapshot.sequences[eraseSequence].bars*lab::Steps);eraseStartStep=std::clamp(eraseStartStep,0,total-1);eraseEndStep=std::clamp(eraseEndStep,eraseStartStep,total-1);
    if(knob==2){eraseStartStep=std::clamp(int(std::lround(ratio*(total-1))),0,total-1);eraseEndStep=std::max(eraseEndStep,eraseStartStep);}else if(knob==3)eraseEndStep=std::clamp(int(std::lround(ratio*(total-1))),eraseStartStep,total-1);
}

void copyEditPad(){
    if(editSourcePad==editDestinationPad){status=L"Copy Pad — la source et la destination sont identiques.";return;}const auto snapshot=engine.snapshot();
    if(!snapshot.paths[editDestinationPad].empty()){const auto question=L"Le pad "+padName(editDestinationPad)+L" contient déjà un son.\n\nLe remplacer par une copie complète du pad "+padName(editSourcePad)+L" ?";if(MessageBoxW(window,question.c_str(),L"Copy Pad KICK",MB_YESNO|MB_ICONWARNING)!=IDYES)return;}
    std::string error;if(!engine.copyProgramPad(editSourcePad,editDestinationPad,error)){MessageBoxW(window,wide(error).c_str(),L"Copy Pad KICK",MB_OK|MB_ICONINFORMATION);return;}markProjectDirty();selectPad(editDestinationPad);status=L"Copy Pad — "+padName(editSourcePad)+L" copié vers "+padName(editDestinationPad)+L" (samples, couches, Pad Edit et FX).";
}

void deleteEditPad(){
    const int pad=editPadRole?editDestinationPad:editSourcePad;const auto question=L"Supprimer le contenu et les réglages du pad "+padName(pad)+L" ?\n\nLes steps du pattern ne seront pas modifiés.";if(MessageBoxW(window,question.c_str(),L"Delete Pad KICK",MB_YESNO|MB_ICONWARNING)!=IDYES)return;
    if(engine.deleteProgramPad(pad)){markProjectDirty();status=L"Delete Pad — pad "+padName(pad)+L" vidé ; les patterns sont conservés.";}else status=L"Delete Pad — le pad "+padName(pad)+L" est déjà vide.";selectPad(pad);
}

void eraseEditNotes(){
    const auto snapshot=engine.snapshot();const auto& sequence=snapshot.sequences[eraseSequence];const bool melodic=sequence.mode!=lab::PerformanceMode::Drum;std::wstring scope=L"Séquence "+number(eraseSequence+1)+L" · "+(melodic?L"notes mélodiques":L"piste "+number(eraseTrack+1))+L"\nPlage "+listEditTimeText(eraseStartStep)+L" à "+listEditTimeText(eraseEndStep)+L"\n\nEffacer ces notes ?";
    if(MessageBoxW(window,scope.c_str(),L"Erase Notes KICK",MB_YESNO|MB_ICONWARNING)!=IDYES)return;const int erased=engine.eraseNotes(eraseSequence,eraseTrack,eraseStartStep,eraseEndStep);if(erased)markProjectDirty();status=L"Erase Notes — "+number(erased)+L" événement(s) effacé(s), sons et réglages de pads conservés.";
}

double surfaceColorRatio(const lab::Snapshot& snapshot,int display,int knob)const{
    const auto& info=snapshot.programs[snapshot.program];
    if(display==0){if(knob==0)return colorSelection/double(lab::PadColors-1);if(knob==1)return colorAllPads?1.:0.;if(knob==2)return int(info.emptyPadColor);return int(info.padOffColor);}
    if(knob==0)return int(info.padOnColor)/2.;if(knob==1)return selected/double(lab::Pads-1);if(knob==2)return snapshot.edits[selected].color/double(lab::PadColors-1);return 0.;
}

void setSurfaceColorRatio(int display,int knob,double ratio){
    ratio=std::clamp(ratio,0.,1.);auto snapshot=engine.snapshot();auto info=snapshot.programs[snapshot.program];
    if(display==0){
        if(knob==0){colorSelection=std::clamp(int(std::lround(ratio*(lab::PadColors-1))),0,lab::PadColors-1);return;}
        if(knob==1){colorAllPads=ratio>=.5;return;}
        if(knob==2)info.emptyPadColor=ratio>=.5?lab::EmptyPadColorMode::Dim:lab::EmptyPadColorMode::Off;
        else info.padOffColor=ratio>=.5?lab::PadOffColorMode::Dim:lab::PadOffColorMode::Off;
    }else{
        if(knob==0)info.padOnColor=lab::PadOnColorMode(std::clamp(int(std::lround(ratio*2)),0,2));
        else if(knob==1){selectPad(std::clamp(int(std::lround(ratio*(lab::Pads-1))),0,lab::Pads-1));return;}
        else if(knob==2){colorSelection=std::clamp(int(std::lround(ratio*(lab::PadColors-1))),0,lab::PadColors-1);applySurfaceColor(selected);return;}
        else return;
    }
    engine.setProgramColorModes(info.emptyPadColor,info.padOffColor,info.padOnColor);markProjectDirty();
}

double surfaceProgramRatio(const lab::PadEdit& edit,int page,int knob)const{
    if(page==0){if(knob==0)return edit.gain/2.;if(knob==1)return (edit.semitones+24.)/48.;if(knob==2)return edit.start;if(knob==3)return edit.end;}
    if(page==1){if(knob==0)return edit.attack;if(knob==1)return edit.decay;if(knob==2)return edit.sustain;if(knob==3)return edit.release;}
    if(page==2){if(knob==0)return edit.loopSync?1.:0.;if(knob==1)return (edit.sourceBpm-40.)/200.;}
    if(page==3){if(knob==0)return int(edit.samplePlay)/2.;if(knob==1)return int(edit.layerPlay)/2.;if(knob==2)return (edit.chokeGroup+1)/16.;if(knob==3)return edit.color/double(lab::PadColors-1);}
    return 0;
}

void setSurfaceProgramRatio(int display,int knob,double ratio){
    if(display<0||display>=lab::SurfaceDisplays||knob<0||knob>=lab::SurfaceQLinks)return;auto& binding=surface.displays[display];if(binding.module!=lab::DisplayModule::Sample)return;
    auto snapshot=engine.snapshot();const int pad=std::clamp(binding.pad,0,lab::Pads-1);auto edit=snapshot.edits[pad];ratio=std::clamp(ratio,0.,1.);
    if(binding.page==0){if(knob==0)edit.gain=ratio*2.;else if(knob==1)edit.semitones=std::round(ratio*48.)-24.;else if(knob==2)edit.start=std::min(ratio,edit.end-.001);else if(knob==3)edit.end=std::max(ratio,edit.start+.001);}
    else if(binding.page==1){if(knob==0)edit.attack=ratio;else if(knob==1)edit.decay=ratio;else if(knob==2)edit.sustain=ratio;else if(knob==3)edit.release=ratio;}
    else if(binding.page==2){if(knob==0)edit.loopSync=ratio>=.5;else if(knob==1)edit.sourceBpm=40.+std::round(ratio*200.);}
    else if(binding.page==3){if(knob==0)edit.samplePlay=lab::SamplePlayType(std::clamp(int(std::lround(ratio*2)),0,2));else if(knob==1)edit.layerPlay=lab::LayerPlayType(std::clamp(int(std::lround(ratio*2)),0,2));else if(knob==2)edit.chokeGroup=std::clamp(int(std::lround(ratio*16))-1,-1,15);else edit.color=std::clamp(int(std::lround(ratio*(lab::PadColors-1))),0,lab::PadColors-1);}
    else if(binding.page==4){const int layer=std::clamp(binding.slot,0,lab::PadLayers-1);const auto& item=snapshot.layers[pad][layer];if(knob==0)binding.slot=std::clamp(int(std::lround(ratio*(lab::PadLayers-1))),0,lab::PadLayers-1);else if(knob==1)engine.setLayerVelocityRange(pad,layer,1+int(std::lround(ratio*126)),item.velocityEnd);else if(knob==2)engine.setLayerVelocityRange(pad,layer,item.velocityStart,1+int(std::lround(ratio*126)));else engine.setLayerReverse(pad,layer,ratio>=.5);markProjectDirty();return;}
    engine.setEdit(pad,edit);
}

std::wstring surfaceProgramParameterName(int page,int knob)const{
    static const std::array<std::wstring,20> names={L"Volume",L"Pitch",L"Début",L"Fin",L"Attack",L"Decay",L"Sustain",L"Release",L"Loop Sync",L"BPM source",L"—",L"—",L"Lecture",L"Sélection couche",L"Choke groupe",L"Couleur pad",L"Couche",L"Vélocité min",L"Vélocité max",L"Reverse"};return names[std::clamp(page*4+knob,0,19)];
}

std::wstring surfaceProgramParameterValue(const lab::PadEdit& edit,int page,int knob,const lab::PadLayerInfo* layer=nullptr,int layerIndex=0)const{
    if(page==0){if(knob==0)return decimal(edit.gain*100,0)+L" %";if(knob==1)return (edit.semitones>=0?L"+":L"")+decimal(edit.semitones,0)+L" st";if(knob==2)return decimal(edit.start*100,1)+L" %";return decimal(edit.end*100,1)+L" %";}
    if(page==1){if(knob==0)return decimal(edit.attack*edit.attack*2000,0)+L" ms";if(knob==1)return decimal(edit.decay*edit.decay*2000,0)+L" ms";if(knob==2)return decimal(edit.sustain*100,0)+L" %";return decimal(edit.release*edit.release*4000,0)+L" ms";}
    if(page==2){if(knob==0)return edit.loopSync?L"ACTIF":L"ONE-SHOT";if(knob==1)return decimal(edit.sourceBpm,0)+L" BPM";return L"Non assigné";}
    if(page==3){if(knob==0)return std::array<std::wstring,3>{L"ONE SHOT",L"NOTE OFF",L"NOTE ON"}[int(edit.samplePlay)];if(knob==1)return std::array<std::wstring,3>{L"CYCLE",L"VÉLOCITÉ",L"ALÉATOIRE"}[int(edit.layerPlay)];if(knob==2)return edit.chokeGroup<0?L"AUCUN":number(edit.chokeGroup+1);return number(edit.color+1);}
    if(page==4&&layer){if(knob==0)return L"L"+number(layerIndex+1)+L" · "+wide(layer->name);if(knob==1)return number(layer->velocityStart);if(knob==2)return number(layer->velocityEnd);return layer->reverse?L"ACTIF":L"NORMAL";}
    return L"Non assigné";
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
    if(binding.module==lab::DisplayModule::Events){setSurfaceEventRatio(display,knob,ratio);return;}
    if(binding.module==lab::DisplayModule::ListEdit){setListEditRatio(display,knob,ratio);return;}
    if(binding.module==lab::DisplayModule::ColorEditor){setSurfaceColorRatio(display,knob,ratio);return;}
    if(binding.module==lab::DisplayModule::EditOperations){setEditOperationRatio(display,knob,ratio);return;}
    if(binding.module==lab::DisplayModule::Tempo){if(knob==0)engine.setTempo(40.+ratio*200.);else if(knob==1)engine.setTempo(std::round((40.+ratio*200.)*10.)/10.);return;}
    if(binding.module==lab::DisplayModule::Mixer){const int track=mixerTrackFor(display,knob);if(mixerParameter==0)engine.setTrackLevel(track,ratio);else engine.setTrackPan(track,ratio*2.-1.);return;}
    if(binding.module==lab::DisplayModule::PadMixer){const int pad=padMixerPadFor(display,knob);if(padMixerParameter==0)engine.setPadLevel(pad,ratio*2.);else engine.setPadPan(pad,ratio*2.-1.);markProjectDirty();return;}
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
    if(binding.module==lab::DisplayModule::Events){auto snapshot=engine.snapshot();const auto event=surfaceStepEvent(snapshot,binding);const double step=knob==0?1./126.:knob==1?.01:knob==2?1./7.:1./15.;setSurfaceEventRatio(display,knob,surfaceEventRatio(event,knob)+delta*step);return;}
    if(binding.module==lab::DisplayModule::ListEdit){auto snapshot=engine.snapshot();ListEditItem item;if(!currentListEditItem(snapshot,item))return;double step=0.;if(display==0){if(knob==0)step=1./std::max(1,snapshot.sequences[snapshot.sequence].bars*lab::Steps-1);else if(knob==1)step=1./(item.melodic?127.:double(lab::Pads-1));else if(knob==2)step=1./std::max(1,int(listEditItems(snapshot).size())-1);}else step=knob==0?1./15.:knob==1?1./126.:knob==2?.01:1./7.;if(step>0)setListEditRatio(display,knob,listEditRatio(snapshot,display,knob)+delta*step);return;}
    if(binding.module==lab::DisplayModule::ColorEditor){auto snapshot=engine.snapshot();double step=1.;if(display==0&&knob==0)step=1./(lab::PadColors-1);else if(display==1&&knob==0)step=.5;else if(display==1&&knob==1)step=1./(lab::Pads-1);else if(display==1&&knob==2)step=1./(lab::PadColors-1);setSurfaceColorRatio(display,knob,surfaceColorRatio(snapshot,display,knob)+delta*step);return;}
    if(binding.module==lab::DisplayModule::EditOperations){const auto snapshot=engine.snapshot();double step=1.;if(display==0&&(knob==0||knob==1))step=1./(lab::Pads-1);else if(display==1&&knob==0)step=1./(lab::Sequences-1);else if(display==1&&knob==1)step=1./(lab::Pads-1);else if(display==1){const int total=std::max(1,snapshot.sequences[eraseSequence].bars*lab::Steps);step=1./std::max(1,total-1);}setEditOperationRatio(display,knob,editOperationRatio(snapshot,display,knob)+delta*step);return;}
    if(binding.module==lab::DisplayModule::Mixer){auto snapshot=engine.snapshot();const int track=mixerTrackFor(display,knob);const auto& data=snapshot.sequences[snapshot.sequence].tracks[track];if(mixerParameter==0)engine.setTrackLevel(track,data.level+delta*.01);else engine.setTrackPan(track,data.pan+delta*.02);return;}
    if(binding.module==lab::DisplayModule::PadMixer){auto snapshot=engine.snapshot();const int pad=padMixerPadFor(display,knob);const auto& edit=snapshot.edits[pad];if(padMixerParameter==0)engine.setPadLevel(pad,edit.gain+delta*.02);else engine.setPadPan(pad,edit.pan+delta*.02);markProjectDirty();return;}
    if(binding.module==lab::DisplayModule::Sample){auto snapshot=engine.snapshot();const int pad=std::clamp(binding.pad,0,lab::Pads-1);const auto& edit=snapshot.edits[pad];double ratio=surfaceProgramRatio(edit,binding.page,knob),step=.01;if(binding.page==0&&knob==1)step=1./48.;else if(binding.page==2&&knob==1)step=1./200.;else if((binding.page==2&&knob==0)||binding.page==3)step=knob<2?.5:knob==2?1./16.:1./17.;else if(binding.page==4){const auto& item=snapshot.layers[pad][std::clamp(binding.slot,0,lab::PadLayers-1)];ratio=knob==0?binding.slot/double(lab::PadLayers-1):knob==1?(item.velocityStart-1)/126.:knob==2?(item.velocityEnd-1)/126.:item.reverse?1.:0.;step=knob==0?1./3.:knob==3?1.:1./126.;}setSurfaceProgramRatio(display,knob,ratio+delta*step);return;}
    if(binding.module==lab::DisplayModule::Tempo){auto snapshot=engine.snapshot();engine.setTempo(snapshot.tempo+delta*(knob==1?.1:1.));return;}
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
    if(module==lab::DisplayModule::Programs){const int first=surface.focusedDisplay*8;int local=std::clamp(programSelected-first,0,7);int row=local/2,col=local%2;col=std::clamp(col+dx,0,1);row=std::clamp(row+dy,0,3);programSelected=first+row*2+col;return;}
    if(module==lab::DisplayModule::Browser){if(dx)moveSurfaceBrowserCategory(surface.focusedDisplay,dx);if(dy)moveSurfaceBrowserSelection(surface.focusedDisplay,dy);return;}
    if(module==lab::DisplayModule::TrackMute){const int first=surface.focusedDisplay*8;int local=std::clamp(stepTrack-first,0,7);int row=local/2,col=local%2;col=std::clamp(col+dx,0,1);row=std::clamp(row+dy,0,3);selectTrack(first+row*2+col);return;}
    if(module==lab::DisplayModule::Mixer){if(dx){mixerBank=std::clamp(mixerBank+(dx>0?1:-1),0,1);showSurfaceMixer();}if(dy){mixerParameter=std::clamp(mixerParameter+(dy>0?1:-1),0,1);}return;}
    if(module==lab::DisplayModule::PadMixer){if(dx){padMixerBank=std::clamp(padMixerBank+(dx>0?1:-1),0,1);showSurfacePadMixer();}if(dy){padMixerParameter=std::clamp(padMixerParameter+(dy>0?1:-1),0,1);}return;}
    if(module==lab::DisplayModule::PadMute){selectPad(std::clamp(selected+dx+dy*4,0,lab::Pads-1));surface.focusDisplay(selected/8);return;}
    if(module==lab::DisplayModule::NextSequence){if(dx)turnSurfaceQLink(surface.focusedDisplay,0,dx);if(dy)turnSurfaceQLink(surface.focusedDisplay,2,dy);return;}
    if(module==lab::DisplayModule::PadPerform){if(dx)turnSurfaceQLink(surface.focusedDisplay,0,dx);if(dy)turnSurfaceQLink(surface.focusedDisplay,2,-dy);return;}
    if(module==lab::DisplayModule::Song){if(dx)turnSurfaceQLink(surface.focusedDisplay,1,dx);if(dy)turnSurfaceQLink(surface.focusedDisplay,2,dy);return;}
    if(module==lab::DisplayModule::Events){auto& binding=surface.displays[surface.focusedDisplay];if(dx){eventStep=std::clamp(eventStep+(dx>0?1:-1),0,lab::Steps-1);binding.slot=eventStep;}if(dy){selectTrack(stepTrack+(dy>0?1:-1));binding.pad=stepTrack;}return;}
    if(module==lab::DisplayModule::ListEdit){if(dx||dy)moveListEditSelection(dx?dx:dy);return;}
    if(module==lab::DisplayModule::ColorEditor){if(dx)turnSurfaceQLink(0,0,dx);if(dy)turnSurfaceQLink(1,1,dy);return;}
    if(module==lab::DisplayModule::EditOperations){if(surface.focusedDisplay==0){if(dx)turnSurfaceQLink(0,editPadRole,dx);if(dy)setEditOperationRatio(0,2,dy>0?1.:0.);}else {if(dx)turnSurfaceQLink(1,2,dx);if(dy)turnSurfaceQLink(1,1,dy);}return;}
    if(module==lab::DisplayModule::MidiControl&&dx){auto& binding=surface.displays[surface.focusedDisplay];binding.page=std::clamp(binding.page+(dx>0?1:-1),0,midiControlPages()-1);return;}
    if(module==lab::DisplayModule::Sample&&dx){auto& binding=surface.displays[surface.focusedDisplay];binding.page=std::clamp(binding.page+(dx>0?1:-1),0,4);return;}
    if(module==lab::DisplayModule::Tempo){if(dx)turnSurfaceQLink(surface.focusedDisplay,0,dx);if(dy)turnSurfaceQLink(surface.focusedDisplay,1,-dy);return;}
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
    if(module==lab::DisplayModule::Programs){const int first=surface.focusedDisplay*8;programSelected=std::clamp(programSelected+(delta>0?1:-1),first,first+7);return;}
    if(module==lab::DisplayModule::Browser){moveSurfaceBrowserSelection(surface.focusedDisplay,delta);return;}
    if(module==lab::DisplayModule::TrackMute){const int first=surface.focusedDisplay*8;selectTrack(std::clamp(stepTrack+(delta>0?1:-1),first,first+7));return;}
    if(module==lab::DisplayModule::Mixer){mixerBank=std::clamp(mixerBank+(delta>0?1:-1),0,1);showSurfaceMixer();return;}
    if(module==lab::DisplayModule::PadMixer){padMixerBank=std::clamp(padMixerBank+(delta>0?1:-1),0,1);showSurfacePadMixer();return;}
    if(module==lab::DisplayModule::PadMute){selectPad(std::clamp(selected+(delta>0?1:-1),0,lab::Pads-1));surface.focusDisplay(selected/8);return;}
    if(module==lab::DisplayModule::NextSequence){turnSurfaceQLink(surface.focusedDisplay,0,delta);return;}
    if(module==lab::DisplayModule::PadPerform){turnSurfaceQLink(surface.focusedDisplay,2,delta);return;}
    if(module==lab::DisplayModule::Song){turnSurfaceQLink(surface.focusedDisplay,2,delta);return;}
    if(module==lab::DisplayModule::Events){auto& binding=surface.displays[surface.focusedDisplay];eventStep=std::clamp(eventStep+(delta>0?1:-1),0,lab::Steps-1);binding.slot=eventStep;return;}
    if(module==lab::DisplayModule::ListEdit){moveListEditSelection(delta);return;}
    if(module==lab::DisplayModule::ColorEditor){turnSurfaceQLink(0,0,delta);return;}
    if(module==lab::DisplayModule::EditOperations){turnSurfaceQLink(surface.focusedDisplay,surface.focusedDisplay?2:editPadRole,delta);return;}
    if(module==lab::DisplayModule::Tempo){turnSurfaceQLink(surface.focusedDisplay,0,delta);return;}
    if(module==lab::DisplayModule::Inserts){const auto binding=surface.displays[surface.focusedDisplay];const int slot=(binding.slot+(delta>0?1:-1)+lab::InsertSlots)%lab::InsertSlots;selectedInsert=slot;arrangeSurfaceInsertPages(binding.pad,slot,surface.focusedDisplay);status=L"FX — rotation : slot "+number(slot+1)+L" sélectionné.";return;}
    if(module!=lab::DisplayModule::Menu)return;
    int item=surface.focusedDisplay*8+surface.menuSelections[surface.focusedDisplay];
    selectSurfaceMenu(std::clamp(item+(delta>0?1:-1),0,15));
}

void activateSurfaceMenu(){
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::Programs){assignSelectedProgram();return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::Browser){loadSurfaceBrowserSelection(surface.focusedDisplay);return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::TrackMute){toggleTrackMute(stepTrack);return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::Mixer){selectTrack(mixerTrackFor(surface.focusedDisplay,0));return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::PadMixer){selectPad(padMixerPadFor(surface.focusedDisplay,0));return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::PadMute){toggleProgramPadMute(selected);return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::Song){engine.addSongStep(songChoice,songRepeats);markProjectDirty();auto snapshot=engine.snapshot();songSelected=std::max(0,snapshot.songLength-1);return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::Events){dispatch({lab::SurfaceAction::StepToggle,stepTrack,eventStep});return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::ListEdit){ListEditItem item;if(currentListEditItem(engine.snapshot(),item))status=L"List Edit — "+listEditTimeText(item.step)+L" · "+(item.melodic?L"Note "+noteName(item.note):L"Pad "+padName(item.track));return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::ColorEditor){applySurfaceColor(selected);return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::EditOperations){if(surface.focusedDisplay==0)copyEditPad();else eraseEditNotes();return;}
    if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::Inserts){auto& binding=surface.displays[surface.focusedDisplay];selectedInsert=binding.slot;insertMenu();binding.page=0;return;}
    if(surface.displays[surface.focusedDisplay].module!=lab::DisplayModule::Menu)return;
    const int item=surface.focusedDisplay*8+surface.menuSelections[surface.focusedDisplay];
    if(item==0){showSurfaceBrowser(surface.focusedDisplay);return;}
    if(item==1){showSurfaceMixer(0);return;}
    if(item==2){showSurfaceTrackMute();return;}
    if(item==3){showSurfaceNextSequence(surface.focusedDisplay);return;}
    if(item==4){showSurfaceListEdit();return;}
    if(item==5){showSurfacePadMixer(0);return;}
    if(item==6){showSurfacePadMute();return;}
    if(item==7){showSurfaceSong(surface.focusedDisplay);return;}
    if(item==8){showSurfaceColorEditor();return;}
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
        else if(surface.displays[display].module==lab::DisplayModule::ListEdit)moveListEditSelection(-delta);
        else if(surface.displays[display].module==lab::DisplayModule::ColorEditor)turnSurfaceQLink(0,0,-delta);
        else if(surface.displays[display].module==lab::DisplayModule::EditOperations)turnSurfaceQLink(display,display?2:editPadRole,-delta);
        else if(surface.displays[display].module==lab::DisplayModule::Programs)turnSurfaceMenu(delta);
        else if(surface.displays[display].module==lab::DisplayModule::PadMixer){padMixerBank=std::clamp(padMixerBank+(delta<0?1:-1),0,1);showSurfacePadMixer();}
        else if(surface.displays[display].module==lab::DisplayModule::PadMute){selectPad(std::clamp(selected+(delta<0?1:-1),0,lab::Pads-1));surface.focusDisplay(selected/8);}
        return true;
    }
    return false;
}

bool surfaceClick(int x,int y,bool right){
    for(int display=0;display<2;++display){
        for(int knob=0;knob<4;++knob)if(surfaceKnobBox(display,knob).contains(x,y)){
            surface.focusDisplay(display);
            const auto module=surface.displays[display].module;
            if(!right&&(module==lab::DisplayModule::Inserts||module==lab::DisplayModule::Sample||module==lab::DisplayModule::PadPerform||module==lab::DisplayModule::NextSequence||module==lab::DisplayModule::Song||module==lab::DisplayModule::Mixer||module==lab::DisplayModule::PadMixer||module==lab::DisplayModule::Tempo||module==lab::DisplayModule::EditOperations)){
                surfaceDragDisplay=display;surfaceDragKnob=knob;surfaceDragStartY=y;surfaceDragLastStep=0;SetCapture(window);
            }
            return true;
        }
        if(!surfaceDisplayBox(display).contains(x,y))continue;
        surface.focusDisplay(display);
        auto& binding=surface.displays[display];
        if(binding.module==lab::DisplayModule::Programs){for(int local=0;local<8;++local)if(surfaceDisplayItemBox(display,local).contains(x,y)){programSelected=display*8+local;if(right)assignSelectedProgram();return true;}return true;}
        if(binding.module==lab::DisplayModule::TrackMute){
            for(int local=0;local<8;++local)if(surfaceTrackMuteBox(display,local).contains(x,y)){const int track=display*8+local;selectTrack(track);if(right||surface.shift)toggleTrackSolo(track);else toggleTrackMute(track);return true;}
            return true;
        }
        if(binding.module==lab::DisplayModule::Mixer){
            for(int row=0;row<4;++row)if(surfaceMixerTrackBox(display,row).contains(x,y)){const int track=mixerTrackFor(display,row);selectTrack(track);if(right)toggleTrackMute(track);return true;}
            return true;
        }
        if(binding.module==lab::DisplayModule::PadMixer){
            for(int row=0;row<4;++row)if(surfaceMixerTrackBox(display,row).contains(x,y)){const int pad=padMixerPadFor(display,row);selectPad(pad);if(right)toggleProgramPadMute(pad);return true;}
            return true;
        }
        if(binding.module==lab::DisplayModule::PadMute){
            for(int local=0;local<8;++local)if(surfaceTrackMuteBox(display,local).contains(x,y)){const int pad=display*8+local;selectPad(pad);toggleProgramPadMute(pad);return true;}
            return true;
        }
        if(binding.module==lab::DisplayModule::ListEdit){
            if(display==0){const auto items=listEditItems(engine.snapshot());const int first=std::clamp(listEditRow-2,0,std::max(0,int(items.size())-5));for(int row=0;row<5;++row)if(surfaceBrowserFileBox(display,row).contains(x,y)&&first+row<int(items.size())){listEditRow=first+row;return true;}}
            return true;
        }
        if(binding.module==lab::DisplayModule::ColorEditor){
            if(display==0)for(int color=0;color<lab::PadColors;++color)if(surfaceColorBox(color).contains(x,y)){colorSelection=color;return true;}
            if(display==1)for(int pad=0;pad<lab::Pads;++pad)if(surfaceColorPadBox(pad).contains(x,y)){applySurfaceColor(pad);return true;}
            return true;
        }
        if(binding.module==lab::DisplayModule::EditOperations){
            if(display==0)for(int pad=0;pad<lab::Pads;++pad)if(surfaceEditPadBox(pad).contains(x,y)){selectEditOperationPad(pad);return true;}
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
            if(surfacePageNextBox(display).contains(x,y)){binding.page=std::min(4,binding.page+1);return true;}
            if(surfaceProgramAuditionBox(display).contains(x,y)){if(right&&binding.page==4){const auto source=choose(false,false,library);if(!source.empty()){std::string error;if(engine.loadLayer(binding.pad,std::clamp(binding.slot,0,lab::PadLayers-1),utf8(source),error)){markProjectDirty();status=L"Couche "+number(binding.slot+1)+L" chargée : "+std::filesystem::path(source).filename().wstring();}else status=L"Couche non chargée : "+wide(error);}}else engine.trigger(binding.pad);return true;}
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
        case 0: if(surface.shift)showSurfaceMidiControl(surface.focusedDisplay);else showSurfaceMixer(0);break;
        case 1: showSurfacePrograms(surface.focusedDisplay);break;
        case 2: showSurfaceSong(surface.focusedDisplay);break;
        case 3: if(right||surface.shift)showSurfacePadMixer(0);else showSurfaceMixer(0);break;
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
    for(int column=0;column<2;++column)if(utilityButtonBox(column).contains(x,y)){
        if(column==0)showSurfaceHome();
        else status=L"Bouton réservé — aucune fonction assignée pour le moment.";
        return true;
    }
    for(int group=0;group<8;++group)if(groupBox(group).contains(x,y)){
        selectGroupBlock(group);return true;
    }
    for(int mode=0;mode<4;++mode)if(modeBox(mode).contains(x,y)){
        if(mode==2){padMode=2;auto snapshot=engine.snapshot();if(snapshot.sequences[snapshot.sequence].mode==lab::PerformanceMode::Drum)engine.setPerformanceMode(lab::PerformanceMode::Major);engine.setRepeatDivision(recordQuantize);engine.setNoteRepeat(true,true);status=L"Chords — chaque pad lance une triade arpégée ; FOLLOW choisit 1/4, 1/8 ou 1/16.";return true;}
        if(mode==3){enterStepMode();return true;}padMode=mode;
        if(mode==0){engine.setTrackProgram(stepTrack,lab::ProgramType::Drum);engine.setPerformanceMode(lab::PerformanceMode::Drum);}
        else if(mode==1){selectTrack(selected);engine.setTrackProgram(stepTrack,lab::ProgramType::Keygroup);engine.setMelodicSourcePad(selected);engine.setPerformanceMode(lab::PerformanceMode::Chromatic);}
        status=mode==0?L"Pad Mode — les 16 pads jouent les samples du groupe.":L"Keyboard — joue une hauteur puis passe sur Step pour la séquencer.";
        return true;
    }
    for(int command=0;command<9;++command)if(sideCommandBox(command).contains(x,y)){
        if(command==2){showSurfaceNextSequence(surface.focusedDisplay);status=L"Pattern / Clip — sélection des séquences KICK.";}
        else if(command==3){if(right||surface.shift)showSurfaceListEdit();else showSurfaceEvents(surface.focusedDisplay);}
        else if(command==5){if(right||surface.shift)showSurfaceEditOperations();else {duplicateSourceSequence=engine.snapshot().sequence;status=L"Duplicate armé — clique maintenant sur A–H pour choisir le bloc de destination.";}}
        else if(command==6){selectLatch=!selectLatch;if(selectLatch)status=L"Select actif — choisis maintenant la piste avec un pad.";else status=L"Select relâché — piste "+number(stepTrack+1)+L".";}
        else if(command==7){soloLatch=!soloLatch;muteLatch=selectLatch=false;status=soloLatch?L"Solo actif — frappe un pad pour isoler sa piste.":L"Mode Solo relâché.";}
        else if(command==8){if(right||surface.shift)showSurfacePadMute();else {muteLatch=!muteLatch;soloLatch=selectLatch=false;status=muteLatch?L"Mute actif — frappe un pad pour couper sa piste.":L"Mode Mute relâché.";}}
        else status=std::array<std::wstring,9>{L"Fixed Velocity / 16 Vel",L"Scene / Section",L"Pattern / Clip",L"Events",L"Variation",L"Duplicate / Double",L"Select",L"Solo",L"Mute / Choke"}[command]+L" — commande visible, portage KICK à venir.";
        return true;
    }
    for(int column=0;column<2;++column)for(int row=0;row<3;++row)if(smallControlBox(column,row).contains(x,y)){
        if(column==0&&row==0){auto snapshot=engine.snapshot();engine.setVolume(std::clamp(snapshot.volume+(right?-.05:.05),0.,1.));markProjectDirty();status=L"Master Volume "+number(int(engine.snapshot().volume*100))+L" %.";}
        else if(column==1&&row==0)toggleNoteRepeat();
        else if(column==0&&row==2)showSurfaceTempo(surface.focusedDisplay);
        else if(column==1&&row==2)dispatch({lab::SurfaceAction::TempoDelta,0,right?-1:1});
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
        else if(index==1)showSurfaceEditOperations();
        else if(index==3)cycleRecordQuantize();
        else if(index==5)toggleRecord();
        else if(index==6)dispatch({lab::SurfaceAction::Stop});
        else if(index==7){surface.shift=!surface.shift;status=surface.shift?L"SHIFT actif.":L"SHIFT relâché.";}
        else status=L"Commande transport visible ; fonction KICK pas encore portée.";
        return true;
    }
    for(int p=0;p<16;++p)if(padBox(p).contains(x,y)){
        if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::EditOperations){selectEditOperationPad(p);return true;}
        if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::ColorEditor){applySurfaceColor(p);return true;}
        if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::ListEdit){selectListEditTrack(p);return true;}
        if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::PadMute){selectPad(p);toggleProgramPadMute(p);return true;}
        if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::PadMixer){selectPad(p);if(right)toggleProgramPadMute(p);return true;}
        if(surface.displays[surface.focusedDisplay].module==lab::DisplayModule::TrackMute){selectTrack(p);if(surface.shift)toggleTrackSolo(p);else toggleTrackMute(p);return true;}
        if(soloLatch){selectTrack(p);toggleTrackSolo(p);status=L"Solo piste "+number(p+1)+L" basculé.";return true;}
        if(muteLatch){selectTrack(p);toggleTrackMute(p);status=L"Mute piste "+number(p+1)+L" basculé.";return true;}
        if(padMode==3&&(selectLatch||surface.shift)){stepTrack=p;selectPad(p);if(selectLatch)selectLatch=false;status=L"Step — piste "+number(stepTrack+1)+L" sélectionnée.";return true;}
        if(padMode==3){dispatch({lab::SurfaceAction::StepToggle,stepTrack,p});return true;}
        if(right&&padMode==0){selectPad(p);sampleMenu();return true;}
        dispatch({lab::SurfaceAction::PadPress,p});return true;
    }
    for(int macro=0;macro<8;++macro)if(brandMacroBox(macro).contains(x,y)){status=L"Macro "+number(macro+1)+L" réservée — assignation à venir.";return true;}
    for(int buttonIndex=0;buttonIndex<4;++buttonIndex)if(brandPushBox(buttonIndex).contains(x,y)){status=L"Poussoir "+number(buttonIndex+1)+L" réservé — assignation à venir.";return true;}
    if(brandMasterBox().contains(x,y)){
        const auto box=brandMasterBox();auto snapshot=engine.snapshot();engine.setVolume(std::clamp((x-(box.x+12))/double(box.w-24),0.,1.));markProjectDirty();
        return true;
    }
    return false;
}

void paintSurfaceKnob(HDC dc,int display,int knob,const lab::Snapshot& snapshot){
    const auto knobBox=surfaceKnobBox(display,knob);const int cx=knobBox.x+knobBox.w/2,cy=knobBox.y+22;
    double ratio=.5;bool assigned=false;
    const auto& binding=surface.displays[display];
    if(binding.module==lab::DisplayModule::Mixer){const auto& track=snapshot.sequences[snapshot.sequence].tracks[mixerTrackFor(display,knob)];ratio=mixerParameter==0?track.level:(track.pan+1.)*.5;assigned=true;}
    else if(binding.module==lab::DisplayModule::PadMixer){const auto& edit=snapshot.edits[padMixerPadFor(display,knob)];ratio=padMixerParameter==0?edit.gain*.5:(edit.pan+1.)*.5;assigned=true;}
    else if(binding.module==lab::DisplayModule::Sample){const int pad=std::clamp(binding.pad,0,lab::Pads-1);if(binding.page==4){const auto& item=snapshot.layers[pad][std::clamp(binding.slot,0,lab::PadLayers-1)];ratio=knob==0?binding.slot/double(lab::PadLayers-1):knob==1?(item.velocityStart-1)/126.:knob==2?(item.velocityEnd-1)/126.:item.reverse?1.:0.;}else ratio=surfaceProgramRatio(snapshot.edits[pad],binding.page,knob);assigned=binding.page!=2||knob<2;}
    else if(binding.module==lab::DisplayModule::Events){ratio=surfaceEventRatio(surfaceStepEvent(snapshot,binding),knob);assigned=true;}
    else if(binding.module==lab::DisplayModule::ListEdit){ratio=listEditRatio(snapshot,display,knob);assigned=display==1||knob<3;}
    else if(binding.module==lab::DisplayModule::ColorEditor){ratio=surfaceColorRatio(snapshot,display,knob);assigned=display==0||knob<3;}
    else if(binding.module==lab::DisplayModule::EditOperations){ratio=editOperationRatio(snapshot,display,knob);assigned=display==1||knob<3;}
    else if(binding.module==lab::DisplayModule::Tempo){ratio=(snapshot.tempo-40.)/200.;assigned=knob<2;}
    else if(binding.module==lab::DisplayModule::PadPerform){const auto& sequence=snapshot.sequences[snapshot.sequence];if(knob==0)ratio=int(sequence.mode)/3.;else if(knob==1)ratio=sequence.sourcePad/15.;else if(knob==2)ratio=(sequence.rootNote-24)/60.;else ratio=sequence.bars>1?double(snapshot.viewBar)/(sequence.bars-1):0;assigned=true;}
    else if(binding.module==lab::DisplayModule::NextSequence){const auto& sequence=snapshot.sequences[snapshot.sequence];if(knob==0)ratio=snapshot.sequence/15.;else if(knob==1)ratio=(sequence.bars-1)/3.;else if(knob==2)ratio=sequence.bars>1?double(snapshot.viewBar)/(sequence.bars-1):0;else ratio=int(sequence.mode)/3.;assigned=true;}
    else if(binding.module==lab::DisplayModule::Song){if(knob==0)ratio=songChoice/15.;else if(knob==1)ratio=(songRepeats-1)/127.;else if(knob==2)ratio=snapshot.songLength>1?double(songSelected)/(snapshot.songLength-1):0;else ratio=0;assigned=knob<3;}
    else if(binding.module==lab::DisplayModule::Inserts){
        const auto& insert=snapshot.edits[binding.pad].inserts[binding.slot];
        const int parameter=binding.parameterForKnob(knob);
        if(parameter>=0&&parameter<lab::insertParameterCount(insert.effect)){ratio=insert.parameters[parameter];assigned=true;}
    }
    HBRUSH shadow=CreateSolidBrush(RGB(9,13,18));HPEN shadowEdge=CreatePen(PS_SOLID,1,RGB(8,11,15));auto oldB=SelectObject(dc,shadow),oldP=SelectObject(dc,shadowEdge);Ellipse(dc,cx-17,cy-15,cx+19,cy+21);SelectObject(dc,oldB);SelectObject(dc,oldP);DeleteObject(shadow);DeleteObject(shadowEdge);
    HBRUSH brush=CreateSolidBrush(themeButtons);HPEN edge=CreatePen(PS_SOLID,2,assigned?RGB(54,76,91):line);
    oldB=SelectObject(dc,brush);oldP=SelectObject(dc,edge);Ellipse(dc,cx-17,cy-19,cx+17,cy+15);SelectObject(dc,oldB);SelectObject(dc,oldP);DeleteObject(brush);DeleteObject(edge);
    constexpr double pi=3.14159265358979323846;const double angle=(135.+270.*ratio)*pi/180.;
    HPEN mark=CreatePen(PS_SOLID,3,assigned?accent:muted);oldP=SelectObject(dc,mark);MoveToEx(dc,cx,cy-2,nullptr);LineTo(dc,cx+int(std::cos(angle)*10),cy-2+int(std::sin(angle)*10));SelectObject(dc,oldP);DeleteObject(mark);
    text(dc,{knobBox.x,knobBox.y+41,knobBox.w,13},L"P"+number(display*4+knob+1),8,assigned?accent:muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

void paintSurfaceDisplay(HDC dc,int display,const lab::Snapshot& snapshot){
    auto module=surfaceModuleBox(display),box=surfaceDisplayBox(display);bool focused=surface.focusedDisplay==display;
    round(dc,module,panel,focused?accent:line);
    round(dc,box,RGB(9,18,22),focused?accent:RGB(34,48,57));
    const auto& binding=surface.displays[display];
    if(binding.module==lab::DisplayModule::Programs){
        text(dc,{box.x+14,box.y+9,310,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  PROGRAM MANAGER",12,focused?accent:muted,FW_SEMIBOLD);
        text(dc,{box.x+316,box.y+9,124,24},L"PATTERN "+number(snapshot.sequence+1),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        const int assigned=snapshot.sequences[snapshot.sequence].program;
        for(int local=0;local<8;++local){const int index=display*8+local;const auto& info=snapshot.programs[index];const auto b=surfaceDisplayItemBox(display,local);const bool selectedProgram=index==programSelected,linked=index==assigned;
            if(selectedProgram)round(dc,b,accent,accent);else fill(dc,b,local%2?RGB(18,31,35):RGB(17,28,33));
            text(dc,{b.x+9,b.y+3,b.w-18,18},number(index+1)+L"  "+(info.used?wide(info.name):L"— SLOT VIDE —"),9,selectedProgram?bg:(info.used?ink:muted),selectedProgram?FW_SEMIBOLD:FW_NORMAL);
            text(dc,{b.x+9,b.y+23,b.w-18,16},linked?L"ASSIGNÉ AU PATTERN":(info.used?(info.type==lab::ProgramType::Drum?L"DRUM PROGRAM":L"KEYGROUP"):L"NOUVEAU DISPONIBLE"),8,selectedProgram?bg:(linked?accent:muted),linked?FW_SEMIBOLD:FW_NORMAL);
        }
        text(dc,{box.x+14,box.y+270,box.w-28,16},L"ENCODEUR : CHOISIR · ENTER/CLIC DROIT : ASSIGNER",8,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
    }
    if(binding.module==lab::DisplayModule::TrackMute){
        text(dc,{box.x+14,box.y+9,300,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  TRACK MUTES",12,focused?accent:muted,FW_SEMIBOLD);
        text(dc,{box.x+316,box.y+9,124,24},L"PISTES "+number(display*8+1)+L"–"+number(display*8+8),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        for(int local=0;local<8;++local){const int track=display*8+local;const auto& data=snapshot.sequences[snapshot.sequence].tracks[track];const auto b=surfaceTrackMuteBox(display,local);const bool selectedTrack=track==stepTrack;round(dc,b,data.mute?RGB(81,31,39):(selectedTrack?RGB(31,62,59):RGB(18,31,35)),data.solo?RGB(241,184,88):(selectedTrack?accent:RGB(34,48,57)));text(dc,{b.x+9,b.y+4,b.w-18,18},number(track+1)+L"  "+wide(snapshot.names[track]),10,data.mute?RGB(255,154,135):ink,FW_SEMIBOLD);const std::wstring state=(data.mute?std::wstring(L"MUTE  "):std::wstring(L"ACTIF  "))+(data.solo?L"SOLO":L"");text(dc,{b.x+9,b.y+23,b.w-18,17},state,8,data.solo?RGB(241,184,88):muted,FW_SEMIBOLD);}
        text(dc,{box.x+14,box.y+272,box.w-28,14},L"CLIC / ENTER : MUTE  ·  SHIFT + PAD : SOLO",8,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
    }
    if(binding.module==lab::DisplayModule::Mixer){
        text(dc,{box.x+14,box.y+9,290,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  TRACK MIXER",12,focused?accent:muted,FW_SEMIBOLD);
        text(dc,{box.x+310,box.y+9,130,24},L"PISTES "+number(mixerBank*8+1)+L"–"+number(mixerBank*8+8),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        for(int row=0;row<4;++row){
            const int trackIndex=mixerTrackFor(display,row);const auto& track=snapshot.sequences[snapshot.sequence].tracks[trackIndex];auto b=surfaceMixerTrackBox(display,row);const bool selectedTrack=trackIndex==stepTrack;fill(dc,b,selectedTrack?RGB(31,62,59):(row%2?RGB(18,31,35):RGB(17,28,33)));
            const std::wstring type=track.program==lab::ProgramType::Drum?L"D":L"K";text(dc,{b.x+10,b.y+2,190,20},number(trackIndex+1)+L" ["+type+L"] "+wide(snapshot.names[trackIndex]),9,selectedTrack?accent:ink,FW_SEMIBOLD,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
            std::wstring flags=(track.armed?std::wstring(L"REC "):std::wstring())+(track.solo?L"SOLO ":L"")+(track.mute?L"MUTE":L"");text(dc,{b.x+204,b.y+2,74,20},flags,8,track.mute?RGB(255,133,103):(track.solo?RGB(241,184,88):muted),FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
            const double levelDb=track.level>0?20.*std::log10(track.level):-60.;const std::wstring value=mixerParameter==0?(track.level>0?decimal(levelDb,1)+L" dB":L"−∞ dB"):(std::abs(track.pan)<.01?L"C":track.pan<0?L"L "+number(int(std::round(-track.pan*100))):L"R "+number(int(std::round(track.pan*100))));text(dc,{b.x+282,b.y+2,104,20},value,9,accent,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
            const Box fader{b.x+10,b.y+29,226,8};round(dc,fader,RGB(11,18,24),line);const int marker=fader.x+2+int((fader.w-4)*std::clamp(track.level,0.,1.));fill(dc,{marker-1,fader.y-2,3,fader.h+4},accent);
            const Box meter{b.x+248,b.y+29,138,8};round(dc,meter,RGB(11,18,24),line);const double peak=snapshot.trackPeaks[trackIndex],peakDb=peak>0?20.*std::log10(peak):-60.;const int lit=int((meter.w-4)*std::clamp((peakDb+60.)/66.,0.,1.));if(lit>0)fill(dc,{meter.x+2,meter.y+2,lit,meter.h-4},peak>=1?RGB(255,95,90):(peakDb> -6?RGB(241,184,88):accent));
        }
        text(dc,{box.x+14,box.y+268,box.w-28,16},mixerParameter==0?L"KNOBS : LEVEL · bas : PAN · droite : page suivante":L"KNOBS : PAN · haut : LEVEL · gauche/droite : banques",9,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
    }
    if(binding.module==lab::DisplayModule::PadMixer){
        text(dc,{box.x+14,box.y+9,290,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  PAD MIXER",12,focused?accent:muted,FW_SEMIBOLD);
        text(dc,{box.x+310,box.y+9,130,24},L"PADS "+number(padMixerBank*8+1)+L"–"+number(padMixerBank*8+8),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        for(int row=0;row<4;++row){const int pad=padMixerPadFor(display,row);const auto& edit=snapshot.edits[pad];auto b=surfaceMixerTrackBox(display,row);const bool selectedPad=pad==selected;fill(dc,b,edit.mute?RGB(81,31,39):(selectedPad?RGB(31,62,59):(row%2?RGB(18,31,35):RGB(17,28,33))));text(dc,{b.x+10,b.y,b.w-210,b.h},padName(pad)+L"  "+wide(snapshot.names[pad]),10,selectedPad?accent:(edit.mute?RGB(255,154,135):ink),FW_SEMIBOLD,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);text(dc,{b.x+b.w-202,b.y,90,b.h},edit.mute?L"MUTE":L"ACTIF",8,edit.mute?RGB(255,133,103):muted,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);const std::wstring value=padMixerParameter==0?number(int(std::round(edit.gain*100)))+L" %":(std::abs(edit.pan)<.01?L"C":edit.pan<0?L"L "+number(int(std::round(-edit.pan*100))):L"R "+number(int(std::round(edit.pan*100))));text(dc,{b.x+b.w-102,b.y,92,b.h},value,10,accent,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);}
        text(dc,{box.x+14,box.y+268,box.w-28,16},padMixerParameter==0?L"KNOBS : LEVEL · bas : PAN · clic droit : MUTE":L"KNOBS : PAN · haut : LEVEL · gauche/droite : banques",9,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
    }
    if(binding.module==lab::DisplayModule::PadMute){
        text(dc,{box.x+14,box.y+9,300,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  PAD MUTES",12,focused?accent:muted,FW_SEMIBOLD);
        text(dc,{box.x+316,box.y+9,124,24},L"PROGRAM "+number(snapshot.program+1),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        for(int local=0;local<8;++local){const int pad=display*8+local;const auto& edit=snapshot.edits[pad];const auto b=surfaceTrackMuteBox(display,local);const bool selectedPad=pad==selected;round(dc,b,edit.mute?RGB(81,31,39):(selectedPad?RGB(31,62,59):RGB(18,31,35)),selectedPad?accent:RGB(34,48,57));text(dc,{b.x+9,b.y+4,b.w-18,18},padName(pad)+L"  "+wide(snapshot.names[pad]),10,edit.mute?RGB(255,154,135):ink,FW_SEMIBOLD,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);const std::wstring group=edit.chokeGroup<0?L"AUCUN GROUPE":L"CHOKE "+number(edit.chokeGroup+1);text(dc,{b.x+9,b.y+23,b.w-18,17},(edit.mute?L"MUTE · ":L"ACTIF · ")+group,8,edit.mute?RGB(255,133,103):muted,FW_SEMIBOLD);}
        text(dc,{box.x+14,box.y+272,box.w-28,14},L"CLIC / PAD / ENTER : MUTE PROGRAM",8,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
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
    if(binding.module==lab::DisplayModule::Tempo){
        text(dc,{box.x+14,box.y+9,360,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  TEMPO",12,focused?accent:muted,FW_SEMIBOLD);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        text(dc,{box.x+20,box.y+60,box.w-40,92},decimal(snapshot.tempo,1)+L" BPM",42,accent,FW_BOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        text(dc,{box.x+24,box.y+168,box.w-48,30},L"P1 : ±1 BPM     P2 : ±0,1 BPM",13,ink,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        text(dc,{box.x+24,box.y+210,box.w-48,44},L"Soft : −10   −1   +1   +10",11,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
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
        text(dc,{box.x+318,box.y+9,48,24},number(binding.page+1)+L"/5",9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);button(dc,surfacePagePreviousBox(display),L"<");button(dc,surfacePageNextBox(display),L">");fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        const int layerIndex=std::clamp(binding.slot,0,lab::PadLayers-1);const auto& layer=snapshot.layers[pad][layerIndex];button(dc,surfaceProgramAuditionBox(display),binding.page==4?L"Écouter · clic droit charge L"+number(layerIndex+1)+L" · "+wide(layer.name):L"Écouter  ·  "+wide(snapshot.names[pad]));
        for(int knob=0;knob<4;++knob){const int y=box.y+94+knob*45;fill(dc,{box.x+14,y,box.w-28,38},knob%2?RGB(18,31,35):RGB(17,28,33));text(dc,{box.x+24,y,42,38},L"P"+number(display*4+knob+1),10,accent,FW_SEMIBOLD);text(dc,{box.x+68,y,220,38},surfaceProgramParameterName(binding.page,knob),11,ink);text(dc,{box.x+300,y,126,38},surfaceProgramParameterValue(edit,binding.page,knob,binding.page==4?&layer:nullptr,layerIndex),10,accent,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);}
        return;
    }
    if(binding.module==lab::DisplayModule::ColorEditor){
        const auto& info=snapshot.programs[snapshot.program];
        if(display==0){
            text(dc,{box.x+14,box.y+9,292,24},L"ÉCRAN A  ·  COLOR EDITOR",12,focused?accent:muted,FW_SEMIBOLD);text(dc,{box.x+304,box.y+9,136,24},L"PROGRAM "+number(snapshot.program+1),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
            for(int color=0;color<lab::PadColors;++color){const auto b=surfaceColorBox(color);round(dc,b,programPadColor(color),color==colorSelection?RGB(255,255,255):programPadColor(color));text(dc,b,number(color+1),10,color<12||color>=15?RGB(20,25,32):RGB(25,31,40),FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);}
            const std::wstring empty=info.emptyPadColor==lab::EmptyPadColorMode::Dim?L"DIM":L"OFF",off=info.padOffColor==lab::PadOffColorMode::Dim?L"DIM":L"OFF";text(dc,{box.x+14,box.y+252,box.w-28,30},L"P1 COLOR  ·  P2 "+std::wstring(colorAllPads?L"ALL PADS":L"SINGLE PAD")+L"  ·  P3 EMPTY "+empty+L"  ·  P4 OFF "+off,8,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
        }
        text(dc,{box.x+14,box.y+9,292,24},L"ÉCRAN B  ·  ASSIGNED PADS",12,focused?accent:muted,FW_SEMIBOLD);text(dc,{box.x+304,box.y+9,136,24},L"PAD "+padName(selected),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        for(int pad=0;pad<lab::Pads;++pad){const auto b=surfaceColorPadBox(pad);const auto color=programPadColor(snapshot.edits[pad].color);round(dc,b,color,pad==selected?RGB(255,255,255):color);text(dc,{b.x+6,b.y+3,b.w-12,18},padName(pad)+L"  C"+number(snapshot.edits[pad].color+1),9,RGB(20,25,32),FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);text(dc,{b.x+6,b.y+22,b.w-12,16},snapshot.paths[pad].empty()?L"EMPTY":wide(snapshot.names[pad]),7,RGB(35,42,50),FW_NORMAL,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);}
        const std::wstring on=info.padOnColor==lab::PadOnColorMode::Off?L"OFF":info.padOnColor==lab::PadOnColorMode::Velocity?L"VELOCITY":L"FULL";text(dc,{box.x+14,box.y+270,box.w-28,16},L"P5 PAD ON "+on+L"  ·  P6 PAD  ·  P7 COLOR",8,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
    }
    if(binding.module==lab::DisplayModule::EditOperations){
        if(display==0){text(dc,{box.x+14,box.y+9,292,24},L"ÉCRAN A  ·  COPY / DELETE PAD",12,focused?accent:muted,FW_SEMIBOLD);text(dc,{box.x+304,box.y+9,136,24},L"PROGRAM "+number(snapshot.program+1),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
            for(int pad=0;pad<lab::Pads;++pad){const auto b=surfaceEditPadBox(pad);const bool source=pad==editSourcePad,destination=pad==editDestinationPad,active=pad==(editPadRole?editDestinationPad:editSourcePad);auto fillColor=active?accent:(source?RGB(38,76,85):destination?RGB(89,54,34):RGB(18,31,35));round(dc,b,fillColor,active?RGB(255,255,255):(source?accent:destination?RGB(241,148,88):line));text(dc,{b.x+5,b.y+2,b.w-10,18},padName(pad)+(source?L"  SRC":L"")+(destination?L"  DST":L""),8,active?bg:ink,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);text(dc,{b.x+5,b.y+21,b.w-10,17},snapshot.paths[pad].empty()?L"EMPTY":wide(snapshot.names[pad]),7,active?bg:muted,FW_NORMAL,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);}
            text(dc,{box.x+14,box.y+270,box.w-28,16},L"P1 SOURCE "+padName(editSourcePad)+L" · P2 DEST "+padName(editDestinationPad)+L" · P3 "+(editPadRole?L"DEST":L"SOURCE"),8,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;}
        const auto& sequence=snapshot.sequences[eraseSequence];const bool melodic=sequence.mode!=lab::PerformanceMode::Drum;const std::array<std::wstring,4> names={L"Séquence",L"Piste",L"Début",L"Fin"};const std::array<std::wstring,4> values={number(eraseSequence+1)+L" · "+wide(sequence.name),melodic?L"MÉLODIQUE":L"TRACK "+number(eraseTrack+1)+L" · "+padName(eraseTrack),listEditTimeText(eraseStartStep),listEditTimeText(eraseEndStep)};
        text(dc,{box.x+14,box.y+9,292,24},L"ÉCRAN B  ·  ERASE NOTES",12,focused?accent:muted,FW_SEMIBOLD);text(dc,{box.x+304,box.y+9,136,24},performanceModeName(sequence.mode),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));for(int knob=0;knob<4;++knob){const int y=box.y+52+knob*51;fill(dc,{box.x+14,y,box.w-28,43},knob%2?RGB(18,31,35):RGB(17,28,33));text(dc,{box.x+24,y,42,43},L"P"+number(5+knob),10,accent,FW_SEMIBOLD);text(dc,{box.x+68,y,180,43},names[knob],11,ink);text(dc,{box.x+248,y,178,43},values[knob],10,accent,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);}text(dc,{box.x+14,box.y+265,box.w-28,18},L"ENTER / ERASE : confirmer l'effacement de cette plage",8,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
    }
    if(binding.module==lab::DisplayModule::ListEdit){
        const auto items=listEditItems(snapshot);const bool has=!items.empty();const int selectedRow=has?std::clamp(listEditRow,0,int(items.size())-1):0;
        if(display==0){
            text(dc,{box.x+14,box.y+9,292,24},L"ÉCRAN A  ·  LIST EDIT",12,focused?accent:muted,FW_SEMIBOLD);text(dc,{box.x+308,box.y+9,132,24},number(int(items.size()))+L" EVENT(S)",9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
            const int first=std::clamp(selectedRow-2,0,std::max(0,int(items.size())-5));for(int row=0;row<5;++row){const int index=first+row;const auto b=surfaceBrowserFileBox(display,row);const bool chosen=has&&index==selectedRow;if(chosen)round(dc,b,accent,accent);else fill(dc,b,row%2?RGB(18,31,35):RGB(17,28,33));std::wstring line;if(index<int(items.size())){const auto& item=items[index];line=number(index+1)+L"  "+listEditTimeText(item.step)+L"  "+(item.melodic?L"NOTE "+noteName(item.note):L"PAD "+padName(item.track))+L"  LEN "+number(item.event.length)+L"  VEL "+number(item.event.velocity);}else if(row==0&&!has)line=L"Pattern vide — INSERT ajoute un événement";text(dc,{b.x+10,b.y,b.w-20,b.h},line,9,chosen?bg:(has?ink:muted),chosen?FW_SEMIBOLD:FW_NORMAL,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);}
            if(has){const auto& item=items[selectedRow];text(dc,{box.x+14,box.y+252,box.w-28,30},L"P1 TIME "+listEditTimeText(item.step)+L" · P2 "+(item.melodic?L"NOTE "+noteName(item.note):L"PAD "+padName(item.track))+L" · P3 EVENT "+number(selectedRow+1),9,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);}return;
        }
        text(dc,{box.x+14,box.y+9,292,24},L"ÉCRAN B  ·  EVENT DATA",12,focused?accent:muted,FW_SEMIBOLD);text(dc,{box.x+308,box.y+9,132,24},has?number(selectedRow+1)+L" / "+number(int(items.size())):L"AUCUN",9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        if(!has){text(dc,{box.x+20,box.y+80,box.w-40,100},L"Aucun événement dans ce pattern",18,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_WORDBREAK);return;}
        const auto& item=items[selectedRow];const std::array<std::wstring,4> names={L"Length",L"Velocity",L"Probability",L"Ratchet"};const std::array<std::wstring,4> values={number(item.event.length)+L" step"+(item.event.length>1?L"s":L""),number(item.event.velocity),number(item.event.probability)+L" %",L"×"+number(item.event.ratchet)};for(int knob=0;knob<4;++knob){const int y=box.y+52+knob*51;fill(dc,{box.x+14,y,box.w-28,43},knob%2?RGB(18,31,35):RGB(17,28,33));text(dc,{box.x+24,y,42,43},L"P"+number(5+knob),10,accent,FW_SEMIBOLD);text(dc,{box.x+68,y,220,43},names[knob],11,ink);text(dc,{box.x+300,y,126,43},values[knob],11,accent,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);}text(dc,{box.x+14,box.y+265,box.w-28,18},item.melodic?L"NOTE · "+noteName(item.note):L"DRUM · PAD "+padName(item.track),9,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
    }
    if(binding.module==lab::DisplayModule::Events){
        const auto& sequence=snapshot.sequences[snapshot.sequence];const bool melodic=sequence.mode!=lab::PerformanceMode::Drum;const int step=std::clamp(binding.slot,0,lab::Steps-1),absolute=snapshot.viewBar*lab::Steps+step;const auto event=surfaceStepEvent(snapshot,binding);const bool present=melodic?sequence.melody[absolute]>=0:sequence.drum[std::clamp(binding.pad,0,lab::Pads-1)][absolute];
        text(dc,{box.x+14,box.y+9,300,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  EVENTS  ·  STEP "+number(step+1),12,focused?accent:muted,FW_SEMIBOLD);fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        std::wstring eventTitle=present?L"EVENT ACTIF · ":L"EVENT VIDE · ";eventTitle+=melodic?L"NOTE "+noteName(sequence.melody[absolute]):L"TRACK "+padName(binding.pad);button(dc,surfaceProgramAuditionBox(display),eventTitle,present);
        const std::array<std::wstring,4> names={L"Velocity",L"Probability",L"Ratchet",L"Length"};const std::array<std::wstring,4> values={number(event.velocity),number(event.probability)+L" %",L"×"+number(event.ratchet),number(event.length)+L" step"+(event.length>1?L"s":L"")};
        for(int knob=0;knob<4;++knob){const int y=box.y+94+knob*45;fill(dc,{box.x+14,y,box.w-28,38},knob%2?RGB(18,31,35):RGB(17,28,33));text(dc,{box.x+24,y,42,38},L"P"+number(display*4+knob+1),10,accent,FW_SEMIBOLD);text(dc,{box.x+68,y,220,38},names[knob],11,ink);text(dc,{box.x+300,y,126,38},values[knob],10,accent,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);}return;
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
    if(module==lab::DisplayModule::PadMixer){const int pad=padMixerPadFor(display,soft);return (snapshot.edits[pad].mute?L"M ":L"")+padName(pad);}
    if(module==lab::DisplayModule::PadMute){const int pad=display*8+soft;return (snapshot.edits[pad].mute?L"M ":L"")+padName(pad);}
    if(module==lab::DisplayModule::Browser)return std::array<std::wstring,4>{L"RETOUR",L"ÉCOUTER",L"CHARGER",L"ACTUALISER"}[soft];
    if(module==lab::DisplayModule::Programs)return display==0?std::array<std::wstring,4>{L"ASSIGNER",L"NOUVEAU",L"DUPLIQUER",L"SUPPRIMER"}[soft]:std::array<std::wstring,4>{L"CHARGER KIT",L"SAUVER KIT",L"PAD EDIT",L"MENU"}[soft];
    if(module==lab::DisplayModule::Inserts)return L"SLOT "+number(soft+1);
    if(module==lab::DisplayModule::Sample)return std::array<std::wstring,4>{L"SAMPLE",L"ADSR",L"DRUM",L"LAYERS"}[soft];
    if(module==lab::DisplayModule::Events)return std::array<std::wstring,4>{L"VELOCITY",L"PROBABILITY",L"RATCHET",L"LENGTH"}[soft];
    if(module==lab::DisplayModule::ListEdit)return display==0?std::array<std::wstring,4>{L"PRÉC.",L"SUIV.",L"INSÉRER",L"SUPPRIMER"}[soft]:std::array<std::wstring,4>{L"LENGTH",L"VELOCITY",L"PROB.",L"RATCHET"}[soft];
    if(module==lab::DisplayModule::ColorEditor)return display==0?std::array<std::wstring,4>{L"COLOR",colorAllPads?L"ALL PADS":L"SINGLE",L"EMPTY",L"PAD OFF"}[soft]:std::array<std::wstring,4>{L"PAD ON",L"NEXT PAD",L"APPLY",L"DEFAULT"}[soft];
    if(module==lab::DisplayModule::EditOperations)return display==0?std::array<std::wstring,4>{L"SOURCE",L"DEST.",L"COPY",L"DELETE"}[soft]:std::array<std::wstring,4>{L"CURRENT",L"FULL TRACK",L"BAR",L"ERASE"}[soft];
    if(module==lab::DisplayModule::Tempo)return std::array<std::wstring,4>{L"−10",L"−1",L"+1",L"+10"}[soft];
    if(module==lab::DisplayModule::MidiControl)return std::array<std::wstring,4>{L"SAVE MIDI",L"DOSSIER",L"< PAGE",L"PAGE >"}[soft];
    if(module==lab::DisplayModule::Menu){static const std::array<int,8> items={0,1,2,3,4,11,12,15};return surfaceMenuLabels[items[display*4+soft]];}
    return L"SOFT "+number(mixerBank*8+display*4+soft+1);
}

void activateSurfaceSoft(int display,int soft,int bank=-1){
    display=std::clamp(display,0,1);soft=std::clamp(soft,0,3);surface.focusDisplay(display);auto module=surface.displays[display].module;if(bank>=0){if(module==lab::DisplayModule::PadMixer)padMixerBank=std::clamp(bank,0,1);else mixerBank=std::clamp(bank,0,1);}
    if(module==lab::DisplayModule::Mixer){showSurfaceMixer();const int track=mixerTrackFor(display,soft);selectTrack(track);if(surface.shift)toggleTrackArm(track);status=L"Track "+number(track+1)+(surface.shift?L" : armement basculé.":L" sélectionnée dans le Mixer.");return;}
    if(module==lab::DisplayModule::PadMixer){showSurfacePadMixer();const int pad=padMixerPadFor(display,soft);selectPad(pad);if(surface.shift)toggleProgramPadMute(pad);status=L"Pad "+padName(pad)+(surface.shift?L" : mute basculé.":L" sélectionné dans le Pad Mixer.");return;}
    if(module==lab::DisplayModule::PadMute){const int pad=display*8+soft;selectPad(pad);toggleProgramPadMute(pad);return;}
    if(module==lab::DisplayModule::TrackMute){const int track=display*8+soft;selectTrack(track);if(surface.shift)toggleTrackSolo(track);else toggleTrackMute(track);return;}
    if(module==lab::DisplayModule::Browser){if(soft==0)moveSurfaceBrowserCategory(display,-1);else if(soft==1)auditionSurfaceBrowser(display);else if(soft==2)loadSurfaceBrowserSelection(display);else refreshSurfaceBrowser(display,true);return;}
    if(module==lab::DisplayModule::Programs){if(display==0){if(soft==0)assignSelectedProgram();else if(soft==1)createSurfaceProgram(false);else if(soft==2)createSurfaceProgram(true);else deleteSurfaceProgram();}else {if(soft<2){const auto snapshot=engine.snapshot();if(programSelected>=0&&programSelected<lab::Programs&&snapshot.programs[programSelected].used){assignSelectedProgram();if(soft==0)loadKit();else saveCurrentKit();}else status=L"Slot vide — crée d'abord un Program.";}else if(soft==2)showSurfaceProgram(display);else showSurfaceHome();}return;}
    if(module==lab::DisplayModule::Inserts){selectedInsert=soft;arrangeSurfaceInsertPages(surface.displays[display].pad,soft,display);return;}
    if(module==lab::DisplayModule::Sample){surface.displays[display].page=soft<2?soft:soft==2?3:4;return;}
    if(module==lab::DisplayModule::ListEdit){if(display==0){if(soft==0)moveListEditSelection(-1);else if(soft==1)moveListEditSelection(1);else if(soft==2)insertListEditEvent();else deleteListEditEvent();}return;}
    if(module==lab::DisplayModule::ColorEditor){auto snapshot=engine.snapshot();if(display==0){if(soft==0)turnSurfaceQLink(0,0,1);else if(soft==1){colorAllPads=!colorAllPads;}else if(soft==2)setSurfaceColorRatio(0,2,snapshot.programs[snapshot.program].emptyPadColor==lab::EmptyPadColorMode::Dim?0.:1.);else setSurfaceColorRatio(0,3,snapshot.programs[snapshot.program].padOffColor==lab::PadOffColorMode::Dim?0.:1.);}else {if(soft==0)turnSurfaceQLink(1,0,1);else if(soft==1){selectPad((selected+1)%lab::Pads);}else if(soft==2)applySurfaceColor(selected);else {engine.resetProgramPadColors();colorSelection=3;markProjectDirty();status=L"Color Editor — couleur KICK par défaut appliquée aux seize pads.";}}return;}
    if(module==lab::DisplayModule::EditOperations){if(display==0){if(soft==0){editPadRole=0;selectPad(editSourcePad);}else if(soft==1){editPadRole=1;selectPad(editDestinationPad);}else if(soft==2)copyEditPad();else deleteEditPad();}else {auto snapshot=engine.snapshot();if(soft==0){eraseSequence=snapshot.sequence;eraseTrack=stepTrack;const int total=snapshot.sequences[eraseSequence].bars*lab::Steps;eraseStartStep=std::clamp(eraseStartStep,0,total-1);eraseEndStep=std::clamp(eraseEndStep,eraseStartStep,total-1);}else if(soft==1){eraseStartStep=0;eraseEndStep=snapshot.sequences[eraseSequence].bars*lab::Steps-1;}else if(soft==2){eraseSequence=snapshot.sequence;eraseStartStep=snapshot.viewBar*lab::Steps;eraseEndStep=std::min(snapshot.sequences[eraseSequence].bars*lab::Steps-1,eraseStartStep+lab::Steps-1);}else eraseEditNotes();}return;}
    if(module==lab::DisplayModule::Tempo){static const int changes[4]={-10,-1,1,10};engine.setTempo(engine.snapshot().tempo+changes[soft]);markProjectDirty();return;}
    if(module==lab::DisplayModule::MidiControl){auto& binding=surface.displays[display];if(soft==0){const bool saved=saveMidiProfile();status=saved?L"Profil MIDI sauvegardé : "+portableMidiProfile.wstring():L"Impossible de sauvegarder le profil MIDI.";}else if(soft==1)ShellExecuteW(window,L"open",portableMidiProfile.parent_path().c_str(),nullptr,nullptr,SW_SHOWNORMAL);else if(soft==2)binding.page=std::max(0,binding.page-1);else binding.page=std::min(midiControlPages()-1,binding.page+1);return;}
    if(module==lab::DisplayModule::Menu){static const std::array<int,8> items={0,1,2,3,4,11,12,15};selectSurfaceMenu(items[display*4+soft]);activateSurfaceMenu();return;}
    status=L"Soft contextuel "+number(mixerBank*8+display*4+soft+1)+L" reçu.";
}

void paintSurface(HDC dc,const lab::Snapshot& snapshot){
    for(int display=0;display<2;++display)for(int soft=0;soft<4;++soft)
        button(dc,surfaceSoftButtonBox(display,soft),surfaceSoftLabel(display,soft,snapshot));
    paintSurfaceDisplay(dc,0,snapshot);paintSurfaceDisplay(dc,1,snapshot);
    for(int display=0;display<2;++display)for(int knob=0;knob<4;++knob)paintSurfaceKnob(dc,display,knob,snapshot);
}

void hardwareButton(HDC dc,Box box,const std::wstring& primary,const std::wstring& shifted=L"",bool on=false,bool available=true){
    round(dc,box,on?accent:themeButtons,on?accent:(available?line:RGB(35,41,51)));
    text(dc,{box.x+6,box.y+3,box.w-12,shifted.empty()?box.h-6:17},primary,10,on?bg:(available?ink:muted),FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
    if(!shifted.empty())text(dc,{box.x+6,box.y+19,box.w-12,box.h-21},shifted,8,on?RGB(24,55,51):muted,FW_NORMAL,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
}

void paintMaschineControls(HDC dc,const lab::Snapshot& snapshot){
    static const std::array<std::pair<const wchar_t*,const wchar_t*>,12> shortcuts={{
        {L"CHANNEL",L"MIDI"},{L"PLUG-IN",L"Instance"},{L"IDEAS",L"Song"},{L"MIXER",L""},
        {L"BROWSER",L"Plug-in"},{L"SAMPLING",L""},{L"<",L"Page"},{L">",L"Page"},
        {L"FILE",L"Save"},{L"SETTINGS",L""},{L"AUTO",L""},{L"MACRO",L"Set"}
    }};
    for(int i=0;i<12;++i){const bool available=i!=10;const auto module=surface.displays[0].module;hardwareButton(dc,shortcutBox(i),shortcuts[i].first,shortcuts[i].second,i==3&&(module==lab::DisplayModule::Mixer||module==lab::DisplayModule::PadMixer),available);}

    hardwareButton(dc,utilityButtonBox(0),L"MENU",L"Écrans A + B",false,true);
    hardwareButton(dc,utilityButtonBox(1),L"RÉSERVE",L"",false,false);

    static const std::array<std::pair<const wchar_t*,const wchar_t*>,5> smallButtons={{
        {L"VOLUME",L"Velocity"},{L"NOTE REPEAT",L"Arp"},{L"SWING",L"Position"},{L"LOCK",L"Ext Lock"},{L"TEMPO",L"Tune"}
    }};
    hardwareButton(dc,smallControlBox(0,0),smallButtons[0].first,smallButtons[0].second);
    hardwareButton(dc,smallControlBox(1,0),smallButtons[1].first,smallButtons[1].second,snapshot.noteRepeat,true);
    hardwareButton(dc,smallControlBox(0,1),smallButtons[2].first,smallButtons[2].second,false,false);
    hardwareButton(dc,smallControlBox(1,1),smallButtons[3].first,smallButtons[3].second,false,false);
    hardwareButton(dc,smallControlBox(0,2),smallButtons[4].first,smallButtons[4].second,surface.displays[surface.focusedDisplay].module==lab::DisplayModule::Tempo);
    text(dc,smallControlBox(1,2),number(int(snapshot.tempo))+L" BPM",15,accent,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

    static const std::array<const wchar_t*,4> performance={L"PITCH / PAD EDIT",L"MOD",L"PERFORM / FX",L"NOTES"};
    for(int i=0;i<4;++i)hardwareButton(dc,performanceButtonBox(i),performance[i],L"",false,i==0||i==2||i==3);

    for(int group=0;group<8;++group)hardwareButton(dc,groupBox(group),std::wstring(1,wchar_t(L'A'+group)),group==snapshot.queuedGroup?L"NEXT":L"",group==snapshot.group||group==snapshot.queuedGroup);

    static const std::array<std::pair<const wchar_t*,const wchar_t*>,9> commands={{
        {L"FIXED VEL",L"16 Vel"},{L"SCENE",L"Section"},{L"PATTERN",L"Clip"},
        {L"EVENTS",L""},{L"VARIATION",L""},{L"DUPLICATE",L"Double"},
        {L"SELECT",L""},{L"SOLO",L""},{L"MUTE",L"Choke"}
    }};
    for(int i=0;i<9;++i){const bool available=i==2||i==3||i==5||i>=6;const auto focused=surface.displays[surface.focusedDisplay].module;const bool active=i==3?(focused==lab::DisplayModule::Events||focused==lab::DisplayModule::ListEdit):i==5?(duplicateSourceSequence>=0||focused==lab::DisplayModule::EditOperations):i==6?selectLatch:i==7?soloLatch:i==8?muteLatch:false;hardwareButton(dc,sideCommandBox(i),commands[i].first,commands[i].second,active,available);}

    // Reserved symmetrically for the next KICK modules. They are deliberately
    // blank and have no hit handling yet.
    for(int i=0;i<9;++i)round(dc,futureCommandBox(i),RGB(18,24,34),RGB(35,47,62));

    static const std::array<const wchar_t*,4> modes={L"PAD MODE",L"KEYBOARD",L"CHORDS",L"STEP"};
    for(int mode=0;mode<4;++mode)hardwareButton(dc,modeBox(mode),modes[mode],L"",mode==padMode,true);

    const auto& sequence=snapshot.sequences[snapshot.sequence];const bool notes=padMode==1||padMode==2;const bool steps=padMode==3;const bool melodicSteps=steps&&sequence.mode!=lab::PerformanceMode::Drum;const auto focusedModule=surface.displays[surface.focusedDisplay].module;const bool editingEvents=focusedModule==lab::DisplayModule::Events;const bool editingColors=focusedModule==lab::DisplayModule::ColorEditor;const bool editingPads=focusedModule==lab::DisplayModule::EditOperations;const bool editingPadMix=focusedModule==lab::DisplayModule::PadMixer||focusedModule==lab::DisplayModule::PadMute;
    const bool playVisible=snapshot.playing&&snapshot.playBar==snapshot.viewBar;ULONGLONG now=GetTickCount64();
    for(int p=0;p<16;++p){
        const auto box=padBox(p);const bool active=editingPadMix?snapshot.edits[p].mute:steps?(melodicSteps?snapshot.melody[p]==engine.performanceNote(stepTrack):snapshot.pattern[stepTrack][p]):notes?false:snapshot.active[p];
        const bool hot=now<flashed[p]||active||(steps&&playVisible&&snapshot.step==p)||(editingEvents&&p==eventStep)||(editingPadMix&&p==selected)||(editingPads&&(p==editSourcePad||p==editDestinationPad));const COLORREF color=editingPadMix&&snapshot.edits[p].mute?RGB(225,92,78):editingPads&&p==editDestinationPad?RGB(241,148,88):steps?RGB(241,184,88):programPadColor(snapshot.edits[p].color);COLORREF face=dimColor(themePads,.28);const auto& colorMode=snapshot.programs[snapshot.program];
        if(editingColors)face=color;else if(hot&&colorMode.padOnColor!=lab::PadOnColorMode::Off)face=color;else if(snapshot.paths[p].empty()&&colorMode.emptyPadColor==lab::EmptyPadColorMode::Dim)face=dimColor(color,.22);else if(!snapshot.paths[p].empty()&&colorMode.padOffColor==lab::PadOffColorMode::Dim)face=dimColor(color,.28);
        round(dc,box,face,(padMode==0&&p==selected)||editingColors&&p==selected?color:line);
        std::wstring title=editingPadMix?wide(snapshot.names[p]):steps?L"STEP "+number(p+1):notes?noteName(engine.performanceNote(p)):wide(snapshot.names[p]);
        std::wstring sub=editingPads?(p==editSourcePad?L"SOURCE":p==editDestinationPad?L"DESTINATION":L"PAD "+padName(p)):editingPadMix?(snapshot.edits[p].mute?L"PAD MUTE":L"PAD ACTIF"):steps?(melodicSteps?noteName(engine.performanceNote(stepTrack)):L"TRACK "+padName(stepTrack)):notes?(padMode==2?L"TRIADE ARP":L"PAD "+padName(sequence.sourcePad)):L"PAD "+padName(p);
        text(dc,{box.x+8,box.y+7,box.w-16,25},title,12,hot||editingColors?bg:ink,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
        text(dc,{box.x+8,box.y+52,box.w-16,18},sub,9,hot||editingColors?RGB(40,46,50):muted,FW_NORMAL,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
    }

    hardwareButton(dc,transportBox(0),L"RESTART",L"Loop",false,false);
    hardwareButton(dc,transportBox(1),L"ERASE",L"Replace",focusedModule==lab::DisplayModule::EditOperations,true);
    hardwareButton(dc,transportBox(2),L"TAP",L"Metro",false,false);
    hardwareButton(dc,transportBox(3),L"FOLLOW",L"Grid "+recordQuantizeName(),false,true);
    hardwareButton(dc,transportBox(4),snapshot.playing?L"PAUSE":L"PLAY",L"",snapshot.playing);
    hardwareButton(dc,transportBox(5),L"REC",recordArmed?(snapshot.playing?L"Recording ":L"Armed ")+recordQuantizeName():L"Count-in",recordArmed,true);
    hardwareButton(dc,transportBox(6),L"STOP");
    hardwareButton(dc,transportBox(7),L"SHIFT",L"",surface.shift);

    paintBrandPanel(dc,snapshot);
}

void paintMasterMeter(HDC dc,double value,double peak,double preLimiterPeak,double reductionDb){
    const auto master=brandMasterBox();value=std::clamp(value,0.,1.);peak=std::max(0.,peak);preLimiterPeak=std::max(0.,preLimiterPeak);const Box meter{master.x+12,master.y+178,master.w-24,16};
    const double db=peak>0?20.*std::log10(peak):-60.,preDb=preLimiterPeak>0?20.*std::log10(preLimiterPeak):-60.;const double ratio=std::clamp((db+60.)/60.,0.,1.);const int lit=int((meter.w-4)*ratio),green=std::min(lit,int((meter.w-4)*.8)),amber=std::max(0,lit-green);
    const double masterDb=value>0?20.*std::log10(value):-60.;text(dc,{master.x+12,master.y+106,master.w-24,18},value>0?decimal(masterDb,1)+L" dB":L"−∞ dB",13,ink,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    std::wstring level=db<=-59.9?L"OUT −∞":L"OUT "+decimal(db,1)+L" dB";if(reductionDb>.05)level+=L" · GR "+decimal(reductionDb,1);else if(preDb> -59.9)level+=L" · PRE "+decimal(preDb,1);text(dc,{master.x+10,master.y+142,master.w-20,20},level,8,reductionDb>6?RGB(255,133,103):(reductionDb>.05?RGB(241,184,88):muted),FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    round(dc,meter,RGB(20,26,34),line);if(green>0)fill(dc,{meter.x+2,meter.y+3,green,meter.h-6},accent);if(amber>0)fill(dc,{meter.x+2+green,meter.y+3,amber,meter.h-6},RGB(241,184,88));
    const int marker=meter.x+2+int((meter.w-4)*value);fill(dc,{marker-1,meter.y-2,2,meter.h+4},ink);
}

void paintBackdrop(HDC dc){
    // Quiet organic/cybernetic texture: thin traces only, kept behind every
    // control so readability remains identical on low-contrast displays.
    HPEN blue=CreatePen(PS_SOLID,1,RGB(15,42,58));HPEN red=CreatePen(PS_SOLID,1,RGB(56,22,30));
    auto old=SelectObject(dc,blue);
    for(int y=408;y<890;y+=48){MoveToEx(dc,270,y,nullptr);LineTo(dc,1400,y+((y/48)%2?18:-18));}
    SelectObject(dc,red);
    for(int x=270;x<1400;x+=92){MoveToEx(dc,x,404,nullptr);LineTo(dc,x+118,884);}
    Arc(dc,330,470,635,790,330,630,635,630);Arc(dc,1040,490,1344,820,1040,650,1344,650);
    SelectObject(dc,old);DeleteObject(blue);DeleteObject(red);
}

void paintBrandPanel(HDC dc,const lab::Snapshot& snapshot){
    const auto identity=brandHomeBox(),controls=brandControlBox(),master=brandMasterBox();
    round(dc,identity,RGB(11,17,27),RGB(42,69,88));fill(dc,{identity.x+8,identity.y+7,identity.w-16,2},accent);
    text(dc,{identity.x+8,identity.y+12,identity.w-16,52},L"KICK",36,ink,FW_BOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    text(dc,{identity.x+8,identity.y+62,identity.w-16,18},L"ANKS Labs",12,accent,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    text(dc,{identity.x+8,identity.y+82,identity.w-16,14},L"0.20.0",9,muted,FW_NORMAL,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

    round(dc,controls,RGB(11,17,27),RGB(42,69,88));
    for(int macro=0;macro<8;++macro){const auto box=brandMacroBox(macro);const int cx=box.x+box.w/2,cy=box.y+23;HBRUSH brush=CreateSolidBrush(themeButtons);HPEN edge=CreatePen(PS_SOLID,2,line);auto oldB=SelectObject(dc,brush),oldP=SelectObject(dc,edge);Ellipse(dc,cx-15,cy-15,cx+15,cy+15);SelectObject(dc,oldB);SelectObject(dc,oldP);DeleteObject(brush);DeleteObject(edge);HPEN marker=CreatePen(PS_SOLID,2,muted);oldP=SelectObject(dc,marker);MoveToEx(dc,cx,cy,nullptr);LineTo(dc,cx,cy-10);SelectObject(dc,oldP);DeleteObject(marker);text(dc,{box.x,box.y+40,box.w,14},L"M"+number(macro+1),8,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);}
    for(int buttonIndex=0;buttonIndex<4;++buttonIndex){const auto box=brandPushBox(buttonIndex);round(dc,box,themeButtons,line);text(dc,box,L"P"+number(buttonIndex+1),9,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);}

    round(dc,master,RGB(11,17,27),RGB(42,69,88));text(dc,{master.x+8,master.y+18,master.w-16,24},L"MASTER",13,ink,FW_BOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);text(dc,{master.x+8,master.y+50,master.w-16,28},L"●",24,accent,FW_NORMAL,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    meterPeak=std::max(double(snapshot.peak),meterPeak*.9);paintMasterMeter(dc,snapshot.volume,meterPeak,snapshot.preLimiterPeak,snapshot.limiterReductionDb);
}

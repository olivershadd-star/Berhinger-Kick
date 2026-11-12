// SPDX-License-Identifier: AGPL-3.0-or-later
// App member functions; coordinates share the same scalable canvas as the pads.
enum EditorParameter {EditorVolume=1,EditorTune,EditorFade};
static constexpr int InsertKnobBase=10;
int editorDrag=0,editorDragStartY=0,selectedInsert=0;
double editorDragStartRatio=0;
Box editorKnob(int id)const {
    if(id<=EditorFade)return {40+(id-1)*180,474,172,158};
    return {622+(id-InsertKnobBase)*176,556,172,104};
}
Box insertSlotBox(int slot)const{return {622+slot*132,476,126,34};}
Box insertEffectBox()const{return {622,516,226,34};}
Box insertBypassBox()const{return {858,516,140,34};}
Box insertsBypassBox()const{return {1008,516,142,34};}
double editorRatio(const lab::PadEdit& edit,int id)const {
    if(id==EditorVolume)return edit.gain/2;
    if(id==EditorTune)return (edit.semitones+24)/48;
    return edit.fadeMs/20;
}
void setEditorRatio(lab::PadEdit& edit,int id,double ratio){
    ratio=std::clamp(ratio,0.,1.);
    if(id==EditorVolume)edit.gain=ratio*2;
    else if(id==EditorTune)edit.semitones=std::round(ratio*48)-24;
    else if(id==EditorFade)edit.fadeMs=ratio*20;
}
std::wstring insertName(lab::InsertEffect effect)const{return wide(lab::insertEffectName(effect));}
std::wstring insertSlotName(int slot,const lab::InsertState& insert)const {
    std::wstring name=insert.effect==lab::InsertEffect::None?L"Vide":insertName(insert.effect);
    return number(slot+1)+L"  "+name+(insert.bypass&&insert.effect!=lab::InsertEffect::None?L"  B":L"");
}
std::wstring insertValue(lab::InsertEffect effect,int parameter,double value)const {
    if((effect==lab::InsertEffect::Lowpass||effect==lab::InsertEffect::Highpass)&&parameter==1)return decimal(value*200-100,0)+L" %";
    return decimal(value*100,0)+L" %";
}
void insertMenu(){
    HMENU menu=CreatePopupMenu();AppendMenuW(menu,MF_STRING,800,L"Aucun effet");AppendMenuW(menu,MF_SEPARATOR,0,nullptr);
    std::vector<std::string> categories;
    for(const auto& info:lab::insertEffectCatalog())if(std::find(categories.begin(),categories.end(),info.category)==categories.end())categories.emplace_back(info.category);
    for(const auto& category:categories){HMENU submenu=CreatePopupMenu();for(const auto& info:lab::insertEffectCatalog())if(category==info.category)AppendMenuW(submenu,MF_STRING,800+int(info.effect),wide(info.name).c_str());AppendMenuW(menu,MF_POPUP,reinterpret_cast<UINT_PTR>(submenu),wide(category).c_str());}
    POINT p;GetCursorPos(&p);int choice=TrackPopupMenu(menu,TPM_RETURNCMD|TPM_NONOTIFY,p.x,p.y,0,window,nullptr);DestroyMenu(menu);if(choice>800&&choice<=800+int(lab::InsertEffect::Mackity))engine.setInsertEffect(selected,selectedInsert,lab::InsertEffect(choice-800));else if(choice==800)engine.setInsertEffect(selected,selectedInsert,lab::InsertEffect::None);
}
void editorPointer(int x,int y){
    auto s=engine.snapshot();auto edit=s.edits[selected];
    if(editorDrag==6||editorDrag==7){double p=std::clamp((x-52)/1096.,0.,1.);if(editorDrag==6)edit.start=std::min(p,edit.end-.001);else edit.end=std::max(p,edit.start+.001);engine.setEdit(selected,edit);}
    else if(editorDrag>=EditorVolume&&editorDrag<=EditorFade){setEditorRatio(edit,editorDrag,editorDragStartRatio+(editorDragStartY-y)/140.);engine.setEdit(selected,edit);}
    else if(editorDrag>=InsertKnobBase){engine.setInsertParameter(selected,selectedInsert,editorDrag-InsertKnobBase,editorDragStartRatio+(editorDragStartY-y)/120.);}
}
bool editorWheel(int x,int y,int delta){
    if(tab!=1)return false;
    auto s=engine.snapshot();auto edit=s.edits[selected];
    for(int id=EditorVolume;id<=EditorFade;++id)if(editorKnob(id).contains(x,y)){double step=id==EditorTune?1./48.:.01;setEditorRatio(edit,id,editorRatio(edit,id)+(delta>0?step:-step));engine.setEdit(selected,edit);return true;}
    auto effect=edit.inserts[selectedInsert].effect;int count=lab::insertParameterCount(effect);
    for(int parameter=0;parameter<count;++parameter)if(editorKnob(InsertKnobBase+parameter).contains(x,y)){engine.setInsertParameter(selected,selectedInsert,parameter,edit.inserts[selectedInsert].parameters[parameter]+(delta>0?.01:-.01));return true;}
    return false;
}
void editorClick(int x,int y,bool right){
    for(int p=0;p<16;++p)if(Box{28+p*71,110,65,42}.contains(x,y)){selectPad(p);if(right)sampleMenu();else dispatch({lab::SurfaceAction::PadPress,p});return;}
    auto edit=engine.snapshot().edits[selected];
    for(int slot=0;slot<lab::InsertSlots;++slot)if(insertSlotBox(slot).contains(x,y)){selectedInsert=slot;return;}
    if(insertEffectBox().contains(x,y)){insertMenu();return;}
    if(insertBypassBox().contains(x,y)&&edit.inserts[selectedInsert].effect!=lab::InsertEffect::None){engine.setInsertBypass(selected,selectedInsert,!edit.inserts[selectedInsert].bypass);return;}
    if(insertsBypassBox().contains(x,y)){engine.setInsertsBypass(selected,!edit.insertsBypass);return;}
    if(Box{44,226,1112,150}.contains(x,y)){double p=(x-52)/1096.;editorDrag=std::abs(p-edit.start)<=std::abs(p-edit.end)?6:7;}
    else for(int id=EditorVolume;id<=EditorFade;++id)if(editorKnob(id).contains(x,y)){editorDrag=id;editorDragStartY=y;editorDragStartRatio=editorRatio(edit,id);break;}
    if(!editorDrag){int count=lab::insertParameterCount(edit.inserts[selectedInsert].effect);for(int parameter=0;parameter<count;++parameter)if(editorKnob(InsertKnobBase+parameter).contains(x,y)){editorDrag=InsertKnobBase+parameter;editorDragStartY=y;editorDragStartRatio=edit.inserts[selectedInsert].parameters[parameter];break;}}
    if(editorDrag){SetCapture(window);if(editorDrag==6||editorDrag==7)editorPointer(x,y);return;}
    if(Box{28,690,200,42}.contains(x,y))engine.setEdit(selected,{});
    else if(Box{244,690,180,42}.contains(x,y))sampleMenu();
    else if(Box{440,690,200,42}.contains(x,y))project(true);
    else if(Box{656,690,250,42}.contains(x,y))savePadToLibrary();
}
std::wstring decimal(double value,int precision=2)const {wchar_t buffer[64];std::swprintf(buffer,64,L"%.*f",precision,value);return buffer;}
void paintEditorKnob(HDC dc,int id,const std::wstring& name,const std::wstring& value,double ratio){
    auto b=editorKnob(id);bool compact=id>=InsertKnobBase;ratio=std::clamp(ratio,0.,1.);text(dc,{b.x,b.y,b.w,20},name,compact?11:13,ink,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    int cx=b.x+b.w/2,cy=b.y+(compact?48:69),r=compact?25:35;HBRUSH brush=CreateSolidBrush(RGB(31,38,50));HPEN edge=CreatePen(PS_SOLID,2,line);auto oldB=SelectObject(dc,brush),oldP=SelectObject(dc,edge);Ellipse(dc,cx-r,cy-r,cx+r,cy+r);SelectObject(dc,oldB);SelectObject(dc,oldP);DeleteObject(brush);DeleteObject(edge);
    constexpr double pi=3.14159265358979323846;double angle=(135.+270.*ratio)*pi/180.;int x2=cx+int(std::cos(angle)*(r-8)),y2=cy+int(std::sin(angle)*(r-8));HPEN mark=CreatePen(PS_SOLID,compact?3:4,accent);oldP=SelectObject(dc,mark);MoveToEx(dc,cx,cy,nullptr);LineTo(dc,x2,y2);SelectObject(dc,oldP);DeleteObject(mark);
    round(dc,{cx-3,cy-3,6,6},ink,ink);text(dc,{b.x,b.y+(compact?76:112),b.w,24},value,compact?11:13,accent,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}
void paintEditor(HDC dc){
    auto s=engine.snapshot();const auto& edit=s.edits[selected];
    for(int p=0;p<16;++p)button(dc,{28+p*71,110,65,42},L"PAD "+padName(p),p==selected);
    round(dc,{28,174,1144,240},panel);text(dc,{48,184,810,30},L"PAD "+padName(selected)+L" / "+wide(s.names[selected]),20,padColor(selected),FW_SEMIBOLD);text(dc,{914,184,232,30},decimal(s.durations[selected],3)+L" s  ·  48 kHz",13,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
    const int a=52+int(edit.start*1096),b=52+int(edit.end*1096);fill(dc,{52,234,1096,132},RGB(17,22,30));fill(dc,{a,234,std::max(1,b-a),132},RGB(27,50,52));const auto& wave=s.waveforms[selected];
    if(wave.empty())text(dc,{70,260,1058,64},L"Charger un WAV pour régler sa zone de lecture",18,muted,FW_NORMAL,DT_CENTER|DT_VCENTER|DT_SINGLELINE);else for(int i=0;i<int(wave.size());++i){int x=52+i*1096/int(wave.size());int h=std::max(1,int(wave[i]*58));fill(dc,{x,300-h,3,h*2},x>=a&&x<b?padColor(selected):RGB(64,77,95));}
    fill(dc,{a,230,2,140},accent);fill(dc,{b-2,230,2,140},accent);round(dc,{a-7,228,14,12},accent,accent);round(dc,{b-7,360,14,12},accent,accent);if(s.active[selected]){int head=52+int(s.progress[selected]*1096);fill(dc,{head,236,1,128},ink);}
    text(dc,{52,377,510,25},L"DÉBUT  "+decimal(edit.start*s.durations[selected],3)+L" s  /  "+decimal(edit.start*100,1)+L" %",13,accent);text(dc,{650,377,498,25},L"FIN  "+decimal(edit.end*s.durations[selected],3)+L" s  /  "+decimal(edit.end*100,1)+L" %",13,accent,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
    round(dc,{28,430,560,236},panel);text(dc,{48,441,510,24},L"LECTURE DU PAD",15,ink,FW_SEMIBOLD);paintEditorKnob(dc,EditorVolume,L"Volume",decimal(edit.gain*100,0)+L" %",edit.gain/2);paintEditorKnob(dc,EditorTune,L"Accordage",(edit.semitones>=0?L"+":L"")+decimal(edit.semitones,0)+L" demi-tons",(edit.semitones+24)/48);paintEditorKnob(dc,EditorFade,L"Fondus début / fin",decimal(edit.fadeMs,1)+L" ms",edit.fadeMs/20);
    round(dc,{608,430,564,236},panel);text(dc,{628,441,300,24},L"EFFETS / 4 INSERTS KICK",15,ink,FW_SEMIBOLD);
    for(int slot=0;slot<lab::InsertSlots;++slot)button(dc,insertSlotBox(slot),insertSlotName(slot,edit.inserts[slot]),slot==selectedInsert);
    auto selectedState=edit.inserts[selectedInsert];button(dc,insertEffectBox(),selectedState.effect==lab::InsertEffect::None?L"Choisir un effet":L"Effet : "+insertName(selectedState.effect));button(dc,insertBypassBox(),selectedState.bypass?L"Slot bypass":L"Slot actif",selectedState.effect!=lab::InsertEffect::None&&!selectedState.bypass);button(dc,insertsBypassBox(),edit.insertsBypass?L"Tout bypass":L"Chaîne active",!edit.insertsBypass);
    int count=lab::insertParameterCount(selectedState.effect);for(int parameter=0;parameter<count;++parameter)paintEditorKnob(dc,InsertKnobBase+parameter,wide(lab::insertParameterName(selectedState.effect,parameter)),insertValue(selectedState.effect,parameter,selectedState.parameters[parameter]),selectedState.parameters[parameter]);
    if(!count)text(dc,{630,572,512,55},L"Choisis une catégorie puis un effet pour ce slot.",13,muted,FW_NORMAL,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    button(dc,{28,690,200,42},L"Réinitialiser ce pad");button(dc,{244,690,180,42},L"Charger un WAV");button(dc,{440,690,200,42},L"Enregistrer projet");button(dc,{656,690,250,42},L"Ajouter à la bibliothèque");
    text(dc,{28,751,1144,32},L"Sélectionner un slot, choisir son effet, puis régler ses paramètres. La chaîne suit l'ordre 1 → 4.",13,muted);
}

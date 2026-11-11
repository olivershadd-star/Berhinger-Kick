// SPDX-License-Identifier: AGPL-3.0-or-later
// App member functions; coordinates share the same scalable canvas as the pads.
enum EditorParameter {EditorVolume=1,EditorTune,EditorFade,EditorFilter,EditorDrive};
int editorDrag=0,editorDragStartY=0;
double editorDragStartRatio=0;
Box editorKnob(int id)const {
    if(id<=3)return {40+(id-1)*180,474,172,158};
    return {624+(id-4)*270,482,260,150};
}
double editorRatio(const lab::PadEdit& edit,int id)const {
    if(id==EditorVolume)return edit.gain/2;
    if(id==EditorTune)return (edit.semitones+24)/48;
    if(id==EditorFade)return edit.fadeMs/20;
    if(id==EditorFilter)return edit.filterOpen;
    return edit.drive;
}
void setEditorRatio(lab::PadEdit& edit,int id,double ratio){
    ratio=std::clamp(ratio,0.,1.);
    if(id==EditorVolume)edit.gain=ratio*2;
    else if(id==EditorTune)edit.semitones=std::round(ratio*48)-24;
    else if(id==EditorFade)edit.fadeMs=ratio*20;
    else if(id==EditorFilter)edit.filterOpen=ratio;
    else if(id==EditorDrive)edit.drive=ratio;
}
void editorPointer(int x,int y){
    auto s=engine.snapshot();auto edit=s.edits[selected];
    if(editorDrag==6||editorDrag==7){double p=std::clamp((x-52)/1096.,0.,1.);
        if(editorDrag==6)edit.start=std::min(p,edit.end-.001);else edit.end=std::max(p,edit.start+.001);
    }else if(editorDrag>=EditorVolume&&editorDrag<=EditorDrive)setEditorRatio(edit,editorDrag,editorDragStartRatio+(editorDragStartY-y)/140.);
    else return;
    engine.setEdit(selected,edit);
}
bool editorWheel(int x,int y,int delta){
    if(tab!=1)return false;
    auto s=engine.snapshot();auto edit=s.edits[selected];
    for(int id=EditorVolume;id<=EditorDrive;++id)if(editorKnob(id).contains(x,y)){
        double step=id==EditorTune?1./48.:.01;
        setEditorRatio(edit,id,editorRatio(edit,id)+(delta>0?step:-step));engine.setEdit(selected,edit);return true;
    }
    return false;
}
void editorClick(int x,int y,bool right){
    for(int p=0;p<16;++p)if(Box{28+p*71,110,65,42}.contains(x,y)){selected=p;if(right)sampleMenu();else play(p);return;}
    if(Box{44,226,1112,150}.contains(x,y)){
        auto e=engine.snapshot().edits[selected];double p=(x-52)/1096.;
        editorDrag=std::abs(p-e.start)<=std::abs(p-e.end)?6:7;
    }else for(int id=EditorVolume;id<=EditorDrive;++id)if(editorKnob(id).contains(x,y)){auto edit=engine.snapshot().edits[selected];editorDrag=id;editorDragStartY=y;editorDragStartRatio=editorRatio(edit,id);break;}
    if(editorDrag){SetCapture(window);if(editorDrag>=6)editorPointer(x,y);return;}
    if(Box{970,443,178,32}.contains(x,y)){auto e=engine.snapshot().edits[selected];e.bypass=!e.bypass;engine.setEdit(selected,e);}
    else if(Box{28,690,200,42}.contains(x,y))engine.setEdit(selected,{});
    else if(Box{244,690,180,42}.contains(x,y))sampleMenu();
    else if(Box{440,690,200,42}.contains(x,y))project(true);
    else if(Box{656,690,250,42}.contains(x,y))savePadToLibrary();
}
std::wstring decimal(double value,int precision=2){wchar_t buffer[64];std::swprintf(buffer,64,L"%.*f",precision,value);return buffer;}
void paintEditorKnob(HDC dc,int id,const std::wstring& name,const std::wstring& value,double ratio){
    auto b=editorKnob(id);ratio=std::clamp(ratio,0.,1.);text(dc,{b.x,b.y,b.w,22},name,13,ink,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    int cx=b.x+b.w/2,cy=b.y+69,r=35;HBRUSH brush=CreateSolidBrush(RGB(31,38,50));HPEN edge=CreatePen(PS_SOLID,2,line);auto oldB=SelectObject(dc,brush),oldP=SelectObject(dc,edge);Ellipse(dc,cx-r,cy-r,cx+r,cy+r);SelectObject(dc,oldB);SelectObject(dc,oldP);DeleteObject(brush);DeleteObject(edge);
    constexpr double pi=3.14159265358979323846;double angle=(135.+270.*ratio)*pi/180.;int x2=cx+int(std::cos(angle)*(r-9)),y2=cy+int(std::sin(angle)*(r-9));HPEN mark=CreatePen(PS_SOLID,4,accent);oldP=SelectObject(dc,mark);MoveToEx(dc,cx,cy,nullptr);LineTo(dc,x2,y2);SelectObject(dc,oldP);DeleteObject(mark);
    round(dc,{cx-4,cy-4,8,8},ink,ink);text(dc,{b.x,b.y+112,b.w,26},value,13,accent,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}
void paintEditor(HDC dc){
    auto s=engine.snapshot();const auto& edit=s.edits[selected];
    for(int p=0;p<16;++p)button(dc,{28+p*71,110,65,42},L"PAD "+padName(p),p==selected);
    round(dc,{28,174,1144,240},panel);
    text(dc,{48,184,810,30},L"PAD "+padName(selected)+L" / "+wide(s.names[selected]),20,padColor(selected),FW_SEMIBOLD);
    text(dc,{914,184,232,30},decimal(s.durations[selected],3)+L" s  ·  48 kHz",13,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
    const int a=52+int(edit.start*1096),b=52+int(edit.end*1096);
    fill(dc,{52,234,1096,132},RGB(17,22,30));fill(dc,{a,234,std::max(1,b-a),132},RGB(27,50,52));
    const auto& wave=s.waveforms[selected];
    if(wave.empty())text(dc,{70,260,1058,64},L"Charger un WAV pour régler sa zone de lecture",18,muted,FW_NORMAL,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    else for(int i=0;i<int(wave.size());++i){int x=52+i*1096/int(wave.size());int h=std::max(1,int(wave[i]*58));fill(dc,{x,300-h,3,h*2},x>=a&&x<b?padColor(selected):RGB(64,77,95));}
    fill(dc,{a,230,2,140},accent);fill(dc,{b-2,230,2,140},accent);
    round(dc,{a-7,228,14,12},accent,accent);round(dc,{b-7,360,14,12},accent,accent);
    if(s.active[selected]){int head=52+int(s.progress[selected]*1096);fill(dc,{head,236,1,128},ink);}
    text(dc,{52,377,510,25},L"DÉBUT  "+decimal(edit.start*s.durations[selected],3)+L" s  /  "+decimal(edit.start*100,1)+L" %",13,accent);
    text(dc,{650,377,498,25},L"FIN  "+decimal(edit.end*s.durations[selected],3)+L" s  /  "+decimal(edit.end*100,1)+L" %",13,accent,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
    round(dc,{28,430,560,236},panel);text(dc,{48,441,510,24},L"LECTURE DU PAD",15,ink,FW_SEMIBOLD);
    paintEditorKnob(dc,EditorVolume,L"Volume",decimal(edit.gain*100,0)+L" %",edit.gain/2);
    paintEditorKnob(dc,EditorTune,L"Accordage",(edit.semitones>=0?L"+":L"")+decimal(edit.semitones,0)+L" demi-tons",(edit.semitones+24)/48);
    paintEditorKnob(dc,EditorFade,L"Fondus début / fin",decimal(edit.fadeMs,1)+L" ms",edit.fadeMs/20);
    round(dc,{608,430,564,236},panel);text(dc,{628,441,320,24},L"EFFETS AIRWINDOWS",15,ink,FW_SEMIBOLD);
    button(dc,{970,443,178,32},edit.bypass?L"Bypass actif":L"Effets activés",!edit.bypass);
    paintEditorKnob(dc,EditorFilter,L"Lowpass · ouverture",edit.filterOpen>=.9999?L"Ouvert / bypass":decimal(edit.filterOpen*100,0)+L" %",edit.filterOpen);
    paintEditorKnob(dc,EditorDrive,L"PurestDrive · saturation",edit.drive<=.0001?L"Désactivée":decimal(edit.drive*100,0)+L" %",edit.drive);
    text(dc,{636,631,500,21},L"Filtre → saturation · réglages indépendants par pad",12,muted);
    button(dc,{28,690,200,42},L"Réinitialiser ce pad");button(dc,{244,690,180,42},L"Charger un WAV");
    button(dc,{440,690,200,42},L"Enregistrer projet");button(dc,{656,690,250,42},L"Ajouter à la bibliothèque");
    text(dc,{28,751,1144,32},L"Repères : glisser horizontalement · potentiomètres : glisser verticalement ou utiliser la molette.",13,muted);
}

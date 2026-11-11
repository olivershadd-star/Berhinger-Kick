// SPDX-License-Identifier: AGPL-3.0-or-later
// App member functions; coordinates share the same scalable canvas as the pads.
int editorDrag=0;
Box editorSlider(int id)const {
    if(id<=3)return {56,474+(id-1)*58,500,44};
    return {636,500+(id-4)*68,500,44};
}
void editorPointer(int x,int){
    auto s=engine.snapshot();auto edit=s.edits[selected];
    if(editorDrag==6||editorDrag==7){double p=std::clamp((x-52)/1096.,0.,1.);
        if(editorDrag==6)edit.start=std::min(p,edit.end-.001);else edit.end=std::max(p,edit.start+.001);
    }else if(editorDrag>=1&&editorDrag<=5){auto b=editorSlider(editorDrag);double p=std::clamp((x-b.x)/double(b.w),0.,1.);
        if(editorDrag==1)edit.gain=p*2;
        if(editorDrag==2)edit.semitones=std::round(p*48)-24;
        if(editorDrag==3)edit.fadeMs=p*20;
        if(editorDrag==4)edit.filterOpen=p;
        if(editorDrag==5)edit.drive=p;
    }else return;
    engine.setEdit(selected,edit);
}
void editorClick(int x,int y,bool right){
    for(int p=0;p<16;++p)if(Box{28+p*71,110,65,42}.contains(x,y)){selected=p;if(right)load(choose(false,false));else play(p);return;}
    if(Box{44,226,1112,150}.contains(x,y)){
        auto e=engine.snapshot().edits[selected];double p=(x-52)/1096.;
        editorDrag=std::abs(p-e.start)<=std::abs(p-e.end)?6:7;
    }else for(int id=1;id<=5;++id)if(editorSlider(id).contains(x,y)){editorDrag=id;break;}
    if(editorDrag){SetCapture(window);editorPointer(x,y);return;}
    if(Box{970,443,178,32}.contains(x,y)){auto e=engine.snapshot().edits[selected];e.bypass=!e.bypass;engine.setEdit(selected,e);}
    else if(Box{28,690,200,42}.contains(x,y))engine.setEdit(selected,{});
    else if(Box{244,690,150,42}.contains(x,y))play(selected);
    else if(Box{410,690,180,42}.contains(x,y))load(choose(false,false));
    else if(Box{606,690,130,42}.contains(x,y))engine.stop();
    else if(Box{752,690,180,42}.contains(x,y))project(true);
}
std::wstring decimal(double value,int precision=2){wchar_t buffer[64];std::swprintf(buffer,64,L"%.*f",precision,value);return buffer;}
void paintEditorSlider(HDC dc,int id,const std::wstring& name,const std::wstring& value,double ratio){
    auto b=editorSlider(id);text(dc,{b.x,b.y,b.w-150,23},name,14,ink,FW_SEMIBOLD);
    text(dc,{b.x+b.w-150,b.y,150,23},value,14,accent,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
    round(dc,{b.x,b.y+30,b.w,5},line,line);int pos=b.x+int(std::clamp(ratio,0.,1.)*b.w);
    fill(dc,{b.x,b.y+30,std::max(1,pos-b.x),5},accent);round(dc,{pos-5,b.y+25,10,15},ink,ink);
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
    paintEditorSlider(dc,1,L"Volume",decimal(edit.gain*100,0)+L" %",edit.gain/2);
    paintEditorSlider(dc,2,L"Accordage",(edit.semitones>=0?L"+":L"")+decimal(edit.semitones,0)+L" demi-tons",(edit.semitones+24)/48);
    paintEditorSlider(dc,3,L"Fondus début / fin",decimal(edit.fadeMs,1)+L" ms",edit.fadeMs/20);
    round(dc,{608,430,564,236},panel);text(dc,{628,441,320,24},L"EFFETS AIRWINDOWS",15,ink,FW_SEMIBOLD);
    button(dc,{970,443,178,32},edit.bypass?L"Bypass actif":L"Effets activés",!edit.bypass);
    paintEditorSlider(dc,4,L"Lowpass · ouverture",edit.filterOpen>=.9999?L"Ouvert / bypass":decimal(edit.filterOpen*100,0)+L" %",edit.filterOpen);
    paintEditorSlider(dc,5,L"PurestDrive · saturation",edit.drive<=.0001?L"Désactivée":decimal(edit.drive*100,0)+L" %",edit.drive);
    text(dc,{636,631,500,21},L"Filtre → saturation · réglages indépendants par pad",12,muted);
    button(dc,{28,690,200,42},L"Réinitialiser ce pad");button(dc,{244,690,150,42},L"Écouter le pad");
    button(dc,{410,690,180,42},L"Charger un WAV");button(dc,{606,690,130,42},L"Couper");button(dc,{752,690,180,42},L"Enregistrer");
    text(dc,{28,751,1144,32},L"Glisser les repères ou les curseurs. Découpe et accordage : au prochain déclenchement. Volume et effets : en direct.",13,muted);
}

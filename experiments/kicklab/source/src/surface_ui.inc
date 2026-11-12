// SPDX-License-Identifier: AGPL-3.0-or-later
// App member functions for the common KICK mouse/keyboard/MIDI surface.
const std::array<std::wstring,16> surfaceMenuLabels={
    L"Browser",L"Track Mixer",L"Track Mutes",L"Sequences",
    L"List Edit",L"Pad Mixer",L"Pad Mutes",L"Songs",
    L"Color Editor",L"Sampler",L"Sample Editor",L"Program Edit",
    L"Pad Notes",L"Looper",L"Q-Links",L"MIDI Control"
};

Box surfaceModuleBox(int display)const{return {28+display*580,68,552,318};}
Box surfaceDisplayBox(int display)const{return {40+display*580,80,456,294};}
Box surfaceDisplayItemBox(int display,int local)const{return {54+display*580+(local%2)*211,126+(local/2)*56,199,46};}
Box surfaceKnobBox(int display,int knob)const{return {514+display*580,88+knob*67,48,48};}
Box surfaceInsertSlotBox(int display,int slot)const{return {54+display*580+slot*104,127,96,34};}
Box surfaceInsertEffectBox(int display)const{return {54+display*580,174,250,36};}
Box surfaceInsertBypassBox(int display)const{return {314+display*580,174,136,36};}

int surfaceDragDisplay=-1,surfaceDragKnob=-1,surfaceDragStartY=0,surfaceDragLastStep=0;

void showSurfaceMenu(int display){
    display=std::clamp(display,0,1);
    surface.displays[display].module=lab::DisplayModule::Menu;
    const int other=1-display;
    if(surface.displays[other].module==lab::DisplayModule::Inserts)
        surface.setDisplayPage(other,0);
    surface.focusDisplay(display);
}

void showSurfaceInserts(int display){
    display=std::clamp(display,0,1);
    auto& binding=surface.displays[display];
    binding.module=lab::DisplayModule::Inserts;
    binding.pad=selected;
    binding.slot=std::clamp(selectedInsert,0,lab::InsertSlots-1);
    binding.page=0;
    surface.focusDisplay(display);
    status=L"Écran A — paramètres réels du slot d'effet sélectionné.";
}

void turnSurfaceQLink(int display,int knob,int delta){
    if(display!=0||knob<0||knob>=lab::SurfaceQLinks||!delta)return;
    auto& binding=surface.displays[display];
    if(binding.module!=lab::DisplayModule::Inserts)return;
    auto s=engine.snapshot();
    const int pad=std::clamp(binding.pad,0,lab::Pads-1),slot=std::clamp(binding.slot,0,lab::InsertSlots-1);
    const auto& insert=s.edits[pad].inserts[slot];
    const int parameter=binding.parameterForKnob(knob);
    if(parameter<0||parameter>=lab::insertParameterCount(insert.effect))return;
    engine.setInsertParameter(pad,slot,parameter,insert.parameters[parameter]+delta*.01);
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
    if(surface.displays[surface.focusedDisplay].module!=lab::DisplayModule::Menu)return;
    const int first=surface.focusedDisplay*8;
    int local=surface.menuSelections[surface.focusedDisplay];
    int row=local/2,col=local%2;
    col=std::clamp(col+dx,0,1);row=std::clamp(row+dy,0,3);
    selectSurfaceMenu(first+row*2+col);
}

void turnSurfaceMenu(int delta){
    if(!delta||surface.displays[surface.focusedDisplay].module!=lab::DisplayModule::Menu)return;
    const int first=surface.focusedDisplay*8,last=first+7;
    int item=first+surface.menuSelections[surface.focusedDisplay];
    selectSurfaceMenu(std::clamp(item+(delta>0?1:-1),first,last));
}

void activateSurfaceMenu(){
    if(surface.displays[surface.focusedDisplay].module!=lab::DisplayModule::Menu)return;
    const int item=surface.focusedDisplay*8+surface.menuSelections[surface.focusedDisplay];
    if(item==0){status=L"KICK / Browser — choix d'un sample pour le pad sélectionné.";sampleMenu();return;}
    if(item==9||item==10||item==11){tab=1;status=L"KICK / "+surfaceMenuLabels[item]+L" — éditeur transitoire, avant intégration complète dans les deux écrans.";return;}
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
        return true;
    }
    return false;
}

bool surfaceClick(int x,int y,bool right){
    for(int display=0;display<2;++display){
        for(int knob=0;knob<4;++knob)if(surfaceKnobBox(display,knob).contains(x,y)){
            surface.focusDisplay(display);
            if(!right&&display==0&&surface.displays[display].module==lab::DisplayModule::Inserts){
                surfaceDragDisplay=display;surfaceDragKnob=knob;surfaceDragStartY=y;surfaceDragLastStep=0;SetCapture(window);
            }
            return true;
        }
        if(!surfaceDisplayBox(display).contains(x,y))continue;
        surface.focusDisplay(display);
        auto& binding=surface.displays[display];
        if(binding.module==lab::DisplayModule::Inserts&&display==0){
            for(int slot=0;slot<lab::InsertSlots;++slot)if(surfaceInsertSlotBox(display,slot).contains(x,y)){
                binding.slot=slot;binding.page=0;selectedInsert=slot;return true;
            }
            if(surfaceInsertEffectBox(display).contains(x,y)){selectedInsert=binding.slot;insertMenu();return true;}
            if(surfaceInsertBypassBox(display).contains(x,y)){
                auto s=engine.snapshot();const auto& insert=s.edits[binding.pad].inserts[binding.slot];
                if(insert.effect!=lab::InsertEffect::None)engine.setInsertBypass(binding.pad,binding.slot,!insert.bypass);
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
    if(display==0&&binding.module==lab::DisplayModule::Inserts){
        const auto& insert=snapshot.edits[binding.pad].inserts[binding.slot];
        const int parameter=binding.parameterForKnob(knob);
        if(parameter>=0&&parameter<lab::insertParameterCount(insert.effect)){ratio=insert.parameters[parameter];assigned=true;}
    }
    HBRUSH brush=CreateSolidBrush(RGB(31,38,50));HPEN edge=CreatePen(PS_SOLID,2,line);
    auto oldB=SelectObject(dc,brush),oldP=SelectObject(dc,edge);Ellipse(dc,cx-19,cy-19,cx+19,cy+19);SelectObject(dc,oldB);SelectObject(dc,oldP);DeleteObject(brush);DeleteObject(edge);
    constexpr double pi=3.14159265358979323846;const double angle=(135.+270.*ratio)*pi/180.;
    HPEN mark=CreatePen(PS_SOLID,3,assigned?accent:muted);oldP=SelectObject(dc,mark);MoveToEx(dc,cx,cy,nullptr);LineTo(dc,cx+int(std::cos(angle)*12),cy+int(std::sin(angle)*12));SelectObject(dc,oldP);DeleteObject(mark);
    text(dc,{cx-24,cy+21,48,16},L"P"+number(display*4+knob+1),9,assigned?accent:muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

void paintSurfaceDisplay(HDC dc,int display,const lab::Snapshot& snapshot){
    auto module=surfaceModuleBox(display),box=surfaceDisplayBox(display);bool focused=surface.focusedDisplay==display;
    round(dc,module,panel,focused?accent:line);
    round(dc,box,RGB(9,18,22),focused?accent:RGB(34,48,57));
    const auto& binding=surface.displays[display];
    if(display==0&&binding.module==lab::DisplayModule::Inserts){
        const int pad=std::clamp(binding.pad,0,lab::Pads-1),slot=std::clamp(binding.slot,0,lab::InsertSlots-1);
        const auto& edit=snapshot.edits[pad];const auto& insert=edit.inserts[slot];const int count=lab::insertParameterCount(insert.effect);
        const int pages=std::max(1,(count+lab::SurfaceQLinks-1)/lab::SurfaceQLinks);
        text(dc,{box.x+14,box.y+9,300,24},L"ÉCRAN A  ·  INSERTS  ·  PAD "+padName(pad),12,focused?accent:muted,FW_SEMIBOLD);
        text(dc,{box.x+326,box.y+9,114,24},L"PAGE "+number(binding.page+1)+L" / "+number(pages),9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
        fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
        for(int s=0;s<lab::InsertSlots;++s)button(dc,surfaceInsertSlotBox(display,s),number(s+1)+L"  "+insertName(edit.inserts[s].effect),s==slot);
        button(dc,surfaceInsertEffectBox(display),insert.effect==lab::InsertEffect::None?L"Choisir un effet":L"Effet : "+insertName(insert.effect));
        button(dc,surfaceInsertBypassBox(display),insert.bypass?L"Slot bypass":L"Slot actif",insert.effect!=lab::InsertEffect::None&&!insert.bypass);
        for(int knob=0;knob<lab::SurfaceQLinks;++knob){
            const int parameter=binding.parameterForKnob(knob),y=222+knob*34;
            const bool valid=parameter>=0&&parameter<count;
            fill(dc,{box.x+14,y,box.w-28,28},knob%2?RGB(18,31,35):RGB(17,28,33));
            text(dc,{box.x+24,y,42,28},L"P"+number(knob+1),10,valid?accent:muted,FW_SEMIBOLD);
            text(dc,{box.x+68,y,220,28},valid?wide(lab::insertParameterName(insert.effect,parameter)):L"—",10,valid?ink:muted);
            text(dc,{box.x+300,y,126,28},valid?insertValue(insert.effect,parameter,insert.parameters[parameter]):L"Non assigné",10,valid?accent:muted,FW_SEMIBOLD,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
        }
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

void paintMasterKnob(HDC dc,double value){
    value=std::clamp(value,0.,1.);const int cx=248,cy=724,r=17;
    HBRUSH brush=CreateSolidBrush(RGB(31,38,50));HPEN edge=CreatePen(PS_SOLID,2,line);auto oldB=SelectObject(dc,brush),oldP=SelectObject(dc,edge);Ellipse(dc,cx-r,cy-r,cx+r,cy+r);SelectObject(dc,oldB);SelectObject(dc,oldP);DeleteObject(brush);DeleteObject(edge);
    constexpr double pi=3.14159265358979323846;double angle=(135.+270.*value)*pi/180.;HPEN mark=CreatePen(PS_SOLID,3,accent);oldP=SelectObject(dc,mark);MoveToEx(dc,cx,cy,nullptr);LineTo(dc,cx+int(std::cos(angle)*11),cy+int(std::sin(angle)*11));SelectObject(dc,oldP);DeleteObject(mark);
    text(dc,{208,744,80,14},L"MASTER "+number(int(value*100))+L" %",8,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

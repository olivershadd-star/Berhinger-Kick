// SPDX-License-Identifier: AGPL-3.0-or-later
// App member functions for the common KICK mouse/keyboard/MIDI surface.
const std::array<std::wstring,16> surfaceMenuLabels={
    L"Browser",L"Track Mixer",L"Track Mutes",L"Sequences",
    L"List Edit",L"Pad Mixer",L"Pad Mutes",L"Songs",
    L"Color Editor",L"Sampler",L"Sample Editor",L"Program Edit",
    L"Pad Notes",L"Looper",L"Q-Links",L"MIDI Control"
};

Box surfaceDisplayBox(int display)const{return {28+display*486,105,470,225};}
Box surfaceDisplayItemBox(int display,int local)const{return {46+display*486+(local%2)*218,151+(local/2)*36,208,30};}
Box surfaceMenuTileBox(int item)const{return {44+(item%4)*278,492+(item/4)*43,260,36};}
Box surfaceEncoderBox(int action)const{
    switch(action){
    case 0:return {1061,126,50,34};
    case 1:return {1013,166,46,34};
    case 2:return {1061,166,50,34};
    case 3:return {1113,166,46,34};
    case 4:return {1061,206,50,34};
    case 5:return {1013,250,68,30};
    case 6:return {1091,250,68,30};
    default:return {1013,289,146,27};
    }
}

void selectSurfaceMenu(int item){
    surface.menuSelection=std::clamp(item,0,15);
    surface.focusDisplay(surface.menuSelection/8);
}

void moveSurfaceMenu(int dx,int dy){
    int row=surface.menuSelection/4,col=surface.menuSelection%4;
    col=std::clamp(col+dx,0,3);row=std::clamp(row+dy,0,3);
    selectSurfaceMenu(row*4+col);
}

void turnSurfaceMenu(int delta){
    if(!delta)return;
    const int first=surface.focusedDisplay*8,last=first+7;
    int item=surface.menuSelection;
    if(item<first||item>last)item=first;
    selectSurfaceMenu(std::clamp(item+(delta>0?1:-1),first,last));
}

void activateSurfaceMenu(){
    status=L"KICK / "+surfaceMenuLabels[surface.menuSelection]+L" sélectionné — module original pas encore relié à cette base Windows.";
}

bool surfaceWheel(int x,int y,int delta){
    if(tab!=2)return false;
    for(int display=0;display<2;++display)if(surfaceDisplayBox(display).contains(x,y)){
        surface.focusDisplay(display);turnSurfaceMenu(delta);return true;
    }
    return false;
}

void surfaceClick(int x,int y){
    for(int display=0;display<2;++display){
        if(surfaceDisplayBox(display).contains(x,y))surface.focusDisplay(display);
        for(int local=0;local<8;++local)if(surfaceDisplayItemBox(display,local).contains(x,y)){
            selectSurfaceMenu(display*8+local);activateSurfaceMenu();return;
        }
    }
    for(int item=0;item<16;++item)if(surfaceMenuTileBox(item).contains(x,y)){
        selectSurfaceMenu(item);activateSurfaceMenu();return;
    }
    if(surfaceEncoderBox(0).contains(x,y)){dispatch({lab::SurfaceAction::CenterUp});return;}
    if(surfaceEncoderBox(1).contains(x,y)){dispatch({lab::SurfaceAction::CenterLeft});return;}
    if(surfaceEncoderBox(2).contains(x,y)){dispatch({lab::SurfaceAction::CenterPush});return;}
    if(surfaceEncoderBox(3).contains(x,y)){dispatch({lab::SurfaceAction::CenterRight});return;}
    if(surfaceEncoderBox(4).contains(x,y)){dispatch({lab::SurfaceAction::CenterDown});return;}
    if(surfaceEncoderBox(5).contains(x,y)){dispatch({lab::SurfaceAction::CenterTurn,0,-1});return;}
    if(surfaceEncoderBox(6).contains(x,y)){dispatch({lab::SurfaceAction::CenterTurn,0,1});return;}
    if(surfaceEncoderBox(7).contains(x,y)){dispatch({lab::SurfaceAction::FocusNext});return;}
}

void paintSurfaceKnob(HDC dc,int display,int knob){
    int x=48+display*486+knob*111,cx=x+43,cy=386;
    HBRUSH brush=CreateSolidBrush(RGB(31,38,50));HPEN edge=CreatePen(PS_SOLID,2,line);
    auto oldB=SelectObject(dc,brush),oldP=SelectObject(dc,edge);Ellipse(dc,cx-27,cy-27,cx+27,cy+27);SelectObject(dc,oldB);SelectObject(dc,oldP);DeleteObject(brush);DeleteObject(edge);
    HPEN mark=CreatePen(PS_SOLID,3,muted);oldP=SelectObject(dc,mark);MoveToEx(dc,cx,cy,nullptr);LineTo(dc,cx-15,cy-17);SelectObject(dc,oldP);DeleteObject(mark);
    text(dc,{x,339,86,20},L"Q"+number(display*4+knob+1),11,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    text(dc,{x,420,86,18},L"—",11,muted,FW_NORMAL,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

void paintSurfaceDisplay(HDC dc,int display){
    auto box=surfaceDisplayBox(display);bool focused=surface.focusedDisplay==display;
    round(dc,box,RGB(11,20,24),focused?accent:line);
    text(dc,{box.x+18,box.y+10,250,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  MENU KICK",13,focused?accent:muted,FW_SEMIBOLD);
    text(dc,{box.x+330,box.y+10,120,24},L"PAGE "+number(display+1)+L" / 2",11,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
    for(int local=0;local<8;++local){
        int item=display*8+local;auto b=surfaceDisplayItemBox(display,local);bool selected=item==surface.menuSelection;
        if(selected)round(dc,b,accent,accent);else fill(dc,b,local%2?RGB(18,31,35):RGB(17,28,33));
        text(dc,{b.x+8,b.y,b.w-16,b.h},number(item+1)+L"  "+surfaceMenuLabels[item],12,selected?bg:ink,selected?FW_SEMIBOLD:FW_NORMAL);
    }
}

void paintSurface(HDC dc){
    paintSurfaceDisplay(dc,0);paintSurfaceDisplay(dc,1);
    round(dc,{1000,105,172,225},panel,surface.focusedDisplay==0?padColor(0):padColor(12));
    text(dc,{1013,109,146,17},L"ENCODEUR / SOURIS",10,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    button(dc,surfaceEncoderBox(0),L"↑");button(dc,surfaceEncoderBox(1),L"←");button(dc,surfaceEncoderBox(2),L"OK");button(dc,surfaceEncoderBox(3),L"→");button(dc,surfaceEncoderBox(4),L"↓");
    button(dc,surfaceEncoderBox(5),L"↶");button(dc,surfaceEncoderBox(6),L"↷");button(dc,surfaceEncoderBox(7),surface.focusedDisplay?L"Focus : B":L"Focus : A",true);
    for(int display=0;display<2;++display)for(int knob=0;knob<4;++knob)paintSurfaceKnob(dc,display,knob);
    text(dc,{1000,350,172,76},L"Les Q-Links seront actifs sur les pages de paramètres.",11,muted,FW_NORMAL,DT_CENTER|DT_VCENTER|DT_WORDBREAK);
    round(dc,{28,458,1144,218},panel);text(dc,{44,466,720,22},L"MENU PRINCIPAL ORIGINAL · 16 FONCTIONS",14,ink,FW_SEMIBOLD);
    text(dc,{790,466,366,22},L"Souris · flèches · Entrée · Tab · pads MIDI",11,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
    for(int item=0;item<16;++item)button(dc,surfaceMenuTileBox(item),number(item+1)+L"  "+surfaceMenuLabels[item],item==surface.menuSelection);
    text(dc,{28,692,1144,40},L"Un seul mode KICK, deux pages visibles. Tab change l'écran actif ; la molette ou Haut/Bas déplace la sélection ; Entrée valide.",13,muted,FW_NORMAL,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    text(dc,{28,754,1144,30},status,12,muted);
}

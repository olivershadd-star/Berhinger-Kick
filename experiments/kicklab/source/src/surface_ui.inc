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

void selectSurfaceMenu(int item){
    surface.menuSelection=std::clamp(item,0,15);
    surface.focusDisplay(surface.menuSelection/8);
}

void moveSurfaceMenu(int dx,int dy){
    const int first=surface.focusedDisplay*8;
    int local=surface.menuSelection-first;
    if(local<0||local>7)local=0;
    int row=local/2,col=local%2;
    col=std::clamp(col+dx,0,1);row=std::clamp(row+dy,0,3);
    selectSurfaceMenu(first+row*2+col);
}

void turnSurfaceMenu(int delta){
    if(!delta)return;
    const int first=surface.focusedDisplay*8,last=first+7;
    int item=surface.menuSelection;
    if(item<first||item>last)item=first;
    selectSurfaceMenu(std::clamp(item+(delta>0?1:-1),first,last));
}

void activateSurfaceMenu(){
    const int item=surface.menuSelection;
    if(item==0){status=L"KICK / Browser — choix d'un sample pour le pad sélectionné.";sampleMenu();return;}
    if(item==9||item==10||item==11){tab=1;status=L"KICK / "+surfaceMenuLabels[item]+L" — éditeur transitoire, avant intégration complète dans les deux écrans.";return;}
    status=L"KICK / "+surfaceMenuLabels[item]+L" sélectionné — module original pas encore relié à cette base Windows.";
}

bool surfaceWheel(int x,int y,int delta){
    if(tab!=0)return false;
    for(int display=0;display<2;++display)if(surfaceDisplayBox(display).contains(x,y)){
        surface.focusDisplay(display);turnSurfaceMenu(delta);return true;
    }
    return false;
}

bool surfaceClick(int x,int y){
    for(int display=0;display<2;++display){
        if(surfaceDisplayBox(display).contains(x,y))surface.focusDisplay(display);
        for(int local=0;local<8;++local)if(surfaceDisplayItemBox(display,local).contains(x,y)){
            selectSurfaceMenu(display*8+local);activateSurfaceMenu();return true;
        }
    }
    return false;
}

void paintSurfaceKnob(HDC dc,int display,int knob){
    const int cx=538+display*580,cy=112+knob*67;
    HBRUSH brush=CreateSolidBrush(RGB(31,38,50));HPEN edge=CreatePen(PS_SOLID,2,line);
    auto oldB=SelectObject(dc,brush),oldP=SelectObject(dc,edge);Ellipse(dc,cx-19,cy-19,cx+19,cy+19);SelectObject(dc,oldB);SelectObject(dc,oldP);DeleteObject(brush);DeleteObject(edge);
    HPEN mark=CreatePen(PS_SOLID,3,accent);oldP=SelectObject(dc,mark);MoveToEx(dc,cx,cy,nullptr);LineTo(dc,cx-10,cy-12);SelectObject(dc,oldP);DeleteObject(mark);
    text(dc,{cx-24,cy+21,48,16},L"P"+number(display*4+knob+1),9,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

void paintSurfaceDisplay(HDC dc,int display){
    auto module=surfaceModuleBox(display),box=surfaceDisplayBox(display);bool focused=surface.focusedDisplay==display;
    round(dc,module,panel,focused?accent:line);
    round(dc,box,RGB(9,18,22),focused?accent:RGB(34,48,57));
    text(dc,{box.x+14,box.y+9,250,24},L"ÉCRAN "+std::wstring(display?L"B":L"A")+L"  ·  MENU KICK",12,focused?accent:muted,FW_SEMIBOLD);
    text(dc,{box.x+316,box.y+9,124,24},L"PAGE "+number(display+1)+L" / 2",9,muted,FW_NORMAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
    fill(dc,{box.x+14,box.y+39,box.w-28,1},RGB(34,48,57));
    for(int local=0;local<8;++local){
        int item=display*8+local;auto b=surfaceDisplayItemBox(display,local);bool selected=item==surface.menuSelection;
        if(selected)round(dc,b,accent,accent);else fill(dc,b,local%2?RGB(18,31,35):RGB(17,28,33));
        text(dc,{b.x+10,b.y,b.w-20,b.h},number(item+1)+L"  "+surfaceMenuLabels[item],11,selected?bg:ink,selected?FW_SEMIBOLD:FW_NORMAL);
    }
}

void paintSurface(HDC dc){
    paintSurfaceDisplay(dc,0);paintSurfaceDisplay(dc,1);
    for(int display=0;display<2;++display)for(int knob=0;knob<4;++knob)paintSurfaceKnob(dc,display,knob);
}

void paintMasterKnob(HDC dc,double value){
    value=std::clamp(value,0.,1.);const int cx=248,cy=724,r=17;
    HBRUSH brush=CreateSolidBrush(RGB(31,38,50));HPEN edge=CreatePen(PS_SOLID,2,line);auto oldB=SelectObject(dc,brush),oldP=SelectObject(dc,edge);Ellipse(dc,cx-r,cy-r,cx+r,cy+r);SelectObject(dc,oldB);SelectObject(dc,oldP);DeleteObject(brush);DeleteObject(edge);
    constexpr double pi=3.14159265358979323846;double angle=(135.+270.*value)*pi/180.;HPEN mark=CreatePen(PS_SOLID,3,accent);oldP=SelectObject(dc,mark);MoveToEx(dc,cx,cy,nullptr);LineTo(dc,cx+int(std::cos(angle)*11),cy+int(std::sin(angle)*11));SelectObject(dc,oldP);DeleteObject(mark);
    text(dc,{208,744,80,14},L"MASTER "+number(int(value*100))+L" %",8,muted,FW_SEMIBOLD,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

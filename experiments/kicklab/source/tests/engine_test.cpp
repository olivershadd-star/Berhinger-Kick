// SPDX-License-Identifier: AGPL-3.0-or-later
#include "engine.hpp"
#include "audio_queue.hpp"
#include "control_surface.hpp"
#include <thread>
#include <atomic>
#include <fstream>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <vector>
#include <cstdint>
#include <cstring>
#include <iomanip>
using namespace lab;
void check(bool b,const char* m){if(!b)throw std::runtime_error(m);}
void wav(const std::string& file,int rate,int channels,int frames,double frequency=440,double amplitude=8000){std::ofstream f(std::filesystem::u8path(file),std::ios::binary);auto u16=[&](unsigned n){char b[2]={char(n),char(n>>8)};f.write(b,2);};auto u32=[&](unsigned n){u16(n);u16(n>>16);};f.write("RIFF",4);u32(36+frames*channels*2);f.write("WAVEfmt ",8);u32(16);u16(1);u16(channels);u32(rate);u32(rate*channels*2);u16(channels*2);u16(16);f.write("data",4);u32(frames*channels*2);for(int n=0;n<frames;++n)for(int c=0;c<channels;++c)u16(static_cast<uint16_t>(int16_t(amplitude*std::sin(2*3.141592653589793*frequency*n/rate))));}
void rampWav(const std::string& file,int frames){std::ofstream f(std::filesystem::u8path(file),std::ios::binary);auto u16=[&](unsigned n){char b[2]={char(n),char(n>>8)};f.write(b,2);};auto u32=[&](unsigned n){u16(n);u16(n>>16);};f.write("RIFF",4);u32(36+frames*2);f.write("WAVEfmt ",8);u32(16);u16(1);u16(1);u32(48000);u32(96000);u16(2);u16(16);f.write("data",4);u32(frames*2);for(int n=0;n<frames;++n)u16(static_cast<uint16_t>(int16_t(100+29000.*n/std::max(1,frames-1))));}
void extensibleWav(const std::string& file,int rate,int channels,int frames){std::ofstream f(std::filesystem::u8path(file),std::ios::binary);auto u16=[&](unsigned n){char b[2]={char(n),char(n>>8)};f.write(b,2);};auto u32=[&](unsigned n){u16(n);u16(n>>16);};const unsigned bytes=frames*channels*2;f.write("RIFF",4);u32(60+bytes);f.write("WAVEfmt ",8);u32(40);u16(0xfffe);u16(channels);u32(rate);u32(rate*channels*2);u16(channels*2);u16(16);u16(22);u16(16);u32(channels==1?4:3);u32(1);const char tail[12]={0,0,16,0,char(128),0,0,char(170),0,56,char(155),113};f.write(tail,12);f.write("data",4);u32(bytes);for(int n=0;n<frames;++n)for(int c=0;c<channels;++c)u16(static_cast<uint16_t>(int16_t(8000*std::sin(2*3.141592653589793*220*n/rate))));}
int main(int argc,char**argv){try{
    SurfaceState surface;
    check(surface.displays[0].parameterForKnob(3)==3,"left display Q-Link mapping failed");
    surface.setDisplayPage(1,1);
    check(surface.displays[1].parameterForKnob(0)==4,"right display page mapping failed");
    surface.selectPad(11);check(surface.selectedPad==11&&surface.displays[0].pad==11&&surface.displays[1].pad==11,"display did not follow selected pad");
    surface.displays[1].followsSelectedPad=false;surface.displays[1].pad=3;surface.selectPad(5);check(surface.displays[0].pad==5&&surface.displays[1].pad==3,"independent display binding failed");
    surface.setDisplayPage(0,2);check(surface.displays[0].parameterForKnob(1)==9,"four-knob page mapping failed");
    surface.focusDisplay(1);check(surface.focusedDisplay==1,"right display focus failed");surface.focusDisplay(-4);check(surface.focusedDisplay==0,"display focus clamp failed");
    surface.menuSelections[0]=3;surface.menuSelections[1]=6;surface.focusDisplay(1);surface.focusDisplay(0);
    check(surface.menuSelections[0]==3&&surface.menuSelections[1]==6,"display menu cursors are not independent");
    check(midiCcDelta(MidiCcMode::Relative1,1)==1&&midiCcDelta(MidiCcMode::Relative1,127)==-1,"relative 1/127 MIDI decoding failed");
    check(midiCcDelta(MidiCcMode::Relative65,65)==1&&midiCcDelta(MidiCcMode::Relative65,63)==-1,"relative 63/65 MIDI decoding failed");
    check(midiAbsoluteDelta(-1,64)==0&&midiAbsoluteDelta(64,65)==1&&midiAbsoluteDelta(65,63)==-2,"absolute 4D wheel decoding failed");
    check(midiEncoderDelta(MidiCcMode::Absolute,-1,0)==0&&midiEncoderDelta(MidiCcMode::Absolute,0,1)==1&&midiEncoderDelta(MidiCcMode::Absolute,9,8)==-1,"absolute encoder direction failed");
    check(midiEncoderDelta(MidiCcMode::Absolute,127,127)==1&&midiEncoderDelta(MidiCcMode::Absolute,0,0)==-1,"absolute encoder stopped at an endpoint");
    check(midiEncoderDelta(MidiCcMode::Relative1,-1,1)==1&&midiEncoderDelta(MidiCcMode::Relative1,1,127)==-1,"1/127 encoder direction failed");
    check(midiEncoderDelta(MidiCcMode::Relative1,-1,65)==1&&midiEncoderDelta(MidiCcMode::Relative1,65,63)==-1,"65/63 encoder direction failed");
    MidiPadBinding padBinding{0,12};check(padBinding.matches(0,12)&&!padBinding.matches(0,13)&&!padBinding.matches(1,12),"learned pad matching failed");
    auto dir=std::filesystem::absolute(argc>1?argv[1]:"test-output");std::filesystem::create_directories(dir);
    auto path=[&](const char* n){return (dir/n).u8string();};
    wav(path("mono.wav"),48000,1,300);wav(path("resample.wav"),44100,2,4410);wav(path("long.wav"),48000,2,48000);wav(path("layer-low.wav"),48000,1,4800,220,1000);wav(path("layer-high.wav"),48000,1,4800,220,10000);rampWav(path("ramp.wav"),4800);wav(path("Bass_Loop_130BPM.wav"),48000,2,int(std::llround(48000.*240./130.)));std::filesystem::create_directories(dir/"packs"/"industrial"/"loops");auto nested=(dir/"packs"/"industrial"/"loops"/"extensible.wav").u8string();extensibleWav(nested,48000,2,4800);
    Engine e;float block[Block*2];std::string error;
    e.render(block);for(float v:block)check(v==0,"empty engine is not silent");
    check(e.preview(path("mono.wav"),error),error.c_str());e.render(block);double previewEnergy=0;for(float value:block)previewEnergy+=value*value;
    check(previewEnergy>1e-5,"browser preview is silent");auto previewState=e.snapshot();check(previewState.paths[0].empty(),"browser preview replaced a pad sample");
    e.stopPreview();e.render(block);for(float value:block)check(value==0,"browser preview did not stop");
    check(e.load(0,path("mono.wav"),error),error.c_str());e.trigger(0);e.render(block);
    // The short clip now has 2 ms fades plus the engine startup ramp.
    double energy=0;for(int n=0;n<Block;++n){check(block[n*2]==block[n*2+1],"mono is not duplicated");energy+=block[n*2]*block[n*2];}check(energy>1e-5,"playback is silent with short fade");
    e.render(block);check(block[43*2]==0,"end fade is not silent at boundary");for(int n=44;n<Block;++n)check(block[n*2]==0&&block[n*2+1]==0,"stale samples after EOF");e.render(block);for(float v:block)check(v==0,"one-shot does not stop");
    check(e.load(1,path("resample.wav"),error),error.c_str());e.trigger(1);int active=0;for(int b=0;b<24;++b){e.render(block);for(int n=0;n<Block;++n)if(std::abs(block[n*2])>.00001)++active;}check(active>4500&&active<4801,"44.1 kHz conversion duration incorrect");
    check(e.load(2,nested,error),error.c_str());check(e.snapshot().paths[2]==nested,"nested WAV Extensible path was not retained");e.trigger(2);e.render(block);double nestedEnergy=0;for(float value:block)nestedEnergy+=value*value;check(nestedEnergy>1e-5,"nested WAV Extensible file is silent");e.stop();
    check(e.load(0,path("long.wav"),error),error.c_str());e.stop();e.trigger(0);for(int b=0;b<200;++b)e.render(block);check(!e.snapshot().active[0],"manual pad audition looped after WAV end");
    // A named loop keeps its source tempo and follows project BPM by playback
    // speed. A 130 BPM one-bar loop must last one 120 BPM bar after sync.
    Engine synced;check(synced.load(0,path("Bass_Loop_130BPM.wav"),error),error.c_str());auto loopEdit=synced.snapshot().edits[0];check(loopEdit.loopSync&&std::abs(loopEdit.sourceBpm-130)<.01,"loop BPM was not detected");synced.setTempo(120);synced.trigger(0);int syncBlocks=0;do{synced.render(block);++syncBlocks;}while(synced.snapshot().active[0]&&syncBlocks<500);check(syncBlocks>=374&&syncBlocks<=376,"loop did not stretch to the project tempo");
    check(synced.save(path("loop-sync.kicklab"),error),error.c_str());loopEdit.loopSync=false;loopEdit.sourceBpm=90;synced.setEdit(0,loopEdit);check(synced.open(path("loop-sync.kicklab"),error),error.c_str());check(synced.snapshot().edits[0].loopSync&&std::abs(synced.snapshot().edits[0].sourceBpm-130)<.01,"KICKLAB 10 did not retain loop sync");
    // The original KICK Drum Program model: four velocity/cycle/random layers,
    // three sample play types, choke groups, reverse and pad colours.
    {Engine drum;check(drum.load(0,path("layer-low.wav"),error),error.c_str());check(drum.loadLayer(0,1,path("layer-high.wav"),error),error.c_str());drum.setLayerVelocityRange(0,0,1,63);drum.setLayerVelocityRange(0,1,64,127);PadEdit drumEdit=drum.snapshot().edits[0];drumEdit.fadeMs=0;drumEdit.layerPlay=LayerPlayType::Velocity;drumEdit.color=9;drum.setEdit(0,drumEdit);
    drum.triggerVelocity(0,0,20);drum.render(block);double layerLowEnergy=0;for(float value:block)layerLowEnergy+=value*value;drum.stop();drum.triggerVelocity(0,0,110);drum.render(block);double layerHighEnergy=0;for(float value:block)layerHighEnergy+=value*value;check(layerHighEnergy>layerLowEnergy*20,"velocity layer selection did not choose the high layer");
    drumEdit.layerPlay=LayerPlayType::Cycle;drum.setEdit(0,drumEdit);drum.stop();drum.triggerVelocity(0,0,100);drum.render(block);double cycleA=0;for(float value:block)cycleA+=value*value;drum.stop();drum.triggerVelocity(0,0,100);drum.render(block);double cycleB=0;for(float value:block)cycleB+=value*value;check(std::max(cycleA,cycleB)>std::min(cycleA,cycleB)*20,"cycle layer mode did not alternate layers");
    check(drum.save(path("drum-v11.kicklab"),error),error.c_str());}
    {Engine restoredDrum;check(restoredDrum.open(path("drum-v11.kicklab"),error),error.c_str());auto drumRestored=restoredDrum.snapshot();check(drumRestored.layers[0][0].path==path("layer-low.wav")&&drumRestored.layers[0][1].path==path("layer-high.wav")&&drumRestored.layers[0][0].velocityEnd==63&&drumRestored.layers[0][1].velocityStart==64&&drumRestored.edits[0].layerPlay==LayerPlayType::Cycle&&drumRestored.edits[0].color==9,"KICKLAB 11 did not retain Drum Program layers and behaviour");}
    {Engine reverseEngine;check(reverseEngine.load(0,path("ramp.wav"),error),error.c_str());auto reverseEdit=reverseEngine.snapshot().edits[0];reverseEdit.fadeMs=0;reverseEngine.setEdit(0,reverseEdit);reverseEngine.trigger(0);reverseEngine.render(block);double forwardEnergy=0;for(float value:block)forwardEnergy+=value*value;reverseEngine.stop();reverseEngine.setLayerReverse(0,0,true);reverseEngine.trigger(0);reverseEngine.render(block);double backwardEnergy=0;for(float value:block)backwardEnergy+=value*value;check(backwardEnergy>forwardEnergy*20,"reverse layer playback did not start at the sample end");}
    {Engine playTypes;check(playTypes.load(0,path("long.wav"),error),error.c_str());auto playEdit=playTypes.snapshot().edits[0];playEdit.fadeMs=0;playEdit.samplePlay=SamplePlayType::OneShot;playTypes.setEdit(0,playEdit);playTypes.trigger(0);playTypes.render(block);playTypes.release(0);playTypes.render(block);check(playTypes.snapshot().active[0],"one-shot incorrectly stopped on note-off");playTypes.stop();playEdit.samplePlay=SamplePlayType::NoteOn;playTypes.setEdit(0,playEdit);playTypes.trigger(0);playTypes.render(block);playTypes.release(0);playTypes.render(block);check(!playTypes.snapshot().active[0],"note-on mode ignored note-off");playEdit.samplePlay=SamplePlayType::NoteOff;playTypes.setEdit(0,playEdit);playTypes.trigger(0);playTypes.render(block);double beforeRelease=0;for(float value:block)beforeRelease+=value*value;playTypes.release(0);playTypes.render(block);double afterRelease=0;for(float value:block)afterRelease+=value*value;check(beforeRelease<1e-10&&afterRelease>1e-5,"note-off mode did not trigger on release");}
    {Engine choke;check(choke.load(0,path("long.wav"),error),error.c_str());check(choke.load(1,path("long.wav"),error),error.c_str());auto choke0=choke.snapshot().edits[0],choke1=choke.snapshot().edits[1];choke0.chokeGroup=2;choke1.chokeGroup=2;choke.setEdit(0,choke0);choke.setEdit(1,choke1);choke.trigger(0);choke.render(block);choke.trigger(1);choke.render(block);check(!choke.snapshot().active[0]&&choke.snapshot().active[1],"choke group did not stop the previous voice");}
    // Note Repeat is generated in the audio clock, not by the UI timer.
    Engine repeated;check(repeated.load(0,path("mono.wav"),error),error.c_str());repeated.setTempo(120);repeated.setRepeatDivision(16);repeated.setNoteRepeat(true);repeated.setRepeatVoice(0,0,true);int energeticBlocks=0;for(int b=0;b<55;++b){repeated.render(block);double repeatedEnergy=0;for(float value:block)repeatedEnergy+=value*value;if(repeatedEnergy>1e-5)++energeticBlocks;}check(energeticBlocks>=3,"Note Repeat did not retrigger from the audio clock");repeated.stop();check(!repeated.snapshot().noteRepeat,"STOP did not clear Note Repeat");
    PadEdit preserved;preserved.gain=.37;preserved.semitones=-5;preserved.inserts[2]=defaultInsertState(InsertEffect::Chorus);e.setEdit(0,preserved);
    check(e.load(0,path("mono.wav"),error,true),error.c_str());auto preservedState=e.snapshot().edits[0];
    check(preservedState.gain==.37&&preservedState.semitones==-5&&preservedState.inserts[2].effect==InsertEffect::Chorus,"confirmed browser load reset pad editing");
    e.trigger(0);e.setVolume(0);e.render(block);for(float v:block)check(v==0,"master mute failed");e.setVolume(.65);e.stop();
    for(int p=0;p<Pads;++p)check(e.load(p,path("long.wav"),error),error.c_str());for(int p=0;p<Pads;++p)e.trigger(p);e.render(block);for(float v:block)check(std::isfinite(v)&&std::abs(v)<=MasterCeiling+1e-6f,"master limiter exceeded its -1 dBFS ceiling");auto gainStage=e.snapshot();check(gainStage.preLimiterPeak>MasterCeiling&&gainStage.peak<=MasterCeiling+1e-6f&&gainStage.limiterReductionDb>0&&gainStage.trackPeaks[1]>0,"master gain staging or per-track meter did not detect the summed overload");
    e.stop();e.demoPattern();e.setTempo(120);e.setPlaying(true);e.render(block);e.setPlaying(false);int paused=e.snapshot().step;for(int b=0;b<30;++b)e.render(block);check(e.snapshot().step==paused,"pause advances sequencer");e.stop();e.setPlaying(true);std::ofstream raw(std::filesystem::u8path(path("demo.f32")),std::ios::binary);
    int transitions=0,last=-1;double sum=0;for(int b=0;b<375;++b){e.render(block);raw.write(reinterpret_cast<const char*>(block),sizeof(block));for(float v:block)sum+=v*v;int s=e.snapshot().step;if(s!=last){++transitions;last=s;}}check(transitions==16,"sequencer timing failed");check(sum>1,"sequencer is silent");e.stop();
    // Export renders the active pattern through the real engine at an exact
    // musical length. One 4/4 bar at 120 BPM is 96,000 stereo frames.
    const auto exportedPath=path("pattern-export.wav");check(e.exportCurrentPatternWav(exportedPath,error),error.c_str());
    std::ifstream exported(std::filesystem::u8path(exportedPath),std::ios::binary|std::ios::ate);check(bool(exported),"exported WAV was not created");
    check(uint64_t(exported.tellg())==44ull+96000ull*6ull,"exported WAV duration or 24-bit frame size is incorrect");exported.seekg(0);std::vector<char> exportedBytes(size_t(44+96000*6));exported.read(exportedBytes.data(),std::streamsize(exportedBytes.size()));
    check(!std::memcmp(exportedBytes.data(),"RIFF",4)&&!std::memcmp(exportedBytes.data()+8,"WAVE",4)&&!std::memcmp(exportedBytes.data()+36,"data",4),"exported WAV header is invalid");bool exportedSignal=false;int32_t exportedPeak=0;for(size_t byte=44;byte+2<exportedBytes.size();byte+=3){exportedSignal=exportedSignal||exportedBytes[byte]!=0||exportedBytes[byte+1]!=0||exportedBytes[byte+2]!=0;int32_t sample=uint8_t(exportedBytes[byte])|(int32_t(uint8_t(exportedBytes[byte+1]))<<8)|(int32_t(uint8_t(exportedBytes[byte+2]))<<16);if(sample&0x800000)sample|=~0xffffff;exportedPeak=std::max(exportedPeak,std::abs(sample));}check(exportedSignal,"exported WAV is silent");check(exportedPeak<=int32_t(std::ceil(MasterCeiling*8388607.)),"exported 24-bit WAV exceeded the -1 dBFS ceiling");check(!e.snapshot().playing,"WAV export left transport running");
    // KICK Pad Perform / Keygroup: one sample can be played and sequenced chromatically.
    e.selectSequence(0);e.setSequenceBars(2);e.setViewBar(1);e.setPerformanceMode(PerformanceMode::Chromatic);e.setMelodicSourcePad(12);e.setRootNote(48);e.toggle(7,3);
    auto arrangement=e.snapshot();
    check(arrangement.sequences[0].bars==2&&arrangement.sequences[0].mode==PerformanceMode::Chromatic,"Pad Perform mode was not stored");
    check(arrangement.sequences[0].melody[19]==55&&arrangement.melody[3]==55,"melodic step did not retain its note");
    check(e.performanceNote(7)==55,"chromatic pad mapping is incorrect");
    e.setPerformanceMode(PerformanceMode::Major);check(e.performanceNote(7)==60,"major scale pad mapping is incorrect");e.setPerformanceMode(PerformanceMode::Chromatic);
    // Live REC writes the running playhead to the selected quantized grid.
    e.stop();e.selectSequence(15);e.clearPattern();e.setSequenceBars(1);e.setPerformanceMode(PerformanceMode::Drum);e.setPlaying(true);e.render(block);
    check(e.recordHit(4,-1,16,83),"quantized drum recording was rejected");check(e.snapshot().sequences[15].drum[4][0]&&e.snapshot().sequences[15].drumEvents[4][0].velocity==83,"quantized drum hit or velocity was not written");
    e.setPerformanceMode(PerformanceMode::Chromatic);e.setMelodicSourcePad(12);e.setRootNote(48);check(e.recordHit(12,55,16,61),"quantized melodic recording was rejected");check(e.snapshot().sequences[15].melody[0]==55&&e.snapshot().sequences[15].melodyEvents[0].velocity==61,"quantized melodic note or velocity was not written");e.stop();
    // Original KICK Events metadata: every note stores velocity, probability,
    // ratchet and length. Probability participates in playback and all four
    // values survive a KICKLAB 12 round trip.
    {Engine events;check(events.load(0,path("long.wav"),error),error.c_str());events.toggle(0,0);StepEvent event{72,0,4,3};events.setStepEvent(0,0,event);events.setPlaying(true);events.render(block);check(!events.snapshot().active[0],"zero-probability event was triggered");events.stop();event.probability=100;events.setStepEvent(0,0,event);events.setPlaying(true);events.render(block);check(events.snapshot().active[0],"enabled event was not triggered");events.stop();events.setPerformanceMode(PerformanceMode::Chromatic);events.setMelodicSourcePad(0);events.toggle(5,3);events.setStepEvent(0,3,StepEvent{49,64,2,6},true);check(events.save(path("events-v12.kicklab"),error),error.c_str());}
    {Engine restoredEvents;check(restoredEvents.open(path("events-v12.kicklab"),error),error.c_str());const auto restored=restoredEvents.snapshot();const auto drumEvent=restored.sequences[0].drumEvents[0][0],melodicEvent=restored.sequences[0].melodyEvents[3];check(drumEvent.velocity==72&&drumEvent.probability==100&&drumEvent.ratchet==4&&drumEvent.length==3,"KICKLAB 12 did not retain drum Events");check(melodicEvent.velocity==49&&melodicEvent.probability==64&&melodicEvent.ratchet==2&&melodicEvent.length==6,"KICKLAB 12 did not retain melodic Events");}
    // KICK List Edit inserts, moves and removes the same note data exposed by
    // Events. Moves preserve all note properties and reject occupied targets.
    {Engine listEdit;check(listEdit.insertStepEvent(3,2),"List Edit could not insert a drum event");StepEvent listed{91,73,3,5};listEdit.setStepEvent(3,2,listed);check(listEdit.moveStepEvent(3,2,7,9),"List Edit could not move a drum event");auto state=listEdit.snapshot();check(!state.sequences[0].drum[3][2]&&state.sequences[0].drum[7][9],"List Edit drum move left the source active or lost the destination");const auto moved=state.sequences[0].drumEvents[7][9];check(moved.velocity==91&&moved.probability==73&&moved.ratchet==3&&moved.length==5,"List Edit drum move lost event properties");check(listEdit.insertStepEvent(4,10)&&!listEdit.moveStepEvent(7,9,4,10),"List Edit overwrote an occupied drum destination");check(listEdit.deleteStepEvent(7,9),"List Edit could not delete a drum event");listEdit.setPerformanceMode(PerformanceMode::Chromatic);check(listEdit.insertStepEvent(64,5,true),"List Edit could not insert a melodic event");listEdit.setStepEvent(0,5,StepEvent{66,88,2,7},true);check(listEdit.moveStepEvent(0,5,69,11,true),"List Edit could not move a melodic event");state=listEdit.snapshot();check(state.sequences[0].melody[5]<0&&state.sequences[0].melody[11]==69&&state.sequences[0].melodyEvents[11].length==7,"List Edit melodic move lost the note or metadata");check(listEdit.deleteStepEvent(0,11,true),"List Edit could not delete a melodic event");}
    // Next Sequence waits for the complete sequence boundary.
    e.selectSequence(1);e.clearPattern();e.setSequenceBars(1);e.setPerformanceMode(PerformanceMode::Drum);e.toggle(0,0);
    e.selectSequence(2);e.clearPattern();e.setSequenceBars(1);e.setPerformanceMode(PerformanceMode::Drum);e.toggle(1,0);
    e.selectSequence(1);e.setPlaying(true);e.queueSequence(2);for(int b=0;b<400;++b)e.render(block);
    arrangement=e.snapshot();check(arrangement.sequence==2&&arrangement.queuedSequence==-1,"queued sequence did not switch at the boundary");e.stop();
    // Song chains complete sequences and honours repetitions.
    e.clearSong();e.addSongStep(1,1);e.addSongStep(2,2);e.playSong();for(int b=0;b<400;++b)e.render(block);
    arrangement=e.snapshot();check(arrangement.songPlaying&&arrangement.songPosition==1&&arrangement.sequence==2,"Song did not advance to its second step");
    for(int b=0;b<400;++b)e.render(block);arrangement=e.snapshot();check(arrangement.songPosition==1&&arrangement.songRepeat==1,"Song repetition counter is incorrect");
    for(int b=0;b<400;++b)e.render(block);arrangement=e.snapshot();check(!arrangement.playing&&!arrangement.songPlaying&&arrangement.songPosition==-1,"Song did not stop after its arrangement");
    // KICKLAB 5 keeps all sequences, melodic notes and Song data.
    e.selectSequence(0);e.setViewBar(1);
    check(e.save(path("pattern.kicklab"),error),error.c_str());e.clearPattern();check(e.open(path("pattern.kicklab"),error),error.c_str());e.setViewBar(0);check(e.snapshot().pattern[0][0]&&e.snapshot().pattern[1][4],"pattern roundtrip failed");
    arrangement=e.snapshot();
    check(arrangement.sequences[0].bars==2&&arrangement.sequences[0].mode==PerformanceMode::Chromatic&&arrangement.sequences[0].sourcePad==12&&arrangement.sequences[0].rootNote==48,"sequence settings roundtrip failed");
    check(arrangement.sequences[0].melody[19]==55&&arrangement.sequences[1].drum[0][0]&&arrangement.sequences[2].drum[1][0],"multi-sequence roundtrip failed");
    check(arrangement.songLength==2&&arrangement.song[0].sequence==1&&arrangement.song[0].repeats==1&&arrangement.song[1].sequence==2&&arrangement.song[1].repeats==2,"Song roundtrip failed");
    // A-H are complete linked blocks: each one recalls an independent kit and
    // pattern, and a running change waits for the current pattern boundary.
    Engine linked;check(linked.load(0,path("long.wav"),error),error.c_str());linked.toggle(0,0);linked.setGroup(1);check(linked.snapshot().group==1&&linked.snapshot().sequence==1&&!linked.snapshot().pattern[0][0],"block B reused block A steps");check(linked.load(1,path("mono.wav"),error),error.c_str());linked.toggle(1,4);linked.setGroup(0);check(linked.snapshot().group==0&&linked.snapshot().sequence==0&&linked.snapshot().pattern[0][0]&&!linked.snapshot().pattern[1][4]&&!linked.snapshot().paths[0].empty()&&linked.snapshot().paths[1].empty(),"block A kit or pattern was not restored");
    linked.setPlaying(true);linked.setGroup(1);auto queuedBlock=linked.snapshot();check(queuedBlock.playing&&queuedBlock.group==0&&queuedBlock.queuedGroup==1&&queuedBlock.queuedSequence==1,"running A-H change stopped or switched early");for(int b=0;b<400;++b)linked.render(block);auto switchedBlock=linked.snapshot();check(switchedBlock.playing&&switchedBlock.group==1&&switchedBlock.sequence==1&&switchedBlock.queuedGroup==-1&&switchedBlock.pattern[1][4]&&switchedBlock.paths[0].empty()&&!switchedBlock.paths[1].empty(),"queued A-H kit/pattern did not switch at the boundary");linked.stop();linked.copySequence(1,2);linked.setGroup(2);check(linked.snapshot().pattern[1][4],"Duplicate pattern copy failed");
    // KICKLAB 10 separates reusable Programs from patterns. Copying a pattern
    // preserves the destination Program, while assigning a Program makes its
    // kit immediately reusable by another pattern.
    Engine programs;check(programs.load(0,path("long.wav"),error),error.c_str());programs.toggle(0,0);programs.setProgramColorModes(EmptyPadColorMode::Dim,PadOffColorMode::Off,PadOnColorMode::Full);const int copiedProgram=programs.duplicateProgram(0,"Industrial Copy");check(copiedProgram==8,"Program duplication did not use the first free slot");check(programs.snapshot().programs[copiedProgram].emptyPadColor==EmptyPadColorMode::Dim&&programs.snapshot().programs[copiedProgram].padOffColor==PadOffColorMode::Off&&programs.snapshot().programs[copiedProgram].padOnColor==PadOnColorMode::Full,"Program duplication lost Color Editor modes");check(programs.assignProgramToSequence(1,copiedProgram),"Program could not be assigned to pattern B");programs.setGroup(1);auto programB=programs.snapshot();check(programB.program==copiedProgram&&programB.paths[0]==path("long.wav")&&!programB.pattern[0][0],"pattern B did not recall its assigned Program independently");programs.copySequence(0,1);programs.setGroup(1);programB=programs.snapshot();check(programB.sequences[1].program==copiedProgram&&programB.pattern[0][0],"copying a pattern also replaced its Program link");check(!programs.deleteProgram(copiedProgram),"a referenced Program was deleted");const int emptyProgram=programs.createProgram("Temporary");check(emptyProgram==9&&programs.renameProgram(emptyProgram,"Temporary Renamed")&&programs.deleteProgram(emptyProgram),"Program create/rename/delete lifecycle failed");check(programs.save(path("programs-v14.kicklab"),error),error.c_str());programs.resetProject();check(programs.open(path("programs-v14.kicklab"),error),error.c_str());auto restoredProgram=programs.snapshot();check(restoredProgram.program==copiedProgram&&restoredProgram.sequences[1].program==copiedProgram&&restoredProgram.programs[copiedProgram].used&&restoredProgram.programs[copiedProgram].name=="Industrial Copy"&&restoredProgram.paths[0]==path("long.wav")&&restoredProgram.programs[copiedProgram].emptyPadColor==EmptyPadColorMode::Dim&&restoredProgram.programs[copiedProgram].padOffColor==PadOffColorMode::Off&&restoredProgram.programs[copiedProgram].padOnColor==PadOnColorMode::Full,"KICKLAB 14 Program metadata, Color Editor, link or kit did not roundtrip");
    // The final KICK edit workflow copies or deletes complete Program pads and
    // erases only the requested sequence/track/time range.
    {Engine editOps;check(editOps.load(2,path("layer-low.wav"),error),error.c_str());check(editOps.loadLayer(2,1,path("layer-high.wav"),error),error.c_str());editOps.setLayerVelocityRange(2,0,1,63);editOps.setLayerVelocityRange(2,1,64,127);auto sourceEdit=editOps.snapshot().edits[2];sourceEdit.gain=.42;sourceEdit.color=10;sourceEdit.inserts[2]=defaultInsertState(InsertEffect::Fracture);editOps.setEdit(2,sourceEdit);check(editOps.copyProgramPad(2,7,error),error.c_str());auto copied=editOps.snapshot();check(copied.layers[7][0].path==path("layer-low.wav")&&copied.layers[7][1].path==path("layer-high.wav")&&copied.layers[7][0].velocityEnd==63&&copied.layers[7][1].velocityStart==64&&copied.edits[7].gain==.42&&copied.edits[7].color==10&&copied.edits[7].inserts[2].effect==InsertEffect::Fracture,"Copy Pad lost layers or Program pad settings");sourceEdit.color=1;editOps.setEdit(2,sourceEdit);check(editOps.snapshot().edits[7].color==10,"copied pad still shared source settings");check(editOps.deleteProgramPad(7),"Delete Pad rejected a populated destination");auto deleted=editOps.snapshot();check(deleted.paths[7].empty()&&!deleted.layers[7][0].loaded&&!deleted.layers[7][1].loaded&&deleted.names[7]=="Vide"&&deleted.edits[7].gain==1&&deleted.edits[7].color==3&&deleted.edits[7].inserts[2].effect==InsertEffect::None,"Delete Pad retained sample data or Program pad settings");editOps.setSequenceBars(2);editOps.setPerformanceMode(PerformanceMode::Drum);editOps.insertStepEvent(4,1,false);editOps.insertStepEvent(4,8,false);editOps.insertStepEvent(4,20,false);editOps.setStepEvent(4,8,StepEvent{75,84,3,6});check(editOps.eraseNotes(0,4,4,15)==1,"Erase Notes removed the wrong drum event count");auto erased=editOps.snapshot();check(erased.sequences[0].drum[4][1]&&!erased.sequences[0].drum[4][8]&&erased.sequences[0].drum[4][20]&&erased.sequences[0].drumEvents[4][8].velocity==127,"Erase Notes changed events outside its drum range or retained metadata");editOps.selectSequence(1);editOps.setPerformanceMode(PerformanceMode::Chromatic);editOps.toggle(5,2);editOps.toggle(7,12);check(editOps.eraseNotes(1,0,0,5)==1,"Erase Notes removed the wrong melodic event count");erased=editOps.snapshot();check(erased.sequences[1].melody[2]<0&&erased.sequences[1].melody[12]>=0,"Erase Notes changed melodic events outside its range");}
    // Maschine-style A-H groups keep independent samples and pad settings while
    // reusing the same sixteen active AudIO voices.
    e.setGroup(1);check(e.snapshot().group==1&&e.snapshot().paths[0].empty(),"new group was not empty");
    check(e.load(0,path("mono.wav"),error),error.c_str());PadEdit groupEdit;groupEdit.gain=.23;groupEdit.semitones=5;e.setEdit(0,groupEdit);
    e.setGroup(0);check(e.snapshot().group==0&&!e.snapshot().paths[0].empty()&&e.snapshot().edits[0].gain!=.23,"group A was not restored");
    e.setGroup(1);check(e.snapshot().paths[0]==path("mono.wav")&&e.snapshot().edits[0].gain==.23&&e.snapshot().edits[0].semitones==5,"group B was not restored");
    check(e.save(path("groups.kicklab"),error),error.c_str());e.setGroup(2);check(e.load(0,path("resample.wav"),error),error.c_str());check(e.open(path("groups.kicklab"),error),error.c_str());
    check(e.snapshot().group==1&&e.snapshot().paths[0]==path("mono.wav")&&e.snapshot().edits[0].gain==.23,"pad group active roundtrip failed");e.setGroup(0);check(!e.snapshot().paths[0].empty(),"pad group A roundtrip failed");
    // A KICKKIT is the reusable Drum Program only: it recalls the current
    // sixteen-pad kit and its edits, but never the pattern or another A-H kit.
    const auto sampleRoot=dir/"sample-library";std::filesystem::create_directories(sampleRoot/"Kicks");const auto kitSample=(sampleRoot/"Kicks"/"Personal Kick.wav").u8string();wav(kitSample,48000,1,1200);
    Engine kits;kits.toggle(3,5);check(kits.load(0,kitSample,error),error.c_str());PadEdit kitEdit;kitEdit.gain=.41;kitEdit.pan=-.35;kitEdit.mute=true;kitEdit.semitones=-4;kitEdit.attack=.17;kitEdit.inserts[2]=defaultInsertState(InsertEffect::Fracture);kitEdit.inserts[2].parameters[1]=.73;kits.setEdit(0,kitEdit);kits.setProgramPadColor(0,11);kits.setProgramColorModes(EmptyPadColorMode::Dim,PadOffColorMode::Off,PadOnColorMode::Full);kits.setGroup(1);check(kits.load(1,path("mono.wav"),error),error.c_str());kits.setGroup(0);
    const auto kitPath=path("personal.kickkit");check(kits.saveKit(kitPath,sampleRoot.u8string(),error),error.c_str());{std::ifstream saved(std::filesystem::u8path(kitPath));std::string text((std::istreambuf_iterator<char>(saved)),{});check(text.find("KICKKIT 4")!=std::string::npos&&text.find("COLOURS 1 0 2")!=std::string::npos&&text.find("Kicks/Personal Kick.wav")!=std::string::npos&&text.find(sampleRoot.u8string())==std::string::npos,"kit did not store Color Editor state or a portable sample-relative path");}
    check(kits.load(0,path("long.wav"),error),error.c_str());PadEdit changedKitEdit;changedKitEdit.gain=1.7;kits.setEdit(0,changedKitEdit);kits.setProgramPadColor(0,2);kits.setProgramColorModes(EmptyPadColorMode::Off,PadOffColorMode::Dim,PadOnColorMode::Velocity);check(kits.openKit(kitPath,sampleRoot.u8string(),error),error.c_str());auto restoredKit=kits.snapshot();const auto& restoredKitInfo=restoredKit.programs[restoredKit.program];check(restoredKit.paths[0]==std::filesystem::absolute(std::filesystem::u8path(kitSample)).lexically_normal().u8string()&&restoredKit.edits[0].gain==.41&&restoredKit.edits[0].pan==-.35&&restoredKit.edits[0].mute&&restoredKit.edits[0].semitones==-4&&restoredKit.edits[0].color==11&&restoredKit.edits[0].inserts[2].effect==InsertEffect::Fracture&&restoredKit.edits[0].inserts[2].parameters[1]==.73&&restoredKitInfo.emptyPadColor==EmptyPadColorMode::Dim&&restoredKitInfo.padOffColor==PadOffColorMode::Off&&restoredKitInfo.padOnColor==PadOnColorMode::Full,"kit sample, Color Editor or pad settings roundtrip failed");check(restoredKit.pattern[3][5],"loading a kit changed the active pattern");kits.setGroup(1);check(kits.snapshot().paths[1]==path("mono.wav"),"loading a kit changed another A-H block");kits.setGroup(0);
    std::filesystem::rename(sampleRoot/"Kicks"/"Personal Kick.wav",sampleRoot/"Kicks"/"Missing.wav");const auto beforeMissing=kits.snapshot().paths[0];check(!kits.openKit(kitPath,sampleRoot.u8string(),error),"kit with a missing WAV was accepted");check(kits.snapshot().paths[0]==beforeMissing,"failed kit load modified the current kit");std::filesystem::rename(sampleRoot/"Kicks"/"Missing.wav",sampleRoot/"Kicks"/"Personal Kick.wav");
    // The original KICK Pad Mixer belongs to the Program, independently of
    // Track Mixer data. Pad Mute is immediate and follows shared mute groups.
    {Engine padMix;check(padMix.load(0,path("long.wav"),error),error.c_str());check(padMix.load(1,path("long.wav"),error),error.c_str());padMix.setPadLevel(0,.6);padMix.setPadPan(0,-1);padMix.trigger(0);padMix.render(block);double leftPad=0,rightPad=0;for(int n=0;n<Block;++n){leftPad+=std::abs(block[n*2]);rightPad+=std::abs(block[n*2+1]);}check(leftPad>0&&rightPad<leftPad*1e-8,"Program pad pan is not audible");auto first=padMix.snapshot().edits[0],second=padMix.snapshot().edits[1];first.chokeGroup=4;second.chokeGroup=4;padMix.setEdit(0,first);padMix.setEdit(1,second);padMix.setPadMute(0,true);auto mutedPads=padMix.snapshot();check(mutedPads.edits[0].mute&&mutedPads.edits[1].mute,"Pad Mute group did not follow the original KICK grouping");padMix.stop();padMix.trigger(0);padMix.render(block);double mutedPadEnergy=0;for(float value:block)mutedPadEnergy+=std::abs(value);check(mutedPadEnergy<1e-8,"Program Pad Mute is not audible");check(padMix.save(path("pad-mixer-v14.kicklab"),error),error.c_str());}
    {Engine restoredPadMix;check(restoredPadMix.open(path("pad-mixer-v14.kicklab"),error),error.c_str());const auto padMixState=restoredPadMix.snapshot();check(padMixState.edits[0].gain==.6&&padMixState.edits[0].pan==-1&&padMixState.edits[0].mute&&padMixState.edits[1].mute,"KICKLAB 14 did not retain Pad Mixer or Pad Mute state");}
    // KICK Track Mixer state is audible, sequence-local and persisted by KICKLAB 8.
    e.stop();e.selectSequence(0);e.clearPattern();e.setPerformanceMode(PerformanceMode::Drum);e.toggle(0,0);e.selectTrack(0);e.setTrackLevel(0,.5);e.setTrackPan(0,-1);e.setTrackMute(0,false);e.setTrackSolo(0,true);e.setTrackArmed(0,true);e.setTrackProgram(0,ProgramType::Drum);e.setPlaying(true);e.render(block);double leftTrack=0,rightTrack=0;for(int n=0;n<Block;++n){leftTrack+=std::abs(block[n*2]);rightTrack+=std::abs(block[n*2+1]);}check(leftTrack>0&&rightTrack<leftTrack*1e-8,"track pan is not audible");e.stop();
    e.setTrackMute(0,true);e.setPlaying(true);e.render(block);double mutedTrack=0;for(float value:block)mutedTrack+=std::abs(value);check(mutedTrack<1e-8,"track mute is not audible");e.stop();e.setTrackMute(0,false);
    check(e.save(path("tracks.kicklab"),error),error.c_str());e.setTrackLevel(0,1);e.setTrackPan(0,0);e.setTrackSolo(0,false);e.setTrackArmed(0,false);check(e.open(path("tracks.kicklab"),error),error.c_str());auto trackState=e.snapshot();check(trackState.selectedTrack==0&&trackState.sequences[0].tracks[0].level==.5&&trackState.sequences[0].tracks[0].pan==-1&&trackState.sequences[0].tracks[0].solo&&trackState.sequences[0].tracks[0].armed,"KICKLAB 8 track state roundtrip failed");
    {std::ofstream f(std::filesystem::u8path(path("broken.wav")),std::ios::binary);f<<"RIFF";}check(!e.load(0,path("broken.wav"),error),"truncated WAV accepted");
    {std::ofstream f(std::filesystem::u8path(path("bad.kicklab")));f<<"INVALID";}check(!e.open(path("bad.kicklab"),error),"invalid project accepted");
    {std::ofstream f(std::filesystem::u8path(path("sparse.kicklab")));f<<"KICKLAB 1\n120 0.65\n";for(int p=0;p<Pads;++p)f<<0<<' '<<std::quoted(p==0?path("long.wav"):std::string{})<<'\n';}
    check(e.open(path("sparse.kicklab"),error),error.c_str());
    check(e.snapshot().paths[1].empty(),"loading sparse project retained old clip");
    // Trim, pitch and fades use actual rendered samples, not UI metadata.
    PadEdit edit;edit.start=.25;edit.end=.5;edit.fadeMs=0;e.setEdit(0,edit);e.stop();e.trigger(0);
    int nonzero=0;for(int b=0;b<60;++b){e.render(block);for(int n=0;n<Block;++n){if(std::abs(block[n*2])>1e-5)++nonzero;if(b*Block+n>=12000)check(block[n*2]==0,"trim leaked beyond end");}}
    check(nonzero>11500&&nonzero<12001,"trim duration incorrect");
    edit.start=0;edit.end=1;edit.semitones=12;e.setEdit(0,edit);e.stop();e.trigger(0);
    int crossings=0;float previous=0;
    for(int b=0;b<100;++b){e.render(block);for(int n=0;n<Block;++n){float v=block[n*2];if(previous<=0&&v>1e-5)++crossings;previous=v;if(b*Block+n>=24000)check(v==0,"pitch octave duration incorrect");}}
    check(crossings>=438&&crossings<=442,"octave pitch does not double frequency");
    edit.semitones=0;e.setEdit(0,edit);e.stop();e.triggerPitch(0,12);crossings=0;previous=0;
    for(int b=0;b<100;++b){e.render(block);for(int n=0;n<Block;++n){float v=block[n*2];if(previous<=0&&v>1e-5)++crossings;previous=v;}}
    check(crossings>=438&&crossings<=442,"per-note Pad Perform pitch is incorrect");
    wav(path("high.wav"),48000,2,48000,8000,26000);
    auto measure=[&](const PadEdit& settings){
        check(e.load(0,path("high.wav"),error),error.c_str());e.setEdit(0,settings);e.stop();e.trigger(0);
        std::vector<float> result;
        for(int b=0;b<96;++b){e.render(block);if(b>=32)result.insert(result.end(),block,block+Block*2);}
        return result;
    };
    PadEdit dry;dry.fadeMs=0;auto clean=measure(dry);double dryEnergy=0;for(float v:clean)dryEnergy+=v*v;
    auto half=dry;half.gain=.5;auto halfSignal=measure(half);double halfEnergy=0;for(float v:halfSignal)halfEnergy+=v*v;
    check(std::abs(halfEnergy/dryEnergy-.25)<1e-5,"per-pad gain is incorrect");
    auto muted=dry;muted.gain=0;auto silence=measure(muted);double muteEnergy=0;for(float v:silence)muteEnergy+=v*v;
    check(muteEnergy<dryEnergy*1e-12,"per-pad mute leaves audible signal");
    auto filtered=dry;filtered.inserts[0].bypass=false;filtered.inserts[0].parameters[0]=.1;auto low=measure(filtered);double lowEnergy=0;for(float v:low)lowEnergy+=v*v;
    check(lowEnergy<dryEnergy*.2,"Airwindows lowpass does not attenuate highs");
    auto saturated=dry;saturated.inserts[1].bypass=false;saturated.inserts[1].parameters[0]=1;auto drive=measure(saturated);double changed=0;
    for(size_t i=0;i<clean.size();++i){check(std::isfinite(drive[i]),"drive output is not finite");changed+=std::pow(clean[i]-drive[i],2);}
    check(changed>dryEnergy*.0001,"Airwindows drive does not change waveform");
    auto lowThenDrive=dry;lowThenDrive.inserts[0].bypass=false;lowThenDrive.inserts[0].parameters[0]=.35;lowThenDrive.inserts[1].bypass=false;lowThenDrive.inserts[1].parameters[0]=1;auto firstOrder=measure(lowThenDrive);
    auto driveThenLow=dry;driveThenLow.inserts[0]=lowThenDrive.inserts[1];driveThenLow.inserts[1]=lowThenDrive.inserts[0];auto secondOrder=measure(driveThenLow);double orderDifference=0;
    for(size_t i=0;i<firstOrder.size();++i)orderDifference+=std::pow(firstOrder[i]-secondOrder[i],2);
    check(orderDifference>dryEnergy*1e-8,"insert slot order does not affect processing order");
    check(insertEffectCatalog().size()==31,"effect catalog is incomplete");
    for(const auto& info:insertEffectCatalog()){
        check(info.parameterCount==insertParameterCount(info.effect),"effect parameter count mismatch");
        auto settings=dry;settings.inserts={};settings.inserts[0]=defaultInsertState(info.effect);auto signal=measure(settings);double effectEnergy=0;
        for(float value:signal){check(std::isfinite(value),"catalog effect output is not finite");effectEnergy+=value*value;}
        check(effectEnergy>1e-8,"catalog effect unexpectedly silenced the test signal");
    }
    check(e.load(0,path("mono.wav"),error),error.c_str());auto reverb=dry;reverb.inserts={};reverb.inserts[0]=defaultInsertState(InsertEffect::Reverb);e.setEdit(0,reverb);e.stop();e.trigger(0);double lateTail=0;
    for(int b=0;b<80;++b){e.render(block);if(b>40)for(float value:block)lateTail+=value*value;}
    check(lateTail>1e-8,"reverb tail was cut with the sample");
    saturated.insertsBypass=true;auto bypass=measure(saturated);double bypassDifference=0;
    for(size_t i=0;i<clean.size();++i)bypassDifference+=std::pow(clean[i]-bypass[i],2);
    check(bypassDifference<dryEnergy*1e-8,"FX bypass changes dry signal");
    edit={};edit.start=.12;edit.end=.83;edit.gain=.75;edit.semitones=-7;edit.fadeMs=4.5;edit.attack=.2;edit.decay=.3;edit.sustain=.6;edit.release=.4;edit.insertsBypass=true;
    edit.inserts[0]=defaultInsertState(InsertEffect::Highpass);edit.inserts[0].parameters[0]=.4;edit.inserts[0].parameters[1]=.2;edit.inserts[0].parameters[2]=.7;
    edit.inserts[1]=defaultInsertState(InsertEffect::DrumSlam);edit.inserts[1].parameters[0]=.8;
    edit.inserts[2]=defaultInsertState(InsertEffect::Chorus);edit.inserts[2].parameters[0]=.35;
    edit.inserts[3]=defaultInsertState(InsertEffect::Reverb);edit.inserts[3].bypass=true;edit.inserts[3].parameters[0]=.65;edit.inserts[3].parameters[1]=.75;e.setEdit(0,edit);
    check(e.save(path("edited.kicklab"),error),error.c_str());e.setEdit(0,{});check(e.open(path("edited.kicklab"),error),error.c_str());auto restored=e.snapshot().edits[0];
    check(restored.start==edit.start&&restored.end==edit.end&&restored.semitones==-7&&restored.gain==.75&&restored.fadeMs==4.5&&restored.attack==.2&&restored.decay==.3&&restored.sustain==.6&&restored.release==.4&&restored.insertsBypass,"edited project roundtrip failed");
    for(int slot=0;slot<InsertSlots;++slot){check(restored.inserts[slot].effect==edit.inserts[slot].effect&&restored.inserts[slot].bypass==edit.inserts[slot].bypass,"insert identity roundtrip failed");for(int parameter=0;parameter<insertParameterCount(edit.inserts[slot].effect);++parameter)check(restored.inserts[slot].parameters[parameter]==edit.inserts[slot].parameters[parameter],"insert parameter roundtrip failed");}
    check(e.open(path("sparse.kicklab"),error),error.c_str());restored=e.snapshot().edits[0];check(restored.inserts[0].effect==InsertEffect::Lowpass&&restored.inserts[0].bypass&&restored.inserts[1].effect==InsertEffect::PurestDrive&&restored.inserts[1].bypass,"v1 project did not restore bypassed default inserts");
    {std::ofstream f(std::filesystem::u8path(path("legacy-v2.kicklab")));f<<"KICKLAB 2\n120 0.65\n";for(int p=0;p<Pads;++p)f<<0<<' '<<std::quoted(p==0?path("long.wav"):std::string{})<<" 0.1 0.9 0.75 -3 4 0.4 0.8 1\n";}
    check(e.open(path("legacy-v2.kicklab"),error),error.c_str());restored=e.snapshot().edits[0];
    check(restored.insertsBypass&&restored.inserts[0].effect==InsertEffect::Lowpass&&!restored.inserts[0].bypass&&restored.inserts[0].parameters[0]==.4&&restored.inserts[1].effect==InsertEffect::PurestDrive&&!restored.inserts[1].bypass&&restored.inserts[1].parameters[0]==.8,"v2 effect migration failed");
    {std::ofstream f(std::filesystem::u8path(path("legacy-v3.kicklab")));f<<"KICKLAB 3\n120 0.65\n";for(int p=0;p<Pads;++p){f<<0<<' '<<std::quoted(p==0?path("long.wav"):std::string{})<<" 0 1 1 0 2 0";for(int slot=0;slot<InsertSlots;++slot)f<<" 0 0 0";f<<'\n';}}
    check(e.open(path("legacy-v3.kicklab"),error),error.c_str());restored=e.snapshot().edits[0];check(restored.attack==0&&restored.decay==0&&restored.sustain==1&&restored.release==0,"v3 project did not receive the default envelope");
    {std::ofstream f(std::filesystem::u8path(path("legacy-v4.kicklab")));f<<"KICKLAB 4\n120 0.65\n";for(int p=0;p<Pads;++p){f<<0<<' '<<std::quoted(p==0?path("long.wav"):std::string{})<<" 0 1 1 0 2 .1 .2 .7 .3 0";for(int slot=0;slot<InsertSlots;++slot)f<<" 0 0 0";f<<'\n';}}
    check(e.open(path("legacy-v4.kicklab"),error),error.c_str());restored=e.snapshot().edits[0];check(restored.attack==.1&&restored.decay==.2&&restored.sustain==.7&&restored.release==.3,"v4 envelope migration failed");
    PadEdit envelope;envelope.fadeMs=0;e.setEdit(0,envelope);e.stop();e.trigger(0);e.render(block);double immediateEnergy=0;for(float value:block)immediateEnergy+=value*value;
    envelope.attack=.5;e.setEdit(0,envelope);e.stop();e.trigger(0);e.render(block);double attackEnergy=0;for(float value:block)attackEnergy+=value*value;check(attackEnergy<immediateEnergy*.01,"amplitude envelope attack is ineffective");
    envelope.attack=0;envelope.release=.1;envelope.samplePlay=SamplePlayType::NoteOn;e.setEdit(0,envelope);e.stop();e.trigger(0);e.render(block);e.release(0);for(int b=0;b<12;++b)e.render(block);check(!e.snapshot().active[0],"amplitude envelope release did not stop the voice");
    e.setInsertEffect(0,2,InsertEffect::PurestDrive);e.setInsertParameter(0,2,0,.42);e.setInsertBypass(0,2,true);restored=e.snapshot().edits[0];check(restored.inserts[2].effect==InsertEffect::PurestDrive&&restored.inserts[2].parameters[0]==.42&&restored.inserts[2].bypass,"insert control API failed");
    e.resetProject();auto reset=e.snapshot();check(reset.paths[0].empty()&&!reset.pattern[0][0]&&reset.songLength==0&&reset.sequence==0,"new project reset retained musical data");
    std::cout<<"PASS: trim, Pad Perform pitch, ADSR, 31 Airwindows effects and four AudIO inserts\n";
    std::cout<<"PASS: -1 dBFS master limiter, track meters, Copy/Delete Pad, Erase Notes, KICKLAB 14 and KICKKIT 4\n";
    // Batched driver completions across the ring boundary must preserve time order.
    AudioQueue<4> queue;std::array<bool,4> ready={true,true,true,false};std::vector<int> submitted;
    auto submit=[&](size_t b){submitted.push_back(int(b));ready[b]=false;return true;};
    check(queue.refill([&](size_t b){return ready[b];},submit),"initial refill failed");
    ready[3]=ready[0]=true;
    check(queue.refill([&](size_t b){return ready[b];},submit),"wrap refill failed");
    check(submitted==std::vector<int>({0,1,2,3,0}),"driver completion reordered audio buffers");
    // UI snapshots/control edits must not insert silent blocks or pause audio time.
    e.stop();e.setVolume(.65);for(int b=0;b<32;++b)e.render(block);
    constexpr int stressBlocks=1200;
    std::vector<float> reference(stressBlocks*Block*2);
    for(int b=0;b<stressBlocks;++b){if(b%32==0)e.trigger(0);e.render(reference.data()+b*Block*2);}
    e.stop();
    std::atomic<bool> go{true};std::atomic<int> snapshots{0};
    std::vector<std::thread> ui;
    for(int t=0;t<3;++t)ui.emplace_back([&]{while(go.load()){auto copy=e.snapshot();e.setTempo(120);++snapshots;}});
    while(snapshots.load()<20)std::this_thread::yield();
    bool continuous=true;
    for(int b=0;b<stressBlocks;++b){if(b%32==0)e.trigger(0);e.render(block);for(int n=0;n<Block*2;++n)if(std::abs(block[n]-reference[b*Block*2+n])>1e-6f)continuous=false;}
    go=false;for(auto& thread:ui)thread.join();
    check(continuous,"UI activity changed audio samples or dropped blocks");
    std::cout<<"PASS: queue wraparound and concurrent UI/audio continuity ("<<snapshots<<" snapshots)\n";
    std::cout<<"PASS: silence, WAV playback, mono, EOF, resampling, mute, 16-pad mixing, sequencer, save/load, invalid input\n";
    return 0;
}catch(const std::exception& e){std::cerr<<"FAIL: "<<e.what()<<"\n";return 1;}}

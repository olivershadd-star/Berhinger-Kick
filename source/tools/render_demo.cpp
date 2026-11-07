// SPDX-License-Identifier: AGPL-3.0-or-later
#include "engine.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <cmath>
int main(int argc,char**argv){if(argc!=3)return 1;lab::Engine e;std::string error;int p=0;const char* names[]={"01-Kick.wav","02-Snare.wav","03-Closed-Hat.wav","04-Open-Hat.wav","05-Clap.wav","06-Low-Tom.wav","07-Mid-Tom.wav","08-High-Tom.wav","09-Rim.wav","10-Shaker.wav","11-Claves.wav","12-Cowbell.wav","13-Bass-C.wav","14-Bass-E.wav","15-Bass-G.wav","16-Chime.wav"};for(auto name:names)if(!e.load(p++,(std::filesystem::path(argv[1])/name).u8string(),error)){std::cerr<<error;return 2;}e.demoPattern();e.setPlaying(true);std::ofstream f(argv[2],std::ios::binary);auto u16=[&](unsigned n){char b[2]={char(n),char(n>>8)};f.write(b,2);};auto u32=[&](unsigned n){u16(n);u16(n>>16);};constexpr int blocks=1500,frames=blocks*lab::Block;f.write("RIFF",4);u32(36+frames*4);f.write("WAVEfmt ",8);u32(16);u16(1);u16(2);u32(lab::Rate);u32(lab::Rate*4);u16(4);u16(16);f.write("data",4);u32(frames*4);float buffer[lab::Block*2];double sum=0;for(int b=0;b<blocks;++b){e.render(buffer);for(float v:buffer){sum+=v*v;u16(uint16_t(int16_t(std::lround(v*32767))));}}std::cout<<"8 seconds; RMS="<<std::sqrt(sum/(frames*2))<<"\n";return sum>0?0:3;}

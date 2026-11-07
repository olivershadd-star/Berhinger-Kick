// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <array>
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace lab {
constexpr int Pads=16, Steps=16, Block=256, Rate=48000;
struct Snapshot {
    std::array<std::string,Pads> names{},paths{};
    std::array<std::array<bool,Steps>,Pads> pattern{};
    std::array<std::vector<float>,Pads> waveforms{};
    std::array<bool,Pads> active{};
    double tempo=120,volume=.65;
    bool playing=false;
    int step=-1;
    float peak=0;
};
class Engine {
public:
    Engine(); ~Engine();
    Engine(const Engine&)=delete;
    Engine& operator=(const Engine&)=delete;
    bool load(int pad,const std::string& path,std::string& error);
    void trigger(int pad);
    void toggle(int pad,int step);
    void clearPattern(); void demoPattern();
    void setPlaying(bool); void stop();
    void setTempo(double); void setVolume(double);
    Snapshot snapshot();
    // Exactly Block stereo frames; non-blocking when the UI replaces a clip.
    void render(float* interleaved);
    bool save(const std::string&,std::string& error);
    bool open(const std::string&,std::string& error);
private:
    struct Impl; std::unique_ptr<Impl> impl;
};
}

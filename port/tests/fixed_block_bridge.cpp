// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "FixedBlockBridge.h"
static void check(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
int main() {
    constexpr unsigned block = 480, frames = block * 30;
    std::vector<float> pool(32 * block);
    AudIO::MemoryPool<float>::get().initialize(pool.data(), 32, block);
    {
        AudIO::Buffer<float> buffer(2, true, 2, true);
        mt::FixedBlockBridge bridge(block);
        std::vector<float> left(frames), right(frames), outL(frames, -1), outR(frames, -1);
        for (unsigned i=0; i<frames; ++i) { left[i] = float(i); right[i] = -float(i); }
        unsigned offset=0, renders=0;
        for (unsigned size : {1u, 127u, 512u, 1024u, 3u, 480u, 960u, 17u, frames}) {
            size=std::min(size,frames-offset);
            const float* in[]{left.data()+offset,right.data()+offset};
            float* out[]{outL.data()+offset,outR.data()+offset};
            bridge.process(buffer,in,2,out,2,int(size),true,[&](auto& b){++renders;b.copyInputsToOutputs();});
            offset+=size;
        }
        check(offset==frames,"all frames visited");
        check(renders==frames/block,"one graph render per complete block");
        for(unsigned i=0;i<frames;++i) {
            check(outL[i]==(i<block ? 0.f:left[i-block]),"left samples lost, duplicated or reordered");
            check(outR[i]==(i<block ? 0.f:right[i-block]),"right samples lost, duplicated or reordered");
        }
        bridge.reset();
        float* out[]{outL.data(),outR.data()};
        bridge.process(buffer,nullptr,0,out,2,frames,false,[&](auto&){throw std::runtime_error("render while paused");});
        check(std::all_of(outL.begin(),outL.end(),[](float f){return f==0;}),"paused output not silent");
        bridge.reset();
        bridge.process(buffer,nullptr,0,out,2,frames,true,[&](auto& b){b.copyInputsToOutputs();});
        check(std::all_of(outL.begin(),outL.end(),[](float f){return f==0;}),"missing inputs must be zero");
    }
    std::cout << "PASS: irregular device buffers, stereo continuity, pause, reset, absent inputs\n";
}

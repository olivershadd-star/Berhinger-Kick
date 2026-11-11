// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <AudIO/Includes/AudIOBuffer.h>
namespace mt {
// Device-buffer adaptation only. The graph retains its fixed upstream block size.
class FixedBlockBridge {
public:
    explicit FixedBlockBridge(unsigned blockSize) : block(blockSize), read(blockSize) {}
    void reset() noexcept { fill = 0; read = block; }
    template<class Render>
    void process(AudIO::Buffer<float>& buffer, const float* const* inputs, int ins,
                 float* const* outputs, int outs, int frames, bool playing, Render&& render) {
        for (int frame = 0; frame < frames; ++frame) {
            for (unsigned ch = 0; ch < buffer.getNumInputChannels(); ++ch)
                buffer.getInputBuffer(ch)[fill] = ch < static_cast<unsigned>(ins) && inputs[ch] ? inputs[ch][frame] : 0.f;
            for (int ch = 0; ch < outs; ++ch)
                if (outputs[ch]) outputs[ch][frame] = playing && read < block && static_cast<unsigned>(ch) < buffer.getNumOutputChannels()
                    ? buffer.getOutputBuffer(ch)[read] : 0.f;
            ++read;
            if (++fill == block) {
                fill = 0;
                buffer.clearOutputs();
                if (playing) render(buffer);
                read = 0;
            }
        }
    }
private:
    const unsigned block;
    unsigned fill = 0, read;
};
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "FixedBlockBridge.h"
namespace mt {
// Bridges JUCE's variable device callbacks to the unmodified 480-frame AudIO graph.
// This class contains no sequencing, synthesis, effects or project behavior.
class AudioDevice final : public AudIO::IAudioDevice, private juce::AudioIODeviceCallback {
public:
    AudioDevice(unsigned inputs, unsigned outputs, const juce::String& preferred)
        : AudIO::IAudioDevice(inputs, outputs), numInputs(inputs), numOutputs(outputs), preferredName(preferred) {}
    ~AudioDevice() { stop(); }
    juce::AudioDeviceManager& getJuceAudioDeviceMgr() { return manager; }
    void start(uint32_t rate, uint32_t block) override {
        stop();
        if (block != AUDIO_BLOCK_SIZE) {
            juce::Logger::writeToLog("KICK: incompatible graph block size");
            return;
        }
        auto error = manager.initialise(static_cast<int>(numInputs), static_cast<int>(numOutputs), nullptr, true, preferredName);
        if (error.isNotEmpty()) { juce::Logger::writeToLog("KICK audio: " + error); return; }
        juce::AudioDeviceManager::AudioDeviceSetup setup;
        manager.getAudioDeviceSetup(setup);
        setup.sampleRate = rate;
        setup.bufferSize = static_cast<int>(block);
        error = manager.setAudioDeviceSetup(setup, true);
        if (error.isNotEmpty()) juce::Logger::writeToLog("KICK audio device setup: " + error);
        setState(AudIO::AudioDeviceState::playing);
        manager.addAudioCallback(this);
    }
    void resume() override { setState(AudIO::AudioDeviceState::playing); }
    void pause() override { setState(AudIO::AudioDeviceState::paused); }
    void stop() override {
        setState(AudIO::AudioDeviceState::stopped);
        manager.removeAudioCallback(this);
        manager.closeAudioDevice();
    }
private:
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override {
        bridge.reset();
        _buffer.clearInputs(); _buffer.clearOutputs();
        prepareToPlay(static_cast<uint32_t>(device->getCurrentSampleRate()), AUDIO_BLOCK_SIZE);
    }
    void audioDeviceStopped() override { teardown(); }
    void audioDeviceIOCallbackWithContext(const float* const* inputs, int ins,
        float* const* outputs, int outs, int samples, const juce::AudioIODeviceCallbackContext&) override {
        bridge.process(_buffer, inputs, ins, outputs, outs, samples, isPlaying(),
            [this](AudIO::Buffer<float>& buffer) { process(buffer); });
    }
    juce::AudioDeviceManager manager;
    const unsigned numInputs, numOutputs;
    const juce::String preferredName;
    FixedBlockBridge bridge{AUDIO_BLOCK_SIZE};
};
}

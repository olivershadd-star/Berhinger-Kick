// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <array>
#include <cmath>

namespace mt
{
    // The published KICK voice code expects these convenience types from the
    // missing AudIO_juce module. They adapt the published AudIO DSP classes;
    // they do not replace the voice, sampler, sequencer, or program model.
    enum class FilterType
    {
        lowPass,
        highPass,
        bandPass,
    };

    class Filter
    {
    public:
        inline static const juce::NormalisableRange<double> freqRange{ 20.0, 20000.0, 0.0, 0.25 };

        void setSampleRate( double newSampleRate ) noexcept { sampleRate = newSampleRate; }

        void setFilterType( FilterType type ) noexcept
        {
            filterType = type;
        }

        void setResonance( double resonance ) noexcept
        {
            // KICK stores resonance as a MIDI-style 0..127 value.
            normalizedResonance = juce::jlimit( 0.0, 0.99, resonance / 127.0 );
        }

        void setCutoff( double normalized ) noexcept
        {
            normalizedCutoff = juce::jlimit( 0.01, 0.99, normalized );
        }

        void reset() noexcept
        {
            states = {};
        }

        void process( juce::AudioBuffer<float>& sample ) noexcept
        {
            const auto channels = juce::jmin( sample.getNumChannels(), static_cast<int>( states.size() ) );
            const auto cutoffHz = freqRange.convertFrom0to1( normalizedCutoff );
            // The coefficient follows the state-variable topology shipped in
            // AudIO/Dsp/filter.h, but keeps independent state per channel.
            const auto coefficient = static_cast<float>( juce::jlimit( 0.001, 0.99,
                2.0 * std::sin( juce::MathConstants<double>::pi * cutoffHz / juce::jmax( 1.0, sampleRate ) ) ) );
            const auto feedback = static_cast<float>( normalizedResonance
                + normalizedResonance / juce::jmax( 0.01, 1.0 - static_cast<double>( coefficient ) ) );

            for( int channel = 0; channel < channels; ++channel )
            {
                auto* data = sample.getWritePointer( channel );
                for( int frame = 0; frame < sample.getNumSamples(); ++frame )
                {
                    auto& state = states[static_cast<size_t>( channel )];
                    const auto input = data[frame];
                    state.low += coefficient * ( input - state.low + feedback * ( state.low - state.band ) );
                    state.band += coefficient * ( state.low - state.band );
                    switch( filterType )
                    {
                        case FilterType::highPass: data[frame] = input - state.band; break;
                        case FilterType::bandPass: data[frame] = state.low - state.band; break;
                        case FilterType::lowPass:
                        default: data[frame] = state.band; break;
                    }
                }
            }
        }

    private:
        struct State
        {
            float low = 0.0f;
            float band = 0.0f;
        };

        std::array<State, 2> states{};
        FilterType filterType = FilterType::lowPass;
        double sampleRate = AUDIO_SAMPLE_RATE;
        double normalizedCutoff = 0.99;
        double normalizedResonance = 0.0;
    };

    class ADSR
    {
    public:
        ADSR()
        {
            impl.setEnabled( true );
        }

        void setSampleRate( double sampleRate ) noexcept { impl.setSampleRate( sampleRate ); }
        void setParameters( double attack, double hold, double decay, double release, double sustain ) noexcept
        {
            impl.setParameters( { attack, hold, decay, sustain, release } );
        }
        void gate( bool on ) noexcept { impl.gate( on ); }
        void reset() noexcept { impl.reset(); }
        float operator()() noexcept { return impl(); }

    private:
        AudIO::ADSR<float> impl;
    };
}

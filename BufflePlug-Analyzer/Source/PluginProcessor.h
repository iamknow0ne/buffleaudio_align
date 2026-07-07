#pragma once

#include <array>
#include <atomic>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "DSP/ConsonantTamer.h"
#include "DSP/ManualNudgeDelay.h"
#include "DSP/NaturalnessGuardrail.h"
#include "DSP/PreviewModeMixer.h"

//==============================================================================
/**
*/
class BufflePlugAnalyzerAudioProcessor  : public juce::AudioProcessor
{
public:
    using AudioProcessorValueTreeState = juce::AudioProcessorValueTreeState;

    //==============================================================================
    BufflePlugAnalyzerAudioProcessor();
    ~BufflePlugAnalyzerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    AudioProcessorValueTreeState& getValueTreeState() noexcept { return parameters; }
    const AudioProcessorValueTreeState& getValueTreeState() const noexcept { return parameters; }

    struct AlignmentSnapshot
    {
        std::array<float, 96> guide {};
        std::array<float, 96> dub {};
        float guideRms = 0.0f;
        float dubRms = 0.0f;
        float match = 0.0f;
        float nudgeMs = 0.0f;
        float estimatedOffsetMs = 0.0f;
        float suggestedNudgeMs = 0.0f;
        float offsetConfidence = 0.0f;
        float removedMaterial = 0.0f;
        float removedPeakDelta = 0.0f;
        buffle::align::NaturalnessRisk naturalnessRisk = buffle::align::NaturalnessRisk::safe;
        int latencyMs = 0;
        bool guideFromSidechain = false;
        bool hasReliableOffset = false;
    };

    AlignmentSnapshot getAlignmentSnapshot() const;
    juce::String getAlignmentReportText() const;
    juce::String getWorkflowStatus() const;

private:
    //==============================================================================
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    AudioProcessorValueTreeState parameters;

    std::array<std::atomic<float>, 96> guideHistory {};
    std::array<std::atomic<float>, 96> dubHistory {};
    std::atomic<int> historyWriteIndex { 0 };
    std::atomic<float> lastGuideRms { 0.0f };
    std::atomic<float> lastDubRms { 0.0f };
    std::atomic<float> lastMatch { 0.0f };
    std::atomic<float> lastRemovedMaterial { 0.0f };
    std::atomic<float> lastRemovedPeakDelta { 0.0f };
    std::atomic<float> analysisHopMs { 0.0f };
    std::atomic<bool> guideSidechainActive { false };

    std::atomic<float>* tightnessParam = nullptr;
    std::atomic<float>* naturalnessParam = nullptr;
    std::atomic<float>* consonantLevelParam = nullptr;
    std::atomic<float>* nudgeParam = nullptr;
    std::atomic<float>* auditionParam = nullptr;
    std::atomic<float>* previewModeParam = nullptr;
    std::atomic<float>* stackRoleParam = nullptr;

    double currentSampleRate = 44100.0;
    buffle::align::ManualNudgeDelay nudgeDelay;
    buffle::align::ConsonantTamer consonantTamer;
    juce::AudioBuffer<float> originalDubBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BufflePlugAnalyzerAudioProcessor)
};

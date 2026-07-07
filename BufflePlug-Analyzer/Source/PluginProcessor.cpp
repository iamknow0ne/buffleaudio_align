#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/AlignmentReport.h"
#include "DSP/BidirectionalNudge.h"
#include "DSP/RemovedMaterialMeter.h"
#include "DSP/StackRolePreset.h"
#include "DSP/TimingOffsetEstimator.h"

#include <vector>

namespace
{
constexpr auto tightnessId = "tightness";
constexpr auto naturalnessId = "naturalness";
constexpr auto consonantLevelId = "consonantLevel";
constexpr auto nudgeId = "nudge";
constexpr auto auditionId = "audition";
constexpr auto previewModeId = "previewMode";
constexpr auto stackRoleId = "stackRole";
constexpr auto guideBlendId = "guideBlend";
constexpr auto stereoFocusId = "stereoFocus";
constexpr auto maxNudgeMs = 120.0f;
constexpr auto reliableOffsetConfidence = 0.45f;
constexpr auto usableSignalFloor = 0.025f;

float normaliseDb (float db)
{
    return juce::jlimit (0.0f, 1.0f, juce::jmap (db, -60.0f, -6.0f, 0.0f, 1.0f));
}

float calculateRmsDb (const juce::AudioBuffer<float>& buffer, int channel)
{
    if (channel < 0 || channel >= buffer.getNumChannels() || buffer.getNumSamples() == 0)
        return -72.0f;

    return juce::Decibels::gainToDecibels (buffer.getRMSLevel (channel, 0, buffer.getNumSamples()), -72.0f);
}

float calculateMatch (float guide, float dub)
{
    if (guide <= 0.001f && dub <= 0.001f)
        return 0.0f;

    return 1.0f - juce::jlimit (0.0f, 1.0f, std::abs (guide - dub));
}

juce::String previewModeName (int mode)
{
    switch (mode)
    {
        case 0: return "Original";
        case 2: return "Difference";
        default: return "Aligned";
    }
}

}

//==============================================================================
BufflePlugAnalyzerAudioProcessor::BufflePlugAnalyzerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Dub",  juce::AudioChannelSet::stereo(), true)
                       .withInput  ("Guide", juce::AudioChannelSet::stereo(), false)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
     , parameters (*this, nullptr, "BuffleAlignState", createParameterLayout())
{
    tightnessParam = parameters.getRawParameterValue (tightnessId);
    naturalnessParam = parameters.getRawParameterValue (naturalnessId);
    consonantLevelParam = parameters.getRawParameterValue (consonantLevelId);
    nudgeParam = parameters.getRawParameterValue (nudgeId);
    auditionParam = parameters.getRawParameterValue (auditionId);
    previewModeParam = parameters.getRawParameterValue (previewModeId);
    stackRoleParam = parameters.getRawParameterValue (stackRoleId);

    for (auto& value : guideHistory)
        value.store (0.0f);

    for (auto& value : dubHistory)
        value.store (0.0f);
}

BufflePlugAnalyzerAudioProcessor::~BufflePlugAnalyzerAudioProcessor()
{
}

//==============================================================================
const juce::String BufflePlugAnalyzerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BufflePlugAnalyzerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BufflePlugAnalyzerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BufflePlugAnalyzerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BufflePlugAnalyzerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BufflePlugAnalyzerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BufflePlugAnalyzerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BufflePlugAnalyzerAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String BufflePlugAnalyzerAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void BufflePlugAnalyzerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void BufflePlugAnalyzerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);

    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
    const auto latencySamples = static_cast<int> (std::round (currentSampleRate * (maxNudgeMs / 1000.0f)));
    const auto maxDelaySamples = latencySamples * 2 + 1;
    setLatencySamples (latencySamples);
    nudgeDelay.prepare (juce::jmax (1, getTotalNumOutputChannels()), maxDelaySamples);
    consonantTamer.prepare (currentSampleRate, juce::jmax (1, getTotalNumOutputChannels()));
    originalDubBuffer.setSize (juce::jmax (1, getTotalNumOutputChannels()),
                               juce::jmax (1, samplesPerBlock),
                               false,
                               false,
                               true);
    historyWriteIndex.store (0);
    lastGuideRms.store (0.0f);
    lastDubRms.store (0.0f);
    lastMatch.store (0.0f);
    lastRemovedMaterial.store (0.0f);
    lastRemovedPeakDelta.store (0.0f);
    analysisHopMs.store (static_cast<float> (static_cast<double> (juce::jmax (1, samplesPerBlock))
                                           * 1000.0 / currentSampleRate));
    guideSidechainActive.store (false);
}

void BufflePlugAnalyzerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BufflePlugAnalyzerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    const auto mainInput = layouts.getMainInputChannelSet();
    const auto mainOutput = layouts.getMainOutputChannelSet();

    if (mainOutput != juce::AudioChannelSet::mono()
     && mainOutput != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (mainInput != mainOutput)
        return false;
   #endif

    if (layouts.inputBuses.size() > 1)
    {
        const auto guideLayout = layouts.inputBuses.getReference (1);
        if (! guideLayout.isDisabled()
         && guideLayout != juce::AudioChannelSet::mono()
         && guideLayout != juce::AudioChannelSet::stereo())
            return false;
    }

    return true;
  #endif
}
#endif

void BufflePlugAnalyzerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused (midiMessages);

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto dubBuffer = getBusBuffer (buffer, true, 0);
    auto guideBuffer = getBusCount (true) > 1 ? getBusBuffer (buffer, true, 1) : juce::AudioBuffer<float>();
    const auto hasGuideSidechain = guideBuffer.getNumChannels() > 0 && getBus (true, 1)->isEnabled();
    guideSidechainActive.store (hasGuideSidechain);
    analysisHopMs.store (static_cast<float> (static_cast<double> (juce::jmax (1, buffer.getNumSamples()))
                                           * 1000.0 / currentSampleRate));

    const auto guide = normaliseDb (calculateRmsDb (hasGuideSidechain ? guideBuffer : dubBuffer, 0));
    const auto dub = normaliseDb (calculateRmsDb (dubBuffer, 0));
    const auto match = calculateMatch (guide, dub);

    const auto writeIndex = historyWriteIndex.fetch_add (1) % static_cast<int> (guideHistory.size());
    guideHistory[static_cast<size_t> (writeIndex)].store (guide);
    dubHistory[static_cast<size_t> (writeIndex)].store (dub);
    lastGuideRms.store (guide);
    lastDubRms.store (dub);
    lastMatch.store (match);

    const auto auditionGain = auditionParam != nullptr ? auditionParam->load() : 1.0f;
    const auto previewMode = previewModeParam != nullptr
        ? static_cast<buffle::align::PreviewMode> (juce::roundToInt (previewModeParam->load()))
        : buffle::align::PreviewMode::aligned;
    const auto nudgeMs = nudgeParam != nullptr ? nudgeParam->load() : 0.0f;
    const auto consonantAmount = consonantLevelParam != nullptr ? consonantLevelParam->load() : 0.0f;
    const auto naturalness = naturalnessParam != nullptr ? naturalnessParam->load() : 0.5f;
    const auto nudgePlan = buffle::align::calculateBidirectionalNudgePlan (nudgeMs,
                                                                           currentSampleRate,
                                                                           maxNudgeMs,
                                                                           nudgeDelay.getMaxDelaySamples());

    if (originalDubBuffer.getNumChannels() < dubBuffer.getNumChannels()
     || originalDubBuffer.getNumSamples() < dubBuffer.getNumSamples())
    {
        originalDubBuffer.setSize (juce::jmax (1, dubBuffer.getNumChannels()),
                                   juce::jmax (1, dubBuffer.getNumSamples()),
                                   false,
                                   false,
                                   true);
    }

    for (int channel = 0; channel < dubBuffer.getNumChannels(); ++channel)
        originalDubBuffer.copyFrom (channel, 0, dubBuffer, channel, 0, dubBuffer.getNumSamples());

    nudgeDelay.process (dubBuffer, dubBuffer.getNumChannels(), nudgePlan.delaySamples);
    consonantTamer.process (dubBuffer, hasGuideSidechain ? &guideBuffer : nullptr, consonantAmount, naturalness);
    const auto removed = buffle::align::measureRemovedMaterial (originalDubBuffer, dubBuffer, dubBuffer.getNumChannels());
    lastRemovedMaterial.store (removed.amount);
    lastRemovedPeakDelta.store (removed.peakDelta);
    buffle::align::renderPreviewMode (dubBuffer, originalDubBuffer, previewMode, dubBuffer.getNumChannels());

    buffer.applyGain (auditionGain);
}

//==============================================================================
bool BufflePlugAnalyzerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BufflePlugAnalyzerAudioProcessor::createEditor()
{
    return new BufflePlugAnalyzerAudioProcessorEditor (*this);
}

//==============================================================================
void BufflePlugAnalyzerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto state = parameters.copyState().createXml())
        copyXmlToBinary (*state, destData);
}

void BufflePlugAnalyzerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto state = getXmlFromBinary (data, sizeInBytes))
        if (state->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*state));
}

BufflePlugAnalyzerAudioProcessor::AlignmentSnapshot BufflePlugAnalyzerAudioProcessor::getAlignmentSnapshot() const
{
    AlignmentSnapshot snapshot;
    const auto writeIndex = historyWriteIndex.load();
    const auto historySize = static_cast<int> (guideHistory.size());

    for (int i = 0; i < historySize; ++i)
    {
        const auto readIndex = (writeIndex + i) % historySize;
        snapshot.guide[static_cast<size_t> (i)] = guideHistory[static_cast<size_t> (readIndex)].load();
        snapshot.dub[static_cast<size_t> (i)] = dubHistory[static_cast<size_t> (readIndex)].load();
    }

    snapshot.guideRms = lastGuideRms.load();
    snapshot.dubRms = lastDubRms.load();
    snapshot.match = lastMatch.load();
    snapshot.removedMaterial = lastRemovedMaterial.load();
    snapshot.removedPeakDelta = lastRemovedPeakDelta.load();
    snapshot.guideFromSidechain = guideSidechainActive.load();

    if (nudgeParam != nullptr)
    {
        snapshot.nudgeMs = nudgeParam->load();
        snapshot.latencyMs = static_cast<int> (std::round (static_cast<double> (getLatencySamples()) * 1000.0 / currentSampleRate));
    }

    std::vector<float> guideEnvelope;
    std::vector<float> dubEnvelope;
    guideEnvelope.reserve (snapshot.guide.size());
    dubEnvelope.reserve (snapshot.dub.size());

    for (size_t i = 0; i < snapshot.guide.size(); ++i)
    {
        guideEnvelope.push_back (snapshot.guide[i]);
        dubEnvelope.push_back (snapshot.dub[i]);
    }

    buffle::align::TimingOffsetEstimator estimator;
    const auto offset = estimator.estimate (guideEnvelope, dubEnvelope, 24, analysisHopMs.load());
    snapshot.offsetConfidence = offset.confidence;
    snapshot.hasReliableOffset = offset.confidence >= reliableOffsetConfidence
                              && snapshot.guideRms >= usableSignalFloor
                              && snapshot.dubRms >= usableSignalFloor;

    if (snapshot.hasReliableOffset)
    {
        snapshot.estimatedOffsetMs = offset.milliseconds;
        snapshot.suggestedNudgeMs = buffle::align::suggestBidirectionalNudgeMs (offset.milliseconds, maxNudgeMs);
    }

    return snapshot;
}

juce::String BufflePlugAnalyzerAudioProcessor::getWorkflowStatus() const
{
    const auto tightness = tightnessParam != nullptr ? tightnessParam->load() : 0.0f;
    const auto consonantLevel = consonantLevelParam != nullptr ? consonantLevelParam->load() : 0.0f;
    const auto nudgeMs = nudgeParam != nullptr ? nudgeParam->load() : 0.0f;
    const auto previewMode = previewModeParam != nullptr ? juce::roundToInt (previewModeParam->load()) : 1;
    const auto stackRole = stackRoleParam != nullptr ? juce::roundToInt (stackRoleParam->load()) : 0;
    const auto snapshot = getAlignmentSnapshot();

    return juce::String (snapshot.hasReliableOffset ? "Guide/Dub monitor locked - " : "Guide/Dub monitor listening - ")
        + previewModeName (previewMode)
        + " mode, "
        + juce::String (buffle::align::getStackRoleName (static_cast<buffle::align::StackRole> (juce::jlimit (0, 4, stackRole))))
        + " role, "
        + "Tightness "
        + juce::String (static_cast<int> (tightness * 100.0f))
        + "%, tamer "
        + juce::String (static_cast<int> (consonantLevel * 100.0f))
        + "%, nudge "
        + juce::String (nudgeMs, 1)
        + " ms"
        + (snapshot.hasReliableOffset
            ? ", suggested " + juce::String (snapshot.suggestedNudgeMs, 1) + " ms"
            : ", waiting for usable Guide/Dub confidence");
}

juce::String BufflePlugAnalyzerAudioProcessor::getAlignmentReportText() const
{
    const auto snapshot = getAlignmentSnapshot();
    buffle::align::AlignmentReportInput input;
    input.guideFromSidechain = snapshot.guideFromSidechain;
    input.hasReliableOffset = snapshot.hasReliableOffset;
    input.guideRms = snapshot.guideRms;
    input.dubRms = snapshot.dubRms;
    input.offsetConfidence = snapshot.offsetConfidence;
    input.estimatedOffsetMs = snapshot.estimatedOffsetMs;
    input.suggestedNudgeMs = snapshot.suggestedNudgeMs;
    input.currentNudgeMs = snapshot.nudgeMs;
    input.removedMaterial = snapshot.removedMaterial;
    input.removedPeakDelta = snapshot.removedPeakDelta;
    input.previewMode = previewModeParam != nullptr ? juce::roundToInt (previewModeParam->load()) : 1;
    input.stackRole = stackRoleParam != nullptr ? juce::roundToInt (stackRoleParam->load()) : 0;
    input.tightness = tightnessParam != nullptr ? tightnessParam->load() : 0.0f;
    input.naturalness = naturalnessParam != nullptr ? naturalnessParam->load() : 0.0f;
    input.consonantLevel = consonantLevelParam != nullptr ? consonantLevelParam->load() : 0.0f;

    if (auto* value = parameters.getRawParameterValue (guideBlendId))
        input.guideBlend = value->load();

    if (auto* value = parameters.getRawParameterValue (stereoFocusId))
        input.stereoFocus = value->load();

    return juce::String (buffle::align::buildAlignmentReport (input));
}

BufflePlugAnalyzerAudioProcessor::AudioProcessorValueTreeState::ParameterLayout
BufflePlugAnalyzerAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        tightnessId, "Tightness", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.72f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        naturalnessId, "Naturalness", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.42f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        consonantLevelId, "Consonant Level", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.68f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        nudgeId, "Manual Nudge", juce::NormalisableRange<float> (-maxNudgeMs, maxNudgeMs, 0.1f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        auditionId, "Audition", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 1.0f));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        previewModeId, "Preview Mode", juce::StringArray { "Original", "Aligned", "Difference" }, 1));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        stackRoleId, "Stack Role", juce::StringArray { "Manual", "Double Tight", "Choir Natural", "Rap Stack", "ADR Loose" }, 0));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        guideBlendId, "Guide Blend", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        stereoFocusId, "Stereo Focus", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.35f));

    return { params.begin(), params.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BufflePlugAnalyzerAudioProcessor();
}

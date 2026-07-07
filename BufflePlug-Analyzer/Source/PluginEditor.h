#pragma once

#include <memory>

#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class BufflePlugAnalyzerAudioProcessorEditor  : public juce::AudioProcessorEditor
                                                , private juce::Timer
{
public:
    BufflePlugAnalyzerAudioProcessorEditor (BufflePlugAnalyzerAudioProcessor&);
    ~BufflePlugAnalyzerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseUp (const juce::MouseEvent& event) override;
    void mouseMove (const juce::MouseEvent& event) override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    class AlignmentView;

    void timerCallback() override;
    void configureSlider (juce::Slider& slider,
                          juce::Label& label,
                          const juce::String& text,
                          const juce::String& tooltip,
                          bool isPercentValue);
    void drawWorkflowRail (juce::Graphics& g, juce::Rectangle<int> area);
    void drawHeader (juce::Graphics& g, juce::Rectangle<int> area);
    void loadBrandAssets();
    void showAboutBox();
    void showTransientStatus (const juce::String& message);
    void applySuggestedNudge();
    void copyAlignmentReport();
    void setPreviewMode (int modeIndex);
    void updatePreviewModeButtons();
    void applyStackRolePreset (int roleIndex);
    void markStackRoleManual();
    void updateStackRoleBox();

    static juce::Image recolourForDarkTheme (const juce::Image& source, juce::Colour tint);

    BufflePlugAnalyzerAudioProcessor& audioProcessor;
    std::unique_ptr<AlignmentView> alignmentView;
    juce::TooltipWindow tooltipWindow { this, 600 };

    juce::Slider tightnessSlider;
    juce::Slider naturalnessSlider;
    juce::Slider consonantSlider;
    juce::Slider nudgeSlider;
    juce::Slider auditionSlider;
    juce::Slider guideBlendSlider;
    juce::Slider stereoFocusSlider;

    juce::Label tightnessLabel;
    juce::Label naturalnessLabel;
    juce::Label consonantLabel;
    juce::Label nudgeLabel;
    juce::Label auditionLabel;
    juce::Label guideBlendLabel;
    juce::Label stereoFocusLabel;
    juce::Label statusLabel;
    juce::ComboBox stackRoleBox;

    std::unique_ptr<SliderAttachment> tightnessAttachment;
    std::unique_ptr<SliderAttachment> naturalnessAttachment;
    std::unique_ptr<SliderAttachment> consonantAttachment;
    std::unique_ptr<SliderAttachment> nudgeAttachment;
    std::unique_ptr<SliderAttachment> auditionAttachment;
    std::unique_ptr<SliderAttachment> guideBlendAttachment;
    std::unique_ptr<SliderAttachment> stereoFocusAttachment;

    juce::TextButton captureButton { "Arm Listen" };
    juce::TextButton analyzeButton { "Check Timing" };
    juce::TextButton alignButton { "Preview Align" };
    juce::TextButton applySuggestedButton { "Apply Nudge" };
    juce::TextButton reportButton { "Copy Report" };
    juce::TextButton originalModeButton { "Original" };
    juce::TextButton alignedModeButton { "Aligned" };
    juce::TextButton differenceModeButton { "All Diff" };
    juce::TextButton tamerModeButton { "Tamer" };

    juce::Image buffleAudioLogo;
    juce::Rectangle<int> brandLogoBounds;
    juce::String transientStatus;
    int transientStatusFrames = 0;
    bool applyingStackRolePreset = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BufflePlugAnalyzerAudioProcessorEditor)
};

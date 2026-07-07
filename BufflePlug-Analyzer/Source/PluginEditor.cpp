#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

#include <algorithm>

namespace
{
const auto background = juce::Colour (0xff0b0b0b);
const auto header = juce::Colour (0xff121216);
const auto panel = juce::Colour (0xff141418);
const auto panelLight = juce::Colour (0xff1c1c22);
const auto border = juce::Colour (0xff2a2a32);
const auto ink = juce::Colour (0xffd9e6e4);
const auto muted = juce::Colour (0xff9fb5b2);
const auto guideColour = juce::Colour (0xff4dd0c4);
const auto dubColour = juce::Colour (0xffffb35c);
const auto alignedColour = juce::Colour (0xff6ee7a6);
const auto brandAccent = juce::Colour (0xff4dd0c4);
const auto warningColour = juce::Colour (0xffffd166);

juce::Rectangle<int> removeRail (juce::Rectangle<int>& bounds)
{
    return bounds.removeFromLeft (168).reduced (22, 8);
}

void drawRoundRect (juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour fill, juce::Colour stroke)
{
    g.setColour (fill);
    g.fillRoundedRectangle (bounds, 8.0f);
    g.setColour (stroke);
    g.drawRoundedRectangle (bounds, 8.0f, 1.0f);
}

juce::String asPercent (float value)
{
    return juce::String (static_cast<int> (juce::roundToInt (juce::jlimit (0.0f, 1.0f, value) * 100.0f))) + "%";
}

juce::String asSignedMs (float value)
{
    return (value > 0.0f ? "+" : "") + juce::String (value, 1) + " ms";
}

juce::String describePhraseHealth (const BufflePlugAnalyzerAudioProcessor::AlignmentSnapshot& snapshot)
{
    if (! snapshot.guideFromSidechain)
        return "Route Guide sidechain";

    if (snapshot.guideRms < 0.025f)
        return "Guide too quiet";

    if (snapshot.dubRms < 0.025f)
        return "Dub too quiet";

    if (! snapshot.hasReliableOffset)
        return "Listening for confidence";

    if (snapshot.suggestedNudgeMs <= 0.05f)
        return "Locked - no delay needed";

    return "Safe nudge ready";
}

juce::Colour phraseHealthColour (const BufflePlugAnalyzerAudioProcessor::AlignmentSnapshot& snapshot)
{
    if (snapshot.hasReliableOffset && snapshot.suggestedNudgeMs > 0.05f)
        return alignedColour;

    if (snapshot.hasReliableOffset)
        return brandAccent;

    return warningColour;
}

void drawReadoutPill (juce::Graphics& g,
                      juce::Rectangle<int> area,
                      const juce::String& label,
                      const juce::String& value,
                      juce::Colour accent)
{
    drawRoundRect (g, area.toFloat(), juce::Colour (0xff11151a), accent.withAlpha (0.24f));

    auto text = area.reduced (10, 4);
    g.setColour (muted.withAlpha (0.82f));
    g.setFont (juce::FontOptions (10.5f, juce::Font::bold));
    g.drawText (label, text.removeFromTop (15), juce::Justification::centredLeft);

    g.setColour (accent);
    g.setFont (juce::FontOptions (15.0f, juce::Font::bold));
    g.drawText (value, text, juce::Justification::centredLeft);
}

void drawHorizontalMeter (juce::Graphics& g, juce::Rectangle<int> area, float value, juce::Colour accent)
{
    auto meter = area.toFloat();
    g.setColour (juce::Colour (0xff242a31));
    g.fillRoundedRectangle (meter, 4.0f);

    auto fill = meter.reduced (2.0f);
    fill.setWidth (fill.getWidth() * juce::jlimit (0.0f, 1.0f, value));
    g.setColour (accent.withAlpha (0.88f));
    g.fillRoundedRectangle (fill, 3.0f);
}

void drawPhraseHealthStrip (juce::Graphics& g,
                            juce::Rectangle<int> area,
                            const BufflePlugAnalyzerAudioProcessor::AlignmentSnapshot& snapshot)
{
    const auto colour = phraseHealthColour (snapshot);
    drawRoundRect (g, area.toFloat(), juce::Colour (0xff12181d), colour.withAlpha (0.3f));

    auto text = area.reduced (12, 0);
    g.setColour (colour);
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    g.drawText ("PHRASE HEALTH", text.removeFromLeft (116), juce::Justification::centredLeft);

    g.setColour (ink);
    g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    g.drawText (describePhraseHealth (snapshot), text, juce::Justification::centredLeft);
}

class AboutComponent final : public juce::Component
{
public:
    AboutComponent (juce::Image logo) : logoImage (std::move (logo))
    {
        setSize (328, 226);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (panel);

        auto area = getLocalBounds().reduced (18);

        if (logoImage.isValid())
        {
            auto logoArea = area.removeFromTop (58).toFloat();
            const auto aspect = static_cast<float> (logoImage.getWidth()) / static_cast<float> (logoImage.getHeight());
            const auto height = logoArea.getHeight();
            const auto width = height * aspect;

            g.drawImage (logoImage,
                         { logoArea.getCentreX() - width * 0.5f, logoArea.getY(), width, height },
                         juce::RectanglePlacement::centred);
            area.removeFromTop (12);
        }

        g.setColour (ink);
        g.setFont (juce::FontOptions (17.0f, juce::Font::bold));
        g.drawText ("Buffle Audio Align", area.removeFromTop (24), juce::Justification::centred);

        g.setColour (brandAccent);
        g.setFont (juce::FontOptions (12.5f, juce::Font::bold));
        g.drawText ("v0.3.0 development build", area.removeFromTop (19), juce::Justification::centred);

        g.setColour (muted.withAlpha (0.9f));
        g.setFont (juce::FontOptions (12.0f));
        g.drawText ("Vocal-stack alignment and articulation cleanup",
                    area.removeFromTop (20), juce::Justification::centred);
        g.drawText ("Guide/Dub monitor - sidechain-ready nudge preview",
                    area.removeFromTop (20), juce::Justification::centred);

        area.removeFromTop (8);
        g.setColour (muted.withAlpha (0.65f));
        g.setFont (juce::FontOptions (10.5f));
        g.drawText ("buffleaudio.com - info@buffleaudio.com",
                    area.removeFromTop (18), juce::Justification::centred);
        g.drawText ("Built with JUCE for professional audio workflows",
                    area.removeFromTop (18), juce::Justification::centred);
    }

private:
    juce::Image logoImage;
};
}

class BufflePlugAnalyzerAudioProcessorEditor::AlignmentView final : public juce::Component
{
public:
    explicit AlignmentView (BufflePlugAnalyzerAudioProcessor& processorToUse)
        : processor (processorToUse)
    {
    }

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        drawRoundRect (g, bounds, panel, juce::Colour (0xff2a3542));

        auto content = getLocalBounds().reduced (20);
        auto title = content.removeFromTop (28);
        auto snapshot = processor.getAlignmentSnapshot();

        auto badge = title.removeFromRight (92).reduced (0, 3);
        drawRoundRect (g, badge.toFloat(), juce::Colour (0xff142320), brandAccent.withAlpha (0.34f));
        g.setColour (brandAccent);
        g.setFont (juce::FontOptions (10.5f, juce::Font::bold));
        g.drawText ("v0.3 PREVIEW", badge, juce::Justification::centred);

        g.setColour (ink);
        g.setFont (juce::FontOptions (18.5f, juce::Font::bold));
        g.drawText ("Guide / Dub Timing", title, juce::Justification::centredLeft);

        auto readouts = content.removeFromTop (42);
        const auto gap = 6;
        const auto pillWidth = (readouts.getWidth() - gap * 3) / 4;
        drawReadoutPill (g, readouts.removeFromLeft (pillWidth), "GUIDE", asPercent (snapshot.guideRms), guideColour);
        readouts.removeFromLeft (gap);
        drawReadoutPill (g, readouts.removeFromLeft (pillWidth), "DUB", asPercent (snapshot.dubRms), dubColour);
        readouts.removeFromLeft (gap);
        drawReadoutPill (g,
                         readouts.removeFromLeft (pillWidth),
                         "OFFSET",
                         snapshot.hasReliableOffset ? asSignedMs (snapshot.estimatedOffsetMs) : "-- ms",
                         alignedColour);
        readouts.removeFromLeft (gap);
        drawReadoutPill (g,
                         readouts,
                         "SUGGEST",
                         snapshot.hasReliableOffset ? juce::String (snapshot.suggestedNudgeMs, 1) + " ms" : "LISTEN",
                         warningColour);

        content.removeFromTop (8);
        drawPhraseHealthStrip (g, content.removeFromTop (30), snapshot);
        content.removeFromTop (8);

        auto nudgeLine = content.removeFromTop (18);
        g.setColour (muted.withAlpha (0.86f));
        g.setFont (juce::FontOptions (11.5f, juce::Font::bold));
        g.drawText ("Manual " + juce::String (snapshot.nudgeMs, 1) + " ms - "
                        + (snapshot.hasReliableOffset
                            ? "suggested safe nudge " + juce::String (snapshot.suggestedNudgeMs, 1) + " ms"
                            : "waiting for reliable Guide/Dub confidence"),
                    nudgeLine,
                    juce::Justification::centredLeft);
        content.removeFromTop (8);

        drawConfidence (g, content.removeFromTop (34), snapshot.offsetConfidence);
        content.removeFromTop (12);

        auto guideLane = content.removeFromTop ((content.getHeight() - 18) / 2);
        content.removeFromTop (18);
        auto dubLane = content;

        drawLane (g, guideLane, "Guide", guideColour, snapshot.guide);
        drawLane (g, dubLane, "Dub", dubColour, snapshot.dub);
        drawWarpPreview (g, guideLane.toFloat(), dubLane.toFloat(), snapshot.match);
    }

private:
    void drawConfidence (juce::Graphics& g, juce::Rectangle<int> area, float match)
    {
        g.setColour (juce::Colour (0xff26313d));
        g.fillRoundedRectangle (area.toFloat(), 6.0f);

        auto fill = area.reduced (2).toFloat();
        fill.setWidth (fill.getWidth() * juce::jlimit (0.0f, 1.0f, match));
        g.setColour (alignedColour.withAlpha (0.9f));
        g.fillRoundedRectangle (fill, 5.0f);

        g.setColour (ink);
        g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
        const auto status = match > 0.82f ? "locked"
                          : match > 0.48f ? "listening"
                          : "waiting for signal";
        g.drawText ("Offset confidence " + asPercent (match) + " - " + status,
                    area.reduced (12, 0), juce::Justification::centredLeft);
    }

    void drawLane (juce::Graphics& g,
                   juce::Rectangle<int> area,
                   const juce::String& title,
                   juce::Colour colour,
                   const std::array<float, 96>& values)
    {
        drawRoundRect (g, area.toFloat(), panelLight, juce::Colour (0xff303b49));

        auto labelArea = area.removeFromLeft (72).reduced (12);
        g.setColour (colour);
        g.setFont (juce::FontOptions (15.0f, juce::Font::bold));
        g.drawText (title, labelArea, juce::Justification::centredLeft);

        auto waveform = area.reduced (16, 14).toFloat();
        g.setColour (juce::Colour (0xff242b34));
        for (int marker = 1; marker < 7; ++marker)
        {
            const auto x = waveform.getX() + waveform.getWidth() * static_cast<float> (marker) / 7.0f;
            g.drawVerticalLine (static_cast<int> (x), waveform.getY(), waveform.getBottom());
        }

        g.setColour (juce::Colour (0xff343d48));
        g.drawHorizontalLine (static_cast<int> (waveform.getCentreY()), waveform.getX(), waveform.getRight());

        juce::Path path;
        for (size_t i = 0; i < values.size(); ++i)
        {
            const auto x = juce::jmap (static_cast<float> (i), 0.0f, static_cast<float> (values.size() - 1),
                                       waveform.getX(), waveform.getRight());
            const auto y = waveform.getCentreY() - (values[i] * waveform.getHeight() * 0.45f);

            if (i == 0)
                path.startNewSubPath (x, y);
            else
                path.lineTo (x, y);
        }

        g.setColour (colour.withAlpha (0.92f));
        g.strokePath (path, juce::PathStrokeType (2.0f));

        const auto peak = *std::max_element (values.begin(), values.end());
        drawHorizontalMeter (g,
                             area.removeFromRight (54).withSizeKeepingCentre (46, 8),
                             peak,
                             colour);

        for (int marker = 1; marker < 4; ++marker)
        {
            const auto x = waveform.getX() + waveform.getWidth() * static_cast<float> (marker) / 4.0f;
            g.setColour (colour.withAlpha (0.22f));
            g.drawVerticalLine (static_cast<int> (x), waveform.getY(), waveform.getBottom());
        }
    }

    void drawWarpPreview (juce::Graphics& g, juce::Rectangle<float> guideLane, juce::Rectangle<float> dubLane, float match)
    {
        g.setColour (alignedColour.withAlpha (0.14f + match * 0.18f));

        for (int i = 1; i < 7; ++i)
        {
            const auto guideX = guideLane.getX() + guideLane.getWidth() * static_cast<float> (i) / 7.0f;
            const auto offset = std::sin (static_cast<float> (i) * 0.9f) * (1.0f - match) * 24.0f;
            const auto dubX = dubLane.getX() + dubLane.getWidth() * static_cast<float> (i) / 7.0f + offset;
            g.drawLine (guideX, guideLane.getBottom() - 8.0f, dubX, dubLane.getY() + 8.0f, 1.0f);
        }
    }

    BufflePlugAnalyzerAudioProcessor& processor;
};

BufflePlugAnalyzerAudioProcessorEditor::BufflePlugAnalyzerAudioProcessorEditor (BufflePlugAnalyzerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    alignmentView = std::make_unique<AlignmentView> (audioProcessor);
    addAndMakeVisible (*alignmentView);
    loadBrandAssets();

    configureSlider (tightnessSlider,
                     tightnessLabel,
                     "Tightness",
                     "How closely the Dub should follow the Guide timing.",
                     true);
    configureSlider (naturalnessSlider,
                     naturalnessLabel,
                     "Naturalness",
                     "How much relaxed performance timing should survive alignment.",
                     true);
    configureSlider (consonantSlider,
                     consonantLabel,
                     "Consonant Tamer",
                     "How strongly unmatched Dub consonant transients should be softened while Guide-matched attacks stay intact.",
                     true);
    configureSlider (nudgeSlider,
                     nudgeLabel,
                     "Nudge",
                     "Manual delay preview in milliseconds.",
                     false);
    configureSlider (auditionSlider,
                     auditionLabel,
                     "Preview Level",
                     "Monitoring level for the alignment preview path.",
                     true);
    configureSlider (guideBlendSlider,
                     guideBlendLabel,
                     "Guide Blend",
                     "Monitor a little Guide against the Dub while checking the timing relationship.",
                     true);
    configureSlider (stereoFocusSlider,
                     stereoFocusLabel,
                     "Stereo Focus",
                     "Preview how tightly doubled layers should hold the stereo image.",
                     true);

    for (auto* button : { &captureButton, &analyzeButton, &alignButton, &applySuggestedButton })
    {
        addAndMakeVisible (*button);
        button->setColour (juce::TextButton::buttonColourId, panelLight);
        button->setColour (juce::TextButton::buttonOnColourId, alignedColour);
        button->setColour (juce::TextButton::textColourOffId, ink);
        button->setColour (juce::TextButton::textColourOnId, background);
        button->setColour (juce::TextButton::buttonOnColourId, brandAccent);
    }

    captureButton.setTooltip ("Prepare the Guide/Dub capture pass.");
    analyzeButton.setTooltip ("Estimate timing confidence from the current monitoring window.");
    alignButton.setTooltip ("Preview the current manual or automatic alignment move.");
    applySuggestedButton.setTooltip ("Apply the current confidence-gated suggested nudge to the Nudge control.");
    applySuggestedButton.setEnabled (false);
    captureButton.onClick = [this] { showTransientStatus ("Capture pass armed - route Guide sidechain, then play the phrase."); };
    analyzeButton.onClick = [this] { showTransientStatus ("Analyzing monitor window - watching Guide/Dub timing confidence."); };
    alignButton.onClick = [this] { showTransientStatus ("Alignment preview active - adjust Nudge, Tightness, and Naturalness."); };
    applySuggestedButton.onClick = [this] { applySuggestedNudge(); };

    addAndMakeVisible (statusLabel);
    statusLabel.setColour (juce::Label::textColourId, muted);
    statusLabel.setJustificationType (juce::Justification::centredLeft);
    statusLabel.setText (audioProcessor.getWorkflowStatus(), juce::dontSendNotification);

    auto& state = audioProcessor.getValueTreeState();
    tightnessAttachment = std::make_unique<SliderAttachment> (state, "tightness", tightnessSlider);
    naturalnessAttachment = std::make_unique<SliderAttachment> (state, "naturalness", naturalnessSlider);
    consonantAttachment = std::make_unique<SliderAttachment> (state, "consonantLevel", consonantSlider);
    nudgeAttachment = std::make_unique<SliderAttachment> (state, "nudge", nudgeSlider);
    auditionAttachment = std::make_unique<SliderAttachment> (state, "audition", auditionSlider);
    guideBlendAttachment = std::make_unique<SliderAttachment> (state, "guideBlend", guideBlendSlider);
    stereoFocusAttachment = std::make_unique<SliderAttachment> (state, "stereoFocus", stereoFocusSlider);

    setResizable (true, true);
    setResizeLimits (820, 680, 1220, 820);
    setSize (1040, 700);
    startTimerHz (24);
}

BufflePlugAnalyzerAudioProcessorEditor::~BufflePlugAnalyzerAudioProcessorEditor()
{
    stopTimer();
}

void BufflePlugAnalyzerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (background);

    auto bounds = getLocalBounds();
    auto headerBounds = bounds.removeFromTop (76);
    g.setColour (header);
    g.fillRect (headerBounds);
    g.setColour (border);
    g.drawHorizontalLine (headerBounds.getBottom(), 0.0f, static_cast<float> (getWidth()));
    drawHeader (g, headerBounds.reduced (28, 18));
    drawWorkflowRail (g, removeRail (bounds));

    auto controls = bounds.removeFromRight (238).reduced (8, 10);
    drawRoundRect (g, controls.toFloat(), panel, border);

    g.setColour (ink);
    g.setFont (juce::FontOptions (16.0f, juce::Font::bold));
    g.drawText ("Control Room", controls.removeFromTop (42).reduced (16, 0), juce::Justification::centredLeft);

    auto controlRows = controls.reduced (16, 12);
    for (int i = 0; i < 7; ++i)
    {
        auto row = controlRows.removeFromTop (62);
        drawRoundRect (g, row.toFloat(), juce::Colour (0xff111116), juce::Colour (0xff202028));
        controlRows.removeFromTop (7);
    }
}

void BufflePlugAnalyzerAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto headerArea = bounds.removeFromTop (76).reduced (28, 18);
    juce::ignoreUnused (headerArea);

    auto rail = bounds.removeFromLeft (168).reduced (22, 8);
    auto railButton = rail.removeFromBottom (160);
    captureButton.setBounds (railButton.removeFromTop (32));
    railButton.removeFromTop (10);
    analyzeButton.setBounds (railButton.removeFromTop (32));
    railButton.removeFromTop (10);
    alignButton.setBounds (railButton.removeFromTop (32));
    railButton.removeFromTop (10);
    applySuggestedButton.setBounds (railButton.removeFromTop (32));

    auto controls = bounds.removeFromRight (238).reduced (24, 62);
    const auto sliderHeight = 62;

    auto placeSlider = [&controls] (juce::Slider& slider, juce::Label& label)
    {
        auto row = controls.removeFromTop (sliderHeight);
        label.setBounds (row.removeFromTop (22));
        slider.setBounds (row);
        controls.removeFromTop (8);
    };

    placeSlider (tightnessSlider, tightnessLabel);
    placeSlider (naturalnessSlider, naturalnessLabel);
    placeSlider (consonantSlider, consonantLabel);
    placeSlider (nudgeSlider, nudgeLabel);
    placeSlider (auditionSlider, auditionLabel);
    placeSlider (guideBlendSlider, guideBlendLabel);
    placeSlider (stereoFocusSlider, stereoFocusLabel);

    statusLabel.setBounds (bounds.removeFromBottom (38).reduced (18, 0));
    alignmentView->setBounds (bounds.reduced (8, 10));
}

void BufflePlugAnalyzerAudioProcessorEditor::timerCallback()
{
    const auto snapshot = audioProcessor.getAlignmentSnapshot();
    applySuggestedButton.setEnabled (snapshot.hasReliableOffset && snapshot.suggestedNudgeMs > 0.05f);

    if (transientStatusFrames > 0)
    {
        --transientStatusFrames;
        statusLabel.setText (transientStatus, juce::dontSendNotification);
    }
    else
    {
        statusLabel.setText (audioProcessor.getWorkflowStatus(), juce::dontSendNotification);
    }

    alignmentView->repaint();
    repaint();
}

void BufflePlugAnalyzerAudioProcessorEditor::configureSlider (juce::Slider& slider,
                                                              juce::Label& label,
                                                              const juce::String& text,
                                                              const juce::String& tooltip,
                                                              bool isPercentValue)
{
    addAndMakeVisible (slider);
    addAndMakeVisible (label);

    slider.setSliderStyle (juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 66, 22);
    slider.setColour (juce::Slider::trackColourId, juce::Colour (0xff5a6675));
    slider.setColour (juce::Slider::backgroundColourId, juce::Colour (0xff2a3440));
    slider.setColour (juce::Slider::thumbColourId, ink);
    slider.setColour (juce::Slider::textBoxTextColourId, ink);
    slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff101014));
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    slider.setTooltip (tooltip);

    if (isPercentValue)
    {
        slider.textFromValueFunction = [] (double value)
        {
            return juce::String (static_cast<int> (juce::roundToInt (value * 100.0))) + "%";
        };
    }
    else
    {
        slider.textFromValueFunction = [] (double value)
        {
            return juce::String (value, 1) + " ms";
        };
    }

    label.setText (text, juce::dontSendNotification);
    label.setColour (juce::Label::textColourId, muted);
    label.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    label.setTooltip (tooltip);
}

void BufflePlugAnalyzerAudioProcessorEditor::drawWorkflowRail (juce::Graphics& g, juce::Rectangle<int> area)
{
    drawRoundRect (g, area.toFloat(), panel, border);

    const char* steps[] = { "Route", "Listen", "Preview", "Tame", "Print" };
    const auto snapshot = audioProcessor.getAlignmentSnapshot();
    const auto currentStep = ! snapshot.guideFromSidechain ? 0
                           : ! snapshot.hasReliableOffset ? 1
                           : snapshot.suggestedNudgeMs > 0.05f ? 2
                           : 3;
    auto row = area.reduced (14, 18);

    for (int i = 0; i < 5; ++i)
    {
        auto stepArea = row.removeFromTop (34);
        auto number = stepArea.removeFromLeft (26).withSizeKeepingCentre (22, 22);
        const auto isCurrent = i == currentStep;
        const auto isDone = i < currentStep;

        if (i < 4)
        {
            g.setColour ((isDone ? alignedColour : border).withAlpha (0.65f));
            g.drawVerticalLine (number.getCentreX(),
                                static_cast<float> (number.getBottom()),
                                static_cast<float> (number.getBottom() + 14));
        }

        g.setColour (isCurrent ? alignedColour.withAlpha (0.22f) : panelLight);
        g.fillEllipse (number.toFloat());
        g.setColour (isDone ? alignedColour : isCurrent ? alignedColour : muted.withAlpha (0.56f));
        g.drawEllipse (number.toFloat(), 1.2f);

        g.setFont (juce::FontOptions (11.5f, juce::Font::bold));
        g.drawText (juce::String (i + 1), number, juce::Justification::centred);

        if (isCurrent)
        {
            g.setColour (alignedColour.withAlpha (0.14f));
            g.fillRoundedRectangle (stepArea.toFloat(), 6.0f);
        }

        g.setColour (isDone || isCurrent ? ink : muted);
        g.setFont (juce::FontOptions (13.5f, isCurrent ? juce::Font::bold : juce::Font::plain));
        g.drawText (steps[i], stepArea.reduced (8, 0), juce::Justification::centredLeft);
        row.removeFromTop (10);
    }
}

void BufflePlugAnalyzerAudioProcessorEditor::drawHeader (juce::Graphics& g, juce::Rectangle<int> area)
{
    auto brandArea = area.removeFromLeft (205);
    brandLogoBounds = brandArea.withTrimmedRight (18);

    if (buffleAudioLogo.isValid())
    {
        const auto logoArea = brandLogoBounds.toFloat();
        const auto aspect = static_cast<float> (buffleAudioLogo.getWidth()) / static_cast<float> (buffleAudioLogo.getHeight());
        const auto height = juce::jmin (logoArea.getHeight(), 36.0f);
        const auto width = height * aspect;

        g.setOpacity (0.9f);
        g.drawImage (buffleAudioLogo,
                     { logoArea.getX(), logoArea.getCentreY() - height * 0.5f, width, height },
                     juce::RectanglePlacement::stretchToFit);
        g.setOpacity (1.0f);
    }
    else
    {
        g.setColour (brandAccent);
        g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
        g.drawText ("Buffle Audio", brandArea, juce::Justification::centredLeft);
    }

    area.removeFromLeft (8);

    g.setColour (ink);
    g.setFont (juce::FontOptions (30.0f, juce::Font::bold));
    g.drawText ("Align", area.removeFromLeft (160), juce::Justification::centredLeft);

    auto chip = area.removeFromRight (320).withSizeKeepingCentre (300, 28);
    const auto snapshot = audioProcessor.getAlignmentSnapshot();
    const auto sourceColour = phraseHealthColour (snapshot);
    drawRoundRect (g, chip.toFloat(), juce::Colour (0xff1d2731), sourceColour.withAlpha (0.35f));
    g.setColour (sourceColour);
    g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    g.drawText (describePhraseHealth (snapshot) + juce::String (" - ") + asPercent (snapshot.offsetConfidence),
                chip,
                juce::Justification::centred);
}

void BufflePlugAnalyzerAudioProcessorEditor::mouseUp (const juce::MouseEvent& event)
{
    if (brandLogoBounds.contains (event.getPosition()))
        showAboutBox();
}

void BufflePlugAnalyzerAudioProcessorEditor::mouseMove (const juce::MouseEvent& event)
{
    setMouseCursor (brandLogoBounds.contains (event.getPosition())
                        ? juce::MouseCursor::PointingHandCursor
                        : juce::MouseCursor::NormalCursor);
}

void BufflePlugAnalyzerAudioProcessorEditor::loadBrandAssets()
{
    auto source = juce::ImageCache::getFromMemory (BinaryData::buffleaudiologo_png,
                                                   BinaryData::buffleaudiologo_pngSize);
    if (source.isValid())
        buffleAudioLogo = recolourForDarkTheme (source, muted);
}

void BufflePlugAnalyzerAudioProcessorEditor::showAboutBox()
{
    auto content = std::make_unique<AboutComponent> (buffleAudioLogo);
    juce::CallOutBox::launchAsynchronously (std::move (content), brandLogoBounds, this);
}

void BufflePlugAnalyzerAudioProcessorEditor::showTransientStatus (const juce::String& message)
{
    transientStatus = message;
    transientStatusFrames = 96;
    statusLabel.setText (transientStatus, juce::dontSendNotification);
}

void BufflePlugAnalyzerAudioProcessorEditor::applySuggestedNudge()
{
    const auto snapshot = audioProcessor.getAlignmentSnapshot();

    if (! snapshot.hasReliableOffset)
    {
        showTransientStatus ("Apply Nudge unavailable - keep playing until Guide/Dub confidence locks.");
        return;
    }

    if (snapshot.suggestedNudgeMs <= 0.05f)
    {
        showTransientStatus ("No positive delay nudge needed - keep the current Dub timing or use manual Nudge.");
        return;
    }

    if (auto* parameter = audioProcessor.getValueTreeState().getParameter ("nudge"))
    {
        parameter->beginChangeGesture();
        parameter->setValueNotifyingHost (parameter->convertTo0to1 (snapshot.suggestedNudgeMs));
        parameter->endChangeGesture();
        showTransientStatus ("Applied suggested nudge: " + juce::String (snapshot.suggestedNudgeMs, 1) + " ms.");
    }
}

juce::Image BufflePlugAnalyzerAudioProcessorEditor::recolourForDarkTheme (const juce::Image& source,
                                                                          juce::Colour tint)
{
    const int targetHeight = 256;
    const int targetWidth = juce::roundToInt (static_cast<float> (targetHeight)
                                            * static_cast<float> (source.getWidth())
                                            / static_cast<float> (source.getHeight()));

    auto scaled = source.rescaled (targetWidth, targetHeight, juce::Graphics::highResamplingQuality);
    juce::Image result (juce::Image::ARGB, targetWidth, targetHeight, true);

    juce::Image::BitmapData srcData (scaled, juce::Image::BitmapData::readOnly);
    juce::Image::BitmapData dstData (result, juce::Image::BitmapData::writeOnly);

    for (int y = 0; y < targetHeight; ++y)
        for (int x = 0; x < targetWidth; ++x)
            dstData.setPixelColour (x, y, tint.withAlpha (srcData.getPixelColour (x, y).getAlpha()));

    return result;
}

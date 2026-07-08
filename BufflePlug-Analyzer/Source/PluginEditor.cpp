#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"
#include "DSP/StackRolePreset.h"

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

juce::String describeNudgeMove (float value)
{
    if (value > 0.05f)
        return "Delay " + asSignedMs (value);

    if (value < -0.05f)
        return "Advance " + asSignedMs (value);

    return "No nudge";
}

juce::String describeNudgeMoveCompact (float value)
{
    if (value > 0.05f)
        return "Delay " + juce::String (std::abs (value), 1);

    if (value < -0.05f)
        return "Adv " + juce::String (std::abs (value), 1);

    return "No nudge";
}

juce::String describeApplyMove (float value)
{
    if (value > 0.05f)
        return "Delay Dub " + asSignedMs (value);

    if (value < -0.05f)
        return "Advance Dub " + asSignedMs (value);

    return "No Nudge";
}

juce::String describePhraseHealth (const BufflePlugAnalyzerAudioProcessor::AlignmentSnapshot& snapshot)
{
    return juce::String (buffle::align::getPhraseHealthLabel (snapshot.phraseHealth));
}

juce::Colour phraseHealthColour (const BufflePlugAnalyzerAudioProcessor::AlignmentSnapshot& snapshot)
{
    switch (snapshot.phraseHealth)
    {
        case buffle::align::PhraseHealth::safeNudge:
            return brandAccent;
        case buffle::align::PhraseHealth::clean:
            return alignedColour;
        case buffle::align::PhraseHealth::watchNaturalness:
        case buffle::align::PhraseHealth::changedMaterial:
            return warningColour;
        case buffle::align::PhraseHealth::watchArticulation:
        case buffle::align::PhraseHealth::tooMuch:
            return dubColour;
        case buffle::align::PhraseHealth::route:
        case buffle::align::PhraseHealth::listen:
        default:
            return warningColour;
    }
}

juce::String describePhraseHealthAdviceCompact (const BufflePlugAnalyzerAudioProcessor::AlignmentSnapshot& snapshot)
{
    switch (snapshot.phraseHealth)
    {
        case buffle::align::PhraseHealth::route: return "Route Guide";
        case buffle::align::PhraseHealth::listen: return "Keep Listening";
        case buffle::align::PhraseHealth::safeNudge: return "Apply Then A/B";
        case buffle::align::PhraseHealth::clean: return "Ready To Print";
        case buffle::align::PhraseHealth::watchNaturalness: return "Check Diff";
        case buffle::align::PhraseHealth::watchArticulation: return "A/B Tamer";
        case buffle::align::PhraseHealth::changedMaterial: return "Review Diff";
        case buffle::align::PhraseHealth::tooMuch: return "Loosen Move";
        default: return "Keep Listening";
    }
}

juce::String describeNextActionTitle (const BufflePlugAnalyzerAudioProcessor::AlignmentSnapshot& snapshot)
{
    if (snapshot.trustState == buffle::align::TrustState::routeGuide)
        return "Route";

    if (snapshot.trustState == buffle::align::TrustState::guideQuiet
     || snapshot.trustState == buffle::align::TrustState::dubQuiet
     || snapshot.trustState == buffle::align::TrustState::listening)
        return "Listen";

    if (snapshot.trustState == buffle::align::TrustState::delayDub
     || snapshot.trustState == buffle::align::TrustState::advanceDub)
        return "Apply";

    if (snapshot.phraseHealth == buffle::align::PhraseHealth::tooMuch
     || snapshot.phraseHealth == buffle::align::PhraseHealth::watchNaturalness
     || snapshot.phraseHealth == buffle::align::PhraseHealth::watchArticulation
     || snapshot.phraseHealth == buffle::align::PhraseHealth::changedMaterial)
        return "A/B";

    if (snapshot.removedMaterial > 0.04f)
        return "A/B";

    return "Print";
}

juce::String describeNextActionBody (const BufflePlugAnalyzerAudioProcessor::AlignmentSnapshot& snapshot)
{
    if (snapshot.trustState == buffle::align::TrustState::routeGuide
     || snapshot.trustState == buffle::align::TrustState::guideQuiet
     || snapshot.trustState == buffle::align::TrustState::dubQuiet
     || snapshot.trustState == buffle::align::TrustState::listening)
    {
        switch (snapshot.trustState)
        {
            case buffle::align::TrustState::routeGuide: return "Route Guide sidechain, then play the phrase.";
            case buffle::align::TrustState::guideQuiet: return "Raise Guide level, then play the phrase.";
            case buffle::align::TrustState::dubQuiet: return "Raise Dub level before nudge or tamer.";
            case buffle::align::TrustState::listening: return "Keep playback rolling until timing locks.";
            case buffle::align::TrustState::locked:
            case buffle::align::TrustState::delayDub:
            case buffle::align::TrustState::advanceDub:
                break;
        }
    }

    if (snapshot.trustState == buffle::align::TrustState::delayDub
     || snapshot.trustState == buffle::align::TrustState::advanceDub)
        return "Apply " + asSignedMs (snapshot.suggestedNudgeMs) + ", then A/B Aligned vs Diff.";

    if (snapshot.phraseHealth == buffle::align::PhraseHealth::tooMuch)
        return "Loosen role, lower Tamer, or reduce nudge.";

    if (snapshot.phraseHealth == buffle::align::PhraseHealth::watchArticulation)
        return "A/B Tamer before printing consonants.";

    if (snapshot.phraseHealth == buffle::align::PhraseHealth::watchNaturalness)
        return "Check Diff before printing this move.";

    if (snapshot.phraseHealth == buffle::align::PhraseHealth::changedMaterial)
        return "Timing locked. Check Diff before print.";

    return "Save Report for notes, or adjust Stack Role.";
}

juce::String describeSuggestPill (const BufflePlugAnalyzerAudioProcessor::AlignmentSnapshot& snapshot)
{
    switch (snapshot.trustState)
    {
        case buffle::align::TrustState::routeGuide: return "Route Guide";
        case buffle::align::TrustState::guideQuiet: return "Raise Guide";
        case buffle::align::TrustState::dubQuiet: return "Raise Dub";
        case buffle::align::TrustState::listening: return "Play Phrase";
        case buffle::align::TrustState::locked: return "No Move";
        case buffle::align::TrustState::delayDub:
        case buffle::align::TrustState::advanceDub: return describeNudgeMoveCompact (snapshot.suggestedNudgeMs);
        default: return "Play Phrase";
    }
}

juce::String describeHeaderTrust (const BufflePlugAnalyzerAudioProcessor::AlignmentSnapshot& snapshot)
{
    switch (snapshot.trustState)
    {
        case buffle::align::TrustState::routeGuide: return "Route Guide";
        case buffle::align::TrustState::guideQuiet: return "Guide Quiet";
        case buffle::align::TrustState::dubQuiet: return "Dub Quiet";
        case buffle::align::TrustState::listening: return "Listening - " + asPercent (snapshot.offsetConfidence);
        case buffle::align::TrustState::locked: return "Locked - " + asPercent (snapshot.offsetConfidence) + " confidence";
        case buffle::align::TrustState::delayDub:
        case buffle::align::TrustState::advanceDub:
            return describeNudgeMove (snapshot.suggestedNudgeMs) + " - " + asPercent (snapshot.offsetConfidence);
        default: return describePhraseHealth (snapshot);
    }
}

juce::String describeArticulationRiskCompact (buffle::align::ArticulationRisk risk)
{
    switch (risk)
    {
        case buffle::align::ArticulationRisk::clean: return "Clean";
        case buffle::align::ArticulationRisk::watch: return "Watch - A/B Tamer";
        case buffle::align::ArticulationRisk::collision: return "Collision - loosen role";
        case buffle::align::ArticulationRisk::listening:
        default: return "Listening";
    }
}

juce::String describeNaturalnessRiskCompact (buffle::align::NaturalnessRisk risk)
{
    switch (risk)
    {
        case buffle::align::NaturalnessRisk::checkDifference: return "Check Diff - A/B before print";
        case buffle::align::NaturalnessRisk::tooMuch: return "Too Much - loosen role or Tamer";
        case buffle::align::NaturalnessRisk::safe:
        default: return "Natural - inside guardrail";
    }
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
    g.drawFittedText (describePhraseHealth (snapshot) + " - " + describePhraseHealthAdviceCompact (snapshot),
                      text,
                      juce::Justification::centredLeft,
                      1);
}

void drawChangedMaterialStrip (juce::Graphics& g,
                               juce::Rectangle<int> area,
                               const BufflePlugAnalyzerAudioProcessor::AlignmentSnapshot& snapshot)
{
    drawRoundRect (g, area.toFloat(), juce::Colour (0xff16151a), dubColour.withAlpha (0.26f));

    auto text = area.reduced (12, 0);
    g.setColour (dubColour.brighter (0.16f));
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    g.drawText ("CHANGED MATERIAL", text.removeFromLeft (134), juce::Justification::centredLeft);

    auto meterArea = text.removeFromRight (120).withSizeKeepingCentre (104, 8);
    drawHorizontalMeter (g, meterArea, snapshot.removedMaterial, dubColour.brighter (0.2f));

    g.setColour (ink);
    g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    g.drawText (asPercent (snapshot.removedMaterial) + " all / "
                    + asPercent (snapshot.consonantRemovedMaterial) + " tamer",
                text,
                juce::Justification::centredLeft);
}

void drawNaturalnessRiskStrip (juce::Graphics& g,
                               juce::Rectangle<int> area,
                               const BufflePlugAnalyzerAudioProcessor::AlignmentSnapshot& snapshot)
{
    const auto risk = snapshot.naturalnessRisk;
    const auto colour = risk == buffle::align::NaturalnessRisk::safe ? alignedColour
                      : risk == buffle::align::NaturalnessRisk::checkDifference ? warningColour
                      : dubColour;

    drawRoundRect (g, area.toFloat(), juce::Colour (0xff14181c), colour.withAlpha (0.28f));

    auto text = area.reduced (12, 0);
    g.setColour (colour);
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    g.drawText ("NATURALNESS", text.removeFromLeft (112), juce::Justification::centredLeft);

    g.setColour (ink);
    g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    g.drawText (describeNaturalnessRiskCompact (risk), text, juce::Justification::centredLeft);
}

void drawArticulationRiskStrip (juce::Graphics& g,
                                juce::Rectangle<int> area,
                                const BufflePlugAnalyzerAudioProcessor::AlignmentSnapshot& snapshot)
{
    const auto risk = snapshot.articulationRisk;
    const auto colour = risk == buffle::align::ArticulationRisk::clean ? alignedColour
                      : risk == buffle::align::ArticulationRisk::watch ? warningColour
                      : risk == buffle::align::ArticulationRisk::collision ? dubColour
                      : muted;

    drawRoundRect (g, area.toFloat(), juce::Colour (0xff14171d), colour.withAlpha (0.28f));

    auto text = area.reduced (12, 0);
    g.setColour (colour);
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    const auto labelWidth = area.getWidth() < 440 ? 86 : 112;
    g.drawText (area.getWidth() < 440 ? "ARTIC." : "ARTICULATION",
                text.removeFromLeft (labelWidth),
                juce::Justification::centredLeft);

    if (area.getWidth() >= 440)
    {
        auto meterArea = text.removeFromRight (124).withSizeKeepingCentre (108, 8);
        drawHorizontalMeter (g, meterArea, snapshot.articulationRiskScore, colour);
    }

    g.setColour (ink);
    g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    g.drawText (describeArticulationRiskCompact (risk), text, juce::Justification::centredLeft);
}

void drawNextActionCard (juce::Graphics& g,
                         juce::Rectangle<int> area,
                         const BufflePlugAnalyzerAudioProcessor::AlignmentSnapshot& snapshot)
{
    const auto accent = phraseHealthColour (snapshot);
    drawRoundRect (g, area.toFloat(), juce::Colour (0xff11161b), accent.withAlpha (0.28f));

    auto text = area.reduced (12, 8);
    auto badge = text.removeFromRight (82).withSizeKeepingCentre (78, 24);
    drawRoundRect (g, badge.toFloat(), accent.withAlpha (0.14f), accent.withAlpha (0.42f));
    g.setColour (accent);
    g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
    g.drawText (describeNextActionTitle (snapshot), badge, juce::Justification::centred);

    g.setColour (muted.withAlpha (0.86f));
    g.setFont (juce::FontOptions (10.5f, juce::Font::bold));
    g.drawText ("NEXT BEST MOVE", text.removeFromTop (14), juce::Justification::centredLeft);

    g.setColour (ink);
    g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    g.drawFittedText (describeNextActionBody (snapshot), text, juce::Justification::centredLeft, 2, 1.0f);
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
        g.drawText ("v0.3.0 macOS preview", area.removeFromTop (19), juce::Justification::centred);

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
        g.drawText ("TRUST METER", badge, juce::Justification::centred);

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
                         describeSuggestPill (snapshot),
                         warningColour);

        content.removeFromTop (8);
        drawPhraseHealthStrip (g, content.removeFromTop (34), snapshot);
        content.removeFromTop (8);
        drawNextActionCard (g, content.removeFromTop (58), snapshot);
        content.removeFromTop (8);

        drawChangedMaterialStrip (g, content.removeFromTop (22), snapshot);
        content.removeFromTop (8);
        drawNaturalnessRiskStrip (g, content.removeFromTop (28), snapshot);
        content.removeFromTop (8);
        drawArticulationRiskStrip (g, content.removeFromTop (26), snapshot);
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

        if (peak < 0.01f)
        {
            g.setColour (muted.withAlpha (0.62f));
            g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
            g.drawText ("No " + title + " signal yet",
                        waveform.toNearestInt().reduced (8, 0),
                        juce::Justification::centred);
        }

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
                     "Nudge Dub",
                     "Move the Dub earlier or later relative to the Guide. Negative advances with host latency compensation; positive delays.",
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

    for (auto* button : { &captureButton, &analyzeButton, &alignButton, &applySuggestedButton,
                          &reportButton, &saveReportButton,
                          &originalModeButton, &alignedModeButton, &differenceModeButton, &tamerModeButton })
    {
        addAndMakeVisible (*button);
        button->setColour (juce::TextButton::buttonColourId, panelLight);
        button->setColour (juce::TextButton::buttonOnColourId, alignedColour);
        button->setColour (juce::TextButton::textColourOffId, ink);
        button->setColour (juce::TextButton::textColourOnId, background);
        button->setColour (juce::TextButton::buttonOnColourId, brandAccent);
    }

    captureButton.setTooltip ("Arm the listening pass: route Guide sidechain, then play the phrase.");
    analyzeButton.setTooltip ("Check timing confidence from the current monitoring window.");
    alignButton.setTooltip ("Preview the current manual or automatic alignment move.");
    applySuggestedButton.setTooltip ("Apply the current confidence-gated timing correction to the Nudge Dub control.");
    reportButton.setTooltip ("Copy a session handoff report with phrase health, offset, confidence, role, and current controls.");
    saveReportButton.setTooltip ("Save the current alignment report as a text file for session notes or tester feedback.");
    originalModeButton.setTooltip ("Monitor the unprocessed Dub.");
    alignedModeButton.setTooltip ("Monitor the current nudge and Consonant Tamer path.");
    differenceModeButton.setTooltip ("Monitor all timing and tamer material changed by the preview path.");
    tamerModeButton.setTooltip ("Solo only the consonant material reduced by Consonant Tamer.");
    applySuggestedButton.setEnabled (false);
    captureButton.onClick = [this] { setWorkflowIntent (buffle::align::WorkflowIntent::armedListen,
                                                         "Listen armed - route Guide sidechain, then play the phrase."); };
    analyzeButton.onClick = [this] { setWorkflowIntent (buffle::align::WorkflowIntent::checkingTiming,
                                                        "Checking timing - watching Guide/Dub confidence."); };
    alignButton.onClick = [this] { setWorkflowIntent (buffle::align::WorkflowIntent::previewing,
                                                      "Previewing alignment - A/B Aligned, Diff, and Tamer."); };
    applySuggestedButton.onClick = [this] { applySuggestedNudge(); };
    reportButton.onClick = [this] { copyAlignmentReport(); };
    saveReportButton.onClick = [this] { saveAlignmentReport(); };
    originalModeButton.onClick = [this] { setPreviewMode (0); };
    alignedModeButton.onClick = [this] { setPreviewMode (1); };
    differenceModeButton.onClick = [this] { setPreviewMode (2); };
    tamerModeButton.onClick = [this] { setPreviewMode (3); };
    updatePreviewModeButtons();

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

    addAndMakeVisible (stackRoleBox);
    stackRoleBox.addItem ("Manual", 1);
    stackRoleBox.addItem ("Double Tight", 2);
    stackRoleBox.addItem ("Choir Natural", 3);
    stackRoleBox.addItem ("Rap Stack", 4);
    stackRoleBox.addItem ("ADR Loose", 5);
    stackRoleBox.setTooltip ("Stack Role: choose the job of this layer. Presets set timing feel, consonant cleanup, Guide Blend, and Stereo Focus.");
    stackRoleBox.setColour (juce::ComboBox::backgroundColourId, panelLight);
    stackRoleBox.setColour (juce::ComboBox::outlineColourId, border);
    stackRoleBox.setColour (juce::ComboBox::textColourId, ink);
    stackRoleBox.setColour (juce::ComboBox::arrowColourId, brandAccent);
    stackRoleBox.onChange = [this]
    {
        const auto roleIndex = stackRoleBox.getSelectedId() - 1;
        if (roleIndex > 0)
            applyStackRolePreset (roleIndex);
    };
    updateStackRoleBox();

    auto markManual = [this]
    {
        if (! applyingStackRolePreset)
            markStackRoleManual();
    };
    tightnessSlider.onValueChange = markManual;
    naturalnessSlider.onValueChange = markManual;
    consonantSlider.onValueChange = markManual;
    guideBlendSlider.onValueChange = markManual;
    stereoFocusSlider.onValueChange = markManual;

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

    auto controlHeader = controls.removeFromTop (42).reduced (16, 0);
    g.setColour (ink);
    g.setFont (juce::FontOptions (13.8f, juce::Font::bold));
    g.drawText ("Control Room", controlHeader.withTrimmedRight (90), juce::Justification::centredLeft);

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
    auto railButton = rail.removeFromBottom (262);
    auto modeArea = railButton.removeFromTop (54);
    auto upperModes = modeArea.removeFromTop (24);
    originalModeButton.setBounds (upperModes.removeFromLeft ((upperModes.getWidth() - 6) / 2));
    upperModes.removeFromLeft (6);
    alignedModeButton.setBounds (upperModes);
    modeArea.removeFromTop (6);
    auto lowerModes = modeArea.removeFromTop (24);
    differenceModeButton.setBounds (lowerModes.removeFromLeft ((lowerModes.getWidth() - 6) / 2));
    lowerModes.removeFromLeft (6);
    tamerModeButton.setBounds (lowerModes);
    railButton.removeFromTop (10);
    captureButton.setBounds (railButton.removeFromTop (32));
    railButton.removeFromTop (10);
    analyzeButton.setBounds (railButton.removeFromTop (32));
    railButton.removeFromTop (10);
    alignButton.setBounds (railButton.removeFromTop (32));
    railButton.removeFromTop (10);
    applySuggestedButton.setBounds (railButton.removeFromTop (32));
    railButton.removeFromTop (10);
    auto reportRow = railButton.removeFromTop (32);
    reportButton.setBounds (reportRow.removeFromLeft ((reportRow.getWidth() - 6) / 2));
    reportRow.removeFromLeft (6);
    saveReportButton.setBounds (reportRow);

    auto controls = bounds.removeFromRight (238).reduced (24, 62);
    auto controlHeader = controls.removeFromTop (42).reduced (0, 7);
    stackRoleBox.setBounds (controlHeader.removeFromRight (126));
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
    const auto canApplyNudge = snapshot.hasReliableOffset && std::abs (snapshot.suggestedNudgeMs) > 0.05f;
    applySuggestedButton.setEnabled (canApplyNudge);
    applySuggestedButton.setButtonText (canApplyNudge ? describeApplyMove (snapshot.suggestedNudgeMs)
                                                       : snapshot.hasReliableOffset ? "No Nudge" : "Need Confidence");
    updatePreviewModeButtons();
    updateStackRoleBox();

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
    slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, isPercentValue ? 66 : 96, 22);
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
            if (value > 0.05)
                return "Delay " + juce::String (value, 1);

            if (value < -0.05)
                return "Adv " + juce::String (std::abs (value), 1);

            return juce::String ("No nudge");
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
    const auto currentWorkflowStep = buffle::align::assessWorkflowStep ({ workflowIntent,
                                                                          snapshot.trustState,
                                                                          snapshot.phraseHealth,
                                                                          snapshot.hasReliableOffset,
                                                                          snapshot.suggestedNudgeMs });
    const auto currentStep = static_cast<int> (currentWorkflowStep);
    const juce::String stepDetails[] =
    {
        snapshot.trustState == buffle::align::TrustState::routeGuide ? "Missing" : "Sidechain OK",
        snapshot.hasReliableOffset ? "confidence " + asPercent (snapshot.offsetConfidence)
                                   : snapshot.trustState == buffle::align::TrustState::guideQuiet ? "Guide quiet"
                                   : snapshot.trustState == buffle::align::TrustState::dubQuiet ? "Dub quiet"
                                   : "Locking",
        snapshot.trustState == buffle::align::TrustState::delayDub
            || snapshot.trustState == buffle::align::TrustState::advanceDub ? describeNudgeMove (snapshot.suggestedNudgeMs)
                                                                            : "A/B Diff",
        snapshot.phraseHealth == buffle::align::PhraseHealth::watchArticulation ? "Loosen"
            : snapshot.phraseHealth == buffle::align::PhraseHealth::watchNaturalness ? "A/B Tamer"
            : snapshot.phraseHealth == buffle::align::PhraseHealth::changedMaterial ? "Check Diff"
            : snapshot.phraseHealth == buffle::align::PhraseHealth::tooMuch ? "Review"
            : "Tune Tamer",
        snapshot.phraseHealth == buffle::align::PhraseHealth::clean ? "Ready" : "Save Report"
    };

    auto row = area.reduced (14, 16);
    auto railTitle = row.removeFromTop (24);
    g.setColour (muted.withAlpha (0.78f));
    g.setFont (juce::FontOptions (10.5f, juce::Font::bold));
    g.drawText ("SESSION FLOW / " + juce::String (buffle::align::getWorkflowIntentLabel (workflowIntent)),
                railTitle,
                juce::Justification::centredLeft);
    row.removeFromTop (6);

    for (int i = 0; i < 5; ++i)
    {
        auto stepArea = row.removeFromTop (42);
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
        auto stepText = stepArea.reduced (8, 0);
        g.drawText (steps[i], stepText.removeFromTop (20), juce::Justification::centredLeft);
        g.setColour (isDone || isCurrent ? muted.withAlpha (0.95f) : muted.withAlpha (0.58f));
        g.setFont (juce::FontOptions (10.5f, juce::Font::bold));
        g.drawText (stepDetails[i], stepText, juce::Justification::centredLeft);
        row.removeFromTop (7);
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
    g.drawText (describeHeaderTrust (snapshot),
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

void BufflePlugAnalyzerAudioProcessorEditor::setWorkflowIntent (buffle::align::WorkflowIntent intent, const juce::String& message)
{
    workflowIntent = intent;
    showTransientStatus (message);
}

void BufflePlugAnalyzerAudioProcessorEditor::applySuggestedNudge()
{
    const auto snapshot = audioProcessor.getAlignmentSnapshot();

    if (! snapshot.hasReliableOffset)
    {
        showTransientStatus ("Apply Nudge unavailable - " + juce::String (buffle::align::getTrustStateAdvice (snapshot.trustState)));
        return;
    }

    if (std::abs (snapshot.suggestedNudgeMs) <= 0.05f)
    {
        showTransientStatus ("No timing nudge needed - keep the current Dub timing or use manual Nudge.");
        return;
    }

    if (auto* parameter = audioProcessor.getValueTreeState().getParameter ("nudge"))
    {
        parameter->beginChangeGesture();
        parameter->setValueNotifyingHost (parameter->convertTo0to1 (snapshot.suggestedNudgeMs));
        parameter->endChangeGesture();
        setWorkflowIntent (buffle::align::WorkflowIntent::previewing,
                           "Applied suggested timing correction: " + asSignedMs (snapshot.suggestedNudgeMs) + ".");
    }
}

void BufflePlugAnalyzerAudioProcessorEditor::copyAlignmentReport()
{
    juce::SystemClipboard::copyTextToClipboard (audioProcessor.getAlignmentReportText());
    setWorkflowIntent (buffle::align::WorkflowIntent::printing, "Alignment report copied to clipboard.");
}

void BufflePlugAnalyzerAudioProcessorEditor::saveAlignmentReport()
{
    workflowIntent = buffle::align::WorkflowIntent::printing;

    const auto defaultFile = juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)
        .getChildFile ("Buffle-Audio-Align-Report-"
                       + juce::String (juce::Time::currentTimeMillis())
                       + ".txt");

    reportFileChooser = std::make_unique<juce::FileChooser> ("Save Buffle Audio Align report",
                                                             defaultFile,
                                                             "*.txt");

    const juce::Component::SafePointer<BufflePlugAnalyzerAudioProcessorEditor> safeThis (this);

    reportFileChooser->launchAsync (juce::FileBrowserComponent::saveMode
                                        | juce::FileBrowserComponent::canSelectFiles
                                        | juce::FileBrowserComponent::warnAboutOverwriting,
                                    [safeThis] (const juce::FileChooser& chooser)
                                    {
                                        if (safeThis == nullptr)
                                            return;

                                        auto file = chooser.getResult();
                                        if (file == juce::File())
                                        {
                                            safeThis->showTransientStatus ("Report save cancelled.");
                                            return;
                                        }

                                        if (! file.hasFileExtension ("txt"))
                                            file = file.withFileExtension ("txt");

                                        if (file.replaceWithText (safeThis->audioProcessor.getAlignmentReportText()))
                                            safeThis->setWorkflowIntent (buffle::align::WorkflowIntent::printing,
                                                                         "Alignment report saved: " + file.getFileName());
                                        else
                                            safeThis->showTransientStatus ("Could not save report - choose another folder.");
                                    });
}

void BufflePlugAnalyzerAudioProcessorEditor::setPreviewMode (int modeIndex)
{
    if (auto* parameter = audioProcessor.getValueTreeState().getParameter ("previewMode"))
    {
        parameter->beginChangeGesture();
        parameter->setValueNotifyingHost (parameter->convertTo0to1 (static_cast<float> (modeIndex)));
        parameter->endChangeGesture();

        const char* labels[] = { "Original", "Aligned", "Difference", "Tamer Removed" };
        const auto clampedMode = juce::jlimit (0, 3, modeIndex);
        const auto intent = clampedMode == 0 ? buffle::align::WorkflowIntent::checkingTiming
                                             : buffle::align::WorkflowIntent::previewing;
        setWorkflowIntent (intent, "Preview mode: " + juce::String (labels[clampedMode]) + ".");
    }
}

void BufflePlugAnalyzerAudioProcessorEditor::updatePreviewModeButtons()
{
    auto mode = 1;

    if (auto* value = audioProcessor.getValueTreeState().getRawParameterValue ("previewMode"))
        mode = juce::jlimit (0, 3, juce::roundToInt (value->load()));

    originalModeButton.setToggleState (mode == 0, juce::dontSendNotification);
    alignedModeButton.setToggleState (mode == 1, juce::dontSendNotification);
    differenceModeButton.setToggleState (mode == 2, juce::dontSendNotification);
    tamerModeButton.setToggleState (mode == 3, juce::dontSendNotification);
}

void BufflePlugAnalyzerAudioProcessorEditor::applyStackRolePreset (int roleIndex)
{
    const auto boundedRole = juce::jlimit (0, 4, roleIndex);
    const auto role = static_cast<buffle::align::StackRole> (boundedRole);
    const auto settings = buffle::align::getStackRoleSettings (role);
    auto& state = audioProcessor.getValueTreeState();

    auto setParameter = [&state] (const char* parameterId, float value)
    {
        if (auto* parameter = state.getParameter (parameterId))
        {
            parameter->beginChangeGesture();
            parameter->setValueNotifyingHost (parameter->convertTo0to1 (value));
            parameter->endChangeGesture();
        }
    };

    applyingStackRolePreset = true;

    setParameter ("tightness", settings.tightness);
    setParameter ("naturalness", settings.naturalness);
    setParameter ("consonantLevel", settings.consonantLevel);
    setParameter ("guideBlend", settings.guideBlend);
    setParameter ("stereoFocus", settings.stereoFocus);

    if (auto* parameter = state.getParameter ("stackRole"))
    {
        parameter->beginChangeGesture();
        parameter->setValueNotifyingHost (parameter->convertTo0to1 (static_cast<float> (boundedRole)));
        parameter->endChangeGesture();
    }

    applyingStackRolePreset = false;
    updateStackRoleBox();
    showTransientStatus ("Stack Role: " + juce::String (buffle::align::getStackRoleName (role)) + ".");
}

void BufflePlugAnalyzerAudioProcessorEditor::markStackRoleManual()
{
    if (auto* parameter = audioProcessor.getValueTreeState().getParameter ("stackRole"))
    {
        parameter->beginChangeGesture();
        parameter->setValueNotifyingHost (parameter->convertTo0to1 (0.0f));
        parameter->endChangeGesture();
    }

    updateStackRoleBox();
}

void BufflePlugAnalyzerAudioProcessorEditor::updateStackRoleBox()
{
    auto role = 0;

    if (auto* value = audioProcessor.getValueTreeState().getRawParameterValue ("stackRole"))
        role = juce::jlimit (0, 4, juce::roundToInt (value->load()));

    if (stackRoleBox.getSelectedId() != role + 1)
        stackRoleBox.setSelectedId (role + 1, juce::dontSendNotification);
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

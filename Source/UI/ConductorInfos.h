#pragma once

#include <JuceHeader.h>
#include "BKEngine.h"

//==============================================================================
/*
*/
class ConductorInfosUI : public ShapeShifterContent
{
public:
    ConductorInfosUI(const String& contentName);
    ~ConductorInfosUI();

    static ConductorInfosUI* create(const String& name) { return new ConductorInfosUI(name); }


};


class ConductorInfos  : public juce::Component
{
public:
    juce_DeclareSingleton(ConductorInfos, true);
    ConductorInfos();
    ~ConductorInfos() override;

    BKEngine* engine = nullptr;
    TextEditor targetId;
    FloatSliderUI* currentFade = nullptr;

    FloatSliderUI* nextHTPInDelay = nullptr;
    FloatSliderUI* nextHTPOutDelay = nullptr;
    FloatSliderUI* nextLTPDelay = nullptr;

    FloatSliderUI* nextHTPInFade = nullptr;
    FloatSliderUI* nextHTPOutFade = nullptr;
    FloatSliderUI* nextLTPFade = nullptr;


    void paint (juce::Graphics&) override;
    void resized() override;
    void linkFadeSlider();
    void linkSlidersTimings();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConductorInfos)
};

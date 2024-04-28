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


class ConductorInfos  : 
    public juce::Component,
    public Label::Listener
{
public:
    juce_DeclareSingleton(ConductorInfos, true);
    ConductorInfos();
    ~ConductorInfos() override;

    BKEngine* engine = nullptr;

    Label currentCueName;
    Label currentCueId;
    Label currentCueText;
    Label commands;
    Label nextCueGo;
    Label nextCueName;

    TextButton displayBtn;
    int displayMode = 0;
    TextButton inspectPrevBtn;
    TextButton inspectCurrBtn;
    TextButton inspectNextBtn;
    TextButton inspectCuelistBtn;
    TextButton loadCurrentCueBtn;

    Label upLabel;
    Label downLabel;
    Label ltpLabel;
    Label fadeLabel;
    Label delayLabel;

    FloatSliderUI* currentFade = nullptr;

    FloatSliderUI* nextHTPInDelay = nullptr;
    FloatSliderUI* nextHTPOutDelay = nullptr;
    FloatSliderUI* nextLTPDelay = nullptr;

    FloatSliderUI* nextHTPInFade = nullptr;
    FloatSliderUI* nextHTPOutFade = nullptr;
    FloatSliderUI* nextLTPFade = nullptr;


    void paint (juce::Graphics&) override;
    void resized() override;
    void updateStyle();
    void linkFadeSlider();
    void updateContent();

    void updateDisplayBtn();

    void labelTextChanged(Label* l) override;

    void inspect(int delta);
    void inspectCuelist();


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConductorInfos)
};

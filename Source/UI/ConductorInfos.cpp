#include <JuceHeader.h>
#include "ConductorInfos.h"
#include "Brain.h"

//==============================================================================
ConductorInfosUI::ConductorInfosUI(const String& contentName):
    ShapeShifterContent(ConductorInfos::getInstance(), contentName)
{
    
}

ConductorInfosUI::~ConductorInfosUI()
{
}

juce_ImplementSingleton(ConductorInfos);

ConductorInfos::ConductorInfos()
{
    //addAndMakeVisible(targetId);
}

ConductorInfos::~ConductorInfos()
{
    if (currentFade != nullptr) {
        removeChildComponent(currentFade);
        delete currentFade;
    }
}

void ConductorInfos::paint (juce::Graphics& g)
{
    if (engine == nullptr) { return; }
    g.fillAll(juce::Colours::black);   // clear the background

    int targetCueId = engine->conductorCuelistId->intValue();
    Cuelist* target = Brain::getInstance()->getCuelistById(targetCueId);
    if (target == nullptr) {
        g.drawText("Cuelist "+String(targetCueId)+" doesn't exist", getLocalBounds(), juce::Justification::centred, true);
        return;
    }

    Cue* nextCue = target->getNextCue();

    float textSize = engine->conductorTextSize->floatValue();
    float titleSize = engine->conductorTitleSize->floatValue();

    float w = getLocalBounds().getWidth();
    float h = getLocalBounds().getHeight() - 20;

    float currentCueHeight = titleSize +10;
    float nextCueHeight = textSize +10;
    float nextCueGoHeight = titleSize +10;
    float currentCueTextHeight = h - currentCueHeight - nextCueHeight - nextCueGoHeight;


    if (currentCueTextHeight < currentCueHeight) {
        currentCueHeight = h/4;
        nextCueHeight = h/4;
        nextCueGoHeight = h/4;
        currentCueTextHeight = h/4;
    }

    float timingHeight = nextCueHeight + nextCueGoHeight;
    float timingElementSize = timingHeight / 3;
    float timingWidth = w/3;

    g.setColour (engine->conductorCurrentCueColor->getColor());

    g.setFont (titleSize);
    String currId = "";
    if (target->conductorCurrentCueId->floatValue() >= 0) {
        currId = String(target->conductorCurrentCueId->floatValue()) + " - ";
    }
    g.drawFittedText(currId+ target->conductorCurrentCueName->stringValue(), 0, 0, w, currentCueHeight, juce::Justification::centredTop, true);

    g.setFont(textSize);
    g.drawFittedText(target->conductorCurrentCueText->stringValue(), 0, currentCueHeight, w, currentCueTextHeight, juce::Justification::centredTop, true);

    g.setColour(engine->conductorNextCueColor->getColor());

    float topNext = h - nextCueHeight - nextCueGoHeight;

    g.setFont(titleSize);
    g.drawFittedText(target->conductorNextCueGo->stringValue(), 0, topNext, w-timingWidth, nextCueGoHeight, juce::Justification::centredTop, true);

    g.setFont(textSize);
    String nextId = "";
    if (target->conductorNextCueId->floatValue() >= 0) {
        nextId = String(target->conductorNextCueId->floatValue()) + " - ";
    }
    g.drawFittedText(nextId + target->conductorNextCueName->stringValue(), 0, h - nextCueHeight, w-timingWidth, nextCueHeight, juce::Justification::centredTop, true);

    if (nextCue != nullptr) {

        int timeW = (w / 3) / 4;
        int timeH = timingHeight / 3;

        g.setFont(timingElementSize*0.5);
        g.drawFittedText("Up", w - (3 * timeW), h-(3*timeH), timeW, timeH, juce::Justification::centred, true);
        g.drawFittedText("Down", w - (2 * timeW), h-(3*timeH), timeW, timeH, juce::Justification::centred, true);
        g.drawFittedText("LTP", w - (1 * timeW), h-(3*timeH), timeW, timeH, juce::Justification::centred, true);
        //int r = Random::getSystemRandom().nextInt(100);

        g.drawFittedText("Delay", w - (4 * timeW), h - (2 * timeH), timeW, timeH, juce::Justification::centred, true);
        g.drawFittedText("Fade", w - (4 * timeW), h - (1 * timeH), timeW, timeH, juce::Justification::centred, true);
    }


}

void ConductorInfos::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

    float textSize = engine->conductorTextSize->floatValue();
    float titleSize = engine->conductorTitleSize->floatValue();

    float w = getLocalBounds().getWidth();
    float h = getLocalBounds().getHeight() - 20;

    float currentCueHeight = titleSize + 10;
    float nextCueHeight = textSize + 10;
    float nextCueGoHeight = titleSize + 10;
    float currentCueTextHeight = h - currentCueHeight - nextCueHeight - nextCueGoHeight;


    if (currentCueTextHeight < currentCueHeight) {
        currentCueHeight = h / 4;
        nextCueHeight = h / 4;
        nextCueGoHeight = h / 4;
        currentCueTextHeight = h / 4;
    }

    float timingHeight = nextCueHeight + nextCueGoHeight;

    int timeW = (w/3)/4;
    int timeH = timingHeight / 3;

    if (h > w) { // portrait
        int s = jmin(int(w-70), 200);
        s = jmax(100,s);
        targetId.setBounds(10, 0, s, s);
    }
    else {
        targetId.setBounds(0, 0, h, h);
    }

    if (currentFade != nullptr) { currentFade->setBounds(0, h , w, 20); }
    if (nextHTPInDelay != nullptr) { nextHTPInDelay->setBounds(w - (3 * timeW), h - (2 * timeH), timeW, timeH); }
    if (nextHTPOutDelay != nullptr) { nextHTPOutDelay->setBounds(w - (2 * timeW), h - (2 * timeH), timeW, timeH); }
    if (nextLTPDelay != nullptr) { nextLTPDelay->setBounds(w - (1 * timeW), h - (2 * timeH), timeW, timeH); }
    if (nextHTPInFade != nullptr) { nextHTPInFade->setBounds(w - (3 * timeW), h - (1 * timeH), timeW, timeH); }
    if (nextHTPOutFade != nullptr) { nextHTPOutFade->setBounds(w - (2 * timeW), h - (1 * timeH), timeW, timeH); }
    if (nextLTPFade != nullptr) { nextLTPFade->setBounds(w - (1 * timeW), h - (1 * timeH), timeW, timeH); }
}

void ConductorInfos::linkFadeSlider()
{
    if (currentFade != nullptr) {
        removeChildComponent(currentFade);
        delete currentFade;
    }
    currentFade = nullptr;

    int targetCueId = engine->conductorCuelistId->intValue();
    Cuelist* target = Brain::getInstance()->getCuelistById(targetCueId);
    if (target == nullptr) {
        return;
    }

    currentFade = target->currentFade->createSlider();
    currentFade->showLabel = false;
    currentFade->showValue = false;
    currentFade->customFGColor = Colour(96, 96, 96);
    currentFade->customBGColor = Colour(32, 32, 32);
    currentFade->useCustomFGColor = true;
    currentFade->useCustomBGColor = true;

    addAndMakeVisible(currentFade);
    resized();
}

void ConductorInfos::linkSlidersTimings()
{
    if (nextHTPInDelay != nullptr) { removeChildComponent(nextHTPInDelay); delete nextHTPInDelay; }
    nextHTPInDelay = nullptr;
    if (nextHTPOutDelay != nullptr) { removeChildComponent(nextHTPOutDelay); delete nextHTPOutDelay; }
    nextHTPOutDelay = nullptr;
    if (nextLTPDelay != nullptr) { removeChildComponent(nextLTPDelay); delete nextLTPDelay; }
    nextLTPDelay = nullptr;
    if (nextHTPInFade != nullptr) { removeChildComponent(nextHTPInFade); delete nextHTPInFade; }
    nextHTPInFade = nullptr;
    if (nextHTPOutFade != nullptr) { removeChildComponent(nextHTPOutFade); delete nextHTPOutFade; }
    nextHTPOutFade = nullptr;
    if (nextLTPFade != nullptr) { removeChildComponent(nextLTPFade); delete nextLTPFade; }
    nextLTPFade = nullptr;

    int targetCueId = engine->conductorCuelistId->intValue();
    Cuelist* target = Brain::getInstance()->getCuelistById(targetCueId);
    if (target == nullptr) {return;}
    Cue* nextCue = target->getNextCue();
    if (nextCue == nullptr) {return;}

    nextHTPInDelay = nextCue->htpInDelay->createSlider();
    nextHTPOutDelay = nextCue->htpOutDelay->createSlider();
    nextLTPDelay = nextCue->ltpDelay->createSlider();
    nextHTPInFade = nextCue->htpInFade->createSlider();
    nextHTPOutFade = nextCue->htpOutFade->createSlider();
    nextLTPFade = nextCue->ltpFade->createSlider();

    nextHTPInDelay->showLabel = false;
    nextHTPOutDelay->showLabel = false;
    nextLTPDelay->showLabel = false;
    nextHTPInFade->showLabel = false;
    nextHTPOutFade->showLabel = false;
    nextLTPFade->showLabel = false;

    nextHTPInDelay->customScaleFactor = 0.000000001;
    nextHTPOutDelay->customScaleFactor = 0.000000001;
    nextLTPDelay->customScaleFactor = 0.000000001;
    nextHTPInFade->customScaleFactor = 0.000000001;
    nextHTPOutFade->customScaleFactor = 0.000000001;
    nextLTPFade->customScaleFactor = 0.000000001;

    addAndMakeVisible(nextHTPInDelay);
    addAndMakeVisible(nextHTPOutDelay);
    addAndMakeVisible(nextLTPDelay);
    addAndMakeVisible(nextHTPInFade);
    addAndMakeVisible(nextHTPOutFade);
    addAndMakeVisible(nextLTPFade);
    resized();
}

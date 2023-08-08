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

    int targetCueId = engine->conductorCuelistId->getValue();
    Cuelist* target = Brain::getInstance()->getCuelistById(targetCueId);
    if (target == nullptr) {
        g.drawText("Cuelist "+String(targetCueId)+" doesn't exist", getLocalBounds(), juce::Justification::centred, true);
        return;
    }

    Cue* nextCue = target->getNextCue();

    float textSize = engine->conductorTextSize->getValue();
    float titleSize = engine->conductorTitleSize->getValue();

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
    float timingWidth = timingElementSize * 5;

    g.setColour (engine->conductorCurrentCueColor->getColor());

    g.setFont (titleSize);
    g.drawFittedText(target->currentCueName->getValue(), 0, 0, w, currentCueHeight, juce::Justification::centredTop, true);

    g.setFont(textSize);
    g.drawFittedText(target->currentCueText->getValue(), 0, currentCueHeight, w, currentCueTextHeight, juce::Justification::centredTop, true);

    g.setColour(engine->conductorNextCueColor->getColor());

    float topNext = h - nextCueHeight - nextCueGoHeight;

    g.setFont(titleSize);
    g.drawFittedText(target->nextCueGo->getValue(), 0, topNext, w-timingWidth, nextCueGoHeight, juce::Justification::centredTop, true);

    g.setFont(textSize);
    g.drawFittedText(target->nextCueName->getValue(), 0, h - nextCueHeight, w-timingWidth, nextCueHeight, juce::Justification::centredTop, true);

    if (nextCue != nullptr) {
        g.setFont(timingElementSize*0.5);
        g.drawFittedText("Up", w - (3 * timingElementSize), topNext, timingElementSize, timingElementSize, juce::Justification::centred, true);
        g.drawFittedText("Down", w - (2*timingElementSize), topNext, timingElementSize, timingElementSize, juce::Justification::centred, true);
        g.drawFittedText("LTP", w - (1*timingElementSize), topNext, timingElementSize, timingElementSize, juce::Justification::centred, true);
        //int r = Random::getSystemRandom().nextInt(100);

        g.drawFittedText("Delay", w - (5 * timingElementSize), topNext+timingElementSize, timingElementSize*2, timingElementSize, juce::Justification::centred, true);
        g.drawFittedText("Fade", w - (5 * timingElementSize), topNext + (2*timingElementSize), timingElementSize*2, timingElementSize, juce::Justification::centred, true);

        float ltpDelay = round(nextCue->ltpDelay->floatValue() * 100) / 100.;
        float ltpFade = round(nextCue->ltpFade->floatValue() * 100) / 100.;
        float htpInDelay = round(nextCue->htpInDelay->floatValue() * 100) / 100.;
        float htpOutDelay = round(nextCue->htpOutDelay->floatValue() * 100) / 100.;
        float htpInFade = round(nextCue->htpInFade->floatValue() * 100) / 100.;
        float htpOutFade = round(nextCue->htpOutFade->floatValue() * 100) / 100.;
    
        g.drawFittedText(String(htpInDelay), w - (3 * timingElementSize), topNext + timingElementSize, timingElementSize, timingElementSize, juce::Justification::centred, true);
        g.drawFittedText(String(htpOutDelay), w - (2 * timingElementSize), topNext + timingElementSize, timingElementSize, timingElementSize, juce::Justification::centred, true);
        g.drawFittedText(String(ltpDelay), w - (1 * timingElementSize), topNext + timingElementSize, timingElementSize, timingElementSize, juce::Justification::centred, true);

        g.drawFittedText(String(htpInFade), w - (3 * timingElementSize), topNext + (2*timingElementSize), timingElementSize, timingElementSize, juce::Justification::centred, true);
        g.drawFittedText(String(htpOutFade), w - (2 * timingElementSize), topNext + (2*timingElementSize), timingElementSize, timingElementSize, juce::Justification::centred, true);
        g.drawFittedText(String(ltpFade), w - (1 * timingElementSize), topNext + (2*timingElementSize), timingElementSize, timingElementSize, juce::Justification::centred, true);
    }


}

void ConductorInfos::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    int h = getHeight();
    int w = getWidth();
    if (h > w) { // portrait
        int s = jmin(w-70, 200);
        s = jmax(100,s);
        targetId.setBounds(10, 0, s, s);
    }
    else {
        targetId.setBounds(0, 0, h, h);
    }

    if (currentFade != nullptr) {
        currentFade->setBounds(0,h-20,w,20);
    }
}

void ConductorInfos::linkFadeSlider()
{
    if (currentFade != nullptr) {
        removeChildComponent(currentFade);
        delete currentFade;
    }
    currentFade = nullptr;

    int targetCueId = engine->conductorCuelistId->getValue();
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

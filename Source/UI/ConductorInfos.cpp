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
}

void ConductorInfos::paint (juce::Graphics& g)
{
    if (engine == nullptr) { return; }
    g.fillAll(juce::Colours::black);   // clear the background

    int targetId = engine->conductorCuelistId->getValue();
    Cuelist* target = Brain::getInstance()->getCuelistById(targetId);
    if (target == nullptr) {
        g.drawText("Cuelist 1 doesn't exist", getLocalBounds(), juce::Justification::centred, true);
        return;
    }

    float textSize = engine->conductorTextSize->getValue();
    float titleSize = engine->conductorTitleSize->getValue();

    float w = getLocalBounds().getWidth();
    float h = getLocalBounds().getHeight();

    float currentCueHeight = titleSize * 1.5;
    float nextCueHeight = titleSize * 1.5;
    float nextCueGoHeight = textSize * 1.5;
    float currentCueTextHeight = h - currentCueHeight - nextCueHeight - nextCueGoHeight;

    if (currentCueTextHeight < currentCueHeight) {
        currentCueHeight = h/4;
        nextCueHeight = h/4;
        nextCueGoHeight = h/4;
        currentCueTextHeight = h/4;
    }

    g.setColour (juce::Colours::grey);

    g.setFont (titleSize);
    g.drawText(target->currentCueName->getValue(), 0, 0, w, currentCueHeight, juce::Justification::centred, true);

    g.setFont(textSize);
    g.drawText(target->currentCueText->getValue(), 0, currentCueHeight, w, currentCueTextHeight, juce::Justification::centred, true);

    g.setFont(titleSize);
    g.drawText(target->nextCueGo->getValue(), 0, h-nextCueHeight-nextCueHeight, w, nextCueGoHeight, juce::Justification::centred, true);

    g.setFont(textSize);
    g.drawText(target->nextCueName->getValue(), 0, h-nextCueHeight, w, nextCueHeight, juce::Justification::centred, true);


    //int r = Random::getSystemRandom().nextInt(100);


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
}

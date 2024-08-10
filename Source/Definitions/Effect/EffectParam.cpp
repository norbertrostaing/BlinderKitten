/*
  ==============================================================================

    FixtureTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "EffectParam.h"
#include "../ChannelFamily/ChannelFamilyManager.h"
#include "Effect.h"
#include "EffectRow.h" 

EffectParam::EffectParam(var params) :
    BaseItem(params.getProperty("name", "Param")),
    objectType(params.getProperty("type", "EffectParam").toString()),
    objectData(params)
{
    saveAndLoadRecursiveData = true;
    paramType = addTargetParameter("Channel type", "Type of data of this channel", ChannelFamilyManager::getInstance());
    paramType->targetType = TargetParameter::CONTAINER;
    paramType->maxDefaultSearchLevel = 2;

    String modeExplain = "How does effect changes the current output value : \n";
    modeExplain += "- Relative adds the effect value to current output\n\n";
    modeExplain += "- Absolute set the output to the base value (no matter master effect value) and adds the effect, HTP values are modified only if effect is higher \n\n";
    modeExplain += "- Attractive calculate the absolute value of the effect at full and interpolate to this value in function of the master size, HTP values are modified only if effect is higher\n";
    effectMode = addEnumParameter("Effect mode", modeExplain);
    effectMode->addOption("Relative", "relative");
    effectMode->addOption("Absolute", "absolute");
    effectMode->addOption("Attracted", "attracted");

    elementsStart = addFloatParameter("First Element position", "Position of the first element, relative to the curve", 0, 0, 1);
    elementsSpread = addFloatParameter("Elements Spread", "Size of distribution", 1, 0);

    buddying = addIntParameter("Buddying", "make groups of X following subfixtures with the same value",1,1);
    blocks = addIntParameter("Blocks", "A block repeat the effect, applies before the wings", 1, 1);
    wings = addIntParameter("Wings", "A wing repeat the effect but inversed", 1, 1);
    wingsInvertSelections = addBoolParameter("Invert wings selection", "Selection are reversed in wings", false);
    wingsInvertValues = addBoolParameter("Invert wings value", "Values on wings are inverted", false);
    wingsSoloCenterDisable = addBoolParameter("Disable wings centered solo value", "Disable value for fixtures on the symetry axis", false);

    curveSize = addFloatParameter("Size", "Size of the effect applied to outputs",1,-2,2);
    baseValue = addFloatParameter("Base Value","Base value applied to channels",0,0,1);
    updateDisplay();
};

EffectParam::~EffectParam()
{
    if (parentEffect!= nullptr) {
        parentEffect->isComputing.enter();
        for (auto it = parentEffect->chanToFxParam.begin(); it != parentEffect->chanToFxParam.end(); it.next()) {
            it.getValue()->removeAllInstancesOf(this);
        }
        parentEffect->isComputing.exit();
    }


};


void EffectParam::checkParentEffect()
{
    if (parentEffect == nullptr && 
        parentContainer != nullptr
        && parentContainer->parentContainer != nullptr
        && parentContainer->parentContainer->parentContainer != nullptr
        && parentContainer->parentContainer->parentContainer->parentContainer != nullptr
        ) {
        parentEffect = dynamic_cast<Effect*>(parentContainer->parentContainer->parentContainer->parentContainer.get());
        parentEffectRow = dynamic_cast<EffectRow*>(parentContainer->parentContainer.get());
    }

}

void EffectParam::onContainerParameterChangedInternal(Parameter* c) {
    if (c == effectMode) {
        updateDisplay();
    }
    if (c == elementsSpread || c == elementsStart || c == wings || c == buddying) {
        checkParentEffect();
        if (parentEffect != nullptr) {
            parentEffect->pleaseComputeIfRunning();
        }
    }
}

void EffectParam::updateDisplay() {
    bool isRel = effectMode->getValue() == "relative";

    baseValue->hideInEditor = isRel;

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}


/*
  ==============================================================================

    FixtureTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "StampMapping.h"
#include "../ChannelFamily/ChannelFamilyManager.h"
#include "../ChannelFamily/ChannelType/ChannelType.h"
#include "../SubFixture/SubFixture.h"
#include "../SubFixture/SubFixtureChannel.h"
#include "Stamp.h"


StampMapping::StampMapping(var params) :
    BaseItem(params.getProperty("name", "StampMapping")),
    objectType(params.getProperty("type", "StampMapping").toString()),
    objectData(params)
{

    pixelVal = addEnumParameter("Pixel value", "wich value of the pixel do you want ?");
    pixelVal->addOption("Luma", Luma)->addOption("Red", R)->addOption("Green", G)->addOption("Blue", B);

    paramType = addTargetParameter("Channel type", "Type of data of this channel", ChannelFamilyManager::getInstance());
    paramType->targetType = TargetParameter::CONTAINER;
    paramType->maxDefaultSearchLevel = 2;
    paramType->typesFilter.add("ChannelType");

    String modeExplain = "How does stamp changes the current output value : \n";
    modeExplain += "- Relative adds the stamp value to current output\n\n";
    modeExplain += "- Absolute set the output to the base value (no matter master stamp value) and adds the stamp, HTP values are modified only if stamp is higher \n\n";
    modeExplain += "- Attractive calculate the absolute value of the stamp at full and interpolate to this value in function of the master size, HTP values are modified only if stamp is higher\n";
    stampMode = addEnumParameter("Stamp mode", modeExplain);
    stampMode->addOption("Relative", "relative");
    stampMode->addOption("Absolute", "absolute");
    stampMode->addOption("Attracted", "attracted");

    baseValue = addFloatParameter("Base Value", "Base value applied to channels", 0, 0, 1);

    outRange = addPoint2DParameter("Output range", "Value to add or set");
    outRange->setDefaultPoint(0,1);
    outRange->setBounds(-2,-2,2,2);

    updateDisplay();
};

StampMapping::~StampMapping()
{
};



void StampMapping::onContainerParameterChangedInternal(Parameter* c) {
    if (c == stampMode) {
        updateDisplay();
    }
}

void StampMapping::updateDisplay() {
    bool isRel = stampMode->getValue() == "relative";

    baseValue->hideInEditor = isRel;

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

/*
  ==============================================================================

    BundleTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "BundleSelection.h"
#include "ChannelFamily/ChannelFamilyManager.h"

BundleSelection::BundleSelection(var params) :
    BaseItem(params.getProperty("name", "Selection")),
    objectType(params.getProperty("type", "BundleSelection").toString()),
    objectData(params)
{
    plusOrMinus = addEnumParameter("Action", "Do you wan to add or remove elements to selection ?");
    plusOrMinus->addOption("+", "add");
    plusOrMinus->addOption("-", "remove");

    targetType = addEnumParameter("Target type", "What kinf of element are you targetting ?");
    targetType->addOption("Cuelist", "cuelist");
    targetType->addOption("Effect", "effect");
    targetType->addOption("Carousel", "carousel");
    targetType->addOption("Mapper", "mapper");
    targetType->addOption("Tracker", "tracker");

    valueFrom = addIntParameter("ID", "ID of the first element",0,0);
    thru = addBoolParameter("Thru", "Do you want to add multiple elements ?", false);
    valueTo = addIntParameter("Last ID", "ID of the last element (only if thru checked)", 0, 0);

    updateDisplay();
};

BundleSelection::~BundleSelection()
{
};

void BundleSelection::updateDisplay()
{
    bool th = thru->getValue();

    valueTo -> hideInEditor = !th;

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void BundleSelection::onContainerParameterChangedInternal(Parameter* p) {
    if (p == thru || p == targetType) {
        updateDisplay();
    }
}

void BundleSelection::afterLoadJSONDataInternal() {
    updateDisplay();
}


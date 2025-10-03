/*
  ==============================================================================

    FixtureTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "FixtureTypeChannel.h"
#include "../ChannelFamily/ChannelFamilyManager.h"
#include "FixtureTypeChannelManager.h"
#include "FixtureTypeVirtualChannelManager.h"
#include "FixtureTypeDMXChannel.h"
#include "FixtureType.h"
#include "Tracker/TrackerManager.h"
#include "Fixture/FixtureManager.h"
#include "Brain.h"
#include "SubFixture/SubFixture.h"

FixtureTypeChannel::FixtureTypeChannel(var params) :
    BaseItem(params.getProperty("name", "Channel")),
    objectType(params.getProperty("type", "FixtureTypeChannel").toString()),
    objectData(params)

{

    curve.editorIsCollapsed = true;
    curve.setNiceName("Out curve");
    curve.allowKeysOutside = false;
    curve.isSelectable = false;
    curve.length->setValue(1);
    curve.addKey(0, 0, false);
    curve.items[0]->easingType->setValueWithData(Easing::LINEAR);
    curve.addKey(1, 1, false);
    curve.selectItemWhenCreated = false;
    curve.editorCanBeCollapsed = true;
    curve.setCanBeDisabled(true);
    curve.enabled->setValue(false);

    curve.saveAndLoadRecursiveData = true;
    saveAndLoadRecursiveData = true;

    nameCanBeChangedByUser = false;
    canBeDisabled = false;
    channelType = addTargetParameter("Channel type", "Type of data of this channel", ChannelFamilyManager::getInstance());
    channelType -> targetType = TargetParameter::CONTAINER;
    channelType -> maxDefaultSearchLevel = 2;
    channelType->typesFilter.add("ChannelType");

    subFixtureId = addIntParameter("SubFixture ID", "0 means not in a subfixture",0,0);
    defaultValue = addFloatParameter("Default value", "Default value of the channel", 0, 0, 1);
    highlightValue = addFloatParameter("Highlight value", "Value of the channel during highlight", 0, 0, 1);
    highlightValue->canBeDisabledByUser = true;
    highlightValue->setEnabled(false);
    killedBySWOP = addBoolParameter("Killed By SWOP", "if checked, this parameter will be set to its default value when cuelists with no command for ot are called with SWOP", false);

    fadeOrSnap = addEnumParameter("Fade or snap", "Is ths channel allowed to fade or should it jump to his new value ?");
    fadeOrSnap->addOption("Fade", "fade");
    fadeOrSnap->addOption("Snap", "snap");

    invertOutput = addBoolParameter("Invert output", "if checked, output will be inverted", false);

    dmxRange = addPoint2DParameter("DMX Output Range", "The range to remap the value to.");
    dmxRange->setBounds(0, 0, 256, 256); // TODO: based on resolution
    dmxRange->setDefaultPoint(0, 256);
    dmxRange->canShowExtendedEditor = false;
    
    virtualMaster = addTargetParameter("Virtual Master", "Select a virtual master");
    virtualMaster->targetType = TargetParameter::CONTAINER;
    virtualMaster->maxDefaultSearchLevel = 2;

    physicalRange = addPoint2DParameter("Physical range", "Range output (degrees for pan, tilt, zoom etc...");

    addChildControllableContainer(&curve);
};

FixtureTypeChannel::~FixtureTypeChannel()
{
    for (SubFixture* sf : Brain::getInstance()->allSubfixtures) {
        Array<SubFixtureChannel*> toDelete;
        for (auto it = sf->channelsMap.begin(); it != sf->channelsMap.end(); it.next()) {
            SubFixtureChannel* sfc = it.getValue();
            if (sfc != nullptr && sfc->parentFixtureTypeChannel == this) {
                toDelete.add(sfc);
            }
        }
        for (SubFixtureChannel* sfc : toDelete) {
            sf->channelsMap.removeValue(sfc);
            sf->channelsContainer.removeObject(sfc);
        }
    }
};

void FixtureTypeChannel::onContainerParameterChangedInternal(Parameter* p) {
    if (p == physicalRange) {
        TrackerManager::getInstance()->recomputeAllTrackers();
    }
    else if (p == defaultValue || p == invertOutput) {
        FixtureManager::getInstance()->defaultValueChanged(this);
    }
    else if (p == highlightValue) {
        if (!p->enabled) p->setEnabled(true);
    }
}

void FixtureTypeChannel::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
    if (cc == &curve) {
        for (SubFixtureChannel* sfc : Brain::getInstance()->allSubfixtureChannels) {
            if (sfc->parentFixtureTypeChannel == this) {
                Brain::getInstance()->pleaseUpdate(sfc);
            }
        }
    }
}

FixtureTypeDMXChannel* FixtureTypeChannel::getParentDMXChannel()
{
    // Get parent from object hierarchy - should be a FixtureTypeDMXChannel
    return dynamic_cast<FixtureTypeDMXChannel*>(parentContainer.get());
}

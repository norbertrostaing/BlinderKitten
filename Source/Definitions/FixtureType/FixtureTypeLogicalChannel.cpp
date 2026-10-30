/*
  ==============================================================================

    FixtureTypeLogicalChannel.cpp
    Created: 24 Oct 2026 11:24:00pm
    Author:  Norb

  ==============================================================================
*/

#include "FixtureTypeLogicalChannel.h"
#include "Definitions/ChannelFamily/ChannelFamilyManager.h"

FixtureTypeLogicalChannel::FixtureTypeLogicalChannel(var params) :
    BaseItem(params.getProperty("name", "Channel")),
    objectType(params.getProperty("type", "FixtureTypeLogicalChannel").toString()),
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

    dmxOutputRange = addPoint2DParameter("DMX Range", "Ouput values in DMX for this logical channel");
    dmxOutputRange->setDefaultPoint(0, 255);

    logicalPriority = addIntParameter("Logical priority", "Acts as a priority when multipls logical channels try to write on the physical channel, highest priority wins, if same priority, last arrived wins !", 1,1);

    channelType = addTargetParameter("Channel type", "Type of data of this channel", ChannelFamilyManager::getInstance());
    channelType->targetType = TargetParameter::CONTAINER;
    channelType->maxDefaultSearchLevel = 2;
    channelType->typesFilter.add("ChannelType");

    defaultValue = addFloatParameter("Default value", "Default value of the channel", 0, 0, 1);
    defaultValue->canBeDisabledByUser = true;
    defaultValue->setEnabled(false, true);

    highlightValue = addFloatParameter("Highlight value", "Value of the channel during highlight", 0, 0, 1);
    highlightValue->canBeDisabledByUser = true;
    highlightValue->setEnabled(false);
    killedBySWOP = addBoolParameter("Killed By SWOP", "if checked, this parameter will be set to its default value when cuelists with no command for ot are called with SWOP", false);

    fadeOrSnap = addEnumParameter("Fade or snap", "Is ths channel allowed to fade or should it jump to his new value ?");
    fadeOrSnap->addOption("Fade", "fade");
    fadeOrSnap->addOption("Snap", "snap");

    virtualMaster = addTargetParameter("Virtual Master", "Select a virtual master");
    virtualMaster->targetType = TargetParameter::CONTAINER;
    virtualMaster->maxDefaultSearchLevel = 2;

    physicalRange = addPoint2DParameter("Physical range", "Range output (degrees for pan, tilt, zoom etc...");

    addChildControllableContainer(&curve);
}


FixtureTypeLogicalChannel::~FixtureTypeLogicalChannel()
{
}


void FixtureTypeLogicalChannel::onContainerParameterChangedInternal(Parameter* p) {

}


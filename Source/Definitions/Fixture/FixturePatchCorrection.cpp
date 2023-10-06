/*
  ==============================================================================

    FixtureTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "FixturePatchCorrection.h"
#include "../Interface/InterfaceIncludes.h"
#include "../ChannelFamily/ChannelFamilyManager.h"
#include "Fixture.h"
#include "Brain.h"

FixturePatchCorrection::FixturePatchCorrection(var params) :
	BaseItem(params.getProperty("name", "Correction")),
	objectType(params.getProperty("type", "FixturePatchCorrection").toString()),
	objectData(params)
{

	subFixtureId = addIntParameter("SubFixture", "ID of the subfixture",0,0);

	channelType = addTargetParameter("Channel type", "Type of Channel", ChannelFamilyManager::getInstance());
	channelType->maxDefaultSearchLevel = 2;
	channelType->targetType = TargetParameter::CONTAINER;

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

	curve.saveAndLoadRecursiveData = true;
	saveAndLoadRecursiveData = true;


	invertChannel = addBoolParameter("Invert Channel", "max is min, min is max", false);
	offsetValue = addFloatParameter("Offset", "add an offset to the output", 0, -1, 1);
	addChildControllableContainer(&curve);
	isOn = true;

};

FixturePatchCorrection::~FixturePatchCorrection()
{
};


void FixturePatchCorrection::onContainerParameterChangedInternal(Parameter* p) {
	if (Brain::getInstance()->loadingIsRunning) {
		return; // bug au load
	}
	if (parentContainer == nullptr) {
		return;
	}
	Fixture* f = dynamic_cast<Fixture*>(parentContainer->parentContainer->parentContainer->parentContainer.get());
	SubFixture* sf = f->subFixtures.contains(subFixtureId->getValue()) ? f->subFixtures.getReference(subFixtureId->getValue()) : nullptr;
	if (sf == nullptr) { return; }
	ChannelType* ct = dynamic_cast<ChannelType*>(channelType->targetContainer.get());
	if (!sf->channelsMap.contains(ct)) {return; }
	SubFixtureChannel* chan = sf->channelsMap.getReference(ct);
	if (chan == nullptr) { return; }
	Brain::getInstance()->pleaseUpdate(chan);
}


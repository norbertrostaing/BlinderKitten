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

FixturePatchCorrection::FixturePatchCorrection(var params) :
	BaseItem(params.getProperty("name", "Patch")),
	objectType(params.getProperty("type", "FixturePatchCorrection").toString()),
	objectData(params)
{

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


};

FixturePatchCorrection::~FixturePatchCorrection()
{
};



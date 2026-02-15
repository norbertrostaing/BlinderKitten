/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "ChannelFamily.h"
#include "ChannelType/ChannelType.h"

ChannelFamily::ChannelFamily(var params) :
	BaseItem(params.getProperty("name", "Channel Family")),
	objectType(params.getProperty("type", "ChannelFamily").toString()),
    objectData(params),
    definitions("Channel Types")
{
	saveAndLoadRecursiveData = true;
	
	editorIsCollapsed = true;

	itemDataType = "ChannelFamily";
	canBeDisabled = false;
	
	addChildControllableContainer(&definitions);
	definitions.selectItemWhenCreated = false;
}

ChannelFamily::~ChannelFamily()
{
}

void ChannelFamily::onContainerParameterChangedInternal(Parameter* p)
{
	BaseItem::onContainerParameterChangedInternal(p);
}

void ChannelFamily::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	BaseItem::onControllableFeedbackUpdateInternal(cc, c);

	if (!enabled->boolValue()) return;

}



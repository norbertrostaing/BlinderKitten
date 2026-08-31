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
	definitions.addBaseManagerListener(this);
	
}

ChannelFamily::~ChannelFamily()
{
	definitions.removeBaseManagerListener(this);
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

void ChannelFamily::itemAdded(ChannelType* ct)
{
	ct->parentFamily = this;
}

void ChannelFamily::itemsAdded(juce::Array<ChannelType*> cts)
{
	for (auto ct : cts) {
		ct->parentFamily = this;
	}
}



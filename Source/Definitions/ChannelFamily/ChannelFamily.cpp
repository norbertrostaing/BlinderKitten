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
	objectType(params.getProperty("Channel types", "ChannelFamily").toString()),
	// parameters("Parameters"),
	definitions("Channel Types"),
	objectData(params)
	// previousID(-1),
	// slideManipParameter(nullptr)
{
	saveAndLoadRecursiveData = true;
	
	editorIsCollapsed = true;

	itemDataType = "ChannelFamily";
	
	// definitions->addBaseManagerListener(this);

	addChildControllableContainer(&definitions);

	var objectsData = params.getProperty("objects", var());

}

ChannelFamily::~ChannelFamily()
{
}

void ChannelFamily::onContainerParameterChangedInternal(Parameter* p)
{
	BaseItem::onContainerParameterChangedInternal(p);
	//if (p == targetInterface)
	//{
	//	rebuildInterfaceParams();
	//}
	
}

void ChannelFamily::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	BaseItem::onControllableFeedbackUpdateInternal(cc, c);

	if (!enabled->boolValue()) return;

	//if (c->type != Parameter::TRIGGER)
	//{
	//    if (ObjectComponent* oc = c->getParentAs<ObjectComponent>())
	//    {
	//        if (targetInterface->targetContainer != nullptr)
	//        {
	//            if (!oc->computedParameters.contains((Parameter*)c)) computeComponentValues(oc);
	//            //else sendComponentParameter(oc, ((Parameter*)c));
	//        }
	//    }
	//}
}



/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "MultiplicatorLink.h"
#include "MultiplicatorManager.h"

MultiplicatorLink::MultiplicatorLink(var params) :
	BaseItem(params.getProperty("name", "Multiplicator")),
	objectType(params.getProperty("MultiplicatorLink", "MultiplicatorLink").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	
	editorIsCollapsed = true;

	itemDataType = "MultiplicatorLink";
	
	var objectsData = params.getProperty("objects", var());

	targetMult = addTargetParameter("multiplicator", "Multiplicator applied to selected value", MultiplicatorManager::getInstance());
	targetMult->targetType = TargetParameter::CONTAINER;
	targetMult->maxDefaultSearchLevel = 0;

}

MultiplicatorLink::~MultiplicatorLink()
{
}

void MultiplicatorLink::onContainerParameterChangedInternal(Parameter* p)
{
	BaseItem::onContainerParameterChangedInternal(p);
	//if (p == targetInterface)
	//{
	//	rebuildInterfaceParams();
	//}
	
}

void MultiplicatorLink::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
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



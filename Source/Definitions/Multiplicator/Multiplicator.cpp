/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Multiplicator.h"

Multiplicator::Multiplicator(var params) :
	BaseItem(params.getProperty("name", "Multiplicator")),
	objectType(params.getProperty("Multiplicator", "Multiplicator").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	
	editorIsCollapsed = true;

	itemDataType = "Multiplicator";
	
	var objectsData = params.getProperty("objects", var());

	multValue = addFloatParameter("Mult value", "multiplicator applied to selected values", 1, 0);

}

Multiplicator::~Multiplicator()
{
}

void Multiplicator::onContainerParameterChangedInternal(Parameter* p)
{
	BaseItem::onContainerParameterChangedInternal(p);
	//if (p == targetInterface)
	//{
	//	rebuildInterfaceParams();
	//}
	
}

void Multiplicator::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
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



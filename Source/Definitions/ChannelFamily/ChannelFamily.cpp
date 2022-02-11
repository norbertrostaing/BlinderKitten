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
	definitions(),
	objectData(params)
	// previousID(-1),
	// slideManipParameter(nullptr)
{
	saveAndLoadRecursiveData = true;
	
	editorIsCollapsed = true;

	itemDataType = "ChannelFamily";

	BaseManager<ChannelType>* m = new BaseManager<ChannelType>("Channel Types");
	m -> selectItemWhenCreated = false;
	definitions.reset(m);
	
	// definitions->addBaseManagerListener(this);

	addChildControllableContainer(definitions.get());

	//addChildControllableContainer(&definitions);

	// File objPath = File(params.getProperty("path", ""));

	// icon = addEnumParameter("Icon", "Bring some fancy in you life ! Put icons in the objects folders and find them here to customize you view");
	// categoryName = addStringParameter("Category name", "This name is used to filter parameters, for example in presets", "", true);
	
	// addChildControllableContainer(&parameters);


	// globalID = addIntParameter("Global ID", "Virtual ID that is used in many places of Blux to filter, alter effects, etc.", 0, 0);
	// stagePosition = addPoint3DParameter("Stage Position", "Position on stage, can be animated with stage layouts");
	// excludeFromScenes = addBoolParameter("Exclude From Scenes", "If enabled, this object will not be modified when loading scenes", false);

	// targetInterface = addTargetParameter("Interface", "The interface to link this object to", InterfaceManager::getInstance());
	// targetInterface->targetType = TargetParameter::CONTAINER;
	// targetInterface->maxDefaultSearchLevel = 0;

	// if (!Engine::mainEngine->isLoadingFile && InterfaceManager::getInstance()->items.size() == 1) targetInterface->setValueFromTarget(InterfaceManager::getInstance()->items[0]);

	var objectsData = params.getProperty("objects", var());

	/*if (objectsData.isObject() || canCustomize)
	{
		objectManager.reset(new SubObjectManager());
		objectManager->userCanAddItemsManually = canCustomize;
		addChildControllableContainer(objectManager.get());
	}*/
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



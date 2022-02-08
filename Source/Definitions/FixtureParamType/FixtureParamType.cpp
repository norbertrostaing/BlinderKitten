/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "FixtureParamType.h"
#include "FixtureParamDefinition/FixtureParamDefinition.h"

FixtureParamType::FixtureParamType(var params) :
	BaseItem(params.getProperty("name", "FixtureParamType")),
	objectType(params.getProperty("type", "FixtureParamType").toString()),
	// parameters("Parameters"),
	definitions(),
	objectData(params)
	// previousID(-1),
	// slideManipParameter(nullptr)
{
	saveAndLoadRecursiveData = true;
	
	editorIsCollapsed = true;

	itemDataType = "FixtureParamType";

	BaseManager<FixtureParamDefinition>* m = new BaseManager<FixtureParamDefinition>("Types");
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

FixtureParamType::~FixtureParamType()
{
}

void FixtureParamType::onContainerParameterChangedInternal(Parameter* p)
{
	BaseItem::onContainerParameterChangedInternal(p);
	//if (p == targetInterface)
	//{
	//	rebuildInterfaceParams();
	//}
	
}

void FixtureParamType::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
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



/*
  ==============================================================================

	GenericAction.cpp
	Created: 8 Oct 2020 3:33:47pm
	Author:  bkupe

  ==============================================================================
*/

#include "GenericAction.h"

GenericAction::GenericAction(var params) :
	Action(params)
{
	target = addTargetParameter("Target", "Target to set the value");

	target->targetType = TargetParameter::CONTROLLABLE;
	actionType = (ActionType)(int)params.getProperty("actionType", SET_VALUE);
	if (actionType == TRIGGER) target->typesFilter.add(Trigger::getTypeStringStatic());
	else target->excludeTypesFilter.add(Trigger::getTypeStringStatic());
	
}


GenericAction::~GenericAction()
{
}

void GenericAction::setValueParameter(Parameter* p)
{
	if (Engine::mainEngine->isLoadingFile) return;

	if (!value.wasObjectDeleted() && value != nullptr)
	{
		ghostValueData = value->getJSONData();
		removeControllable(value.get());
	}

	Parameter* tp = dynamic_cast<Parameter*>(target->target.get());
	if (tp == nullptr) return;

	value = p;

	if (value != nullptr)
	{
		addParameter(p);
		if (!ghostValueData.isVoid()) value->loadJSONData(ghostValueData);
		ghostValueData = var();
	}
}


void GenericAction::triggerInternal()
{
	if (target->target == nullptr) return;

	switch (actionType)
	{
	case SET_VALUE:
	{
		if (value == nullptr) return;
		Parameter* p = static_cast<Parameter*>(target->target.get());
		p->setValue(value->getValue());
	}
	break;

	case TRIGGER:
	{
		Trigger* t = static_cast<Trigger*>(target->target.get());
		t->trigger();
	}
	break;
	}
}

void GenericAction::setValueInternal(var _value)
{
	switch (actionType)
	{
	case SET_VALUE:
	{
		Parameter* p = static_cast<Parameter*>(target->target.get());
		if (p != nullptr) {
			p->setValue(value->getValue());
		}
	}
	break;

	case TRIGGER:
		triggerInternal();
		break;
	}
	
}

void GenericAction::onContainerParameterChanged(Parameter* p)
{
	if (p == target)
	{
		if (actionType == SET_VALUE)
		{
			if (!Engine::mainEngine->isLoadingFile)
			{
				if (target->target == nullptr) setValueParameter(nullptr);
				else
				{
					if (target->target->type == Controllable::TRIGGER) setValueParameter(nullptr);
					else
					{
						Controllable* c = ControllableFactory::createParameterFrom(target->target);
						if (c == nullptr)
						{
							DBG("Should not be null here");
							jassertfalse;
						}

						c->setNiceName("Value");
						Parameter* tp = dynamic_cast<Parameter*>(c);
						setValueParameter(tp);
					}
				}
			}
			
		}

	}
}


void GenericAction::loadJSONDataInternal(var data)
{
	if (Engine::mainEngine->isLoadingFile)
	{
		//DBG("Engine is loading, waiting after load");
		Engine::mainEngine->addEngineListener(this);
		dataToLoad = data;
	}
	else Action::loadJSONDataInternal(data);
}

void GenericAction::endLoadFile()
{
	//reset data we want to reload
	target->setValue("", true);
	//DBG("Engine after load, load command data");

	loadJSONData(dataToLoad);
	dataToLoad = var();

	Engine::mainEngine->removeEngineListener(this);
}

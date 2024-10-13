/*
  ==============================================================================

    MoveInBlackManager.cpp
    Created: 13 Oct 2024 1:33:27am
    Author:  Norb

  ==============================================================================
*/

#include "MoveInBlackManager.h"
#include "Definitions/ChannelFamily/ChannelFamilyManager.h"
#include "Definitions/ChannelFamily/ChannelFamily.h"

MoveInBlackManager::MoveInBlackManager() :
    ControllableContainer("Move in black")
{
	userCanAddControllables = true;
	customUserCreateControllableFunc = &MoveInBlackManager::createItem;
}

MoveInBlackManager::~MoveInBlackManager()
{
}

Array<ChannelFamily*> MoveInBlackManager::getFamilies()
{
	Array<ChannelFamily*> ret;
	for (auto& c : controllables)
	{
		if (TargetParameter* mt = (TargetParameter*)c)
		{
			if (!mt->enabled) continue;
			if (ChannelFamily* m = (ChannelFamily*)(mt->targetContainer.get()))
			{
				ret.add(m);
			}
		}
	}
	return ret;
}

void MoveInBlackManager::createItem(ControllableContainer* cc)
{
	int n = cc->controllables.size() + 1;
	TargetParameter* p = new TargetParameter("Move in black "+String(n), "", "");
	p->setRootContainer(ChannelFamilyManager::getInstance());
	p->targetType = TargetParameter::CONTAINER;
	p->maxDefaultSearchLevel = 0;
	p->isRemovableByUser = true;
	p->canBeDisabledByUser = true;
	p->saveValueOnly = false;
	cc->addParameter(p);
}

void MoveInBlackManager::afterLoadJSONDataInternal()
{
	for (auto& c : controllables)
	{
		if (TargetParameter* mt = dynamic_cast<TargetParameter*>(c))
		{
			mt->targetType = TargetParameter::CONTAINER;
			mt->setRootContainer(ChannelFamilyManager::getInstance());
			//mt->customGetTargetContainerFunc = &InterfaceManager::showAndGetInterfaceOfType<DMXInterface>;
			mt->isRemovableByUser = true;
			mt->canBeDisabledByUser = true;
		}
	}

}

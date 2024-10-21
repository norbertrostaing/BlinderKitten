/*
  ==============================================================================

    FixtureTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "FixturePatch.h"
#include "../Interface/InterfaceIncludes.h"
// #include "../ChannelFamily/ChannelFamilyManager.h"
#include "Fixture.h"
#include "Definitions/FixtureType/FixtureType.h"
#include "UI/PatchSheet/PatchSheet.h"

FixturePatch::FixturePatch(var params) :
	BaseItem(params.getProperty("name", "Patch")),
	objectType(params.getProperty("type", "FixturePatch").toString()),
	corrections("Corrections"),
	objectData(params),
	currentAdresses()
{
    targetInterface = addTargetParameter("Interface", "output interface", InterfaceManager::getInstance());
    targetInterface->targetType = TargetParameter::CONTAINER;
	targetInterface->customGetTargetContainerFunc = &InterfaceManager::showAndGetInterfaceOfType<DMXInterface>;
    targetInterface->maxDefaultSearchLevel = 0;
	saveAndLoadRecursiveData = true;

	currentInterface = addTargetParameter("CurrentInterface", "", InterfaceManager::getInstance());
	currentInterface->targetType = TargetParameter::CONTAINER;
	currentInterface->customGetTargetContainerFunc = &InterfaceManager::showAndGetInterfaceOfType<DMXInterface>;
	currentInterface->maxDefaultSearchLevel = 0;
	currentInterface->hideInEditor = true;
	currentInterface->isSavable = false;

    address = addIntParameter("Address", "Address of this Fixture, 0 means unpatched", 0, 0, 512);
	editorCanBeCollapsed = false;
	corrections.selectItemWhenCreated = false;
	addChildControllableContainer(&corrections);

	Array<DMXInterface*> interfaces = InterfaceManager::getInstance()->getInterfacesOfType<DMXInterface>();
	if (interfaces.size() == 1) {
		targetInterface->setValueFromTarget(interfaces[0]);
	}
};

FixturePatch::~FixturePatch()
{
	disableCurrentPatch();
}

void FixturePatch::onContainerParameterChangedInternal(Parameter* p)
{
	if (p == enabled) {
		if (enabled->boolValue()) {
			if (!patchingInProgess) {
				tryToEnablePatch();
			}
		}
		else {
			disableCurrentPatch();
		}
	}
	if (p == address || p == targetInterface) {
		if (enabled->boolValue()) {
			tryToEnablePatch();
		}
	}
	//LOG(p->niceName);
}

void FixturePatch::disableCurrentPatch()
{
	DMXInterface* inter = dynamic_cast<DMXInterface*>(currentInterface->targetContainer.get());
	if (inter != nullptr) {
		for (int i = 0; i < currentAdresses.size(); i++) {
			if (inter->channelToFixturePatch[currentAdresses[i]] == this) {
				inter->channelToFixturePatch.set(currentAdresses[i], nullptr);
				inter->channelToChannelType.set(currentAdresses[i], nullptr);
				inter->channelToSubFixtureChannel.set(currentAdresses[i], nullptr);
				inter->sendDMXValue(currentAdresses[i], 0);
			}
			else {
				//LOG("meh");
			}
		}
	}
	currentAdresses.clear();
	if (enabled->boolValue()) {
		enabled->setValue(false, false);
	}
}

int FixturePatch::getFixtureId()
{
	if (parentContainer == nullptr) return 0;
	Fixture* parentFixture = dynamic_cast<Fixture*>(parentContainer->parentContainer.get());
	return parentFixture->id->intValue();
}

void FixturePatch::tryToEnablePatch()
{
	int a = address->intValue();
	if (a == 0) {
		return;
	}
	if (parentContainer == nullptr) {
		return;
	}

	disableCurrentPatch();

	Fixture* parentFixture = dynamic_cast<Fixture*>(parentContainer->parentContainer.get());
	FixtureType* ft = dynamic_cast<FixtureType*>(parentFixture->devTypeParam->targetContainer.get());
	DMXInterface* inter = dynamic_cast<DMXInterface*>(targetInterface->targetContainer.get());

	if (ft == nullptr) {return;}
	if (inter == nullptr) {return;}

	Array<ChannelType *> channelTypes;
	int n = 0;
	for (int i = 0; i < ft->chansManager.items.size(); i++) {
		FixtureTypeChannel* chan = ft->chansManager.items[i];
		int c = chan->dmxDelta->intValue();
		channelTypes.add(dynamic_cast<ChannelType*>(chan->channelType->targetContainer.get()));
		if (chan->resolution->getValue() == "16bits") {
			c+=1;
			channelTypes.add(dynamic_cast<ChannelType*>(chan->channelType->targetContainer.get()));
		}
		n = jmax(n,c);
	}

	bool valid = true;
	Array<FixturePatch*> patchs;
	Array<int> fixtureIds; 

	for (int i = 0; i < n; i++) {
		if (inter->channelToFixturePatch[a + i] != nullptr && inter->channelToFixturePatch[a + i] != this) {
			valid = false;
			patchs.add(inter->channelToFixturePatch[a + i]);
			fixtureIds.add(inter->channelToFixturePatch[a + i]->getFixtureId());
		}
	}

	if (!valid) {
		String ids = "";
		for (int i = 0; i < fixtureIds.size(); i++) {
			if (i == 0) ids = " by fixt ";
			if (i>0) ids+= ", ";
			ids += String(fixtureIds[i]);
		}
		const auto callback = juce::ModalCallbackFunction::create([this, patchs](int result) {
			if (result == 1)
			{
				for (int i = 0; i < patchs.size(); i++) {
					patchs[i]->enabled->setValue(false);
				}
				this->enabled->setValue(true);
			}
			else if (result == 2)
			{
			}
				
		});

		juce::AlertWindow::showOkCancelBox(
			juce::AlertWindow::WarningIcon,
			"Be careful !",
			"You can't patch at adress "+String(a)+", already taken"+ids+".",
			"Overwrite",
			"Cancel",
			&ShapeShifterManager::getInstance()->mainContainer,
			callback);

		LOGERROR("You cannot patch on address "+String(a)+"... you're writing on other fixtures.");
		return;
	}

	currentInterface->setValueFromTarget(inter);

	for (int i = 0; i < n; i++) {
		inter->channelToFixturePatch.set(a+i,this);
		if (channelTypes.size() > i) {
			inter->channelToChannelType.set(a+i, channelTypes[i]);
		}
		currentAdresses.add(a+i);
	}


	for (SubFixture* sf : parentFixture->subFixturesContainer) {
		for (SubFixtureChannel* sfc : sf->channelsContainer) {
			if (sfc->parentFixtureTypeChannel != nullptr) {
				int sfcAddress = a + sfc->parentFixtureTypeChannel->dmxDelta->intValue() - 1;
				inter->channelToSubFixtureChannel.set(sfcAddress, sfc);
				if (sfc->resolution == "16bits") {
					inter->channelToSubFixtureChannel.set(sfcAddress + 1, sfc);
				}
			}
		}
	}

	patchingInProgess = true;
	if (!enabled->boolValue()) {
		enabled->setValue(true, false);
	}
	patchingInProgess = false;
	LOG("Patched at address "+String(a));


}

FixturePatchManager::FixturePatchManager():
	BaseManager("Patch")
{
}

FixturePatchManager::FixturePatchManager(const juce::String& name):
	BaseManager(name)
{
}

FixturePatchManager::~FixturePatchManager()
{
}

void FixturePatchManager::askForMoveBefore(BaseItem*)
{
}

void FixturePatchManager::askForMoveAfter(BaseItem*)
{
}

void FixturePatchManager::addItemInternal(FixturePatch* c, var data)
{
}

void FixturePatchManager::askForDuplicateItem(BaseItem* item)
{
	BaseManager::askForDuplicateItem(item);
}

void FixturePatchManager::setItemIndex(FixturePatch* item, int newIndex, bool addToUndo)
{
}

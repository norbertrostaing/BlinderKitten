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

	/*curve->editorIsCollapsed = true;
	curve->setNiceName("Response curve");
	curve->allowKeysOutside = false;
	curve->isSelectable = false;
	curve->length->setValue(1);
	curve->addKey(0, 0, false);
	curve->items[0]->easingType->setValueWithData(Easing::LINEAR);
	curve->addKey(1, 1, false);
	curve->selectItemWhenCreated = false;
	curve->editorCanBeCollapsed = true;
	*/


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
		tryToEnablePatch();
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

void FixturePatch::tryToEnablePatch()
{
	disableCurrentPatch();
	int a = address->intValue();
	if (a == 0) {
		return;
	}
	if (parentContainer == nullptr) {
		return;
	}

	Fixture* parentFixture = dynamic_cast<Fixture*>(parentContainer->parentContainer.get());
	FixtureType* ft = dynamic_cast<FixtureType*>(parentFixture->devTypeParam->targetContainer.get());
	DMXInterface* inter = dynamic_cast<DMXInterface*>(targetInterface->targetContainer.get());

	if (ft == nullptr) {return;}
	if (inter == nullptr) {return;}

	int n = 0;
	for (int i = 0; i < ft->chansManager.items.size(); i++) {
		FixtureTypeChannel* chan = ft->chansManager.items[i];
		int c = chan->dmxDelta->intValue();
		if (chan->resolution->getValue() == "16bits") {
			c+=1;
		}
		n = jmax(n,c);
	}

	bool valid = true;
	for (int i = 0; i < n; i++) {
		if (inter->channelToFixturePatch[a + i] != nullptr && inter->channelToFixturePatch[a + i] != this) {
			valid = false;
			//LOG(inter->channelToFixturePatch[a + i]->parentContainer->parentContainer->niceName);
		}
	}

	if (!valid) {
		LOGERROR("You cannot patch on address "+String(a)+"... you're writing on other fixtures.");
		return;
	}

	currentInterface->setValueFromTarget(inter);

	for (int i = 0; i < n; i++) {
		inter->channelToFixturePatch.set(a+i,this);
		currentAdresses.add(a+i);
	}

	patchingInProgess = true;
	if (!enabled->boolValue()) {
		enabled->setValue(true, false);
	}
	patchingInProgess = false;
	LOG("Patched at address "+String(a));


}


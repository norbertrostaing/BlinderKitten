/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Device.h"
#include "DevicePatch.h"
#include "../../Brain.h"
#include "../DeviceType/DeviceTypeManager.h"
#include "../DeviceType/DeviceType.h"
#include "../Fixture/Fixture.h"
#include "../Fixture/FixtureManager.h"
#include "../Fixture/FixtureChannel.h"
#include "../FixtureParamType/FixtureParamType.h"

Device::Device(var params) :
	BaseItem(params.getProperty("name", "Device")),
	objectType(params.getProperty("type", "Device").toString()),
	objectData(params),
	patchs("Patch"),
	fixtures("Fixtures"),
	devTypeParam()
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = true;

	itemDataType = "Device";
	
	id = addIntParameter("ID", "ID of this device", 1, 1);

	devTypeParam = addTargetParameter("Device type", "Type of device", DeviceTypeManager::getInstance());
	devTypeParam -> targetType = TargetParameter::CONTAINER;
	devTypeParam -> maxDefaultSearchLevel = 0;
	
	// to add a manager with defined data
	patchs.selectItemWhenCreated = false;
	addChildControllableContainer(&patchs);

	fixtures.selectItemWhenCreated = false;
	fixtures.userCanAddItemsManually = false;
	addChildControllableContainer(&fixtures);

	Logger::writeToLog("call from constructor");
	Brain::getInstance()->registerDevice(this, id->getValue());
	checkChildrenFixtures();
}

void Device::afterLoadJSONDataInternal() {
	Logger::writeToLog("call from JSONDataInternal");
	checkChildrenFixtures();
}

Device::~Device()
{
	Brain::getInstance()->unregisterDevice(this);
}


void Device::onContainerNiceNameChanged() 
{
	BaseItem::onContainerNiceNameChanged();
	Logger::writeToLog("call from nameChanged");
	checkChildrenFixtures();
}

void Device::onContainerParameterChangedInternal(Parameter* p) 
{
	LOG("yepaaaa");
	if (p == devTypeParam) 
	{
		Logger::writeToLog("call from ParamChanged");
		checkChildrenFixtures();
	}
	else if(p == id) 
	{
		Brain::getInstance()->registerDevice(this, id->getValue());
	}
}

void Device::applyPatchs() {
	if (devTypeParam == nullptr) { return; }

}

void Device::checkChildrenFixtures() {
	if (devTypeParam == nullptr) { return; }

	DeviceType* t = dynamic_cast<DeviceType*>(devTypeParam->targetContainer.get());
	if (t== nullptr) {
		return ;
	}
	if (t -> chansManager == nullptr) {
		return ;
	}

	Array<WeakReference<ControllableContainer>> chans = t-> chansManager -> getAllContainers();
	Array<String> fixtNames;
	HashMap<String, Array<String>> fixtChannels;

	// clean of unused fixtures
	for (int i = 0; i < chans.size(); i++) {
		DeviceTypeChannel* c = dynamic_cast<DeviceTypeChannel*>(chans[i].get());
		String suffix = c->suffix->getValue();
		String fixtName = niceName+suffix;
		fixtNames.add(fixtName);
		}

	Array<Fixture*> currentFixtures = fixtures.getItemsWithType<Fixture>();
	for (int i = 0; i < currentFixtures.size(); i++) {
		String name = currentFixtures[i]->niceName;
		if (fixtNames.indexOf(name) == -1) {
			fixtures.removeItem(currentFixtures[i]);
		}
	}

	for (int i = 0; i < chans.size(); i++) {
		DeviceTypeChannel* c = dynamic_cast<DeviceTypeChannel*>(chans[i].get());
		String suffix = c->suffix->getValue();
		String fixtName = niceName + suffix;
		Fixture* f = fixtures.getItemWithName(fixtName, true);
		if (f == nullptr) {
			f = new Fixture();
			fixtures.addItem(f);
		}
		if (f != nullptr && f->niceName != fixtName) {
			f->setNiceName(fixtName);
		}
		if (f != nullptr && f->suffixName != suffix) {
			f->suffixName = suffix;
		}
		if (f != nullptr && f->parentDevice != this) {
			f->parentDevice = this;
		}
		FixtureChannel* chan;
		if (c != nullptr) {
			FixtureParamDefinition* param = dynamic_cast<FixtureParamDefinition*>(c -> channelType -> targetContainer.get());
			if (param != nullptr) {
				chan = f -> channels->getItemWithName(param->niceName);
				if (chan == nullptr) {
					chan = new FixtureChannel();
					LOG("hello");
					LOG(param->niceName);
					chan -> setNiceName(param->niceName);
					f -> channels->addItem(chan);
				}
				f -> channelsMap.set(param, chan);
				chan ->isHTP = param->priority->getValue() == "HTP";
				chan-> resolution->setValue(c->resolution->getValue());
				chan-> channelType->setValue(c->channelType->getValue());
				chan->parentParamDefinition = param;
				chan-> parentDeviceTypeChannel = c;
				chan->parentDevice = this;
				chan->parentFixture = f;
			}
		}
		
	}

}


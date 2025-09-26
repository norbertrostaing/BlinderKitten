/*
  ==============================================================================

	DMXInterface.cpp
	Created: 26 Sep 2020 1:51:42pm
	Author:  bkupe

  ==============================================================================
*/

#include "Definitions/Interface/InterfaceIncludes.h"
#include "UI/DMXChannelView.h"
#include "Fixture/FixturePatch.h"
#include "DMXInterface.h"
#include "UserInputManager.h"
#include "Programmer/Programmer.h"
#include "Fixture/Fixture.h"
#include "FixtureType/FixtureTypeChannel.h"

DMXInterface::DMXInterface() :
	Interface(getTypeString())
{
	
	dmxType = addEnumParameter("DMX Type", "Choose the type of dmx interface you want to connect");

	dmxType
		->addOption("Open DMX", DMXDevice::OPENDMX)
		->addOption("Enttec DMX Pro", DMXDevice::ENTTEC_DMXPRO)
		->addOption("Enttec DMX MkII", DMXDevice::ENTTEC_MK2)
		->addOption("Art-Net", DMXDevice::ARTNET)
		->addOption("Eurolite USB-DMX512 Pro", DMXDevice::EUROLITE)
		->addOption("sACN", DMXDevice::SACN)
        ->addOption("DMXKing", DMXDevice::DMXKING);
	dmxType->setValueWithKey("Open DMX");

	dmxConnected = addBoolParameter("Connected", "DMX is connected ?", false);
	dmxConnected->isControllableFeedbackOnly = true;
	dmxConnected->isSavable = false;

	channelTestingMode = addBoolParameter("Channel Testing Mode", "Is testing with the Channel view ?", false);
	channelTestingMode->hideInEditor = true;

	channelTestingFlashValue = addFloatParameter("Testing Value", "Flash value of channel testing", 1, 0, 1);
	channelTestingFlashValue->hideInEditor = true;

	inputToProgrammerBtn = addTrigger("Input to programmer", "Create commands in programmer to copy input");
	
	thruManager.reset(new ControllableContainer("Pass-through"));
	thruManager->userCanAddControllables = true;
	thruManager->customUserCreateControllableFunc = &DMXInterface::createThruControllable;
	addChildControllableContainer(thruManager.get());

	setCurrentDMXDevice(DMXDevice::create((DMXDevice::Type)(int)dmxType->getValueData()));
	addChildControllableContainer(&mappingManager);

	for (int i = 0; i <= 512; i++) {
		channelToFixturePatch.add(nullptr);
		channelToChannelType.add(nullptr);
		channelToSubFixtureChannel.add(nullptr);
	}

}

DMXInterface::~DMXInterface()
{
	tester = nullptr;
}

void DMXInterface::clearItem()
{
	BaseItem::clearItem();
	setCurrentDMXDevice(nullptr);
}

void DMXInterface::onContainerParameterChanged(Parameter* p)
{
	Interface::onContainerParameterChanged(p);
	if (p == enabled)
	{
		if (dmxDevice != nullptr) dmxDevice->enabled = enabled->boolValue();

	}
	else if (p == dmxType)
	{
		setCurrentDMXDevice(DMXDevice::create((DMXDevice::Type)(int)dmxType->getValueData()));
	}
}

void DMXInterface::onContainerNiceNameChanged()
{
	InterfaceManager::getInstance()->managerNotifier.addMessage(new InterfaceManager::ManagerEvent(InterfaceManager::ManagerEvent::NEEDS_UI_UPDATE));
}

void DMXInterface::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	if (cc == thruManager.get()) {
		if (dmxDevice != nullptr) {

		}
		for (int i = 0; i < 512; i++) {
			sendDMXValue(i+1, dmxDevice->dmxDataOut[i]);
		}
	}
}

void DMXInterface::setCurrentDMXDevice(DMXDevice* d)
{
	if (dmxDevice.get() == d) return;

	if (dmxDevice != nullptr)
	{
		dmxDevice->removeDMXDeviceListener(this);
		dmxDevice->clearDevice();
		removeChildControllableContainer(dmxDevice.get());
	}

	dmxDevice.reset(d);

	dmxConnected->hideInEditor = dmxDevice == nullptr || dmxDevice->type == DMXDevice::ARTNET;
	dmxConnected->setValue(false);

	if (dmxDevice != nullptr)
	{
		dmxDevice->enabled = enabled->boolValue();
		dmxDevice->addDMXDeviceListener(this);
		addChildControllableContainer(dmxDevice.get());
		//dmxConnected->setValue(true);
	}
}

void DMXInterface::sendDMXValue(int channel, int value)
{
	sendDMXValue(channel, value, Array<DMXInterface*>());
}

void DMXInterface::sendDMXValues(int startChannel, Array<int> values)
{
	sendDMXValues(startChannel, values, Array<DMXInterface*>());
}

void DMXInterface::send16BitDMXValue(int startChannel, int value, DMXByteOrder byteOrder)
{
	send16BitDMXValue(startChannel, value, byteOrder, Array<DMXInterface*>());
}

void DMXInterface::send16BitDMXValues(int startChannel, Array<int> values, DMXByteOrder byteOrder)
{
	send16BitDMXValues(startChannel, values, byteOrder, Array<DMXInterface*>());
}

void DMXInterface::sendDMXValue(int channel, int value, Array<DMXInterface*>callers)
{
	if (channel<1 || channel> 512) {return;}
	if (callers.contains(this)) {return;}
	if (!enabled->boolValue() || dmxDevice == nullptr) return;
	if (logOutgoingData->boolValue()) NLOG(niceName, "Send DMX : " + String(channel) + " > " + String(value));
	dmxDevice->sendDMXValue(channel, value);
	repaintChannels(channel, 1);
	callers.add(this);
	for (auto& c : thruManager->controllables)
	{
		if (TargetParameter* mt = (TargetParameter*)c)
		{
			if (!mt->enabled) continue;
			if (DMXInterface* m = (DMXInterface*)(mt->targetContainer.get()))
			{
				m->sendDMXValue(channel, value, callers);
			}
		}
	}

}

void DMXInterface::sendDMXValues(int startChannel, Array<int> values, Array<DMXInterface*>callers)
{
	if (callers.contains(this)) { return; }
	if (!enabled->boolValue() || dmxDevice == nullptr) return;
	if (logOutgoingData->boolValue())
	{
		String s = "Send DMX : " + String(startChannel) + ", " + String(values.size()) + " values";
		int ch = startChannel;
		for (auto& v : values)
		{
			s += "\nChannel " + String(ch) + " : " + String(v);
			ch++;
		}
		NLOG(niceName, s);
	}

	dmxDevice->sendDMXRange(startChannel, values);
	repaintChannels(startChannel, values.size());
	callers.add(this);
	for (auto& c : thruManager->controllables)
	{
		if (TargetParameter* mt = (TargetParameter*)c)
		{
			if (!mt->enabled) continue;
			if (DMXInterface* m = (DMXInterface*)(mt->targetContainer.get()))
			{
				m->sendDMXValues(startChannel, values, callers);
			}
		}
	}
}

void DMXInterface::send16BitDMXValue(int startChannel, int value, DMXByteOrder byteOrder, Array<DMXInterface*>callers)
{
	if (callers.contains(this)) { return; }
	if (!enabled->boolValue() || dmxDevice == nullptr) return;
	if (logOutgoingData->boolValue()) NLOG(niceName, "Send 16-bit DMX : " + String(startChannel) + " > " + String(value));
	dmxDevice->sendDMXValue(startChannel, byteOrder == MSB ? (value >> 8) & 0xFF : value & 0xFF);
	dmxDevice->sendDMXValue(startChannel + 1, byteOrder == MSB ? 0xFF : (value >> 8) & 0xFF);
	repaintChannels(startChannel, 2);

	callers.add(this);
	for (auto& c : thruManager->controllables)
	{
		if (TargetParameter* mt = (TargetParameter*)c)
		{
			if (!mt->enabled) continue;
			if (DMXInterface* m = (DMXInterface*)(mt->targetContainer.get()))
			{
				m->send16BitDMXValue(startChannel, value, byteOrder, callers);
			}
		}
	}
}

void DMXInterface::send16BitDMXValues(int startChannel, Array<int> values, DMXByteOrder byteOrder, Array<DMXInterface*>callers)
{
	if (callers.contains(this)) { return; }
	if (!enabled->boolValue() || dmxDevice == nullptr) return;
	if (logOutgoingData->boolValue()) NLOG(niceName, "Send 16-bit DMX : " + String(startChannel) + " > " + String(values.size()) + " values");

	Array<int> dmxValues;
	int numValues = values.size();
	dmxValues.resize(numValues * 2);
	for (int i = 0; i < numValues; ++i)
	{
		int value = values[i];
		dmxValues.set(i * 2, byteOrder == MSB ? (value >> 8) & 0xFF : value & 0xFF);
		dmxValues.set(i * 2 + 1, byteOrder == MSB ? 0xFF : (value >> 8) & 0xFF);
	}

	dmxDevice->sendDMXRange(startChannel, dmxValues);
	repaintChannels(startChannel, dmxValues.size());

	callers.add(this);
	for (auto& c : thruManager->controllables)
	{
		if (TargetParameter* mt = (TargetParameter*)c)
		{
			if (!mt->enabled) continue;
			if (DMXInterface* m = (DMXInterface*)(mt->targetContainer.get()))
			{
				m->send16BitDMXValues(startChannel, values, byteOrder, callers);
			}
		}
	}
}

void DMXInterface::dmxDeviceConnected()
{
	dmxConnected->setValue(true);
}

void DMXInterface::dmxDeviceDisconnected()
{
	dmxConnected->setValue(false);
}

void DMXInterface::dmxDataInChanged(int numChannels, uint8* values)
{
	if (isClearing || !enabled->boolValue()) return;
	//if (logIncomingData->boolValue()) NLOG(niceName, "DMX In : " + String(numChannels) + " channels received.");
}


//void DMXInterface::sendValuesForObject(Object* o)
//{
//	if (channelTestingMode->boolValue()) return;
//
//	Interface::sendValuesForObject(o);
//
//	DMXParams* dmxParams = dynamic_cast<DMXParams*>(o->interfaceParameters.get());
//	
//	jassert(dmxParams != nullptr);
//
//	int startChannel = dmxParams->startChannel->intValue();
//	HashMap<int, float> compValues;
//	
//	for (auto& c : o->componentManager->items)
//	{
//		if (!c->enabled->boolValue()) continue;
//		c->fillOutValueMap(compValues, startChannel);
//	}
//
//	HashMap<int, float>::Iterator it(compValues);
//	while (it.next()) sendDMXValue(it.getKey(), it.getValue() * 255);
//
//}
//


InterfaceUI* DMXInterface::createUI()
{
	return new DMXInterfaceUI(this);
}


// DMX PARAMS
DMXInterface::DMXParams::DMXParams() :
	ControllableContainer("DMX Params")
{

	startChannel = addIntParameter("Start Channel", "The first channel to set the values. The values will automatically distributed depending on the object.", 1, 1, 512);
}

void DMXInterface::createThruControllable(ControllableContainer* cc)
{
	TargetParameter* p = new TargetParameter("Output Universe", "Target module to send the raw data to", "");
	//p->setRootContainer(InterfaceManager::getInstance());
	p->targetType = TargetParameter::CONTAINER;
	p->maxDefaultSearchLevel = 0;
	p->customGetTargetContainerFunc = &InterfaceManager::showAndGetInterfaceOfType<DMXInterface>;
	p->isRemovableByUser = true;
	p->canBeDisabledByUser = true;
	p->saveValueOnly = false;
	cc->addParameter(p);
}

void DMXInterface::afterLoadJSONDataInternal()
{	
	Interface::afterLoadJSONDataInternal();

	if (thruManager != nullptr)
	{
		//thruManager->loadJSONData(data.getProperty("thru", var()));
		for (auto& c : thruManager->controllables)
		{
			if (TargetParameter* mt = dynamic_cast<TargetParameter*>(c))
			{
				mt->targetType = TargetParameter::CONTAINER;
				//mt->setRootContainer(InterfaceManager::getInstance());
				mt->customGetTargetContainerFunc = &InterfaceManager::showAndGetInterfaceOfType<DMXInterface>;
				mt->isRemovableByUser = true;
				mt->canBeDisabledByUser = true;
			}
		}
	}


	/*auto params = thruManager->getAllParameters();
	for (int i = 0; i < params.size(); i++) {
		TargetParameter* p = (TargetParameter*)params[i].get();
		String v = p->ghostValue;

		p->setRootContainer(InterfaceManager::getInstance());
		p->targetType = TargetParameter::CONTAINER;
		p->maxDefaultSearchLevel = 0;
		p->setGhostValue(v);

		if (v != "") {
			auto t = InterfaceManager::getInstance();
			if (t != nullptr) {
				p->setValueFromTarget(t);
			}
		}
	}*/
}

void DMXInterface::repaintChannels(int chan, int n)
{
	chan = chan-1;
	if (channelTestingMode->boolValue()) {
		if (tester != nullptr) {
			for (int i = 0; i < n; i++) {
				int ad = chan + i;
				tester->channelItems[ad]->value = dmxDevice->dmxDataOut[ad] / 255.;
				MessageManager::callAsync([this, ad]() {
					if (tester != nullptr) {
						tester->channelItems[ad]->repaint();
					}
				});
			}
			//tester->repaint();
			//MessageManager::callAsync([this]() {
			//	if (tester != nullptr) {
			//	}
			//});
		}
	}

}

void DMXInterface::dmxChannelInChanged(int channel, uint8 val)
{
	if (!enabled->boolValue()) { return; }
	if (logIncomingData->boolValue()) NLOG(niceName, "DMX Channel received: " << channel << ", value : " << val );
	mappingManager.handleChannel(channel, val, niceName);

}

void DMXInterface::inputToProgrammer()
{
	if (dmxDevice != nullptr) {
		Programmer* p = UserInputManager::getInstance()->getProgrammer(true);
		HashMap<SubFixture*, Command*> sfToCmd;
		Array<SubFixtureChannel*> alreadyDone;
		for (int i = 0; i < 512; i++) {
			if (dmxDevice->dmxDataIn[i] != dmxDevice->dmxDataOut[i]) {
				SubFixtureChannel* sfc = channelToSubFixtureChannel[i+1];
				if (sfc != nullptr && !alreadyDone.contains(sfc)) {
					SubFixture* sf = sfc->parentSubFixture;
					Command* c;
					if (sfToCmd.contains(sf)) {
						c = sfToCmd.getReference(sf);
					}
					else {
						c = p->commands.addItem();
						Fixture* f = sfc->parentFixture;
						c->selection.items[0]->targetType->setValueWithData("Fixture");
						c->selection.items[0]->valueFrom->setValue(f->id->intValue());
						if (f->subFixturesContainer.size() > 1) {
							c->selection.items[0]->subSel->setValue(true);
							c->selection.items[0]->subFrom->setValue(sf->subId);
						}
						sfToCmd.set(sf, c);
						c->values.items.clear();
					}
					CommandValue* cv = c->values.addItem();
					cv->channelType->setValueFromTarget(sfc->channelType);
					if (sfc->resolution == "16bits") {
						int address = channelToFixturePatch[i+1]->address->intValue();
						address += sfc->parentFixtureTypeChannel->dmxDelta->intValue() -1;
						float finalValue = dmxDevice->dmxDataIn[address-1]*256;
						finalValue += dmxDevice->dmxDataIn[address];
						finalValue /= 65535.0;
						cv->valueFrom->setValue(finalValue);
					}
					else {
						float finalValue = dmxDevice->dmxDataIn[i] / 255.0;
						cv->valueFrom->setValue(finalValue);
					}
					alreadyDone.add(sfc);
				}
			}
		}
		p->selectNextCommand();
		p->selectPrevCommand();
		UserInputManager::getInstance()->programmerCommandStructureChanged();
	}
}

void DMXInterface::triggerTriggered(Trigger* t)
{
	if (t == inputToProgrammerBtn) {
		inputToProgrammer();
	}
}

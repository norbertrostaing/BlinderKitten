/*
  ==============================================================================

    NDIDeviceParameter.cpp
    Created: 20 Dec 2016 3:05:54pm
    Author:  Ben

  ==============================================================================
*/
#include "Common/CommonIncludes.h"

NDIDeviceParameter::NDIDeviceParameter(const String & name) :
	Parameter(CUSTOM, name, "NDI Devices",var(), var(),var()),
	inputDevice(nullptr)
{
	NDIManager::getInstance()->addNDIManagerListener(this);
	value = "";
}

NDIDeviceParameter::~NDIDeviceParameter()
{
	if (NDIManager::getInstanceWithoutCreating() != nullptr)
	{
		NDIManager::getInstance()->removeNDIManagerListener(this);
	}
}



void NDIDeviceParameter::setInputDevice(NDIInputDevice * i)
{
	var val = i != nullptr ? i->name : "";

	if (i != nullptr)
	{
		ghostDeviceIn = value;
		ghostDeviceNameIn = i->name;
	}

	inputDevice = i;
	setValue(val);
}

void NDIDeviceParameter::NDIDeviceInAdded(NDIInputDevice * i)
{	
	//DBG("Device In added " << i->name << " / " << ghostDeviceIn);	
	LOG(i->name<< " == "<<ghostDeviceIn);
	if (inputDevice == nullptr && i->name == ghostDeviceIn)
	{
		setInputDevice(nullptr);
		setInputDevice(i);
	}
}

void NDIDeviceParameter::NDIDeviceInRemoved(NDIInputDevice * i)
{
	if (i == inputDevice)
	{
		if (i != nullptr)
		{
			ghostDeviceNameIn = i->name;
		}
		setInputDevice(nullptr);
	}
}

NDIDeviceParameterUI * NDIDeviceParameter::createNDIParameterUI(Array<NDIDeviceParameter *> parameters)
{
	if (parameters.size() == 0) parameters = { this };
	return new NDIDeviceParameterUI(parameters);
}

ControllableUI * NDIDeviceParameter::createDefaultUI(Array<Controllable *> controllables)
{
	Array<NDIDeviceParameter*> parameters = Inspectable::getArrayAs<Controllable, NDIDeviceParameter>(controllables);
	if (parameters.size() == 0) parameters.add(this);
	return createNDIParameterUI(parameters);
}

void NDIDeviceParameter::loadJSONDataInternal(var data)
{
	Parameter::loadJSONDataInternal(data);
	//setInputDevice(NDIManager::getInstance()->getInputDeviceWithID(value[0]));

	if (inputDevice == nullptr) ghostDeviceIn = data.getProperty("value", var());	

}

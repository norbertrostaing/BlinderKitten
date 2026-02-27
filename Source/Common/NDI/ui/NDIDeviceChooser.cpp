/*
  ==============================================================================

	NDIDeviceChooser.cpp
	Created: 20 Dec 2016 12:35:11pm
	Author:  Ben

  ==============================================================================
*/

#include "Common/CommonIncludes.h"

NDIDeviceChooser::NDIDeviceChooser() :
	currentInputDevice(nullptr)
{
	addAndMakeVisible(&inputBox);

	setGhostValue("");

	inputBox.addListener(this);

	NDIManager::getInstance()->addNDIManagerListener(this);

	updateInputComboBox();
}

NDIDeviceChooser::~NDIDeviceChooser()
{
	if (NDIManager::getInstanceWithoutCreating() != nullptr)
	{
		NDIManager::getInstance()->removeNDIManagerListener(this);
	}
}

void NDIDeviceChooser::resized()
{
	Rectangle<int> r = getLocalBounds();
	//int th = (showInputs != showOutputs)?getHeight():getHeight() / 2 - 2;

	if (showInputs)
	{
		Rectangle<int> ir = r;
		//inputLabel.setBounds(ir.removeFromLeft(50));
		//r.removeFromLeft(10);
		inputBox.setBounds(ir);

		r.removeFromTop(4);
	}

}

void NDIDeviceChooser::setGhostValue(const String& inValue)
{
	inputBox.setTextWhenNoChoicesAvailable(inValue.isEmpty() ? "No NDI In Available" : "Disconnected : " + inValue);
	inputBox.setTextWhenNothingSelected(inValue.isEmpty() ? "No NDI In Selected" : "Disconnected : " + inValue);
}

void NDIDeviceChooser::updateInputComboBox()
{
	inputBox.clear(dontSendNotification);
	int index = 1;
	int idToSelect = 0;
	inputBox.addItem("Don't use input", -1);
	for (auto& i : NDIManager::getInstance()->inputs)
	{
		if (currentInputDevice == i) idToSelect = index;
		inputBox.addItem(i->name, index);
		index++;
	}

	inputBox.setSelectedId(idToSelect, dontSendNotification);
}

void NDIDeviceChooser::setSelectedInputDevice(NDIInputDevice* i)
{
	inputBox.setSelectedId(NDIManager::getInstance()->inputs.indexOf(i) + 1, dontSendNotification);
}

void NDIDeviceChooser::setSelectedInputDevice(const String& deviceName)
{
	setSelectedInputDevice(NDIManager::getInstance()->getInputDeviceWithName(deviceName));
}

void NDIDeviceChooser::comboBoxChanged(ComboBox* cb)
{
	int deviceIndex = cb->getSelectedId() - 1;

	if (cb == &inputBox)
	{
		currentInputDevice = deviceIndex >= 0 ? NDIManager::getInstance()->inputs[deviceIndex] : nullptr;
		chooserListeners.call(&ChooserListener::NDIDeviceInSelected, currentInputDevice);
	}

}

void NDIDeviceChooser::NDIDeviceInAdded(NDIInputDevice*)
{
	MessageManager::callAsync([this]() {updateInputComboBox(); });
}

void NDIDeviceChooser::NDIDeviceInRemoved(NDIInputDevice*)
{
	MessageManager::callAsync([this](){updateInputComboBox(); });
}


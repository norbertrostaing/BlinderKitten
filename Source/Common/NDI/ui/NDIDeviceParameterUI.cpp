/*
  ==============================================================================

    NDIDeviceParameterUI.cpp
    Created: 20 Dec 2016 3:06:05pm
    Author:  Ben

  ==============================================================================
*/
#include "Common/CommonIncludes.h"

NDIDeviceParameterUI::NDIDeviceParameterUI(Array<NDIDeviceParameter *> _NDIParams) :
	ParameterUI(Inspectable::getArrayAs<NDIDeviceParameter, Parameter>(_NDIParams)),
	NDIParams(_NDIParams),
	NDIParam(_NDIParams[0])
{
	addAndMakeVisible(&chooser);
	chooser.addNDIChooserListener(this);
	chooser.setSelectedInputDevice(NDIParam->inputDevice);
	chooser.setGhostValue(NDIParam->ghostDeviceIn);

	setSize(100, 20);
}

NDIDeviceParameterUI::~NDIDeviceParameterUI()
{
}

void NDIDeviceParameterUI::resized()
{
	chooser.setBounds(getLocalBounds());
}

void NDIDeviceParameterUI::valueChanged(const var & /*value*/)
{
	chooser.setSelectedInputDevice(NDIParam->inputDevice);
	chooser.setGhostValue(NDIParam->ghostDeviceNameIn);
}

void NDIDeviceParameterUI::NDIDeviceInSelected(NDIInputDevice * d)
{
	NDIParam->setInputDevice(d);
}


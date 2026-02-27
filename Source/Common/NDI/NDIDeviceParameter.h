/*
  ==============================================================================

    NDIDeviceParameter.h
    Created: 20 Dec 2016 3:05:54pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class NDIDeviceParameterUI;

class NDIDeviceParameter :
	public Parameter,
	public NDIManager::NDIManagerListener
{
public:
	NDIDeviceParameter(const String &name);
	~NDIDeviceParameter();

	NDIInputDevice * inputDevice;
	
	String ghostDeviceIn;
	String ghostDeviceOut;
	String ghostDeviceNameIn;
	String ghostDeviceNameOut;

	void setInputDevice(NDIInputDevice * i);

	// Inherited via Listener
	virtual void NDIDeviceInAdded(NDIInputDevice *) override;
	virtual void NDIDeviceInRemoved(NDIInputDevice *) override;

	NDIDeviceParameterUI* createNDIParameterUI(Array<NDIDeviceParameter*> parameters = {});
	ControllableUI * createDefaultUI(Array<Controllable*> controllables = {}) override;

	void loadJSONDataInternal(var data) override;

	String getTypeString() const override { return "NDIDevice"; }

};
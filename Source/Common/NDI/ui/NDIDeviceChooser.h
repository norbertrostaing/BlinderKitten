/*
  ==============================================================================

    NDIDeviceChooser.h
    Created: 20 Dec 2016 12:35:11pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class NDIDeviceChooser :
	public Component,
	public ComboBox::Listener,
	public NDIManager::NDIManagerListener
{
public:
	NDIDeviceChooser();
	~NDIDeviceChooser();

	bool showInputs;
	//Label inputLabel;
	ComboBox inputBox;

	bool showOutputs;
	//Label outputLabel;
	ComboBox outputBox;

	NDIInputDevice * currentInputDevice;

	void resized() override;

	void setGhostValue(const String &inValue);

	void updateInputComboBox();

	void setSelectedInputDevice(NDIInputDevice * );
	void setSelectedInputDevice(const String &deviceName);

	virtual void comboBoxChanged(ComboBox * ccb) override;

	virtual void NDIDeviceInAdded(NDIInputDevice *) override;
	virtual void NDIDeviceInRemoved(NDIInputDevice *) override;

	class  ChooserListener
	{
	public:
		/** Destructor. */
		virtual ~ChooserListener() {}
		virtual void NDIDeviceInSelected(NDIInputDevice * /*input*/) {}
	};

	ListenerList<ChooserListener> chooserListeners;
	void addNDIChooserListener(ChooserListener* newListener) { chooserListeners.add(newListener); }
	void removeNDIChooserListener(ChooserListener* listener) { chooserListeners.remove(listener); }

};

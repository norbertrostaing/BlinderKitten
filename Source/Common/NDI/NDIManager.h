/*
  ==============================================================================

    NDIManager.h
    Created: 20 Dec 2016 12:33:33pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class NDIManager :
	public Thread
{
public:
	juce_DeclareSingleton(NDIManager,true)
	NDIManager();
	~NDIManager();

	NDIlib_find_instance_t pNDI_find;
	HashMap<String, std::shared_ptr<NDIlib_source_t>> sources;
	OwnedArray<NDIInputDevice> inputs;

	void checkDevices();
	NDIInputDevice* addInputDeviceIfNotThere(NDIlib_source_t info);
	void removeInputDevice(NDIInputDevice * d);

	NDIInputDevice * getInputDeviceWithName(const String &name);

	class NDIManagerListener
	{
	public:
		/** Destructor. */
		virtual ~NDIManagerListener() {}
		virtual void NDIDeviceInAdded(NDIInputDevice* /*input*/) {}
		virtual void NDIDeviceInRemoved(NDIInputDevice* /*input*/) {}
	};

	ListenerList<NDIManagerListener> listeners;
	void addNDIManagerListener(NDIManagerListener* newListener) { listeners.add(newListener); }
	void removeNDIManagerListener(NDIManagerListener* listener) { listeners.remove(listener); }



	// Inherited via Timer
	virtual void run() override;
	
	JUCE_DECLARE_NON_COPYABLE(NDIManager)
};

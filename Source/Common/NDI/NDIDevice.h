/*
  ==============================================================================

    NDIDevice.h
    Created: 20 Dec 2016 1:17:56pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class NDIDevice
{
public:
	enum Type { NDI_IN, NDI_OUT };
	NDIDevice(NDIlib_source_t &deviceName, Type t);
	virtual ~NDIDevice() {}

	String name;
	Type type;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NDIDevice)
};

class NDIInputDevice :
	public NDIDevice,
	public Thread
{
public:
	NDIInputDevice(NDIlib_source_t &info);
	~NDIInputDevice();
	//std::unique_ptr<NDIInput> device;

	NDIlib_recv_instance_t pNDI_recv;
	NDIlib_source_t* p_source;

	// Inherited via NDIInputCallback
	//virtual void handleIncomingNDIMessage(NDIInput * source, const NDIMessage & message) override;

	void run() override;
	bool shouldProcess = false;

	class NDIInputListener
	{
	public:
		/** Destructor. */
		virtual ~NDIInputListener() {}
		virtual void videoFrameReceived(NDIlib_video_frame_v2_t* frame) {}
		//virtual void noteOnReceived(const int&/*channel*/, const int&/*pitch*/, const int&/*velocity*/) {}
	};

	ListenerList<NDIInputListener> inputListeners;
	void addNDIInputListener(NDIInputListener* newListener);
	void removeNDIInputListener(NDIInputListener* listener);


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NDIInputDevice)

};

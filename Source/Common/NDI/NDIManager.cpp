/*
  ==============================================================================

	NDIManager.cpp
	Created: 20 Dec 2016 12:33:33pm
	Author:  Ben

  ==============================================================================
*/
#include "Common/CommonIncludes.h"

juce_ImplementSingleton(NDIManager)

NDIManager::NDIManager() :
    Thread("NDI Manager")
{
	// ndiRouterDefaultType = dynamic_cast<BKEngine *>(Engine::mainEngine)->defaultBehaviors.addEnumParameter("NDI Router Ouput Type","Choose the default type when choosing a NDI Module as Router output");
	// ndiRouterDefaultType->addOption("Control Change", NDIManager::CONTROL_CHANGE)->addOption("Note On", NDIManager::NOTE_ON)->addOption("Note Off", NDIManager::NOTE_OFF);
    pNDI_find = NDIlib_find_create_v2();

    startThread();
}

NDIManager::~NDIManager()
{
    stopThread(3000);
}

void NDIManager::checkDevices()
{
	//INPUTS
	//LOG("searching for devices");
    uint32_t no_sources = 0;
    const NDIlib_source_t* p_sources = NULL;
    NDIlib_find_wait_for_sources(pNDI_find, 1000);
    p_sources = NDIlib_find_get_current_sources(pNDI_find, &no_sources);

    for (int i = inputs.size()-1; i >= 0; i--) {
        String key = inputs[i]->name;
        bool isPresent = false;
        for (uint32_t j = 0; j < no_sources && !isPresent; j++) {
            isPresent = String(p_sources[j].p_ndi_name) == key;
        }
        if (!isPresent) {
            removeInputDevice(inputs[i]);
        }
    }

    if (no_sources > 0) {
        for (uint32_t i = 0; i < no_sources; i++) {
            NDIInputDevice* input = getInputDeviceWithName(p_sources[i].p_ndi_name);
            if (input == nullptr) {
                NDIlib_source_t t = p_sources[i];
                input = addInputDeviceIfNotThere(t);
            }
            //input->p_source = const_cast<NDIlib_source_t*>(&p_sources[i]);
        }
    }
}

NDIInputDevice* NDIManager::addInputDeviceIfNotThere(NDIlib_source_t info)
{
	NDIInputDevice* d = new NDIInputDevice(info);
	inputs.add(d);

	NLOG("NDI", "Device In Added : " << d->name << "");

	listeners.call(&NDIManagerListener::NDIDeviceInAdded, d);
    return d;
}

void NDIManager::removeInputDevice(NDIInputDevice* d)
{
	inputs.removeObject(d, false);

	NLOG("NDI", "Device In Removed : " << d->name << "");

	listeners.call(&NDIManagerListener::NDIDeviceInRemoved, d);
	delete d;
}


NDIInputDevice* NDIManager::getInputDeviceWithName(const String& name)
{
	for (auto& d : inputs) if (d->name == name) return d;
	return nullptr;
}

void NDIManager::run()
{
    while (!threadShouldExit()) {
        checkDevices();
        wait(1000);
    }
}
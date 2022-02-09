/*
  ==============================================================================

    DeviceTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "DevicePatch.h"
#include "../Interface/InterfaceIncludes.h"
// #include "../FixtureParamType/FixtureParamTypeManager.h"

DevicePatch::DevicePatch() :
    BaseItem("Patch")
{
    targetInterface = addTargetParameter("Interface", "output interface", InterfaceManager::getInstance());
    targetInterface->targetType = TargetParameter::CONTAINER;
    targetInterface->maxDefaultSearchLevel = 0;

    address = addIntParameter("Address", "Address of this device, 0 means unpatched", 0, 0, 512);
	/*curve->editorIsCollapsed = true;
	curve->setNiceName("Response curve");
	curve->allowKeysOutside = false;
	curve->isSelectable = false;
	curve->length->setValue(1);
	curve->addKey(0, 0, false);
	curve->items[0]->easingType->setValueWithData(Easing::LINEAR);
	curve->addKey(1, 1, false);
	curve->selectItemWhenCreated = false;
	curve->editorCanBeCollapsed = true;
	*/


};

DevicePatch::~DevicePatch()
{
};



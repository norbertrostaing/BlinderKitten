/*
  ==============================================================================

    FixtureTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "FixturePatch.h"
#include "../Interface/InterfaceIncludes.h"
// #include "../ChannelFamily/ChannelFamilyManager.h"

FixturePatch::FixturePatch(var params) :
	BaseItem(params.getProperty("name", "Patch")),
	objectType(params.getProperty("type", "FixturePatch").toString()),
	corrections("Corrections"),
	objectData(params)
{
    targetInterface = addTargetParameter("Interface", "output interface", InterfaceManager::getInstance());
    targetInterface->targetType = TargetParameter::CONTAINER;
    targetInterface->maxDefaultSearchLevel = 0;
	saveAndLoadRecursiveData = true;

    address = addIntParameter("Address", "Address of this Fixture, 0 means unpatched", 0, 0, 512);

	corrections.selectItemWhenCreated = false;
	addChildControllableContainer(&corrections);


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

FixturePatch::~FixturePatch()
{
};



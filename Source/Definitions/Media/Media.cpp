/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Media.h"
#include "MediaManager.h"
#include "../Command/CommandSelectionManager.h"
#include "../../Brain.h"
#include "UserInputManager.h"

MediaUI::MediaUI(Media* item) :
	BaseItemUI(item)
{
}

MediaUI::~MediaUI()
{
}


Media::Media(var params) :
	BaseItem(params.getProperty("name", "Media")),
	objectType(params.getProperty("type", "Media").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;
	canBeDisabled = false;

	itemDataType = "Media";

	id = addIntParameter("ID", "ID of this Media", 1, 1);
	userName = addStringParameter("Name", "Name of this Media","New Media");
	updateName();

	Brain::getInstance()->registerMedia(this, id->getValue());
}

Media::~Media()
{
	Brain::getInstance()->unregisterMedia(this);
}

void Media::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<MediaManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - "+n);
}

void Media::onContainerParameterChangedInternal(Parameter* p) {
	if (p == userName || p == id) {
		updateName();
	}
	if (p == id) {
		Brain::getInstance()->registerMedia(this, id->getValue(), true);
	}
}



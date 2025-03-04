/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "BKPathPreset.h"
#include "../Command/CommandSelectionManager.h"
#include "../../Brain.h"
#include "BKPathPresetManager.h"

BKPathPreset::BKPathPreset(var params) :
	BaseItem(params.getProperty("name", "BKPathPreset")),
	objectType(params.getProperty("type", "BKPathPreset").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;
	canBeDisabled = false;

	itemDataType = "BKPathPreset";

	id = addIntParameter("ID", "ID of this path preset", 1, 1);
	userName = addStringParameter("Name", "Name of this path preset", "New path preset");
	updateName();

	useAnotherId = addIntParameter("Use another", "id of another preset to use instead of this one", 0, 0);

	pointsManager.reset(new ControllableContainer("Points"));
	pointsManager->userCanAddControllables = true;
	pointsManager->customUserCreateControllableFunc = &BKPathPreset::createPoint;
	addChildControllableContainer(pointsManager.get());

	Brain::getInstance()->registerBKPathPreset(this, id->getValue());
	updateDisplay();
}

BKPathPreset::~BKPathPreset()
{
	Brain::getInstance()->unregisterBKPathPreset(this);
}

void BKPathPreset::onContainerParameterChangedInternal(Parameter* p) {
	if (p == userName || p == id) {
		updateName();
	}
	if (p == id) {
		Brain::getInstance()->registerBKPathPreset(this, id->getValue(), true);
	}	
}

void BKPathPreset::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<BKPathPresetManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
}

void BKPathPreset::updateDisplay()
{
	queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void BKPathPreset::afterLoadJSONDataInternal()
{
	if (pointsManager != nullptr)
	{
		for (auto& c : pointsManager->controllables)
		{
			if (Point2DParameter* p = dynamic_cast<Point2DParameter*>(c))
			{
				p->isRemovableByUser = true;
				p->canBeDisabledByUser = false;
				p->saveValueOnly = false;
				p->userCanChangeName = true;
			}
		}
	}

}

void BKPathPreset::createPoint(ControllableContainer* cc)
{
	Point2DParameter* p = new Point2DParameter("Point", "relative position of the point");
	p->isRemovableByUser = true;
	p->canBeDisabledByUser = false;
	p->saveValueOnly = false;
	p->userCanChangeName = true;
	cc->addParameter(p);
	bool valid = false;
	int id = cc->getAllParameters().size()-1;
	while (!valid) {
		id ++;
		Parameter* test = cc->getParameterByName("Point "+String(id));
		valid = test == nullptr;
	}
	p->setNiceName("Point "+String(id));
}


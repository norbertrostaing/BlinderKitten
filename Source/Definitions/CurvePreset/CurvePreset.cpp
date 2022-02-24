/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "CurvePreset.h"
#include "../Command/CommandSelectionManager.h"
#include "../../Brain.h"
#include "CurvePresetManager.h"

CurvePreset::CurvePreset(var params) :
	BaseItem(params.getProperty("name", "CurvePreset")),
	objectType(params.getProperty("type", "CurvePreset").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;

	itemDataType = "CurvePreset";

	id = addIntParameter("ID", "ID of this curve", 1, 1);
	userName = addStringParameter("Name", "Name of this curve", "New curve");
	updateName();


	curve.saveAndLoadRecursiveData = true;
	curve.setNiceName("Curve");
	curve.allowKeysOutside = false;
	curve.isSelectable = false;
	curve.length->setValue(1);
	curve.addKey(0, 0, false);
	curve.items[0]->easingType->setValueWithData(Easing::LINEAR);
	curve.addKey(1, 1, false);
	curve.selectItemWhenCreated = false;
	curve.editorCanBeCollapsed = true;
	addChildControllableContainer(&curve);

	Brain::getInstance()->registerCurvePreset(this, id->getValue());
}

CurvePreset::~CurvePreset()
{
	Brain::getInstance()->unregisterCurvePreset(this);
}

void CurvePreset::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerCurvePreset(this, id->getValue(), true);
	}
	if (p == userName || p == id) {
		updateName();
	}
}

void CurvePreset::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<CurvePresetManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
}


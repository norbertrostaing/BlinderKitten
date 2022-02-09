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

CurvePreset::CurvePreset(var params) :
	BaseItem(params.getProperty("name", "CurvePreset")),
	objectType(params.getProperty("type", "CurvePreset").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;

	itemDataType = "CurvePreset";

	id = addIntParameter("ID", "ID of this curve", 1, 1);

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

void CurvePreset::parameterValueChanged(Parameter* p) {
	BaseItem::parameterValueChanged(p);
	Logger::writeToLog("changed !");
	if (p == id) {
		Brain::getInstance()->registerCurvePreset(this, id->getValue());
	}
}

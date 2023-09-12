/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Stamp.h"
#include "StampManager.h"
#include "../Command/CommandSelectionManager.h"
#include "../../Brain.h"
#include "UserInputManager.h"
#include "Definitions/SubFixture/SubFixtureChannel.h"

Stamp::Stamp(var params) :
	BaseItem(params.getProperty("name", "Stamp")),
	objectType(params.getProperty("type", "Stamp").toString()),
	objectData(params),
	stampValues("Values")
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;
	canBeDisabled = false;

	itemDataType = "Stamp";

	id = addIntParameter("ID", "ID of this Stamp", 1, 1);
	userName = addStringParameter("Name", "Name of this Stamp","New Stamp");
	updateName();

	layerId = addIntParameter("Layer", "Higher layer, higer priority",1,1);
	isStampOn = addBoolParameter("is ON", "Enable or disable this effect", false);
	isStampOn->isControllableFeedbackOnly;
	isStampOn->setEnabled(false);
	isStampOn->isSavable = false;
	isOn = false;

	startBtn = addTrigger("Start", "Start this stamp");
	stopBtn = addTrigger("Stop", "Stop this stamp");

	autoStartAndStop = addBoolParameter("Auto Start / Stop", "Start and stop the stamp when size is modified", true);
	sizeValue = addFloatParameter("Size", "Master of this stamp", 1, 0, 1);

	groupId = addIntParameter("Group", "Id of desired group", 0);
	layoutId = addIntParameter("Layout", "Id of desired layout", 0);
	mediaId = addIntParameter("Media", "Id of desired media", 0);
	position = addPoint2DParameter("Position", "Layout position, center of image, layout units");
	dimensions = addPoint2DParameter("Dimensions", "Size of the stamp (with layout units)");
	angle = addFloatParameter("Angle", "Angle of the stamp", 0, -360, 360);

	redRange = addPoint2DParameter("Red range", "Input min and max");
	greenRange = addPoint2DParameter("Green range", "Input min and max");
	blueRange = addPoint2DParameter("Blue range", "Input min and max");

	var v = var(); v.append(0); v.append(1);
	var min = var(); min.append(0); min.append(0);
	var max = var(); max.append(1); max.append(1);
	redRange->setDefaultValue(v);
	greenRange->setDefaultValue(v);
	blueRange->setDefaultValue(v);
	redRange->setRange(min, max);
	greenRange->setRange(min, max);
	blueRange->setRange(min, max);

	addChildControllableContainer(&stampValues);
	stampValues.saveAndLoadRecursiveData = true;


	redFull.setNiceName("Red channel at full");
	stampValues.addChildControllableContainer(&redFull);
	redZero.setNiceName("Red channel at zero");
	stampValues.addChildControllableContainer(&redZero);
	greenFull.setNiceName("Green channel at full");
	stampValues.addChildControllableContainer(&greenFull);
	greenZero.setNiceName("Green channel at zero");
	stampValues.addChildControllableContainer(&greenZero);
	blueFull.setNiceName("Blue channel at full");
	stampValues.addChildControllableContainer(&blueFull);
	blueZero.setNiceName("Blue channel at zero");
	stampValues.addChildControllableContainer(&blueZero);
	//addChildControllableContainer(&selection);
	Brain::getInstance()->registerStamp(this, id->getValue());

}

Stamp::~Stamp()
{
	Brain::getInstance()->unregisterStamp(this); 
}

void Stamp::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<StampManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - "+n);
}

void Stamp::onContainerParameterChangedInternal(Parameter* p) {
	if (p == userName || p == id) {
		updateName();
	}
	if (p == id) {
		Brain::getInstance()->registerStamp(this, id->getValue(), true);
	}
}


void Stamp::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
}

void Stamp::computeData()
{

	Layout* layout = Brain::getInstance()->getLayoutById(layoutId->intValue());
	if (layout != nullptr) {layout->computeData(); }
	Group* g = Brain::getInstance()->getGroupById(groupId->intValue());
	if (g == nullptr) {return; }
	isComputing.enter();
	subFixtureChannels.clear();

	tempCommand.selection.items[0]->targetType->setValueWithKey("Group");
	tempCommand.selection.items[0]->valueFrom->setValue(g->id->intValue());

	redFullMap.clear();
	tempCommand.values.loadJSONData(redFull.getJSONData());
	tempCommand.computeValues();
	for (auto it = tempCommand.computedValues.begin(); it != tempCommand.computedValues.end(); it.next()) { 
		redFullMap.set(it.getKey(), it.getValue()); 
		subFixtureChannels.addIfNotAlreadyThere(it.getKey());
	}

	redZeroMap.clear();
	tempCommand.values.loadJSONData(redZero.getJSONData());
	tempCommand.computeValues();
	for (auto it = tempCommand.computedValues.begin(); it != tempCommand.computedValues.end(); it.next()) { 
		redZeroMap.set(it.getKey(), it.getValue()); 
		subFixtureChannels.addIfNotAlreadyThere(it.getKey());
	}


	greenFullMap.clear();
	tempCommand.values.loadJSONData(greenFull.getJSONData());
	tempCommand.computeValues();
	for (auto it = tempCommand.computedValues.begin(); it != tempCommand.computedValues.end(); it.next()) { 
		greenFullMap.set(it.getKey(), it.getValue()); 
		subFixtureChannels.addIfNotAlreadyThere(it.getKey());
	}

	greenZeroMap.clear();
	tempCommand.values.loadJSONData(greenZero.getJSONData());
	tempCommand.computeValues();
	for (auto it = tempCommand.computedValues.begin(); it != tempCommand.computedValues.end(); it.next()) { 
		greenZeroMap.set(it.getKey(), it.getValue()); 
		subFixtureChannels.addIfNotAlreadyThere(it.getKey());
	}


	blueFullMap.clear();
	tempCommand.values.loadJSONData(blueFull.getJSONData());
	tempCommand.computeValues();
	for (auto it = tempCommand.computedValues.begin(); it != tempCommand.computedValues.end(); it.next()) { 
		blueFullMap.set(it.getKey(), it.getValue()); 
		subFixtureChannels.addIfNotAlreadyThere(it.getKey());
	}

	blueZeroMap.clear();
	tempCommand.values.loadJSONData(blueZero.getJSONData());
	tempCommand.computeValues();
	for (auto it = tempCommand.computedValues.begin(); it != tempCommand.computedValues.end(); it.next()) { 
		blueZeroMap.set(it.getKey(), it.getValue()); 
		subFixtureChannels.addIfNotAlreadyThere(it.getKey());
	}

	isComputing.exit();
	if (isOn) {
		for (int i = 0; i< subFixtureChannels.size(); i++) {
			subFixtureChannels[i]->stampOnTopOfStack(this);
			Brain::getInstance()->pleaseUpdate(subFixtureChannels[i]);
		}
		Brain::getInstance()->pleaseUpdate(this);
	}


}


void Stamp::triggerTriggered(Trigger* t) {
	if (t == startBtn) {
		start();
	}
	else if (t == stopBtn) {
		stop();
	}
	//else if (t == tapTempoBtn) {
	//	tapTempo();
	//}
	else {}
}


void Stamp::start() {
	isOn = true;
	isStampOn->setValue(true);
	computeData();
}

void Stamp::stop() {
	isOn = false;
	isStampOn->setValue(false);
	for (int i = 0; i < subFixtureChannels.size(); i++) {
		subFixtureChannels[i]->stampOutOfStack(this);
		Brain::getInstance()->pleaseUpdate(subFixtureChannels[i]);
	}
}

float Stamp::applyToChannel(SubFixtureChannel* fc, float currentVal, double now)
{
	if (!subFixtureChannels.contains(fc)) { return currentVal; }
	if (isOn) { Brain::getInstance()->pleaseUpdate(fc); }
	else { return currentVal; }

	Layout* layout = Brain::getInstance()->getLayoutById(layoutId->intValue());
	Media* media = Brain::getInstance()->getMediaById(mediaId->intValue());

	if (media == nullptr || layout == nullptr) { return currentVal; }
	var stompSize = dimensions->getValue();
	float stompWidth = stompSize[0];
	float stompHeight = stompSize[1];
	if (stompWidth == 0 || stompHeight == 0) { return currentVal; }

	SubFixture* sf = fc->parentSubFixture;
	if (!layout->subFixtToPos.contains(sf)) {return currentVal;}
	std::shared_ptr<Point<float>> pos = layout->subFixtToPos.getReference(sf);
	
	Point<float> finalPos(pos->x, pos->y);
	Point<float> stompPos(position->getValue()[0], position->getValue()[1]);
	finalPos -= stompPos;
	BKPath::rotateVect(&finalPos, -angle->floatValue());
	finalPos.x /= stompWidth/2;
	finalPos.y /= stompHeight/2;
	finalPos.y *= -1;

	Colour color = media->getColourAtCoord(&finalPos);

	float fromValue;
	float toValue;
	float stompValue = 0;

	bool HTP = fc->isHTP;

	float red = color.getFloatRed();
	float green = color.getFloatGreen();
	float blue = color.getFloatBlue();

	red = jmap(red, (float)redRange->getValue()[0], (float)redRange->getValue()[1], 0.0f, 1.0f);
	green = jmap(green, (float)greenRange->getValue()[0], (float)greenRange->getValue()[1], 0.0f, 1.0f);
	blue = jmap(blue, (float)blueRange->getValue()[0], (float)blueRange->getValue()[1], 0.0f, 1.0f);

	isComputing.enter();
	fromValue = redZeroMap.contains(fc) ? redZeroMap.getReference(fc)->endValue : currentVal;
	toValue = redFullMap.contains(fc) ? redFullMap.getReference(fc)->endValue : currentVal;
	stompValue = jmax(stompValue, jmap(red, fromValue, toValue));

	fromValue = greenZeroMap.contains(fc) ? greenZeroMap.getReference(fc)->endValue : currentVal;
	toValue = greenFullMap.contains(fc) ? greenFullMap.getReference(fc)->endValue : currentVal;
	stompValue = jmax(stompValue, jmap(green, fromValue, toValue));

	fromValue = blueZeroMap.contains(fc) ? blueZeroMap.getReference(fc)->endValue : currentVal;
	toValue = blueFullMap.contains(fc) ? blueFullMap.getReference(fc)->endValue : currentVal;
	stompValue = jmax(stompValue, jmap(blue, fromValue, toValue));
	isComputing.exit();

	if (HTP) {
		stompValue *= sizeValue->floatValue();
		currentVal = jmax(stompValue, currentVal);
	}
	else {
		currentVal = jmap(sizeValue->floatValue(), currentVal, stompValue);
	}

	return currentVal;
}

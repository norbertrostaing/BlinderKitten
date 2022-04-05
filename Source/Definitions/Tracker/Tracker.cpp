/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Tracker.h"
#include "../Command/CommandSelectionManager.h"
#include "../SubFixture/SubFixtureChannel.h"
#include "../../Brain.h"
#include "TrackerManager.h"
#include "../CurvePreset/CurvePreset.h"
#include "../ChannelValue.h"
#include "UI/GridView/TrackerGridView.h"

Tracker::Tracker(var params) :
	BaseItem(params.getProperty("name", "Tracker")),
	objectType(params.getProperty("type", "Tracker").toString()),
	objectData(params),
	rows("Rows")
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;

	itemDataType = "Tracker";

	id = addIntParameter("ID", "ID of this Tracker", 1, 1);
	userName = addStringParameter("Name", "Name of this Tracker", "New Tracker");
	updateName();

	isTrackerOn = addBoolParameter("is ON", "Enable or disable this Tracker",false);
	isTrackerOn->isControllableFeedbackOnly;
	isTrackerOn->setEnabled(false);
	isOn = false;

	startBtn = addTrigger("Start", "Start this Tracker");
	stopBtn = addTrigger("Stop", "Stop this Tracker");

	autoStartAndStop = addBoolParameter("Auto Start / Stop", "Start and stop the Tracker when size is modified", false);
	sizeValue = addFloatParameter("Size", "Master of this Tracker", 1, 0, 1);

	rows.selectItemWhenCreated = false;
	addChildControllableContainer(&rows);

	Brain::getInstance()->registerTracker(this, id->getValue());

	if (params.isVoid()) {
		rows.addItem();
	}

}

Tracker::~Tracker()
{
	Brain::getInstance()->unregisterTracker(this);
}

void Tracker::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
	pleaseComputeIfRunning();
}


void Tracker::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerTracker(this, id->getValue(), true);
	}
	if (p == userName || p == id) {
		updateName();
		TrackerGridView::getInstance()->updateCells();
	}
	if (p == sizeValue) {
		if (autoStartAndStop->getValue()) {
			if (isOn && (float)sizeValue->getValue() == 0) {
				stop();
			}
			else if(!isOn && (float)sizeValue->getValue() > 0 && lastSize == 0) {
				start();
			}
		}
		lastSize = p->getValue();
	}
}

void Tracker::triggerTriggered(Trigger* t) {
	if (t == startBtn) {
		start();
	}
	else if (t == stopBtn) {
		stop();
	}
	else {}
}

void Tracker::start() {
	isOn = true;
	isTrackerOn->setValue(true);
	computeData();
}

void Tracker::stop() {
	isOn = false;
	isTrackerOn->setValue(false);
	for (auto it = chanToTrackerRow.begin(); it != chanToTrackerRow.end(); it.next()) {
		if (it.getKey() != nullptr) {
			it.getKey()->trackerOutOfStack(this);
		}
	}
}

void Tracker::update(double now) {
	if (computed == false) {
		computeData();
	}
	if (isOn) {
		Brain::getInstance()->pleaseUpdate(this);
	}
}

void Tracker::pleaseComputeIfRunning() {
	if (isOn) {
		computed = false;
		Brain::getInstance()->pleaseUpdate(this);
	}
}

void Tracker::computeData() {
	if (computing) {return;}
	computed = true;
	computing = true;
	chanToTrackerRow.clear();
	for (int i = 0; i < rows.items.size(); i++) {
		rows.items[i]->computeData();
	}
	if (isOn) {
		for (auto it = chanToTrackerRow.begin(); it != chanToTrackerRow.end(); it.next()) {
			if (it.getKey() != nullptr) {
				it.getKey()->trackerOnTopOfStack(this);
				Brain::getInstance()->pleaseUpdate(it.getKey());
			}
		}
		Brain::getInstance()->pleaseUpdate(this);
	}
	computing = false;
}

float Tracker::applyToChannel(SubFixtureChannel* fc, float currentVal, double now) {
	if (!chanToTrackerRow.contains(fc)) {return currentVal; }
	if (computing) { return currentVal; }
	if (isOn) {Brain::getInstance()->pleaseUpdate(fc); }
	float calcValue = currentVal;
	Array<TrackerRow*>* activeRows = chanToTrackerRow.getReference(fc);
	for (int rId = 0; rId < activeRows->size(); rId++) {
		TrackerRow * r = activeRows->getReference(rId);
		
		ChannelType* chanType = dynamic_cast<ChannelType*>(r->followedChannel->targetContainer.get());
		if (chanType != nullptr) {
			SubFixtureChannel* followedChan = fc->parentSubFixture->channelsMap.getReference(chanType);
			if (followedChan != nullptr) {
				double offset = followedChan->value;
				TrackerStep* toApply = nullptr;
				for (int stepId = 0; stepId < r->paramContainer.items.size(); stepId++) {
					TrackerStep* step = r->paramContainer.items[stepId];
					if (step->relativeStartPosition <= offset) {
						toApply = step;
					}
				}

				if (toApply == nullptr) {
					return currentVal;
				}
				ChannelValue* cVal = toApply->computedValues.getReference(fc);
				if (cVal != nullptr) {
					float fadeValue = 1;

					float fadeWidth = 1;
					float pos = (offset - toApply->relativeStartPosition) / toApply->relativeDuration;
					if (pos < fadeWidth) {
						fadeValue = pos / fadeWidth;
						// fadeValue = toApply->curve.getValueAtPosition(fadeValue);
					}

					float start = cVal->startValue;
					float end = cVal->endValue;
					start = start == -1 ? currentVal : start;
					end = end == -1 ? currentVal : end;
					calcValue = jmap(fadeValue, start, end);
				}

			}
		}
	}

	float s = sizeValue->getValue();
	if (fc->isHTP) {
		calcValue *= s;
		currentVal = jmax(calcValue, currentVal);
	}
	else {
		currentVal = jmap(s, currentVal, calcValue);
	}

	return currentVal;
}


void Tracker::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<TrackerManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
}

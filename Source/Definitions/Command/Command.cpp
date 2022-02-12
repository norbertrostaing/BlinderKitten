/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "Command.h"
#include "../../Brain.h"
#include "Definitions/SubFixture/SubFixture.h"
#include "CommandSelectionManager.h"
#include "CommandTiming.h"
#include "../ChannelValue.h"

Command::Command(var params) :
	BaseItem(params.getProperty("name", "Command")),
	objectType(params.getProperty("type", "Command").toString()),
	objectData(params),
	values("Values"),
	timing("Timing")
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = false;
	itemDataType = "Command";

	// to add a manager with defined data
	//selection = new CommandSelectionManager();
	addChildControllableContainer(&selection);
	addChildControllableContainer(&values);
	addChildControllableContainer(&timing);

	values.selectItemWhenCreated = false;

	if (params.isVoid()) {
		selection.addItem();
		values.addItem();
	}

	maxTiming = 0;
	updateDisplay();
}

Command::~Command()
{
}

void Command::updateDisplay() {
	// valueTo -> hideInEditor = !(bool)thru->getValue();
}

void Command::afterLoadJSONDataInternal() {
	BaseItem::afterLoadJSONDataInternal();
	updateDisplay();
}
void Command::parameterValueChanged(Parameter* p) {
	BaseItem::parameterControlModeChanged(p);
	updateDisplay();
}

void Command::computeValues() {
	maxTiming = 0;
	selection.computeSelection();
	computedValues.clear();
	Array<CommandValue*> commandValues = values.getItemsWithType<CommandValue>();
	Array<SubFixture*> SubFixtures = selection.computedSelectedSubFixtures;

	bool delayThru = false;
	bool delaySym = false;
	float delayFrom = 0;
	float delayTo = 0;

	bool fadeThru = false;
	bool fadeSym = false;
	float fadeFrom = 0;
	float fadeTo = 0;

	Automation* fadeCurve;
	Automation* fadeRepartCurve;
	Automation* delayRepartCurve;

	if (timing.presetOrValue->getValue() == "preset") {
		TimingPreset* tp = Brain::getInstance()->getTimingPresetById(timing.presetId->getValue());
		if (tp != nullptr) {
			delayThru = tp->thruDelay->getValue();
			delaySym = tp->symmetryDelay->getValue();
			delayFrom = (float)tp->delayFrom->getValue() * 1000;
			delayTo = (float)tp->delayTo->getValue() * 1000;
			fadeThru = tp->thruFade->getValue();
			fadeSym = tp->symmetryFade->getValue();
			fadeFrom = (float)tp->fadeFrom->getValue() * 1000;
			fadeTo = (float)tp->fadeTo->getValue() * 1000;
			fadeCurve = &tp->curveFade;
			fadeRepartCurve = &tp->curveFadeRepart;
			delayRepartCurve = &tp->curveDelayRepart;
		}
	}
	else {
		delayThru = timing.thruDelay->getValue();
		delaySym = timing.symmetryDelay->getValue();
		delayFrom = (float)timing.delayFrom->getValue() * 1000;
		delayTo = (float)timing.delayTo->getValue() * 1000;
		fadeThru = timing.thruFade->getValue();
		fadeSym = timing.symmetryFade->getValue();
		fadeFrom = (float)timing.fadeFrom->getValue() * 1000;
		fadeTo = (float)timing.fadeTo->getValue() * 1000;
		fadeCurve = &timing.curveFade;
		fadeRepartCurve = &timing.curveFadeRepart;
		delayRepartCurve = &timing.curveDelayRepart;
	}


	for (int commandIndex = 0; commandIndex < commandValues.size(); commandIndex++) {
		CommandValue* cv = commandValues[commandIndex];
		bool symValues = cv->symmetry->getValue();
		Preset* pFrom = nullptr;
		Preset* pTo = nullptr;
		if (cv->presetOrValue->getValue() == "preset") {
			pFrom = Brain::getInstance()->getPresetById(cv->presetIdFrom->getValue());
			pTo = Brain::getInstance()->getPresetById(cv->presetIdTo->getValue());
			if (pFrom != nullptr) {
				pFrom -> computeValues();
			}
			if (pTo != nullptr) {
				pTo->computeValues();
			}
		}

		ChannelType* rawChan = dynamic_cast<ChannelType*>(cv->channelType->targetContainer.get());
		for (int indexFixt = 0; indexFixt < SubFixtures.size(); indexFixt++) {
			
			HashMap<ChannelType*, float>* valuesFrom = new HashMap<ChannelType*, float>();;
			HashMap<ChannelType*, float>* valuesTo = new HashMap<ChannelType*, float>();;
			String test = cv->presetOrValue->getValue();
			if (cv->presetOrValue->getValue() == "preset") {

				HashMap<ChannelType*, float>* tempValuesFrom = pFrom != nullptr ? pFrom->getSubFixtureValues(SubFixtures[indexFixt]) : nullptr;
				HashMap<ChannelType*, float>* tempValuesTo = pTo != nullptr ? pTo->getSubFixtureValues(SubFixtures[indexFixt]) : nullptr;
				if (tempValuesFrom != nullptr) {
					for (auto it = tempValuesFrom->begin(); it != tempValuesFrom->end(); it.next()) {
						valuesFrom->set(it.getKey(), it.getValue());
					}
					tempValuesFrom->~HashMap();
				}
				if (tempValuesTo != nullptr) {
					for (auto it = tempValuesTo->begin(); it != tempValuesTo->end(); it.next()) {
						valuesTo->set(it.getKey(), it.getValue());
					}
					tempValuesTo->~HashMap();
				}
				
			}

			if (cv->presetOrValue->getValue() == "value") {
				valuesFrom->set(rawChan, cv->valueFrom->getValue());
				valuesTo->set(rawChan, cv->valueTo->getValue());
			}

			for (auto it = valuesFrom->begin(); it != valuesFrom->end(); it.next()) {
				SubFixtureChannel* fchan = SubFixtures[indexFixt]->channelsMap.getReference(it.getKey());

				float valueFrom = it.getValue();
				float valueTo = valueFrom;
				if (valuesTo->contains(it.getKey())) {
					valueTo = valuesTo->getReference(it.getKey());
				}

				if (fchan != nullptr) {
					if (!computedValues.contains(fchan)) {
						computedValues.set(fchan, new ChannelValue());
					}
					ChannelValue* finalValue = computedValues.getReference(fchan);
					float val = valueFrom;
					if (cv->thru->getValue() && SubFixtures.size() > 1) {
						float position = float(indexFixt) / float(SubFixtures.size() - 1);
						if (symValues) { position = Brain::symPosition(indexFixt, SubFixtures.size()); }
						val = jmap(position, val, valueTo);
					}
					finalValue->endValue = val;

					float delay = delayFrom;
					if (delayThru && SubFixtures.size() > 1) {
						float position = float(indexFixt) / float(SubFixtures.size() - 1);
						if (delaySym) { position = Brain::symPosition(indexFixt, SubFixtures.size()); }
						position = timing.curveDelayRepart.getValueAtPosition(position);
						delay = jmap(position, delayFrom, delayTo);
					}
					finalValue->delay = delay;

					float fade = fadeFrom;
					if (fadeThru && SubFixtures.size() > 1) {
						float position = float(indexFixt) / float(SubFixtures.size() - 1);
						if (fadeSym) { position = Brain::symPosition(indexFixt, SubFixtures.size()); }
						position = timing.curveFadeRepart.getValueAtPosition(position);
						fade = jmap(position, fadeFrom, fadeTo);
					}
					finalValue->fade = fade;
					finalValue->fadeCurve = &timing.curveFade;
					double tempTiming = (delay + fade);
					maxTiming = std::max(maxTiming, tempTiming);
				}
			}

			if (valuesFrom != nullptr) {
				valuesFrom->~HashMap();
			}
			if (valuesTo != nullptr) {
				valuesTo->~HashMap();
			}
		}

		if (cv->presetOrValue->getValue() == "value") {
			ChannelType* chan = dynamic_cast<ChannelType*>(cv->channelType->targetContainer.get());
			if (chan != nullptr) {

			}
		}
		else if (cv->presetOrValue->getValue() == "preset") {
			Preset* pFrom = Brain::getInstance()->getPresetById(cv->presetIdFrom->getValue());
			Preset* pTo = nullptr;
			if (cv->thru->getValue()) {
				pTo = Brain::getInstance()->getPresetById(cv->presetIdTo->getValue());
			}
			if (pFrom != nullptr) {
				pFrom->computeValues();
				bool thru = false;
				if (pTo != nullptr) {
					thru = true;
					pTo->computeValues();
				}

				for (int indexFixt = 0; indexFixt < SubFixtures.size(); indexFixt++) {
					SubFixture* f = SubFixtures[indexFixt];
					HashMap<ChannelType*, float>* vals = pFrom->getSubFixtureValues(f);
					for (auto it = vals->begin(); it != vals->end(); it.next()) {
						ChannelType* param = it.getKey();
						float val = it.getValue();
						if (f->channelsMap.contains(param)) {
							SubFixtureChannel* fchan = SubFixtures[indexFixt]->channelsMap.getReference(param);

						}
					}
					vals->~HashMap();
				}
			}
		}
		else {}
	}
}


/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "Command.h"
#include "../../Brain.h"
#include "Definitions/Fixture/Fixture.h"
#include "CommandSelectionManager.h"
#include "CommandTiming.h"
#include "../ChannelValue.h"

Command::Command(var params) :
	BaseItem(params.getProperty("name", "Command")),
	objectType(params.getProperty("type", "Command").toString()),
	objectData(params),
	values(),
	timing("Timing")
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = false;
	itemDataType = "Command";

	// to add a manager with defined data
	//selection = new CommandSelectionManager();
	addChildControllableContainer(&selection);

	BaseManager<CommandValue>* mv = new BaseManager<CommandValue>("Values");
	mv->selectItemWhenCreated = false;
	values.reset(mv);
	addChildControllableContainer(values.get());

	addChildControllableContainer(&timing);

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
	Array<CommandValue*> commandValues = values->getItemsWithType<CommandValue>();
	Array<Fixture*> fixtures = selection.computedSelectedFixtures;

	bool delayThru = timing.thruDelay->getValue();
	bool delaySym = timing.symmetryDelay->getValue();
	float delayFrom = (float)timing.delayFrom->getValue() * 1000;
	float delayTo = (float)timing.delayTo->getValue()*1000;

	bool fadeThru = timing.thruFade->getValue();
	bool fadeSym = timing.symmetryFade->getValue();
	float fadeFrom = (float)timing.fadeFrom->getValue()*1000;
	float fadeTo = (float)timing.fadeTo->getValue()*1000;

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

		FixtureParamDefinition* rawChan = dynamic_cast<FixtureParamDefinition*>(cv->channelType->targetContainer.get());
		for (int indexFixt = 0; indexFixt < fixtures.size(); indexFixt++) {

			HashMap<FixtureParamDefinition*, float>* valuesFrom = new HashMap<FixtureParamDefinition*, float>();
			HashMap<FixtureParamDefinition*, float>* valuesTo = new HashMap<FixtureParamDefinition*, float>();
			String test = cv->presetOrValue->getValue();
			if (cv->presetOrValue->getValue() == "preset") {
				valuesFrom = pFrom != nullptr ? pFrom->getFixtureValues(fixtures[indexFixt]) : valuesFrom;
				valuesTo = pTo != nullptr ? pTo->getFixtureValues(fixtures[indexFixt]) : valuesTo;
			}
			else if (cv->presetOrValue->getValue() == "value") {
				valuesFrom->set(rawChan, cv->valueFrom->getValue());
				valuesTo->set(rawChan, cv->valueTo->getValue());
			}

			for (auto it = valuesFrom->begin(); it != valuesFrom->end(); it.next()) {
				FixtureChannel* fchan = fixtures[indexFixt]->channelsMap.getReference(it.getKey());

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
					if (cv->thru->getValue() && fixtures.size() > 1) {
						float position = float(indexFixt) / float(fixtures.size() - 1);
						if (symValues) { position = Brain::symPosition(indexFixt, fixtures.size()); }
						val = jmap(position, val, valueTo);
					}
					finalValue->endValue = val;

					float delay = delayFrom;
					if (delayThru && fixtures.size() > 1) {
						float position = float(indexFixt) / float(fixtures.size() - 1);
						if (delaySym) { position = Brain::symPosition(indexFixt, fixtures.size()); }
						position = timing.curveDelayRepart.getValueAtPosition(position);
						delay = jmap(position, delayFrom, delayTo);
					}
					finalValue->delay = delay;

					float fade = fadeFrom;
					if (fadeThru && fixtures.size() > 1) {
						float position = float(indexFixt) / float(fixtures.size() - 1);
						if (fadeSym) { position = Brain::symPosition(indexFixt, fixtures.size()); }
						position = timing.curveFadeRepart.getValueAtPosition(position);
						fade = jmap(position, fadeFrom, fadeTo);
					}
					finalValue->fade = fade;
					finalValue->fadeCurve = &timing.curveFade;
					double tempTiming = (delay + fade);
					maxTiming = std::max(maxTiming, tempTiming);
				}
			}

		}



		if (cv->presetOrValue->getValue() == "value") {
			FixtureParamDefinition* chan = dynamic_cast<FixtureParamDefinition*>(cv->channelType->targetContainer.get());
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

				for (int indexFixt = 0; indexFixt < fixtures.size(); indexFixt++) {
					Fixture* f = fixtures[indexFixt];
					HashMap<FixtureParamDefinition*, float>* valuesFrom = pFrom->getFixtureValues(f);
					for (auto it = valuesFrom->begin(); it != valuesFrom->end(); it.next()) {
						FixtureParamDefinition* param = it.getKey();
						float val = it.getValue();
						if (f->channelsMap.contains(param)) {
							FixtureChannel* fchan = fixtures[indexFixt]->channelsMap.getReference(param);

						}
					}
				}
			}
		}
		else {}
	}
}


/*
void Command::computeValues() {
	maxTiming = 0;
	selection->computeSelection();
	computedValues.clear();
	Array<CommandValue*> commandValues = values->getItemsWithType<CommandValue>();
	Array<Fixture*> fixtures = selection->computedSelectedFixtures;

	bool delayThru = timing.thruDelay->getValue();
	bool delaySym = timing.symmetryDelay->getValue();
	float delayFrom = timing.delayFrom->getValue();
	float delayTo = timing.delayTo->getValue();

	bool fadeThru = timing.thruFade->getValue();
	bool fadeSym = timing.symmetryFade->getValue();
	float fadeFrom = timing.fadeFrom->getValue();
	float fadeTo = timing.fadeTo->getValue();

	for (int commandIndex = 0; commandIndex < commandValues.size(); commandIndex++) {
		CommandValue* cv = commandValues[commandIndex];
		bool symValues = cv->symmetry->getValue();
		if (cv->presetOrValue->getValue() == "value") {
			FixtureParamDefinition* chan = dynamic_cast<FixtureParamDefinition*>(cv->channelType->targetContainer.get());
			if (chan != nullptr) {
				for (int indexFixt = 0; indexFixt < fixtures.size(); indexFixt++) {

					FixtureChannel* fchan = fixtures[indexFixt]->channelsMap.getReference(chan);

					if (fchan != nullptr) {
						if (!computedValues.contains(fchan)) {
							computedValues.set(fchan, new ChannelValue());
						}
						ChannelValue* finalValue = computedValues.getReference(fchan);
						float val = cv->valueFrom->getValue();
						if (cv->thru->getValue() && fixtures.size() > 1) {
							float position = float(indexFixt) / float(fixtures.size() - 1);
							if (symValues) { position = Brain::symPosition(indexFixt, fixtures.size()); }
							val = jmap(position, val, float(cv->valueTo->getValue()));
						}
						finalValue->endValue = val;

						float delay = delayFrom;
						if (delayThru && fixtures.size() > 1) {
							float position = float(indexFixt) / float(fixtures.size() - 1);
							if (delaySym) { position = Brain::symPosition(indexFixt, fixtures.size()); }
							position = timing.curveDelayRepart->getValueAtPosition(position);
							delay = jmap(position, delayFrom, delayTo);
						}
						finalValue->delay = delay;

						float fade = fadeFrom;
						if (fadeThru && fixtures.size() > 1) {
							float position = float(indexFixt) / float(fixtures.size() - 1);
							if (fadeSym) { position = Brain::symPosition(indexFixt, fixtures.size()); }
							position = timing.curveFadeRepart->getValueAtPosition(position);
							fade = jmap(position, fadeFrom, fadeTo);
						}
						finalValue->fade = fade;
						finalValue->fadeCurve = timing.curveFade;
						int64 tempTiming = round(1000 * (delay + fade));
						maxTiming = std::max(maxTiming, tempTiming);
					}
				}
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

				for (int indexFixt = 0; indexFixt < fixtures.size(); indexFixt++) {
					Fixture* f = fixtures[indexFixt];
					HashMap<FixtureParamDefinition*, float>* valuesFrom = pTo->getFixtureValues(f);
					for (auto it = valuesFrom->begin(); it != valuesFrom->end(); it.next()) {
						FixtureParamDefinition* param = it.getKey();
						float val = it.getValue();
						if (f->channelsMap.contains(param)) {
							FixtureChannel* fchan = fixtures[indexFixt]->channelsMap.getReference(param);

						}
					}
				}
			}
		}
		else {}
	}
}

*/


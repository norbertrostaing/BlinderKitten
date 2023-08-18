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
#include "CommandSelection.h"
#include "../ChannelValue.h"
#include "../Cue/Cue.h"
#include "UserInputManager.h"
#include "CommandManager.h"
#include "Definitions/ChannelFamily/ChannelFamilyManager.h"

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

	//viewCommandBtn = addTrigger("Log command text", "display the textual content of this command in the log windows");
	explodeSelectionBtn = addTrigger("Explode Selection", "Transform this command in one command per subfixture");

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
	listeners.clear();
}

Command::~Command()
{
	toDelete = true;
	computedValues.getLock().enter();
	for (auto it = computedValues.begin(); it != computedValues.end(); it.next()) {
		//delete it.getValue();
	}
	//computedValues.clear();
	computedValues.getLock().exit();

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
	computeValues(nullptr, nullptr);
}

void Command::computeValues(Cuelist* callingCuelist, Cue* callingCue) {
	maxTiming = 0;
	isComputing.enter();
	computedValues.clear();

	if (!enabled->boolValue()) {
		isComputing.exit();
		return;
	}
	selection.computeSelection();
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

	String timingMode = timing.presetOrValue->getValue();

	if (timingMode == "preset") {
		TimingPreset* tp = Brain::getInstance()->getTimingPresetById(timing.presetId->getValue());
		if (tp != nullptr) {
			float delayMult = tp->delayMult.getValue();
			float fadeMult = tp->fadeMult.getValue();
			delayThru = tp->thruDelay->getValue();
			delaySym = tp->symmetryDelay->getValue();
			delayFrom = (float)tp->delayFrom->getValue() * 1000 * delayMult;
			delayTo = (float)tp->delayTo->getValue() * 1000 * delayMult;
			fadeThru = tp->thruFade->getValue();
			fadeSym = tp->symmetryFade->getValue();
			fadeFrom = (float)tp->fadeFrom->getValue() * 1000 * fadeMult;
			fadeTo = (float)tp->fadeTo->getValue() * 1000 * fadeMult;
			fadeCurve = &tp->curveFade;
			fadeRepartCurve = &tp->curveFadeRepart;
			delayRepartCurve = &tp->curveDelayRepart;
		}
	}
	else if (timingMode == "raw"){
		float delayMult = timing.delayMult.getValue();
		float fadeMult = timing.fadeMult.getValue();
		delayThru = timing.thruDelay->getValue();
		delaySym = timing.symmetryDelay->getValue();
		delayFrom = (float)timing.delayFrom->getValue() * 1000 * delayMult;
		delayTo = (float)timing.delayTo->getValue() * 1000 * delayMult;
		fadeThru = timing.thruFade->getValue();
		fadeSym = timing.symmetryFade->getValue();
		fadeFrom = (float)timing.fadeFrom->getValue() * 1000 * fadeMult;
		fadeTo = (float)timing.fadeTo->getValue() * 1000 * fadeMult;
		fadeCurve = &timing.curveFade;
		fadeRepartCurve = &timing.curveFadeRepart;
		delayRepartCurve = &timing.curveDelayRepart;
	}
	for (int commandIndex = 0; commandIndex < commandValues.size(); commandIndex++) {
		CommandValue* cv = commandValues[commandIndex];
		if (cv->enabled->boolValue()) {
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

				float normalizedPosition = indexFixt / (float)(SubFixtures.size()-1);
				bool useNormalized = false;
				LOG(selection.subFixtureToPosition.size());
				if (selection.subFixtureToPosition.contains(SubFixtures[indexFixt])) {
					useNormalized = true;
					normalizedPosition = selection.subFixtureToPosition.getReference(SubFixtures[indexFixt]);
				}

				std::shared_ptr<HashMap<ChannelType*, float>> valuesFrom = std::make_shared <HashMap<ChannelType*, float>>();
				std::shared_ptr <HashMap<ChannelType*, float>> valuesTo = std::make_shared <HashMap<ChannelType*, float>>();
				String test = cv->presetOrValue->getValue();
				if (cv->presetOrValue->getValue() == "preset") {

					std::shared_ptr < HashMap<ChannelType*, float>> tempValuesFrom = pFrom != nullptr ? pFrom->getSubFixtureValues(SubFixtures[indexFixt]) : nullptr;
					std::shared_ptr < HashMap<ChannelType*, float>> tempValuesTo = pTo != nullptr ? pTo->getSubFixtureValues(SubFixtures[indexFixt]) : nullptr;
					if (tempValuesFrom != nullptr) {
						for (auto it = tempValuesFrom->begin(); it != tempValuesFrom->end(); it.next()) {
							valuesFrom->set(it.getKey(), it.getValue());
						}
						//tempValuesFrom->~HashMap();
					}
					if (tempValuesTo != nullptr) {
						for (auto it = tempValuesTo->begin(); it != tempValuesTo->end(); it.next()) {
							valuesTo->set(it.getKey(), it.getValue());
						}
						//tempValuesTo->~HashMap();
					}
				
				}

				if (cv->presetOrValue->getValue() == "value") {
					valuesFrom->set(rawChan, cv->valueFrom->getValue());
					valuesTo->set(rawChan, cv->valueTo->getValue());
				}
				else if (cv->presetOrValue->getValue() == "release") {
					valuesFrom->set(rawChan, -1);
					valuesTo->set(rawChan, -1);
				}

				for (auto it = valuesFrom->begin(); it != valuesFrom->end(); it.next()) {
					SubFixtureChannel* fchan = SubFixtures[indexFixt]->channelsMap.contains(it.getKey()) ? SubFixtures[indexFixt]->channelsMap.getReference(it.getKey()) : nullptr;

					float valueFrom = it.getValue();
					float valueTo = valueFrom;
					if (valuesTo->contains(it.getKey())) {
						valueTo = valuesTo->getReference(it.getKey());
					}

					if (fchan != nullptr) {
						if (!computedValues.contains(fchan)) {
							computedValues.set(fchan, std::make_shared<ChannelValue>());
						}
						std::shared_ptr<ChannelValue> finalValue = computedValues.getReference(fchan);
						float val = valueFrom;
						if (cv->thru->getValue() && SubFixtures.size() > 1) {
							float position = normalizedPosition;
							if (symValues) { position = useNormalized ? normalizedPosition : Brain::symPosition(indexFixt, SubFixtures.size()); }
							val = jmap(position, val, valueTo);
						}
						finalValue->endValue = val;

						float delay = delayFrom;
						if (timingMode == "cue" && callingCuelist != nullptr && callingCue != nullptr) {
							if (!fchan->isHTP) {
								delay = (float)callingCue->ltpDelay->getValue()*1000.;
							}
							else {
								std::shared_ptr<ChannelValue> currentCuelistVal = callingCuelist->activeValues.getReference(fchan);
								if (currentCuelistVal == nullptr || currentCuelistVal->endValue < val) {
									delay = (float)callingCue->htpInDelay->getValue() * 1000.;
								}
								else {
									delay = (float)callingCue->htpOutDelay->getValue() * 1000.;
								}
							}
						}
						else if (delayThru && SubFixtures.size() > 1) {
							float position = normalizedPosition;
							if (delaySym) { position = useNormalized ? normalizedPosition : Brain::symPosition(indexFixt, SubFixtures.size()); }
							position = timing.curveDelayRepart.getValueAtPosition(position);
							position = delayRepartCurve->getValueAtPosition(position);
							delay = jmap(position, delayFrom, delayTo);
						}
						finalValue->delay = delay;

						float fade = fadeFrom;
						if (fchan->snapOnly) {
							fade = 0;
						}
						else if (timingMode == "cue" && callingCuelist != nullptr && callingCue != nullptr) {
							if (!fchan->isHTP) {
								fade = (float)callingCue->ltpFade->getValue() * 1000.;
							}
							else {
								std::shared_ptr<ChannelValue> currentCuelistVal = callingCuelist->activeValues.getReference(fchan);
								if (currentCuelistVal == nullptr || currentCuelistVal->endValue < val) {
									fade = (float)callingCue->htpInFade->getValue() * 1000.;
								}
								else {
									fade = (float)callingCue->htpOutFade->getValue() * 1000.;
								}
							}
						}
						else if (fadeThru && SubFixtures.size() > 1) {
							float position = normalizedPosition;
							if (fadeSym) { position = useNormalized ? normalizedPosition : Brain::symPosition(indexFixt, SubFixtures.size()); }
							position = timing.curveFadeRepart.getValueAtPosition(position);
							position = fadeRepartCurve->getValueAtPosition(position);
							fade = jmap(position, fadeFrom, fadeTo);
						}

						finalValue->fade = fade;
						finalValue->fadeCurve = &timing.curveFade;
						if (fchan->isHTP) {
							if (callingCuelist != nullptr && callingCue != nullptr) {
								std::shared_ptr<ChannelValue> currentCuelistVal = callingCuelist->activeValues.getReference(fchan);
								if (currentCuelistVal != nullptr && currentCuelistVal->endValue > finalValue->endValue) {
									finalValue->isTransitionOut = true;
								}
							}
						}
						double tempTiming = (delay + fade);
						maxTiming = std::max(maxTiming, tempTiming);

						finalValue->htpOverride = cv -> HTPOverride->boolValue();
					}
				}

				if (valuesFrom != nullptr) {
					//delete valuesFrom;
				}
				if (valuesTo != nullptr) {
					//delete valuesTo;
				}
			}
		}
	}
	isComputing.exit();
}

void Command::onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c) {
	if (&selection == cc) {
		UserInputManager::getInstance()->commandSelectionChanged(this);
	}
	else if (&values == cc) {
		queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableFeedbackUpdate, this));

		UserInputManager::getInstance()->commandValueChanged(this);
	}
	else {

	}
}

void Command::triggerTriggered(Trigger* t)
{
	if (t == explodeSelectionBtn) {
		explodeSelection();
	}
}

StringArray Command::getCommandAsTexts() {
	StringArray words;
	userCantPress();
	if (selection.items.size() > 0) {
		currentUserSelection = selection.items[0];
	}
	else {
		currentUserSelection = selection.addItem();
	}

	if (values.items.size() > 0) {
		currentUserValue = values.items[0];
	}
	else {
		currentUserValue = values.addItem();
	}

	userCanPressSelectionType = true;
	for (int i = 0; i < selection.items.size(); i++) {
		CommandSelection* s = selection.items[i];
		currentUserSelection = s;
		if (s->plusOrMinus->getValue() == "-" || words.size() > 0) {
			words.add(s->plusOrMinus->getValueData());
			lastTarget = "selectionPlusOrMinus";
			userCantPress();
			userCanPressSelectionType = true;
		}
		words.add(s->targetType->getValueData());
		lastTarget = "selectionTargetType";
		userCantPress();
		userCanPressSelectionType = true;
		userCanPressNumber = true;
		currentUserTarget = s->valueFrom;
		if ((int)s->valueFrom->value > 0) {
			words.add(s->valueFrom->value);
			lastTarget = "selectionValueFrom";
			userCanPressSelectionType = false;
			userCanPressThru = true;
			currentUserThru = s->thru;
			userCanPressPlusOrMinus = true;
			userCanPressSubSelection = true;
			userCanPressValueType = true;
			if (s->thru->getValue()) {
				words.add("thru");
				lastTarget = "selectionThru";
				userCantPress();
				userCanPressNumber = true;
				currentUserTarget = s->valueTo;
				if ((int)s->valueTo->value > 0) {
					words.add(s->valueTo->value);
					lastTarget = "selectionValueTo";
					userCantPress();
					userCanPressNumber = true;
					userCanPressPlusOrMinus = true;
					userCanPressValueType = true;
					userCanPressSubSelection = true;
				}
				else {
					return words;
				}
			}
		}
		else {
			return words;
		}

		if (s->subSel->getValue()) {
			words.add("subfixture");
			lastTarget = "selectionSubFixture";
			userCantPress();
			userCanPressNumber = true;
			currentUserTarget = s->subFrom;
			if ((int)s->subFrom->value > 0) {
				words.add(s->subFrom->value);
				lastTarget = "selectionSubFrom";
				userCanPressThru = true;
				currentUserThru = s->subThru;
				userCanPressPlusOrMinus = true;
				userCanPressValueType = true;
				if (s->subThru->getValue()) {
					words.add("thru");
					lastTarget = "selectionSubThru";
					userCantPress();
					userCanPressNumber = true;
					currentUserTarget = s->subTo;
					if ((int)s->subTo->value > 0) {
						words.add(s->subTo->value);
						lastTarget = "selectionSubTo";
						userCantPress();
						userCanPressNumber = true;
						userCanPressPlusOrMinus = true;
						userCanPressValueType = true;
					}
					else {
						return words;
					}
				}
			}
		}
	}

	for (int i = 0; i < values.items.size(); i++) {
		CommandValue* v = values.items[i];
		currentUserValue = v;
		if (v->presetOrValue->getValue() == "preset") {
			words.add("preset");
			lastTarget = "valuePreset";
			userCantPress();
			userCanPressNumber = true;
			currentUserTarget = v->presetIdFrom;
			if ((int)v->presetIdFrom->value > 0) {
				words.add(v->presetIdFrom->value);
				lastTarget = "valuePresetFrom";
				userCanPressThru = true;
				currentUserThru = v->thru;
				userCanPressValueType = true;
				userCanPressTimingType = true;
				userCanHaveAnotherCommand = true;
				if (v->thru->getValue()) {
					words.add("thru");
					lastTarget = "valuePresetThru";
					userCantPress();
					userCanPressNumber = true;
					currentUserTarget = v->presetIdTo;
					if ((int)v->presetIdTo->value > 0) {
						words.add(v->presetIdTo->value);
						lastTarget = "valuePresetTo";
						userCantPress();
						userCanPressNumber = true;
						userCanPressValueType = true;
						userCanPressTimingType = true;
						userCanHaveAnotherCommand = true;
					}
					else {
						return words;
					}
				}
			}
		}
		else if (v->presetOrValue->getValue() == "value") {
			ChannelType* c = dynamic_cast<ChannelType*>(v->channelType->targetContainer.get());
			if (c != nullptr) {
				words.add(c->niceName);
				lastTarget = "valueRaw";
				userCantPress();
				userCanPressValue = true;
				currentUserTarget = v->valueFrom;
				userCanPressValueType = true;
				words.add(formatValue(v->valueFrom->value));
				lastTarget = "valueRawFrom";
				userCanPressTimingType = true;
				userCanPressThru = true;
				currentUserThru = v->thru;
				userCanHaveAnotherCommand = true;
				if (v->thru->getValue()) {
					words.add("thru");
					//lastTarget = "valueRawThru";
					userCantPress();
					currentUserTarget = v->valueFrom;
					words.add(formatValue(v->valueTo->value));
					lastTarget = "valueRawTo";
					userCanPressValue = true;
					userCanPressValueType = true;
					userCanPressTimingType = true;
					userCanHaveAnotherCommand = true;
				}
			}
		}
	}

	return words;

}

StringArray Command::getCommandSelectionAsTexts() {
	StringArray words;
	userCantPress();
	if (selection.items.size() > 0) {
		currentUserSelection = selection.items[0];
	}
	else {
		currentUserSelection = selection.addItem();
	}

	if (values.items.size() > 0) {
		currentUserValue = values.items[0];
	}
	else {
		currentUserValue = values.addItem();
	}

	userCanPressSelectionType = true;
	for (int i = 0; i < selection.items.size(); i++) {
		CommandSelection* s = selection.items[i];
		if (s->plusOrMinus->getValueKey() == "-" || words.size() > 0) {
			words.add(s->plusOrMinus->getValueKey());
		}
		if (s->targetType->getValueData() == "group") {
			words.add("grp");
		}
		if ((int)s->valueFrom->value > 0) {
			words.add(s->valueFrom->value);
			if (s->thru->getValue()) {
				words.add(">");
				if ((int)s->valueTo->value > 0) {
					words.add(s->valueTo->value);
				}
				else {
					return words;
				}
			}
		}
		else {
			return words;
		}

		if (s->subSel->getValue()) {
			words.add("sub");
			if ((int)s->subFrom->value > 0) {
				words.add(s->subFrom->value);
				if (s->subThru->getValue()) {
					words.add("thru");
					if ((int)s->subTo->value > 0) {
						words.add(s->subTo->value);
					}
					else {
						return words;
					}
				}
			}
		}
	}

	return words;

}

void Command::userCantPress() {
	userCanPressPlusOrMinus = false;
	userCanPressSelectionType = false;
	userCanPressSubSelection = false;
	userCanPressValueType = false;
	userCanPressTimingType = false;
	userCanPressThru = false;
	userCanPressNumber = false;
	userCanPressValue = false;
	userCanPressSym = false;
	userCanHaveAnotherCommand = false;
}

String Command::formatValue(float v) {
	return String(round(v*1000)/1000.);
}

void Command::userPress(String s) {
	if (currentUserSelection == nullptr) {
		if (selection.items.size() > 0) {
			currentUserSelection = selection.items[0];
		}
		else {
			currentUserSelection = selection.addItem();
		}
	}


	if (s == "group" || s == "fixture") {
		currentUserSelection->targetType->setValueWithData(s);
	}
	else if (s == "subfixture") {
		currentUserSelection->subSel->setValue(true);
	}
	else if (s == "+" || s == "-") {
		String objType = currentUserSelection->targetType->getValueKey();
		currentUserSelection = selection.addItem();
		currentUserSelection->plusOrMinus->setValue(s);
		currentUserSelection->targetType->setValueWithKey(objType);
	}
	else if (s == "thru") {
		currentUserThru->setValue(true);
	}
	else if (s == "preset") {
		String pov = currentUserValue->presetOrValue->getValue();
		if (pov == "preset" && (int)currentUserValue->presetIdFrom->getValue() > 0) {
			currentUserValue = values.addItem();
		} else if (pov == "value" && currentUserValue->channelType->targetContainer != nullptr) {
			currentUserValue = values.addItem();
		}
		currentUserValue->presetOrValue->setValueWithData("preset");
	}
	else if (s.containsOnly("1234567890.")) {
		String v = currentUserTarget->getValue().toString();
		v += s;
		currentUserTarget->setValue(v.getFloatValue());
	}
	else if (s == "backspace") {

		if (lastTarget == "selectionPlusOrMinus") {
			selection.removeItem(currentUserSelection);
			currentUserSelection = selection.items.getLast();
		} else if (lastTarget == "selectionTargetType") {
			if (selection.items.size() > 1) {
				selection.removeItem(currentUserSelection);
				currentUserSelection = selection.items.getLast();
			}

		} else if (lastTarget == "selectionValueFrom") {
			currentUserSelection->valueFrom->setValue(UserInputManager::backspaceOnInt(currentUserSelection->valueFrom->getValue()));
		} else if (lastTarget == "selectionThru") {
			currentUserSelection->thru->setValue(false);
		} else if (lastTarget == "selectionValueTo") {
			currentUserSelection->valueTo->setValue(UserInputManager::backspaceOnInt(currentUserSelection->valueTo->getValue()));
		} else if (lastTarget == "selectionSubFixture") {
			currentUserSelection->subSel->setValue(false);
		} else if (lastTarget == "selectionSubFrom") {
			currentUserSelection->subFrom->setValue(UserInputManager::backspaceOnInt(currentUserSelection->subFrom->getValue()));
		} else if (lastTarget == "selectionSubThru") {
			currentUserSelection->subThru->setValue(false);
		} else if (lastTarget == "selectionSubTo") {
			currentUserSelection->subTo->setValue(UserInputManager::backspaceOnInt(currentUserSelection->subTo->getValue()));
		} else if (lastTarget == "valuePreset") {
			values.removeItem(values.items.getLast());
			currentUserValue = values.items.size() > 0 ? values.items.getLast() : values.addItem();
		} else if (lastTarget == "valuePresetFrom") {
			currentUserValue->presetIdFrom->setValue(UserInputManager::backspaceOnInt(currentUserValue->presetIdFrom->getValue()));
		} else if (lastTarget == "valuePresetThru") {
			currentUserValue->thru->setValue(false);
		} else if (lastTarget == "valuePresetTo") {
			currentUserValue->presetIdTo->setValue(UserInputManager::backspaceOnInt(currentUserValue->presetIdTo->getValue()));
		} else if (lastTarget == "valueRaw") {
			values.removeItem(currentUserValue);
			currentUserValue = values.items.size() > 0 ? values.items.getLast() : values.addItem();
		} else if (lastTarget == "valueRawFrom") {
			values.removeItem(values.items.getLast());
			currentUserValue = values.items.size() > 0 ? values.items.getLast() : values.addItem();
		} else if (lastTarget == "valueRawThru") {
			currentUserValue->thru->setValue(false);
		} else if (lastTarget == "valueRawTo") {
			currentUserValue->thru->setValue(false);
		}
	}
}


float Command::getChannelValue(ChannelType* t, bool thru) {
	float val = -1;
	for (int i = 0; i < values.items.size(); i++) {
		if (values.items[i]->presetOrValue->getValue() == "value") {
			if (dynamic_cast<ChannelType*>(values.items[i]->channelType->targetContainer.get()) == t) {
				if (thru) {
					if (values.items[i]->thru->getValue()) {
						val = values.items[i]->valueTo->getValue();
					}
				}
				else {
					val = values.items[i]->valueFrom->getValue();
				}
			}
		}
	}
	return val;
}

void Command::explodeSelection()
{
	const MessageManagerLock mmLock;
	BaseManager<Command>* parentManager = dynamic_cast<BaseManager<Command>*>(parentContainer.get());
	int index = parentManager->items.indexOf(this)+1;
	computeValues();
	isComputing.enter();
	ChannelFamilyManager::getInstance()->updateOrderedElements();
	for (int i = 0; i < selection.computedSelectedSubFixtures.size(); i++) {
		SubFixture* sf = selection.computedSelectedSubFixtures[i];
		Fixture* f = sf->parentFixture;
		Command* newCommand = parentManager->addItem();
		newCommand->values.clear();
		parentManager->setItemIndex(newCommand, index);
		index++;
		newCommand->selection.items[0]->valueFrom->setValue(f->id->getValue());
		if (f->subFixtures.size() > 1) {
			newCommand->selection.items[0]->subSel->setValue(true);
			newCommand->selection.items[0]->subFrom->setValue(sf->subId);
		}
		for (int idCh = 0; idCh < ChannelFamilyManager::getInstance()->orderedElements.size(); idCh++) {
			ChannelType* chanType = ChannelFamilyManager::getInstance()->orderedElements[idCh];
			if (sf->channelsMap.contains(chanType)) {
				SubFixtureChannel* chan = sf->channelsMap.getReference(chanType);
				if (chan != nullptr && computedValues.contains(chan)) {
					CommandValue* cv = newCommand->values.addItem();
					cv->channelType->setValueFromTarget(chan->channelType);
					std::shared_ptr<ChannelValue> chanVal = computedValues.getReference(chan);
					if (chanVal != nullptr) {
						cv->valueFrom->setValue(chanVal->endValue);
					}
				}

			}
		}
	}
	isComputing.exit();
}

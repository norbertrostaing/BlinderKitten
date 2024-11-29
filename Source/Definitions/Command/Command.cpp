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
	timing("Timing"),
	moveInBlack()
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = false;
	itemDataType = "Command";
	nameCanBeChangedByUser = false;

	//viewCommandBtn = addTrigger("Log command text", "display the textual content of this command in the log windows");
	doNotTrack = addBoolParameter("Do not track", "if checked, this command will not be tracked", false);
	explodeSelectionBtn = addTrigger("Explode Selection", "Transform this command in one command per subfixture");

	moveInBlackDelay = addFloatParameter("MIB Delay", "Delay to wait after light goes off to trigger move in black", -1, -1);

	// to add a manager with defined data
	//selection = new CommandSelectionManager();
	addChildControllableContainer(&moveInBlack);
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
	//listeners.clear();

	Brain::getInstance()->allCommands.add(this);

}

Command::~Command()
{
	toDelete = true;
	Brain::getInstance()->allCommands.removeAllInstancesOf(this);
	computedValues.getLock().enter();
	for (auto it = computedValues.begin(); it != computedValues.end(); it.next()) {
		//delete it.getValue();
	}
	//computedValues.clear();
	computedValues.getLock().exit();

	if (UserInputManager::getInstance()->targetCommand == this) {
		UserInputManager::getInstance()->targetCommand = nullptr;
	}

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
	channelToCommandValue.clear();

	if (!enabled->boolValue()) {
		isComputing.exit();
		return;
	}
	selection.computeSelection();
	Array<CommandValue*> commandValues = values.getItemsWithType<CommandValue>();
	Array<SubFixture*> subFixtures = selection.computedSelectedSubFixtures;

	Array<ChannelFamily*> moveInBlackFamilies;
	float mibDelay = 0;
	if (callingCuelist != nullptr) {
		for (auto f : callingCuelist->moveInBlack.getFamilies()) moveInBlackFamilies.addIfNotAlreadyThere(f);
		mibDelay = callingCuelist->moveInBlackDelay->floatValue();
	}
	if (callingCue != nullptr) {
		for (auto f : callingCue->moveInBlack.getFamilies()) moveInBlackFamilies.addIfNotAlreadyThere(f);
		if (callingCue->moveInBlackDelay->floatValue() >= 0) mibDelay = callingCue->moveInBlackDelay->floatValue();
	}
	for (auto f : moveInBlack.getFamilies()) moveInBlackFamilies.addIfNotAlreadyThere(f);
	if (moveInBlackDelay->floatValue() >= 0) mibDelay = moveInBlackDelay->floatValue();


	bool delayThru = false;
	bool delaySym = false;
	float delayFrom = 0;
	float delayTo = 0;
	bool delayRandom = false;

	bool fadeThru = false;
	bool fadeSym = false;
	float fadeFrom = 0;
	float fadeTo = 0;
	bool fadeRandom = false;

	Automation* fadeCurve = nullptr;
	Automation* fadeRepartCurve = nullptr;
	Automation* delayRepartCurve = nullptr;

	String timingMode = "";

	float delayHTPIn = -1;
	float delayHTPOut = -1;
	float delayLTP = -1;
	float fadeHTPIn = -1;
	float fadeHTPOut = -1;
	float fadeLTP = -1;

	if (callingCue != nullptr) {
		delayHTPIn = callingCue->htpInDelay->floatValue() >= 0 ? callingCue->htpInDelay->floatValue() : -1;
		delayHTPOut = callingCue->htpOutDelay->floatValue() >= 0 ? callingCue->htpOutDelay->floatValue() : delayHTPIn;
		delayLTP = callingCue->ltpDelay->floatValue() >= 0 ? callingCue->ltpDelay->floatValue() : delayHTPIn;
		fadeHTPIn = callingCue->htpInFade->floatValue() >= 0 ? callingCue->htpInFade->floatValue() : -1;
		fadeHTPOut = callingCue->htpOutFade->floatValue() >= 0 ? callingCue->htpOutFade->floatValue() : fadeHTPIn;
		fadeLTP = callingCue->ltpFade->floatValue() >= 0 ? callingCue->ltpFade->floatValue() : fadeHTPIn;
	}

	if (callingCuelist != nullptr) {
		timingMode = callingCuelist->timing.presetOrValue->getValue();
		if (timingMode == "preset") {
			TimingPreset* tp = Brain::getInstance()->getTimingPresetById(callingCuelist->timing.presetId->getValue(), true);
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
				delayRandom = delayThru && tp->randomizeDelay->boolValue();
				fadeRandom = fadeThru && tp->randomizeFade->boolValue();
			}
		}
		else if (timingMode == "raw") {
			float delayMult = callingCuelist->timing.delayMult.getValue();
			float fadeMult = callingCuelist->timing.fadeMult.getValue();
			delayThru = callingCuelist->timing.thruDelay->getValue();
			delaySym = callingCuelist->timing.symmetryDelay->getValue();
			delayFrom = (float)callingCuelist->timing.delayFrom->getValue() * 1000 * delayMult;
			delayTo = (float)callingCuelist->timing.delayTo->getValue() * 1000 * delayMult;
			fadeThru = callingCuelist->timing.thruFade->getValue();
			fadeSym = callingCuelist->timing.symmetryFade->getValue();
			fadeFrom = (float)callingCuelist->timing.fadeFrom->getValue() * 1000 * fadeMult;
			fadeTo = (float)callingCuelist->timing.fadeTo->getValue() * 1000 * fadeMult;
			fadeCurve = &callingCuelist->timing.curveFade;
			fadeRepartCurve = &callingCuelist->timing.curveFadeRepart;
			delayRepartCurve = &callingCuelist->timing.curveDelayRepart;
			delayRandom = delayThru && callingCuelist->timing.randomizeDelay->boolValue();
			fadeRandom = fadeThru && callingCuelist->timing.randomizeFade->boolValue();
		}
	}

	timingMode = timing.presetOrValue->getValue();

	if (timingMode == "preset") {
		TimingPreset* tp = Brain::getInstance()->getTimingPresetById(timing.presetId->getValue(), true);
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
			delayRandom = delayThru && tp->randomizeDelay->boolValue();
			fadeRandom = fadeThru && tp->randomizeFade->boolValue();
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
		delayRandom = delayThru && timing.randomizeDelay->boolValue();
		fadeRandom = fadeThru && timing.randomizeFade->boolValue();
	}
	for (int commandIndex = 0; commandIndex < commandValues.size(); commandIndex++) {
		CommandValue* cv = commandValues[commandIndex];
		if (cv->enabled->boolValue()) {
			bool symValues = cv->symmetry->getValue();
			Preset* pFrom = nullptr;
			Preset* pTo = nullptr;
			if (cv->presetOrValue->getValue() == "preset") {
				pFrom = Brain::getInstance()->getPresetById(cv->presetIdFrom->getValue(), true);
				pTo = Brain::getInstance()->getPresetById(cv->presetIdTo->getValue(), true);
				if (pFrom != nullptr) {
					pFrom -> computeValues();
				}
				if (pTo != nullptr) {
					pTo->computeValues();
				}
			}

			ChannelType* rawChan = dynamic_cast<ChannelType*>(cv->channelType->targetContainer.get());
			for (int indexFixt = 0; indexFixt < subFixtures.size(); indexFixt++) {

				SubFixture* sf = subFixtures[indexFixt];
				float normalizedPosition = indexFixt / (float)(subFixtures.size()-1);
				bool useNormalized = false;
				if (selection.subFixtureToPosition.contains(sf)) {
					useNormalized = true;
					normalizedPosition = selection.subFixtureToPosition.getReference(sf);
				}
				float normalizedPositionSym = normalizedPosition * 2;
				normalizedPositionSym = normalizedPositionSym > 1 ? 2 - normalizedPositionSym : normalizedPositionSym;

				std::shared_ptr<HashMap<ChannelType*, float>> valuesFrom = std::make_shared <HashMap<ChannelType*, float>>();
				std::shared_ptr <HashMap<ChannelType*, float>> valuesTo = std::make_shared <HashMap<ChannelType*, float>>();
				String test = cv->presetOrValue->stringValue();
				if (cv->presetOrValue->stringValue() == "preset") {

					std::shared_ptr < HashMap<ChannelType*, float>> tempValuesFrom = pFrom != nullptr ? pFrom->getSubFixtureValues(sf) : nullptr;
					std::shared_ptr < HashMap<ChannelType*, float>> tempValuesTo = pTo != nullptr ? pTo->getSubFixtureValues(sf) : nullptr;
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

				if (cv->presetOrValue->stringValue() == "value") {
					valuesFrom->set(rawChan, cv->valueFrom->getValue());
					valuesTo->set(rawChan, cv->valueTo->getValue());
				}
				else if (cv->presetOrValue->stringValue() == "release") {
					valuesFrom->set(rawChan, -1);
					valuesTo->set(rawChan, -1);
				}

				for (auto it = valuesFrom->begin(); it != valuesFrom->end(); it.next()) {
					SubFixtureChannel* fchan = sf->channelsMap.contains(it.getKey()) ? sf->channelsMap.getReference(it.getKey()) : nullptr;

					float valueFrom = it.getValue();
					float valueTo = valueFrom;
					if (valuesTo->contains(it.getKey())) {
						valueTo = valuesTo->getReference(it.getKey());
					}

					if (fchan != nullptr) {
						if (!computedValues.contains(fchan)) {
							computedValues.set(fchan, std::make_shared<ChannelValue>());
						}
						channelToCommandValue.set(fchan, cv);
						std::shared_ptr<ChannelValue> finalValue = computedValues.getReference(fchan);
						finalValue->parentCommand = this;
						finalValue->canBeTracked = !doNotTrack->boolValue();

						float val = valueFrom;
						if (cv->thru->getValue() && subFixtures.size() > 1) {
							float position = normalizedPosition;
							if (symValues) { position = useNormalized ? normalizedPositionSym : Brain::symPosition(indexFixt, subFixtures.size()); }
							val = jmap(position, val, valueTo);
						}
						finalValue->endValue = val;

						float delay = delayFrom;
						if (delayThru && subFixtures.size() > 1) {
							float position = normalizedPosition;
							if (delaySym) { position = useNormalized ? normalizedPositionSym : Brain::symPosition(indexFixt, subFixtures.size()); }
							position = timing.curveDelayRepart.getValueAtPosition(position);
							position = delayRepartCurve->getValueAtPosition(position);
							if (delayRandom) {
								Random r;
								position = r.nextFloat();
							}
							delay = jmap(position, delayFrom, delayTo);
						}
						if (timingMode == "cue" && callingCuelist != nullptr) {
							if (!fchan->isHTP) {
								if (delayLTP != -1) delay = delayLTP*1000. ;
							}
							else {
								std::shared_ptr<ChannelValue> currentCuelistVal = callingCuelist->activeValues.getReference(fchan);
								if (currentCuelistVal == nullptr || currentCuelistVal->endValue < val) {
									if (delayHTPIn != -1) delay = delayHTPIn * 1000.;
								}
								else {
									if (delayHTPOut != -1) delay = delayHTPOut * 1000.;
								}
							}
						}
						finalValue->delay = delay;

						float fade = fadeFrom;
						finalValue->fadeCurve = fadeCurve;
						if (fchan->snapOnly) {
							fade = 0;
						}
						else {
							if (fadeThru && subFixtures.size() > 1) {
								float position = normalizedPosition;
								if (fadeSym) { position = useNormalized ? normalizedPositionSym : Brain::symPosition(indexFixt, subFixtures.size()); }
								position = timing.curveFadeRepart.getValueAtPosition(position);
								position = fadeRepartCurve->getValueAtPosition(position);
								if (fadeRandom) {
									Random r;
									position = r.nextFloat();
								}
								fade = jmap(position, fadeFrom, fadeTo);
							}
							if (timingMode == "cue" && callingCuelist != nullptr) {
								if (!fchan->isHTP) {
									if (fadeLTP != -1 ) {
										fade = fadeLTP * 1000.;
										finalValue->fadeCurve = nullptr;
										}
								}
								else {
									std::shared_ptr<ChannelValue> currentCuelistVal = callingCuelist->activeValues.contains(fchan) ? callingCuelist->activeValues.getReference(fchan) : nullptr;
									if (currentCuelistVal == nullptr || currentCuelistVal->endValue < val) {
										if (fadeHTPIn != -1) {
											fade = fadeHTPIn * 1000.;
											finalValue->fadeCurve = nullptr;
										}
									}
									else {
										if (fadeHTPOut != -1) {
											fade = fadeHTPOut * 1000.;
											finalValue->fadeCurve = nullptr;
										}
									}
								}
							}
						}

						finalValue->fade = fade;
						if (fchan->isHTP) {
							if (callingCuelist != nullptr && callingCue != nullptr) {
								std::shared_ptr<ChannelValue> currentCuelistVal = callingCuelist->activeValues.contains(fchan) ? callingCuelist->activeValues.getReference(fchan) : nullptr;
								if (currentCuelistVal != nullptr && currentCuelistVal->endValue > finalValue->endValue) {
									finalValue->isTransitionOut = true;
								}
							}
						}
						double tempTiming = (delay + fade);
						maxTiming = std::max(maxTiming, tempTiming);

						finalValue->htpOverride = cv -> HTPOverride->boolValue();
						
						ChannelType* ct = dynamic_cast<ChannelType*>(fchan->parentParamDefinition);
						ChannelFamily* cf = dynamic_cast<ChannelFamily*>(ct->parentContainer->parentContainer.get());
						finalValue->moveInBlack = moveInBlackFamilies.contains(cf);
						finalValue->moveInBlackDelay = mibDelay;
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
	String name = getCommandAsTexts(true).joinIntoString(" ");
	setNiceName(name);
}

void Command::triggerTriggered(Trigger* t)
{
	if (t == explodeSelectionBtn) {
		explodeSelection();
	}
}

StringArray Command::getCommandAsTexts(bool useNames) {
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
	String userName = "";
	String type = "";
	for (int i = 0; i < selection.items.size(); i++) {
		CommandSelection* s = selection.items[i];
		currentUserSelection = s;
		if (s->plusOrMinus->getValueKey() == "-" || words.size() > 0) {
			words.add(s->plusOrMinus->getValueKey());
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
			type = words.getReference(words.size() - 1);
			userName = useNames ? getUserName(type, s->valueFrom->intValue()) : "";
			if (userName != "") { words.set(words.size() - 1, userName); }
			else { words.add(s->valueFrom->stringValue()); }
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
					userName = useNames ? getUserName(type, s->valueTo->intValue()) : "";
					if (userName != "") { words.add(userName); }
					else { words.add(s->valueTo->stringValue()); }
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
		if (i>0) {words.getReference(words.size()-1) = String(words[words.size() - 1])+","; }
		if (v->presetOrValue->getValue() == "preset") {
			words.add("preset");
			lastTarget = "valuePreset";
			userCantPress();
			userCanPressNumber = true;
			currentUserTarget = v->presetIdFrom;
			if ((int)v->presetIdFrom->value > 0) {
				type = "preset";
				userName = useNames ? getUserName(type, v->presetIdFrom->intValue()) : "";
				if (userName != "") { words.set(words.size() - 1, userName); }
				else { words.add(v->presetIdFrom->stringValue()); }
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
						userName = useNames ? getUserName(type, v->presetIdTo->intValue()) : "";
						if (userName != "") { words.add(userName); }
						else { words.add(v->presetIdTo->stringValue()); }
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
		else if (v->presetOrValue->getValue() == "release") {
			ChannelType* c = dynamic_cast<ChannelType*>(v->channelType->targetContainer.get());
			if (c != nullptr) {
				words.add(c->niceName);
				lastTarget = "valueRaw";
				userCantPress();
				userCanPressValue = true;
				currentUserTarget = v->valueFrom;
				userCanPressValueType = true;
				words.add("release");
				lastTarget = "valueRawFrom";
				userCanPressTimingType = true;
				userCanPressThru = false;
				userCanHaveAnotherCommand = true;
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

String Command::getUserName(String type, int id)
{
	String userName = "";
	if (type == "group") {
		Group* g = Brain::getInstance()->getGroupById(id);
		if (g != nullptr) {
			userName = g->userName->stringValue();
		}
	}
	else if (type == "fixture") {
		Fixture* f = Brain::getInstance()->getFixtureById(id);
		if (f != nullptr) {
			userName = f->userName->stringValue();
		}
	}
	else if (type == "preset") {
		Preset* p = Brain::getInstance()->getPresetById(id);
		if (p != nullptr) {
			userName = p->userName->stringValue();
		}
	}

	return userName;
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
	if (selection.computedSelectedSubFixtures.size()<2) {return;}
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
					std::shared_ptr<ChannelValue> chanVal = computedValues.contains(chan) ? computedValues.getReference(chan) : nullptr;
					if (chanVal != nullptr) {
						cv->valueFrom->setValue(chanVal->endValue);
					}
				}

			}
		}
	}
	isComputing.exit();
}

void Command::cleanUnused()
{
	computeValues();
	Array<CommandValue*> used;
	bool changed = false;

	for (auto it = channelToCommandValue.begin(); it != channelToCommandValue.end(); it.next()) {
		used.addIfNotAlreadyThere(it.getValue());
	}

	for (int i = values.items.size() - 2; i >= 0; i--) {
		if (!used.contains(values.items[i])) {
			values.removeItem(values.items[i]);
			changed = true;
		}
	}

	if (changed) {
		UserInputManager::getInstance()->commandSelectionChanged(this);
	}
}

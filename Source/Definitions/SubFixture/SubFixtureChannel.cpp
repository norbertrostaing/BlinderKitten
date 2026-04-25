/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "SubFixtureChannel.h"
#include "../ChannelFamily/ChannelFamilyManager.h"
#include "../FixtureType/FixtureTypeChannel.h"
#include "../ChannelFamily/ChannelType/ChannelType.h"
#include "Brain.h"
#include "../Fixture/Fixture.h"
#include "../Interface/InterfaceIncludes.h"
#include "../Cuelist/Cuelist.h"
#include "../Programmer/Programmer.h"
#include "../Effect/Effect.h"
#include "../Tracker/Tracker.h"
#include "../ChannelValue.h"
#include "UI/InputPanel.h"
#include "Command/Command.h"
#include "ChannelValue.h"
#include "Definitions/Carousel/Carousel.h"
#include "Definitions/Mapper/Mapper.h"
#include "Definitions/Stamp/Stamp.h"
#include "Definitions/SelectionMaster/SelectionMaster.h"


SubFixtureChannel::SubFixtureChannel(){
	Brain::getInstance()->allSubfixtureChannels.add(this);
}

SubFixtureChannel::~SubFixtureChannel()
{
	//isDeleted = true;
	Brain::getInstance()->allSubfixtureChannels.removeAllInstancesOf(this);
	cs.enter();
	Brain::getInstance()->usingCollections.enter();
	Brain::getInstance()->grandMasterChannels.removeAllInstancesOf(this);
	Brain::getInstance()->swoppableChannels.removeAllInstancesOf(this);
	Brain::getInstance()->subFixtureChannelPoolWaiting.removeAllInstancesOf(this);
	Brain::getInstance()->subFixtureChannelPoolUpdating.removeAllInstancesOf(this);
	Brain::getInstance()->usingCollections.exit();

	for (auto it = Brain::getInstance()->cuelists.begin(); it != Brain::getInstance()->cuelists.end(); it.next()) {
		Cuelist* c = it.getValue();
		c->isComputing.enter();
		if (c->activeValues.contains(this)) c->activeValues.remove(this);
		c->isComputing.exit();
	}
	for (auto it = Brain::getInstance()->programmers.begin(); it != Brain::getInstance()->programmers.end(); it.next()) {
		Programmer* c = it.getValue();
		c->computing.enter();
		if (c->activeValues.contains(this)) c->activeValues.remove(this);
		if (c->computedValues.contains(this)) c->computedValues.remove(this);
		c->computing.exit();
	}
	for (auto it = Brain::getInstance()->effects.begin(); it != Brain::getInstance()->effects.end(); it.next()) {
		Effect* c = it.getValue();
		c->isComputing.enter();
		if (c->chanToFxParam.contains(this)) c->chanToFxParam.remove(this);
		c->isComputing.exit();
	}
	for (auto it = Brain::getInstance()->carousels.begin(); it != Brain::getInstance()->carousels.end(); it.next()) {
		Carousel* c = it.getValue();
		c->isComputing.enter();
		if (c->chanToCarouselRow.contains(this)) c->chanToCarouselRow.remove(this);
		c->isComputing.exit();
		for (CarouselRow* row : c->rows.items) {
			row->isComputing.enter();
			if (row->subFixtureChannelOffsets.contains(this)) row->subFixtureChannelOffsets.remove(this);
			row->isComputing.exit();
			for (CarouselStep* step : row->paramContainer.items) {
				step->isComputing.enter();
				if (step->computedValues.contains(this)) step->computedValues.remove(this);
				step->isComputing.exit();
			}
		}
	}
	for (auto it = Brain::getInstance()->mappers.begin(); it != Brain::getInstance()->mappers.end(); it.next()) {
		Mapper* c = it.getValue();
		c->isComputing.enter();
		if (c->chanToMapperRow.contains(this)) c->chanToMapperRow.remove(this);
		c->isComputing.exit();
	}
	for (auto it = Brain::getInstance()->trackers.begin(); it != Brain::getInstance()->trackers.end(); it.next()) {
		Tracker* c = it.getValue();
		c->isComputing.enter();
		if (c->chanToVal.contains(this)) c->chanToVal.remove(this);
		c->isComputing.exit();
	}
	for (auto it = Brain::getInstance()->selectionMasters.begin(); it != Brain::getInstance()->selectionMasters.end(); it.next()) {
		SelectionMaster* c = it.getValue();
		c->isComputing.enter();
		if (c->channels.contains(this)) c->channels.removeAllInstancesOf(this);
		c->isComputing.exit();
	}

	for (Command* c : Brain::getInstance()->allCommands) {
		c->isComputing.enter();
		c->channelToCommandValue.remove(this);
		c->computedValues.remove(this);
		c->isComputing.exit();
	}

	for (DMXInterface* inter : InterfaceManager::getInstance()->getInterfacesOfType<DMXInterface>()) {
		for (int i = 0; i < 512; i++) {
			if (inter->channelToSubFixtureChannel[i] == this) inter->channelToSubFixtureChannel.set(i,nullptr);
		}
	}

	cs.exit();

}

void SubFixtureChannel::writeValue(float v) {
	v= jmin((float)1, v);
	v= jmax((float)0, v);

	if (virtualMaster != nullptr) {
		v *= virtualMaster->currentValue;
	}

	if (v == currentValue && !forceNextWrite) {
		return;
	}
	forceNextWrite = false;

	currentValue = v;
	parentFixture->channelValueChanged(subFixtureId, channelType, v);

	if (virtualChildren.size() > 0) {
		for (int i = 0; i < virtualChildren.size(); i++) {
			virtualChildren[i]->isDirty = true;
			//Brain::getInstance()->pleaseUpdate(virtualChildren[i]);
		}
		return;
	}

	if (invertOutput) {
		v = 1-v;
	}

	if (parentFixture != nullptr && parentFixtureTypeChannel != nullptr && parentParamDefinition != nullptr) {

		if (parentFixtureTypeChannel->curve.enabled->boolValue()) {
			v = parentFixtureTypeChannel->curve.getValueAtPosition(v);
		}

		int deltaAdress = parentFixtureTypeChannel->dmxDelta->intValue();
		deltaAdress--;
		String chanRes = parentFixtureTypeChannel->resolution->stringValue();
		for (int i = 0; i < parentFixture->patchs.items.size(); i++) {
			FixturePatch* p = parentFixture->patchs.items[i];
			if (p->enabled->boolValue()) {
				float localValue = v;

				DMXInterface* out = static_cast<DMXInterface*>(p->targetInterface->targetContainer.get());
				if (out != nullptr) {
					FixturePatch* patch = p;
					int address = patch->address->getValue();

					for (int iCorr = 0; iCorr < patch->corrections.items.size(); iCorr++) {
						FixturePatchCorrection* c = patch->corrections.items[iCorr];
						if (c->isOn && c->enabled->boolValue() && (int)c->subFixtureId->getValue() == subFixtureId && static_cast<ChannelType*>(c->channelType->targetContainer.get()) == channelType) {
							if (c->invertChannel->getValue()) {
								localValue = 1 - localValue;
							}
							localValue = localValue + (float)c->offsetValue->getValue();
							localValue = jmin((float)1, localValue);
							localValue = jmax((float)0, localValue);
							localValue = c->curve.getValueAtPosition(localValue);
						}
					}


					if (address > 0) {
						address += (deltaAdress);
						if (fineChannelDelta == 0) {
							int val = localValue >= 1 ? 255 : round(255 * localValue);
							//val = val > 255 ? 255 : val;
							out->sendDMXValue(address, val);
						}
						else {
							int val = localValue >= 1 ? 65535 : round(65535.0 * localValue);
							//int val = floor(65535.0 * value);
							//val = value > 65535 ? 65535 : val;
							int valueA = val / 256;
							int valueB = val % 256;
							out->sendDMXValue(address, valueA);
							out->sendDMXValue(address + fineChannelDelta, valueB);
						}
					}
				}
			}
		}

	}

}


void SubFixtureChannel::updateVal(double now) {
	float newValue = defaultValue;
	float noGMValue = 0;
	if (defaultPresetValue >= 0) {
		newValue = defaultPresetValue;
	}

	postCuelistValue = newValue;

	cs.enter();

	cuelistStackSnapshot.addArray(cuelistStack);
	programmerStackSnapshot.addArray(programmerStack);
	effectStackSnapshot.addArray(effectStack);
	stampStackSnapshot.addArray(stampStack);
	carouselStackSnapshot.addArray(carouselStack);
	mapperStackSnapshot.addArray(mapperStack);
	trackerStackSnapshot.addArray(trackerStack);
	selectionMasterStackSnapshot.addArray(selectionMasterStack);
	cuelistFlashStackSnapshot.addArray(cuelistFlashStack);

	cs.exit();

	liveCV.clear();

	Array<int> layers;

	for (int i = 0; i < cuelistStackSnapshot.size(); i++) {
		layers.addIfNotAlreadyThere(cuelistStackSnapshot[i]->layerId->intValue());
	}
	for (int i = 0; i < programmerStackSnapshot.size(); i++) {
		layers.addIfNotAlreadyThere(programmerStackSnapshot[i]->layerId->intValue());
	}
	for (int i = 0; i < mapperStackSnapshot.size(); i++) {
		layers.addIfNotAlreadyThere(mapperStackSnapshot[i]->layerId->intValue());
	}
	for (int i = 0; i < carouselStackSnapshot.size(); i++) {
		layers.addIfNotAlreadyThere(carouselStackSnapshot[i]->layerId->intValue());
	}
	for (int i = 0; i < trackerStackSnapshot.size(); i++) {
		layers.addIfNotAlreadyThere(trackerStackSnapshot[i]->layerId->intValue());
	}
	for (int i = 0; i < stampStackSnapshot.size(); i++) {
		layers.addIfNotAlreadyThere(stampStackSnapshot[i]->layerId->intValue());
	}
	for (int i = 0; i < effectStackSnapshot.size(); i++) {
		layers.addIfNotAlreadyThere(effectStackSnapshot[i]->layerId->intValue());
	}
	for (int i = 0; i < selectionMasterStackSnapshot.size(); i++) {
		layers.addIfNotAlreadyThere(selectionMasterStackSnapshot[i]->layerId->intValue());
	}

	layers.sort();
	
	bool checkSwop = Brain::getInstance()->isSwopping && swopKillable;
	activeCommand = nullptr;

	for (int l = 0; l< layers.size(); l++)
		{
		int currentLayer = layers[l];

		int overWritten = -1;
		for (int i = 0; i < cuelistStackSnapshot.size(); i++)
		{
			Cuelist* c = cuelistStackSnapshot[i];
			if (c->layerId->intValue() == currentLayer) {
				if (!checkSwop || c->isSwopping || c->excludeFromSwop->boolValue()) {
					bool isApplied;
					newValue = c->applyToChannel(this, newValue, now, isApplied);
					if (c->excludeFromGrandMaster->boolValue()) {
						bool temp;
						noGMValue = c->applyToChannel(this, noGMValue, now, temp);
					}
					std::shared_ptr<ChannelValue> cv = c->activeValues.contains(this) ? c->activeValues.getReference(this) : nullptr;
					if (cv != nullptr && isApplied) {
						activeCommand = cv->parentCommand;
						if (cv->isEnded && !c->isFlashing) {
							overWritten = i - 1;
						}
					}
				}
			}
		}

		for (int i = 0; i <= overWritten; i++) {
			if (cuelistStackSnapshot[i]->layerId->intValue() == currentLayer) {
				std::shared_ptr<ChannelValue> cv = cuelistStackSnapshot[i]->activeValues.contains(this) ? cuelistStackSnapshot[i]->activeValues.getReference(this) : nullptr;
				if (cv != nullptr && !cv->isOverWritten) {
					cv->isOverWritten = true;
					Brain::getInstance()->pleaseUpdate(cuelistStackSnapshot[i]);
				}
			}
		}
		if (!checkSwop) {
			for (int i = 0; i < programmerStackSnapshot.size(); i++) {
				if (programmerStackSnapshot[i]->layerId->intValue() == currentLayer) {
					newValue = programmerStackSnapshot[i]->applyToChannel(this, newValue, now);
				}
			}

			for (int i = 0; i < mapperStackSnapshot.size(); i++) {
				if (mapperStackSnapshot[i]->layerId->intValue() == currentLayer) {
					newValue = mapperStackSnapshot[i]->applyToChannel(this, newValue, now);
				}
			}

			for (int i = 0; i < trackerStackSnapshot.size(); i++) {
				if (trackerStackSnapshot[i]->layerId->intValue() == currentLayer) {
					newValue = trackerStackSnapshot[i]->applyToChannel(this, newValue, now);
				}
			}

		}
		postCuelistValue = newValue;
		for (int i = 0; i < carouselStackSnapshot.size(); i++) {
			if (carouselStackSnapshot[i]->layerId->intValue() == currentLayer) {
				if (!checkSwop || carouselStackSnapshot[i]->isSwopping) {
					newValue = carouselStackSnapshot[i]->applyToChannel(this, newValue, now);
					isDirty = isDirty || carouselStackSnapshot[i]->isOn;
				}
			}
		}

		for (int i = 0; i < stampStackSnapshot.size(); i++) {
			if (stampStackSnapshot[i]->layerId->intValue() == currentLayer) {
				if (!checkSwop || stampStackSnapshot[i]->isSwopping) {
					newValue = stampStackSnapshot[i]->applyToChannel(this, newValue, now);
					isDirty = isDirty || stampStackSnapshot[i]->isOn;
				}
			}
		}

		for (int i = 0; i < effectStackSnapshot.size(); i++) {
			if (effectStackSnapshot[i]->layerId->intValue() == currentLayer) {
				if (!checkSwop || effectStackSnapshot[i]->isSwopping) {
					newValue = effectStackSnapshot[i]->applyToChannel(this, newValue, now);
					isDirty = isDirty || effectStackSnapshot[i]->isOn;
				}
			}
		}

		for (int i = 0; i < selectionMasterStackSnapshot.size(); i++) {
			if (selectionMasterStackSnapshot[i]->layerId->intValue() == currentLayer) {
				newValue = selectionMasterStackSnapshot[i]->applyToChannel(this, newValue, now);
			}
		}

	}

	cuelistStackSnapshot.clear();
	programmerStackSnapshot.clear();
	effectStackSnapshot.clear();
	stampStackSnapshot.clear();
	carouselStackSnapshot.clear();
	mapperStackSnapshot.clear();
	trackerStackSnapshot.clear();
	selectionMasterStackSnapshot.clear();
	cuelistFlashStackSnapshot.clear();

	if (reactToGrandMaster) {
		double gm = InputPanel::getInstance()->paramGrandMaster->floatValue();
		if (InputPanel::getInstance()->paramBlackOut->boolValue()) {
			newValue = 0;
		}
		if (isHTP) {
			newValue = jmax(newValue * (float)gm, noGMValue);
		}
		else {
			newValue = jmap((float)gm, 0.0f,1.0f,noGMValue, newValue);
		}
	}

	writeValue(newValue);
}

void SubFixtureChannel::cuelistOnTopOfStack(Cuelist* c) {
	cuelistOutOfStack(c);
	cuelistStack.add(c);
}

void SubFixtureChannel::cuelistOutOfStack(Cuelist* c) {
	cs.enter();
	while (cuelistStack.indexOf(c) >= 0) {
		cuelistStack.removeAllInstancesOf(c);
	}
	cs.exit();
}

void SubFixtureChannel::cuelistOnTopOfFlashStack(Cuelist* c) {
	cuelistOutOfFlashStack(c);
	cuelistFlashStack.add(c);
}

void SubFixtureChannel::cuelistOutOfFlashStack(Cuelist* c) {
	cs.enter();
	while (cuelistFlashStack.indexOf(c) >= 0) {
		cuelistFlashStack.removeAllInstancesOf(c);
	}
	cs.exit();
}

void SubFixtureChannel::programmerOnTopOfStack(Programmer* p) {
	programmerOutOfStack(p);
	programmerStack.add(p);
}

void SubFixtureChannel::programmerOutOfStack(Programmer* p) {
	cs.enter();
	while (programmerStack.indexOf(p) >= 0) {
		programmerStack.removeAllInstancesOf(p);
	}
	cs.exit();
}

void SubFixtureChannel::effectOnTopOfStack(Effect* f) {
	effectOutOfStack(f);
	effectStack.add(f);
}

void SubFixtureChannel::effectOutOfStack(Effect* f) {
	cs.enter();
	while (effectStack.indexOf(f) >= 0) {
		effectStack.removeAllInstancesOf(f);
	}
	cs.exit();
}

void SubFixtureChannel::stampOnTopOfStack(Stamp* f) {
	stampOutOfStack(f);
	stampStack.add(f);
}

void SubFixtureChannel::stampOutOfStack(Stamp* f) {
	cs.enter();
	while (stampStack.indexOf(f) >= 0) {
		stampStack.removeAllInstancesOf(f);
	}
	cs.exit();
}

void SubFixtureChannel::carouselOnTopOfStack(Carousel* f) {
	carouselOutOfStack(f);
	carouselStack.add(f);
}

void SubFixtureChannel::carouselOutOfStack(Carousel* f) {
	cs.enter();
	while (carouselStack.indexOf(f) >= 0) {
		carouselStack.removeAllInstancesOf(f);
	}
	cs.exit();
}

void SubFixtureChannel::mapperOnTopOfStack(Mapper* f) {
	mapperOutOfStack(f);
	mapperStack.add(f);
}

void SubFixtureChannel::mapperOutOfStack(Mapper* f) {
	cs.enter();
	while (mapperStack.indexOf(f) >= 0) {
		mapperStack.removeAllInstancesOf(f);
	}
	cs.exit();
}

void SubFixtureChannel::trackerOnTopOfStack(Tracker* f) {
	trackerOutOfStack(f);
	trackerStack.add(f);
}

void SubFixtureChannel::trackerOutOfStack(Tracker* f) {
	cs.enter();
	while (trackerStack.indexOf(f) >= 0) {
		trackerStack.removeAllInstancesOf(f);
	}
	cs.exit();
}

void SubFixtureChannel::selectionMasterOnTopOfStack(SelectionMaster* f) {
	selectionMasterOutOfStack(f);
	selectionMasterStack.add(f);
}

void SubFixtureChannel::selectionMasterOutOfStack(SelectionMaster* f) {
	cs.enter();
	while (selectionMasterStack.indexOf(f) >= 0) {
		selectionMasterStack.removeAllInstancesOf(f);
	}
	cs.exit();
}


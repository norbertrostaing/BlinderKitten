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
#include "../Fixture/FixturePatch.h"
#include "../Interface/InterfaceIncludes.h"
#include "../Cuelist/Cuelist.h"
#include "../Programmer/Programmer.h"
#include "../Effect/Effect.h"
#include "../Tracker/Tracker.h"
#include "../ChannelValue.h"
#include "UI/InputPanel.h"
#include "Command/Command.h"

SubFixtureChannel::SubFixtureChannel():
	virtualChildren()
{
	cs.enter();
	cuelistStack.clear();
	programmerStack.clear();
	effectStack.clear();
	carouselStack.clear();
	cuelistFlashStack.clear();
	virtualChildren.clear();
	cs.exit();
}

SubFixtureChannel::~SubFixtureChannel()
{
	//isDeleted = true;
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

	for (Command* c : Brain::getInstance()->allCommands) {
		c->isComputing.enter();
		c->channelToCommandValue.remove(this);
		c->computedValues.remove(this);
		c->isComputing.exit();
	}

	cs.exit();

}

void SubFixtureChannel::writeValue(float v) {
	v= jmin((float)1, v);
	v= jmax((float)0, v);

	if (virtualChildren.size() > 0) {
		value = v;
		for (int i = 0; i < virtualChildren.size(); i++) {
			Brain::getInstance()->pleaseUpdate(virtualChildren[i]);
		}
		return;
	}

	if (virtualMaster != nullptr) {
		v *= virtualMaster->value;
	}

	if (invertOutput) {
		v = 1-v;
	}

	if (parentFixture != nullptr && parentFixtureTypeChannel != nullptr && parentParamDefinition != nullptr) {

		int deltaAdress = parentFixtureTypeChannel->dmxDelta->getValue();
		deltaAdress--;
		String chanRes = parentFixtureTypeChannel->resolution->getValue();
		Array<FixturePatch*> patchs = parentFixture->patchs.getItemsWithType<FixturePatch>();
		for (int i = 0; i < patchs.size(); i++) {
			if (patchs[i]->enabled->boolValue()) {
				value = v;

				DMXInterface* out = dynamic_cast<DMXInterface*>(patchs.getReference(i)->targetInterface->targetContainer.get());
				if (out != nullptr) {
					FixturePatch* patch = patchs.getReference(i);
					int address = patch->address->getValue();

					for (int iCorr = 0; iCorr < patch->corrections.items.size(); iCorr++) {
						FixturePatchCorrection* c = patch->corrections.items[iCorr];
						if (c->isOn && c->enabled->boolValue() && (int)c->subFixtureId->getValue() == subFixtureId && dynamic_cast<ChannelType*>(c->channelType->targetContainer.get()) == channelType) {
							if (c->invertChannel->getValue()) {
								value = 1 - value;
							}
							value = value + (float)c->offsetValue->getValue();
							value = jmin((float)1, value);
							value = jmax((float)0, value);
							value = c->curve.getValueAtPosition(value);
						}
					}


					if (address > 0) {
						address += (deltaAdress);
						if (chanRes == "8bits") {
							int val = value >= 1 ? 255 : round(255 * value);
							//val = val > 255 ? 255 : val;
							out->sendDMXValue(address, val);
						}
						else if (chanRes == "16bits") {
							int val = value >= 1 ? 65535 : round(65535.0 * value);
							//int val = floor(65535.0 * value);
							//val = value > 65535 ? 65535 : val;
							int valueA = val / 256;
							int valueB = val % 256;
							out->sendDMXValue(address, valueA);
							out->sendDMXValue(address + 1, valueB);
						}
					}
				}
			}
		}

	}

}


void SubFixtureChannel::updateVal(double now) {
	float newValue = defaultValue;

	cs.enter();
	Array<int> layers;

	for (int i = 0; i < cuelistStack.size(); i++) {
		layers.addIfNotAlreadyThere(cuelistStack.getReference(i)->layerId->getValue());
	}
	for (int i = 0; i < programmerStack.size(); i++) {
		layers.addIfNotAlreadyThere(programmerStack.getReference(i)->layerId->getValue());
	}
	for (int i = 0; i < mapperStack.size(); i++) {
		layers.addIfNotAlreadyThere(mapperStack.getReference(i)->layerId->getValue());
	}
	for (int i = 0; i < carouselStack.size(); i++) {
		layers.addIfNotAlreadyThere(carouselStack.getReference(i)->layerId->getValue());
	}
	for (int i = 0; i < trackerStack.size(); i++) {
		layers.addIfNotAlreadyThere(trackerStack.getReference(i)->layerId->getValue());
	}
	for (int i = 0; i < effectStack.size(); i++) {
		layers.addIfNotAlreadyThere(effectStack.getReference(i)->layerId->getValue());
	}

	layers.sort();
	
	bool checkSwop = Brain::getInstance()->isSwopping && swopKillable;
	activeCommand = nullptr;

	for (int l = 0; l< layers.size(); l++)
		{
		int currentLayer = layers.getReference(l);

		int overWritten = -1;
		for (int i = 0; i < cuelistStack.size(); i++)
		{
			Cuelist* c = cuelistStack.getReference(i);
			if ((int)c->layerId->getValue() == currentLayer) {
				if (!checkSwop || c->isSwopping) {
					bool isApplied;
					newValue = c->applyToChannel(this, newValue, now, isApplied);
					std::shared_ptr<ChannelValue> cv = c->activeValues.contains(this) ? c->activeValues.getReference(this) : nullptr;
					if (cv != nullptr && isApplied) {
						activeCommand = cv->parentCommand;
						if (cv->isEnded) {
							overWritten = i - 1;
						}
					}
				}
			}
		}

		postCuelistValue = newValue;

		for (int i = 0; i <= overWritten; i++) {
			if ((int)cuelistStack.getReference(i)->layerId->getValue() == currentLayer) {
				std::shared_ptr<ChannelValue> cv = cuelistStack.getReference(i)->activeValues.contains(this) ? cuelistStack.getReference(i)->activeValues.getReference(this) : nullptr;
				if (cv != nullptr && !cv->isOverWritten) {
					cv->isOverWritten = true;
					Brain::getInstance()->pleaseUpdate(cuelistStack.getReference(i));
				}
			}
		}
		if (!checkSwop) {

			for (int i = 0; i < programmerStack.size(); i++) {
				if ((int)programmerStack.getReference(i)->layerId->getValue() == currentLayer) {
					newValue = programmerStack.getReference(i)->applyToChannel(this, newValue, now);
				}
			}

			for (int i = 0; i < mapperStack.size(); i++) {
				if ((int)mapperStack.getReference(i)->layerId->getValue() == currentLayer) {
					newValue = mapperStack.getReference(i)->applyToChannel(this, newValue, now);
				}
			}

			for (int i = 0; i < trackerStack.size(); i++) {
				if ((int)trackerStack.getReference(i)->layerId->getValue() == currentLayer) {
					newValue = trackerStack.getReference(i)->applyToChannel(this, newValue, now);
				}
			}

		}
		for (int i = 0; i < carouselStack.size(); i++) {
				if ((int)carouselStack.getReference(i)->layerId->getValue() == currentLayer) {
					if (!checkSwop || carouselStack[i]->isSwopping) {
						newValue = carouselStack.getReference(i)->applyToChannel(this, newValue, now);
					}
				}
			}

			for (int i = 0; i < effectStack.size(); i++) {
				if ((int)effectStack.getReference(i)->layerId->getValue() == currentLayer) {
					if (!checkSwop || effectStack[i]->isSwopping) {
						newValue = effectStack.getReference(i)->applyToChannel(this, newValue, now);
					}
				}
			}

	}

	for (int i = 0; i < cuelistFlashStack.size(); i++)
	{
		// newValue = cuelistFlashStack.getReference(i)->applyToChannel(this, newValue, now, true);
	}

	/*

	if (swopKillable) {
		if (Brain::getInstance()->isSwopping) {
			newValue = defaultValue;
			for (int i = 0; i < cuelistFlashStack.size(); i++)
			{
				if (cuelistFlashStack.getReference(i)->isSwopping) {
					newValue = cuelistFlashStack.getReference(i)->applyToChannel(this, newValue, now, true);
				}
			}
		}
	}
	*/
	if (reactToGrandMaster) {
		double gm = InputPanel::getInstance()->grandMaster.getValue();
		newValue *= gm;
		if (InputPanel::getInstance()->blackoutBtn.getToggleState()) {
			newValue = 0;
		}
	}

	cs.exit();
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


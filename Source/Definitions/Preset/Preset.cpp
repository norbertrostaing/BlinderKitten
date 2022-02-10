/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Preset.h"
#include "PresetValue.h"
#include "../FixtureParamType/FixtureParamDefinition/FixtureParamDefinition.h"
#include "../../Brain.h"
#include "PresetManager.h"

Preset::Preset(var params) :
	BaseItem(params.getProperty("name", "Preset")),
	objectType(params.getProperty("type", "Preset").toString()),
	objectData(params),
	fixtureValues(),
	devTypeParam()
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = true;

	itemDataType = "Preset";
	
	id = addIntParameter("ID", "ID of this Preset", 1, 1);
	userName = addStringParameter("Name", "Name of this preset", "New preset");
	updateName();

	presetType = addEnumParameter("Type", "Type of the preset");
	presetType->addOption("Fixture", 1);
	presetType->addOption("Fixture Type", 2);
	presetType->addOption("Device Type", 3);
	presetType->addOption("Same Parameters", 4);

	// to add a manager with defined data
	BaseManager<PresetFixtureValues>* m = new BaseManager<PresetFixtureValues>("Fixtures");
	m->selectItemWhenCreated = false;
	fixtureValues.reset(m);
	addChildControllableContainer(fixtureValues.get());
	getJSONData();

	Brain::getInstance()->registerPreset(this, id->getValue());

}

void Preset::afterLoadJSONDataInternal() {
	Logger::writeToLog("call from JSONDataInternal");
}

Preset::~Preset()
{
	Brain::getInstance()->unregisterPreset(this);
}

void Preset::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerPreset(this, id->getValue());
	}
	if (p == userName || p == id) {
		updateName();
	}
}

void Preset::computeValues() {
	computedFixtureValues.clear();
	computedDeviceTypeValues.clear();
	computedFixtureTypeValues.clear();
	computedUniversalValues.clear();

	Array<PresetFixtureValues*> fixtValues = fixtureValues->getItemsWithType<PresetFixtureValues>();
	int pType = presetType->getValue();

	for (int commandIndex = 0; commandIndex < fixtValues.size(); commandIndex++) {
		PresetFixtureValues* fixtVal = fixtValues[commandIndex];
		Fixture* fixt = nullptr;
		String suffix = "";
		DeviceType* type = nullptr;
		int fixtureId = fixtVal->targetFixtureId->getValue();
		if (fixtureId > 0) {
			fixt = Brain::getInstance()->getFixtureById(fixtureId);
			if (fixt != nullptr) {
				suffix = fixt -> suffixName;
				type = dynamic_cast<DeviceType*>(fixt -> parentDevice -> devTypeParam->targetContainer.get());
			}
		}
		
		Array<PresetValue *> values = fixtVal->values->getItemsWithType<PresetValue>();
		for (int valIndex = 0; valIndex < values.size(); valIndex++) {
			FixtureParamDefinition* param = dynamic_cast<FixtureParamDefinition*>(values[valIndex]->param->targetContainer.get());
			float value = values[valIndex] -> paramValue -> getValue();
			if (param != nullptr) {
				// fixture mode
				if (pType >= 1 && fixt != nullptr) {
					HashMap<FixtureParamDefinition*, float>* content = computedFixtureValues.getReference(fixt);
					if (content == nullptr) {
						content = new HashMap<FixtureParamDefinition*, float>();
						computedFixtureValues.set(fixt, content);
					}
					content -> set(param, value); 
				}
				// fixture type mode
				if (pType >= 2 && fixt != nullptr && type != nullptr) {
					HashMap<String, HashMap<FixtureParamDefinition*, float>*>* deviceContent = computedFixtureTypeValues.getReference(type);
					if (deviceContent == nullptr) {
						deviceContent = new HashMap<String, HashMap<FixtureParamDefinition*, float>*>();
						computedFixtureTypeValues.set(type, deviceContent);
					}

					HashMap<FixtureParamDefinition*, float>* fixtTypeContent = deviceContent->getReference(suffix);
					if (fixtTypeContent == nullptr) {
						fixtTypeContent = new HashMap<FixtureParamDefinition*, float>();
						deviceContent->set(suffix, fixtTypeContent);
					}

					fixtTypeContent->set(param, value);
				}
				// device mode
				if (pType >= 3 && type != nullptr) {
					HashMap<FixtureParamDefinition*, float>* content = computedDeviceTypeValues.getReference(type);
					if (content == nullptr) {
						content = new HashMap<FixtureParamDefinition*, float>();
						computedDeviceTypeValues.set(type, content);
					}
					content->set(param, value);
				}

				// same param mode
				if (pType >= 4) {
					if (!computedUniversalValues.contains(param)) {
						computedUniversalValues.set(param, value);
					}
				}

			}

		}


		// HashMap<Fixture*, HashMap<FixtureParamDefinition*, float>*> computedFixtureValues;
		// HashMap<DeviceType*, HashMap<FixtureParamDefinition*, float>*> computedDeviceTypeValues;
		// HashMap<DeviceType*, HashMap<String, HashMap<FixtureParamDefinition*, float>*>*> computedFixtureTypeValues;
		// HashMap<FixtureParamDefinition*, float> computedUniversalValues;

	
	
	}

}

HashMap<FixtureParamDefinition*, float>* Preset::getFixtureValues(Fixture* f) {
	HashMap<FixtureParamDefinition*, float>* values = new HashMap<FixtureParamDefinition*, float>();
	for (auto it = computedUniversalValues.begin(); it != computedUniversalValues.end(); it.next()) {
		values->set(it.getKey(), it.getValue());
	}

	DeviceType* dt = dynamic_cast<DeviceType*>(f->parentDevice->devTypeParam->targetContainer.get());
	if (computedDeviceTypeValues.contains(dt)) {
		HashMap<FixtureParamDefinition*, float>* deviceValues = computedDeviceTypeValues.getReference(dt);
		for (auto it = deviceValues->begin(); it != deviceValues->end(); it.next()) {
			values->set(it.getKey(), it.getValue());
		}
	}

	String suffix = f->suffixName;
	if (computedFixtureTypeValues.contains(dt) && computedFixtureTypeValues.getReference(dt)->contains(suffix)) {
		HashMap<FixtureParamDefinition*, float>* FTValues = computedFixtureTypeValues.getReference(dt)->getReference(suffix);
		for (auto it = FTValues->begin(); it != FTValues->end(); it.next()) {
			values->set(it.getKey(), it.getValue());
		}
	}

	if (computedFixtureValues.contains(f)) {
		HashMap<FixtureParamDefinition*, float>* FValues = computedFixtureValues.getReference(f);
		for (auto it = FValues->begin(); it != FValues->end(); it.next()) {
			values->set(it.getKey(), it.getValue());
		}
	}

	return values;
}


void Preset::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<PresetManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
}




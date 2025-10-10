/*
  ==============================================================================

    FixtureTypePreset.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "FixtureTypePreset.h"
#include "../ChannelFamily/ChannelFamilyManager.h"
#include "FixtureTypePresetManager.h"
#include "Fixture/FixtureManager.h"
#include "Brain.h"
#include "Definitions/Preset/PresetManager.h"

FixtureTypePreset::FixtureTypePreset(var params) :
    BaseItem(params.getProperty("name", "Preset")),
    objectType(params.getProperty("type", "FixtureTypePreset").toString()),
    objectData(params),
    values("Subfixture values")
{
    itemDataType = "FixtureTypePreset";

    importFromPresetGridBtn = addTrigger("Import from grid", ""); 
    exportInPresetGridBtn = addTrigger("Export to grid", ""); 

    addChildControllableContainer(&values);
    saveAndLoadRecursiveData = true;
    values.selectItemWhenCreated = false;
};

FixtureTypePreset::~FixtureTypePreset()
{
};

void FixtureTypePreset::importFromPresetGrid()
{
    FixtureType* fixtType = dynamic_cast<FixtureType*>(parentContainer->parentContainer->parentContainer.get());
    Fixture* fixt = nullptr;
    values.clear();
    for (auto it = Brain::getInstance()->fixtures.begin(); it != Brain::getInstance()->fixtures.end() && fixt == nullptr; it.next()) {
        Fixture* f = it.getValue();
        FixtureType* ft = dynamic_cast<FixtureType*>(f->devTypeParam->targetContainer.get());
        if (ft == fixtType) {
            fixt = f;
        }
    }

    if (fixt != nullptr) {
        for (auto it = Brain::getInstance()->presets.begin(); it != Brain::getInstance()->presets.end(); it.next()) {
            Preset* p = it.getValue();
            if (p->userName->stringValue() == niceName) {
                p->computeValues();

                for (auto itSub = fixt->subFixtures.begin(); itSub != fixt->subFixtures.end(); itSub.next()) {
                    int subFixtId = itSub.getKey();
                    SubFixture* sf = itSub.getValue();
                    auto vals = p->getSubFixtureValues(sf);
                    FixtureTypePresetValues* ftpv = nullptr; 
                    for (auto itVal = vals->begin(); itVal != vals->end(); itVal.next()) {
                        ChannelType* ct = itVal.getKey();
                        if (sf->channelsMap.contains(ct)) {
                            if (ftpv == nullptr) {
                                ftpv = values.addItem();
                                ftpv->subFixtureId->setValue(subFixtId);
                                ftpv->subFixtureId->setEnabled(true);
                            }

                            PresetValue* pv = ftpv->values.addItem();
                            pv->param->setValueFromTarget(ct);
                            pv->paramValue->setValue(itVal.getValue());
                        }
                    }
                }
            }
        }
    }
}

void FixtureTypePreset::exportInPresetGrid(bool addIfNotThere)
{
    FixtureType* fixtType = dynamic_cast<FixtureType*>(parentContainer->parentContainer->parentContainer.get());
    Fixture* fixt = nullptr;
    for (auto it = Brain::getInstance()->fixtures.begin(); it != Brain::getInstance()->fixtures.end() && fixt == nullptr; it.next()) {
        Fixture* f = it.getValue();
        FixtureType* ft = dynamic_cast<FixtureType*>(f->devTypeParam->targetContainer.get());
        if (ft == fixtType) {
            fixt = f;
        }
    }

    Preset* targetPreset = nullptr;
    if (fixt != nullptr) {
        for (auto it = Brain::getInstance()->presets.begin(); targetPreset == nullptr && it != Brain::getInstance()->presets.end(); it.next()) {
            Preset* p = it.getValue();
            if (p->userName->stringValue() == niceName) {
                targetPreset = p;
            }
        }
    }

    if (addIfNotThere && targetPreset == nullptr) {
        targetPreset = PresetManager::getInstance()->addItem();
        targetPreset->userName->setValue(niceName);
        targetPreset->subFixtureValues.clear();
    }

    if (targetPreset != nullptr) {
        if ((int)targetPreset->presetType->getValueData() < 3) {
            targetPreset->presetType->setValueWithData(3);
        }
        Array <PresetSubFixtureValues*> toDelete;
        for (int i = 0; i < targetPreset->subFixtureValues.items.size(); i++) {
            PresetSubFixtureValues* psfv = targetPreset->subFixtureValues.items[i];
            Fixture* f = Brain::getInstance()->getFixtureById(psfv->targetFixtureId->intValue());
            if (f != nullptr && dynamic_cast<FixtureType*>(f->devTypeParam->targetContainer.get()) == fixtType) {
                toDelete.add(psfv);
            }
        }
        targetPreset->subFixtureValues.removeItems(toDelete);

        for (int i = 0; i < values.items.size(); i++) {
            FixtureTypePresetValues* ftpv = values.items[i];
            PresetSubFixtureValues* psfv = targetPreset->subFixtureValues.addItem();
            psfv->values.clear();
            //psfv->targetFixtureId->setValue(fixt->id->intValue());
            psfv->targetSubFixtureId->setValue(ftpv->subFixtureId->intValue());
            for (int j = 0; j < ftpv->values.items.size(); j++) {
                psfv->values.addItemFromData(ftpv->values.items[j]->getJSONData(), false);
            }
        }
    }

}

void FixtureTypePreset::onContainerParameterChangedInternal(Parameter* p) {
}

void FixtureTypePreset::triggerTriggered(Trigger* t)
{
    if (t == importFromPresetGridBtn) {importFromPresetGrid(); }
    if (t == exportInPresetGridBtn) {exportInPresetGrid(true);}
}

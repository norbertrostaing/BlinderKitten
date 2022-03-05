/*
  ==============================================================================

    Assistant.cpp
    Created: 29 Jan 2019 3:52:46pm
    Author:  no

  ==============================================================================
*/
#include "JuceHeader.h"
#include "Assistant.h"
#include "Brain.h"
#include "Definitions/Group/Group.h"
#include "Definitions/Cuelist/CuelistManager.h"
#include "Definitions/Cuelist/Cuelist.h"
#include "Definitions/Cue/Cue.h"
#include "Definitions/Preset/Preset.h"
#include "Definitions/TimingPreset/TimingPreset.h"
#include "Definitions/Command/CommandValueManager.h";
#include "Definitions/Command/CommandValue.h";

juce_ImplementSingleton(Assistant)

Assistant::Assistant() :
	BaseItem("Offline Lighting General Assistant"),
    paletteMakerCC("Palette maker"),
    masterMakerCC("Masters maker"),
    Thread("Assistant")
{
    updateDisplay(); 

	paletteGroupId = paletteMakerCC.addIntParameter("Group ID", "", 1,0);
	paletteFirstPresetId = paletteMakerCC.addIntParameter("First Preset ID", "", 1,0);
	paletteLastPresetId = paletteMakerCC.addIntParameter("Last Preset ID", "", 0,0);
	paletteTimingPresetId = paletteMakerCC.addIntParameter("Timing Preset ID", "0 means none", 0,0);
    paletteBtn = paletteMakerCC.addTrigger("Create Palette", "create a new cuelist with selected group and presets");
    addChildControllableContainer(&paletteMakerCC);


    paletteGroupId = paletteMakerCC.addIntParameter("Group ID", "", 1, 0);
    masterFirstGroupId = masterMakerCC.addIntParameter("First group ID", "", 1, 1);
    masterLastGroupId = masterMakerCC.addIntParameter("Last group ID", "", 1, 1);
    masterMakerCC.addChildControllableContainer(&masterValue);
    masterBtn = masterMakerCC.addTrigger("Create Masters", "create a master cuelist for each group with these values");
    addChildControllableContainer(&masterMakerCC);


}

void Assistant::updateDisplay() {
    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void Assistant::onContainerParameterChangedInternal(Parameter* p) {
    // updateDisplay();
}

Assistant::~Assistant()
{
}

void Assistant::run()
{
    if (pleaseCreatePalette) {
        pleaseCreatePalette = false;
        createPalette();
    }
    if (pleaseCreateMasters) {
        pleaseCreateMasters = false;
        createMasters();
    }
}

void Assistant::triggerTriggered(Trigger* t) {
}

void Assistant::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
    if ((Trigger*)c == paletteBtn) {
        pleaseCreatePalette = true;
        startThread(1);
    }
    if ((Trigger*)c == masterBtn) {
        pleaseCreateMasters = true;
        startThread(1);
    }
}

void Assistant::createPalette()
{
    LOG("Start creating palette, please wait...");
    int groupId = paletteGroupId->getValue();
    int presetFrom = paletteFirstPresetId->getValue();
    int presetTo = paletteLastPresetId->getValue();
    int timePreset = paletteTimingPresetId->getValue();
    if (groupId == 0) { LOGERROR("you must type a valid group id";); return; }
    if (presetFrom == 0) { LOGERROR("you must type a valid first preset id";); return; }
    if (presetTo == 0) { LOGERROR("you must type a valid lat preset id";); return; }
    if (presetFrom == presetTo) { LOGERROR("you must specify different preset ids";); return; }

    int delta = presetFrom <= presetTo ? 1 : -1;
    bool valid = false;

    for (int i = presetFrom; i != presetTo + delta; i += delta) {
        if (Brain::getInstance()->getPresetById(i) != nullptr) {
            valid = true;
        }
    }
    if (!valid) {
        LOG("there is no preset in the given range");
        return;
    }

    String cuelistName = "Palette Group " + String(groupId);
    Group* g = Brain::getInstance()->getGroupById(groupId);
    if (g != nullptr) {
        cuelistName = "Palette " + g->userName->getValue().toString();
    }
    Cuelist* cl = CuelistManager::getInstance()->addItem();
    cl->userName->setValue(cuelistName);
    cl->cues.clear();
    cl->deselectThis();
    // supprimer premier cue
    for (int i = presetFrom; i != presetTo + delta; i += delta) {
        Preset* p = Brain::getInstance()->getPresetById(i);
        if (p != nullptr) {
            String name = p->userName->getValue().toString();
            Cue* c = cl->cues.addItem();
            c->setNiceName(name);
            c->commands.items[0]->selection.items[0]->targetType->setValueWithKey("Group");
            c->commands.items[0]->selection.items[0]->valueFrom->setValue(groupId);
            c->commands.items[0]->values.items[0]->presetOrValue->setValueWithKey("Preset");
            c->commands.items[0]->values.items[0]->presetIdFrom->setValue(i);
            if (timePreset != 0) {
                c->commands.items[0]->timing.presetOrValue->setValueWithKey("Preset");
                c->commands.items[0]->timing.presetId->setValue(timePreset);
            }
            LOG("Cue " + name + " Created");
            wait(100);
        }
    }

    cl->selectThis();
    LOG("Palette created :)");


    // create HERE;

}


void Assistant::createMasters()
{
    LOG("Start creating Masters, please wait...");
    int firstGroupId = masterFirstGroupId->getValue();
    int lastGroupId = masterLastGroupId->getValue();
    if (firstGroupId == 0) { LOGERROR("you must type a valid first group id";); return; }
    if (lastGroupId == 0) { LOGERROR("you must type a valid last group id";); return; }
    if (firstGroupId == lastGroupId) {LOGERROR("first and last group ID must be differents"); return;}

    int delta = firstGroupId <= lastGroupId ? 1 : -1;
    bool valid = false;

    for (int i = firstGroupId; i != lastGroupId+ delta; i += delta) {
        if (Brain::getInstance()->getGroupById(i) != nullptr) {
            valid = true;
        }
    }
    if (!valid) {
        LOG("there is no group in the given range");
        return;
    }

    for (int i = firstGroupId; i != lastGroupId + delta; i += delta) {

        Group* g = Brain::getInstance()->getGroupById(i);
        if (g != nullptr) {
            Cuelist* cl = CuelistManager::getInstance()->addItem();
            cl->userName->setValue("Master "+g->userName->getValue().toString());
            cl->deselectThis();
            Cue* c = cl->cues.items[0];
            c->setNiceName("Master");
            c->commands.items[0]->selection.items[0]->targetType->setValueWithKey("Group");
            c->commands.items[0]->selection.items[0]->valueFrom->setValue(i);
            c->commands.items[0]->values.loadJSONData(masterValue.getJSONData());
            LOG("Master" + g->userName->getValue().toString() + " Created");
            wait(100);
        }

    }

    LOG("Masters created :)");

}



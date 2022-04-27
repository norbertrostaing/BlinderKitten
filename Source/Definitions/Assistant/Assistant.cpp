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
#include "Definitions/Group/GroupManager.h"
#include "Definitions/Group/Group.h"
#include "Definitions/Cuelist/CuelistManager.h"
#include "Definitions/Cuelist/Cuelist.h"
#include "Definitions/Cue/Cue.h"
#include "Definitions/Preset/Preset.h"
#include "Definitions/TimingPreset/TimingPreset.h"
#include "Definitions/Command/CommandValueManager.h"
#include "Definitions/Command/CommandValue.h"
#include "Definitions/Interface/InterfaceManager.h"
#include "Definitions/Interface/InterfaceIncludes.h"
#include "Definitions/FixtureType/FixtureTypeManager.h"
#include "Definitions/Fixture/FixtureManager.h"
#include "Definitions/Fixture/Fixture.h"
#include "Definitions/Actions/CuelistAction.h"
#include "Definitions/Actions/EffectAction.h"
#include "Definitions/Actions/CarouselAction.h"

juce_ImplementSingleton(Assistant)


Assistant::Assistant() :
	BaseItem("Offline Lighting General Assistant"),
    patcherCC("Patch Helper"),
    paletteMakerCC("Palette maker"),
    masterMakerCC("Masters maker"),
    midiMapperCC("Midi mappings"),
    Thread("Assistant")
{
    updateDisplay(); 

    patcherFixtureType = patcherCC.addTargetParameter("Fixture type", "", FixtureTypeManager::getInstance());
    patcherFixtureType->targetType = TargetParameter::CONTAINER;
    patcherFixtureType->maxDefaultSearchLevel = 0;
    patcherAmount = patcherCC.addIntParameter("Amount", "",1,1);
    patcherName = patcherCC.addStringParameter("Fixture name", "", "");
    patcherFirstId = patcherCC.addIntParameter("First ID", "", 1, 1);
    patcherInterface = patcherCC.addTargetParameter("Interface", "", InterfaceManager::getInstance());
    patcherInterface->targetType = TargetParameter::CONTAINER;
    patcherInterface->maxDefaultSearchLevel = 0;
    patcherFirstAddress = patcherCC.addIntParameter("Start Address", "",1);
    patcherAddressInterval = patcherCC.addIntParameter("Address Interval", "",1);
    patcherBtn = patcherCC.addTrigger("Add Fixtures", "Add desired fixtures");
    addChildControllableContainer(&patcherCC);

	paletteGroupId = paletteMakerCC.addIntParameter("Group ID", "", 1,0);
	paletteFirstPresetId = paletteMakerCC.addIntParameter("First Preset ID", "", 1,0);
	paletteLastPresetId = paletteMakerCC.addIntParameter("Last Preset ID", "", 0,0);
	paletteTimingPresetId = paletteMakerCC.addIntParameter("Timing Preset ID", "0 means none", 0,0);
    paletteBtn = paletteMakerCC.addTrigger("Create Palette", "create a new cuelist with selected group and presets");
    addChildControllableContainer(&paletteMakerCC);

    masterFirstGroupId = masterMakerCC.addIntParameter("First group ID", "", 1, 1);
    masterLastGroupId = masterMakerCC.addIntParameter("Last group ID", "", 1, 1);
    masterMakerCC.addChildControllableContainer(&masterValue);
    masterBtn = masterMakerCC.addTrigger("Create Masters", "create a master cuelist for each group with these values");
    addChildControllableContainer(&masterMakerCC);
    
    midiMapperTargetInterface = midiMapperCC.addTargetParameter("Midi interface", "Midi interface to connect your new mapping, let empty to create a new one", InterfaceManager::getInstance());
    midiMapperTargetInterface->maxDefaultSearchLevel = 0;
    midiMapperTargetInterface->targetType = TargetParameter::CONTAINER;
    // midiMapperTargetInterface->typesFilter.add("MIDIInterface");
    addChildControllableContainer(&midiMapperCC);
    midiMapperTargetType = midiMapperCC.addEnumParameter("Target Type","Type of the midim mapping target");
    midiMapperTargetType->addOption("Cuelist", "Cuelist");
    midiMapperTargetType->addOption("Effect", "Effect");
    midiMapperTargetType->addOption("Carousel", "Carousel");
    //midiMapperTargetType->addOption("Input Panel", "Input Panel");
    //midiMapperTargetType->addOption("Virtual buttons", "Virtual Buttons");
    //midiMapperTargetType->addOption("Virtual faders", "Virtual Faders");
    midiMapperTargetId = midiMapperCC.addIntParameter("Target ID","",0,0);
    midiMapperPageNumber = midiMapperCC.addIntParameter("Page number","0 means current page",0,0);
    midiMapperBtn = midiMapperCC.addTrigger("Create Mappings", "Create mappings for desired target");

    updateDisplay();
}

void Assistant::updateDisplay() {
    if (midiMapperTargetType != nullptr) {
        String midiType = midiMapperTargetType->getValue();
        if (midiType == "virtualButtons" || midiType == "virtualFaders") {
            midiMapperTargetId->hideInEditor = true;
            midiMapperPageNumber->hideInEditor = false;
        }
        else if (midiType == "inputPanel" ) {
            midiMapperTargetId->hideInEditor = true;
            midiMapperPageNumber->hideInEditor = true;
        }
        else {
            midiMapperTargetId->hideInEditor = false;
            midiMapperPageNumber->hideInEditor = true;
        }
    }   

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
    if (pleasePatchFixtures) {
        pleasePatchFixtures = false;
        patchFixtures();
    }
    if (pleaseCreatePalette) {
        pleaseCreatePalette = false;
        createPalette();
    }
    if (pleaseCreateMasters) {
        pleaseCreateMasters = false;
        createMasters();
    }
    if (pleaseCreateMidiMappings) {
        pleaseCreateMidiMappings = false;
        createMidiMappings();
    }
}

void Assistant::triggerTriggered(Trigger* t) {
}

void Assistant::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
    if ((Trigger*)c == patcherBtn) {
        pleasePatchFixtures = true;
        startThread(1);
    }
    else if ((Trigger*)c == paletteBtn) {
        pleaseCreatePalette = true;
        startThread(1);
    }
    else if ((Trigger*)c == masterBtn) {
        pleaseCreateMasters = true;
        startThread(1);
    }
    else if ((Trigger*)c == midiMapperBtn) {
        pleaseCreateMidiMappings = true;
        startThread(1);
    }
    else if (c == midiMapperTargetType) {
        updateDisplay();
    }
}

void Assistant::patchFixtures()
{
    FixtureType* fixtureType = dynamic_cast<FixtureType*>(patcherFixtureType->targetContainer.get());
    int amount = patcherAmount->getValue();
    String name = patcherName->getValue();
    int firstId= patcherFirstId->getValue();
    Interface* targetInterface = dynamic_cast<Interface*>(patcherInterface->targetContainer.get());
    int firstAddress = patcherFirstAddress->getValue();
    int addressInterval = patcherAddressInterval->getValue();

    if (fixtureType == nullptr) {LOGERROR("You must provide me a valid fixture type"); return;}
    if (name == "") { name = fixtureType->niceName; }

    LOG("Patching your fixtures, please wait");
    int currentAdress = firstAddress;

    for (int i = 0; i < amount; i++) {
        Fixture* f = FixtureManager::getInstance()->addItem();
        f-> id->setValue(firstId+i);
        if (amount > 1) {
            f->userName->setValue(name + " " +String(i+1));
        }
        else {
            f->userName->setValue(name);
        }
        f->devTypeParam->setValueFromTarget(fixtureType);
        if (targetInterface != nullptr) {
            FixturePatch* p = f->patchs.addItem();
            p->targetInterface->setValueFromTarget(targetInterface);
            p->address->setValue(currentAdress);
            int delta = addressInterval;

            for (int cn = 0; cn < fixtureType->chansManager.items.size(); cn++) {
                int chanSize = fixtureType->chansManager.items[cn]->resolution->getValue().toString() == "16bits" ? 1 : 0;
                delta = jmax(delta, (int)fixtureType->chansManager.items[cn]->dmxDelta->getValue() + chanSize);
            }
            currentAdress += delta;
        }
    }

    if (amount > 1) {
        Group* g = GroupManager::getInstance()->addItem();
        g->userName->setValue(name);
        g->selection.items[0]->valueFrom->setValue(firstId);
        g->selection.items[0]->thru->setValue(true);
        g->selection.items[0]->valueTo->setValue(firstId+amount-1);
    }
}

void Assistant::createPalette()
{
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

    LOG("Start creating palette, please wait...");
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
            cl->autoStart->setValue(true);
            cl->autoStop->setValue(true);
            cl->HTPLevel->setValue(0);
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

void Assistant::createMidiMappings()
{
    MIDIInterface* targetInterface = dynamic_cast<MIDIInterface*>(midiMapperTargetInterface->targetContainer.get());
    bool changeInterfaceName = false;

    if (targetInterface == nullptr || targetInterface->getTypeString()!="MIDI") {
    changeInterfaceName = true;
        targetInterface = new MIDIInterface();
        InterfaceManager::getInstance()->addItem(targetInterface);
        targetInterface->setNiceName("Auto mapped midi");
    }
    
    String targetType = midiMapperTargetType->getValue();
    if (targetType == "Cuelist") {
        int targetId = midiMapperTargetId->getValue();
        if (targetId == 0) { return; }

        for (int i = 0; i < ActionFactory::getInstance()->defs.size(); i++) {
            FactorySimpleParametricDefinition<CuelistAction>* t = (FactorySimpleParametricDefinition<CuelistAction>*)ActionFactory::getInstance()->defs[i];
            if (t->menuPath == targetType) {
                MIDIMapping* m = targetInterface->mappingManager.addItem();
                m->enabled->setValue(false);
                m->setNiceName(targetType +" " + String(targetId) + " " + String(t->type));
                DynamicObject* obj = new DynamicObject();
                obj->setProperty("type", t->type);
                obj->setProperty("actionType", (EffectAction::ActionType)(int)t->params.getProperty("actionType", ""));
                CuelistAction* a = CuelistAction::create(var(obj));
                a->cuelistId->setValue(targetId);
                a->setNiceName(t->type);
                m->actionManager.addItem(a);
                m->editorIsCollapsed = true;
            }
        }
    }
    else if (targetType == "Effect") {
        int targetId = midiMapperTargetId->getValue();
        if (targetId == 0) { return; }

        for (int i = 0; i < ActionFactory::getInstance()->defs.size(); i++) {
            FactorySimpleParametricDefinition<EffectAction>* t = (FactorySimpleParametricDefinition<EffectAction>*)ActionFactory::getInstance()->defs[i];
            if (t->menuPath == targetType) {
                MIDIMapping* m = targetInterface->mappingManager.addItem();
                m->setNiceName(targetType +" " + String(targetId) + " " + String(t->type));
                DynamicObject* obj = new DynamicObject();
                obj->setProperty("type", t->type);
                obj->setProperty("actionType", (EffectAction::ActionType)(int)t->params.getProperty("actionType", ""));
                EffectAction* a = EffectAction::create(var(obj));
                a->targetId->setValue(targetId);
                a->setNiceName(t->type);
                m->actionManager.addItem(a);
                m->editorIsCollapsed = true;
            }
        }
    }
    else if (targetType == "Carousel") {
        int targetId = midiMapperTargetId->getValue();
        if (targetId == 0) { return; }

        for (int i = 0; i < ActionFactory::getInstance()->defs.size(); i++) {
            FactorySimpleParametricDefinition<CarouselAction>* t = (FactorySimpleParametricDefinition<CarouselAction>*)ActionFactory::getInstance()->defs[i];
            if (t->menuPath == targetType) {
                MIDIMapping* m = targetInterface->mappingManager.addItem();
                m->setNiceName(targetType +" " + String(targetId) + " " + String(t->type));
                DynamicObject* obj = new DynamicObject();
                obj->setProperty("type", t->type);
                obj->setProperty("actionType", (CarouselAction::ActionType)(int)t->params.getProperty("actionType", ""));
                CarouselAction* a = CarouselAction::create(var(obj));
                a->targetId->setValue(targetId);
                a->setNiceName(t->type);
                m->actionManager.addItem(a);
                m->editorIsCollapsed = true;
            }
        }
    }
    else if(targetType == "Input Panel") {

    }
    targetInterface->selectThis();
}



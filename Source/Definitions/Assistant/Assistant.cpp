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
#include "Definitions/Preset/PresetManager.h"
#include "Definitions/TimingPreset/TimingPresetManager.h"
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
#include "Definitions/ChannelFamily/ChannelFamilyManager.h"
#include "Definitions/Effect/Effect.h"
#include "Definitions/Carousel/Carousel.h"
#include "BKEngine.h"

juce_ImplementSingleton(Assistant)

Assistant::Assistant() :
	BaseItem("Offline Lighting General Assistant"),
    patcherCC("Patch Helper"),
    paletteMakerCC("Palette maker"),
    masterMakerCC("Masters maker"),
    soloPaletteMakerCC("SoloPool palette maker"),
    fixtureSwapperCC("Fixture Type Swapper"),
    midiMapperCC("Midi mappings"),
    asciiCC("ASCII import / export"),
    controlsCC("Generic controls"),
    Thread("Assistant"),
    masterValue("CommandValue")
{
    updateDisplay(); 

    patcherFixtureType = patcherCC.addTargetParameter("Fixture type", "", FixtureTypeManager::getInstance());
    patcherFixtureType->targetType = TargetParameter::CONTAINER;
    patcherFixtureType->maxDefaultSearchLevel = 0;
    patcherAmount = patcherCC.addIntParameter("Amount", "",1,1);
    patcherName = patcherCC.addStringParameter("Fixture name", "", "");
    patcherFirstId = patcherCC.addIntParameter("First ID", "", 1, 1);
    patcherMakeGroup = patcherCC.addBoolParameter("Create Group", "Create a group with added fixtures", true);
    patcherInterface = patcherCC.addTargetParameter("Interface", "", InterfaceManager::getInstance());
    patcherInterface->targetType = TargetParameter::CONTAINER;
    patcherInterface->customGetTargetContainerFunc = &InterfaceManager::showAndGetInterfaceOfType<DMXInterface>;
    patcherInterface->maxDefaultSearchLevel = 0;
    patcherFirstAddress = patcherCC.addIntParameter("Start Address", "",1);
    patcherAddressInterval = patcherCC.addIntParameter("Address Interval", "",1);
    patcherBtn = patcherCC.addTrigger("Add Fixtures", "Add desired fixtures");
    addChildControllableContainer(&patcherCC);

	paletteGroupId = paletteMakerCC.addIntParameter("Group ID", "", 1,0);
	paletteFirstPresetId = paletteMakerCC.addIntParameter("First Preset ID", "", 1,0);
	paletteLastPresetId = paletteMakerCC.addIntParameter("Last Preset ID", "", 0,0);
	paletteTimingPresetId = paletteMakerCC.addIntParameter("Timing Preset ID", "0 means none", 0,0);
    paletteCuelistId = paletteMakerCC.addIntParameter("Palette ID", "Id for your palette, if already taken, search for a number above. Leave at 0 for auto number.", 0,0);
    paletteName = paletteMakerCC.addStringParameter("Palette Name", "Name your palette here, leave empty to have an auto name", "");
    paletteKeepEmpty = paletteMakerCC.addBoolParameter("Keep unused presets", "If checked, all presets will have a cue, even if the preset has no target in the group", false);
    paletteBtn = paletteMakerCC.addTrigger("Create Palette", "create a new cuelist with selected group and presets");
    addChildControllableContainer(&paletteMakerCC);

    masterFirstGroupId = masterMakerCC.addIntParameter("First group ID", "", 1, 1);
    masterLastGroupId = masterMakerCC.addIntParameter("Last group ID", "", 1, 1);
    masterMakerCC.addChildControllableContainer(&masterValue);
    masterBtn = masterMakerCC.addTrigger("Create Masters", "create a master cuelist for each group with these values");
    addChildControllableContainer(&masterMakerCC);

    soloPalettePoolId = soloPaletteMakerCC.addIntParameter("Pool ID", "", 1, 0);
    soloPaletteCuelistId = soloPaletteMakerCC.addIntParameter("Palette ID", "Id for your soloPalette, if already taken, search for a number above. Leave at 0 for auto number.", 0, 0);
    soloPaletteName = soloPaletteMakerCC.addStringParameter("Palette Name", "Name your soloPalette here, leave empty to have an auto name", "");
    soloPaletteBtn = soloPaletteMakerCC.addTrigger("Create Palette", "Create a new cuelist with selected soloPool");
    addChildControllableContainer(&soloPaletteMakerCC);

    swapperOld = fixtureSwapperCC.addTargetParameter("Replace fixtures", "", FixtureTypeManager::getInstance());
    swapperOld->targetType = TargetParameter::CONTAINER;
    swapperOld->maxDefaultSearchLevel = 0;
    swapperNew = fixtureSwapperCC.addTargetParameter("With this", "", FixtureTypeManager::getInstance());
    swapperNew->targetType = TargetParameter::CONTAINER;
    swapperNew->maxDefaultSearchLevel = 0;
    swapperBtn = fixtureSwapperCC.addTrigger("Change fixtures", "Replace all fixtures of first type with the second one");
    addChildControllableContainer(&fixtureSwapperCC);

    userCanRemove = false;
    /*
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
    */

    addChildControllableContainer(&asciiCC);
    asciiPatch = asciiCC.addBoolParameter("Patchs", "Do you want to import or export patch ?", true);
    asciiGroups = asciiCC.addBoolParameter("Groups", "Do you want to import or export groups ?", true);
    asciiGroupValuesAsPreset = asciiCC.addBoolParameter("Groups as presets", "If checked, the values stored in GROUP commands will be saved as new presets", true);
    asciiCues = asciiCC.addBoolParameter("Cues", "Do you want to import or export cues ?", true);
    asciiSubs = asciiCC.addBoolParameter("Subs", "Do you want to import or export subs ?", true);
    asciiRespectCueNumbers = asciiCC.addBoolParameter("Respect cue number", "If checked, the cues will be ordered by cue ID, if not, they will be ordered by step number", true);
    asciiEraseCuelist = asciiCC.addBoolParameter("Erase main cuelist", "If not checked, the current cuelist will be updated with ascii values", false);
    asciiChannelFixtureType = asciiCC.addTargetParameter("Channel Fixture type", "Fixture used for ascii import/export", FixtureTypeManager::getInstance());
    asciiChannelFixtureType->targetType = TargetParameter::CONTAINER;
    asciiChannelFixtureType->maxDefaultSearchLevel = 0;
    asciiChannelFixtureType->typesFilter.add("FixtureType");
    asciiDimmerChannel = asciiCC.addTargetParameter("Intensity Channel", "Channel used as intensity for ascii import/export", ChannelFamilyManager::getInstance());
    asciiDimmerChannel->targetType = TargetParameter::CONTAINER;
    asciiDimmerChannel->maxDefaultSearchLevel = 2;
    asciiDimmerChannel->typesFilter.add("ChannelType");

    asciiCuelistId = asciiCC.addIntParameter("Main Sequence ID", "Sequence to export or to import in", 1, 1);
    importAsciiBtn = asciiCC.addTrigger("Import", "Click to import a file");
    //exportAsciiBtn = asciiCC.addTrigger("Export", "Click to export selected cuelist in file");

    addChildControllableContainer(&controlsCC);
    offCuelistsBtn = controlsCC.addTrigger("Off all cuelists", "");
    killCuelistsBtn = controlsCC.addTrigger("Kill all cuelists", "");
    stopEffectsBtn = controlsCC.addTrigger("Stop all effects", "");
    stopCarouselsBtn = controlsCC.addTrigger("Stop all carousels", "");
    randomSeed = controlsCC.addIntParameter("Random seed", "", 0, 0);
    resetRandomBtn = controlsCC.addTrigger("Reset random seed", "");
    loadRunningCuelistsBtn = controlsCC.addTrigger("Load all cuelists", "");

    updateDisplay();
}

void Assistant::updateDisplay() {
    if (midiMapperTargetType != nullptr) {
        String midiType = midiMapperTargetType->stringValue();
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
    if (pleaseSwapFixtures) {
        pleaseSwapFixtures = false;
        swapFixtures();
    }
    if (pleaseCreateMidiMappings) {
        pleaseCreateMidiMappings = false;
        createMidiMappings();
    }
    if (pleaseImportAscii) {
        pleaseImportAscii= false;
    }
}

void Assistant::triggerTriggered(Trigger* t) {
}

void Assistant::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
    if ((Trigger*)c == patcherBtn) {
        pleasePatchFixtures = true;
        startThread();
    }
    else if ((Trigger*)c == paletteBtn) {
        pleaseCreatePalette = true;
        startThread();
    }
    else if ((Trigger*)c == soloPaletteBtn) {
        MessageManager::callAsync([this]() {createSoloPalette(); });
    }
    else if ((Trigger*)c == masterBtn) {
        pleaseCreateMasters = true;
        startThread();
    }
    else if ((Trigger*)c == swapperBtn) {
        pleaseSwapFixtures = true;
        startThread();
    }
    else if ((Trigger*)c == midiMapperBtn) {
        pleaseCreateMidiMappings = true;
        startThread();
    }
    else if (c == midiMapperTargetType) {
        updateDisplay();
    }
    else if (c == importAsciiBtn) {
        MessageManager::callAsync([this]() {importAscii(); });
    }
    else if (c == exportAsciiBtn) {
        exportAscii();
    }
    else if (c == offCuelistsBtn) {
        Brain::getInstance()->offAllCuelists();
    }
    else if (c == killCuelistsBtn) {
        Brain::getInstance()->killAllCuelists();
    }
    else if (c == stopEffectsBtn) {
        Brain::getInstance()->stopAllEffects();
    }
    else if (c == stopCarouselsBtn) {
        Brain::getInstance()->stopAllCarousels();
    }
    else if (c == resetRandomBtn) {
        Brain::getInstance()->resetRandomSeed(randomSeed->intValue());
    }
    else if (c == loadRunningCuelistsBtn) {
        Brain::getInstance()->loadRunningCuelistsInProgrammer();
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

    Array<int> usedIds;
    for (int i = 0; i < amount; i++) {
        Fixture* test = Brain::getInstance()->getFixtureById(firstId + i);
        if (test != nullptr) {
            usedIds.add(firstId + i);
        }
    }

    if (usedIds.size() > 0) {
        String msg = "";
        for (int i = 0; i < usedIds.size() - 1; i++) {
            msg+= String(usedIds[i])+", ";
        }
        msg += String(usedIds[usedIds.size()-1]);
        if (usedIds.size() == 1) {
            msg = "there is already a fixture with ID "+msg;
        }
        else {
            msg = "The following IDs are already taken : " + msg;
        }

        LOGERROR(msg);
        return;
    }


    LOG("Patching your fixtures, please wait");
    Brain::getInstance()->usingCollections.enter();
    int currentAdress = firstAddress;

    Array<Fixture*> newFixts;
    bool continuePatch = true;
    for (int i = 0; i < amount && continuePatch; i++) {
        const MessageManagerLock mmLock;
        Fixture* f = new Fixture();
        newFixts.add(f);
        f->id->setValue(firstId+i);
        if (amount > 1) {
            f->userName->setValue(name + " " +String(i+1));
        }
        else {
            f->userName->setValue(name);
        }
        f->devTypeParam->setValueFromTarget(fixtureType);
        if (targetInterface != nullptr && firstAddress > 0) {
            FixturePatch* p = f->patchs.addItem();
            p->targetInterface->setValueFromTarget(targetInterface);
            p->address->setValue(currentAdress);
            int delta = abs(addressInterval);

            for (int cn = 0; cn < fixtureType->chansManager.items.size(); cn++) {
                int chanSize = fixtureType->chansManager.items[cn]->resolution->getValue().toString() == "16bits" ? 1 : 0;
                delta = jmax(delta, (int)fixtureType->chansManager.items[cn]->dmxDelta->getValue() + chanSize);

            }

            if (addressInterval >= 0) currentAdress += delta;
            else currentAdress -= delta;
            if (currentAdress>512||currentAdress<1) continuePatch = false;
        }
    }

    FixtureManager::getInstance()->addItems(newFixts);

    if (amount > 0 && patcherMakeGroup->boolValue()) {
        Group* g = GroupManager::getInstance()->addItem();
        g->userName->setValue(name);
        g->selection.items[0]->valueFrom->setValue(firstId);
        if (amount > 1) {
            g->selection.items[0]->thru->setValue(true);
            g->selection.items[0]->valueTo->setValue(firstId + amount - 1);
        }
    }
    Brain::getInstance()->usingCollections.exit();
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
    int askedId = paletteCuelistId->intValue();
    if (askedId > cl->id->intValue()) {
        while (Brain::getInstance()->getCuelistById(askedId) != nullptr) {
            askedId++;
        }
        cl->id->setValue(askedId);
    }

    if (paletteName->stringValue() != "") {
        cuelistName = paletteName->stringValue();
    }
    cl->userName->setValue(cuelistName);
    cl->cues.clear();
    cl->deselectThis();

    if (timePreset != 0) {
        cl->timing.presetOrValue->setValueWithKey("Preset");
        cl->timing.presetId->setValue(timePreset);
    }

    Array<Cue*> cuesToAdd;

    bool nextIsNewLine = false;
    for (int i = presetFrom; i != presetTo + delta; i += delta) {
        const MessageManagerLock mmLock;
        Preset* p = Brain::getInstance()->getPresetById(i);
        if (p != nullptr) {
            String name = p->userName->getValue().toString();
            Cue* c = new Cue();
            cuesToAdd.add(c);
            c->editorIsCollapsed = true;
            c->parentCuelist = cl;

            c->setNiceName(name);
            c->loadWindowBreakLine->setValue(nextIsNewLine);
            nextIsNewLine = false;
            c->id->setValue(i);
            c->commands.items[0]->selection.items[0]->targetType->setValueWithKey("Group");
            c->commands.items[0]->selection.items[0]->valueFrom->setValue(groupId);
            c->commands.items[0]->values.items[0]->presetOrValue->setValueWithKey("Preset");
            c->commands.items[0]->values.items[0]->presetIdFrom->setValue(i);
            //if (timePreset != 0) {
            //    c->commands.items[0]->timing.presetOrValue->setValueWithKey("Preset");
            //    c->commands.items[0]->timing.presetId->setValue(timePreset);
            //}

            bool log = true;
            if (!paletteKeepEmpty->boolValue()) {
                c->computeValues();
                if (c->computedValues.size() == 0) {
                    log = false;
                    cuesToAdd.removeAllInstancesOf(c);
                    delete c;
                }
            }
            if (log) {
                LOG("Cue " + name + " Created");
            }

            //wait(10);
        }
        else {
            nextIsNewLine = true;
        }
    }

    cl->cues.addItems(cuesToAdd);

    if (timePreset > 0) {
        TimingPreset* tp = Brain::getInstance()->getTimingPresetById(timePreset);
        if (tp == nullptr) {
            tp = TimingPresetManager::getInstance()->addItem();
            tp->id->setValue(timePreset);
        }
    }
    
    cl->selectThis();
    LOG("Palette created :)");


}


void Assistant::createMasters()
{
    const MessageManagerLock mmLock;
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
    const MessageManagerLock mmLock;
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

void Assistant::swapFixtures()
{
    LOG("Swapping fixture types");
    String oldVal = swapperOld->stringValue();
    FixtureType* newVal = dynamic_cast<FixtureType*>(swapperNew->targetContainer.get());
    if (newVal != nullptr) {
        int count = 0;
        for (int i = 0; i < FixtureManager::getInstance()->items.size(); i++) {
            Fixture* f = FixtureManager::getInstance()->items[i];
            if (f->devTypeParam->stringValue() == oldVal) {
                f->devTypeParam->setValueFromTarget(newVal);
                count++;
            }
        }
        LOG(String(count)+" fixtures changed.");
    }
    else {
        LOGWARNING("You must specify a new fixture type");
    }

}

void Assistant::createSoloPalette()
{

    int poolId = soloPalettePoolId->intValue();
    if (poolId == 0) { LOGERROR("you must type a valid group id";); return; }

    LOG("Start creating palette, please wait...");

    String cuelistName = "Palette SoloPool " + String(poolId);

    Cuelist* cl = CuelistManager::getInstance()->addItem();
    int askedId = soloPaletteCuelistId->intValue();
    if (askedId > cl->id->intValue()) {
        while (Brain::getInstance()->getCuelistById(askedId) != nullptr) {
            askedId++;
        }
        cl->id->setValue(askedId);
    }

    if (soloPaletteName->stringValue() != "") {
        cuelistName = soloPaletteName->stringValue();
    }
    cl->userName->setValue(cuelistName);
    cl->cues.clear();
    cl->deselectThis();
    // supprimer premier cue
    Brain::getInstance()->usingCollections.enter();
    int id = 1;
    bool nextIsNewLine = false;
    Array<Cue*> cuesToAdd;

    for (auto it = Brain::getInstance()->cuelists.begin(); it != Brain::getInstance()->cuelists.end(); it.next()) {
        Cuelist* target = it.getValue();
        if (target->soloPool->intValue() == poolId) {
            const MessageManagerLock mmLock;
            String name = target->userName->stringValue();
            Cue* c = new Cue();
            cuesToAdd.add(c);
            c->commands.clear();
            c->editorIsCollapsed = true;
            c->setNiceName(name);
            c->loadWindowBreakLine->setValue(nextIsNewLine);
            c->id->setValue(id);
            id++;
            nextIsNewLine = false;
            Task* t = c->tasks.addItem();
            t->targetType->setValueWithData("cuelist");
            t->targetId->setValue(target->id->intValue());
            t->cuelistAction->setValueWithData("go");
            Task* t2 = c->tasksOffCue.addItem();
            t2->targetType->setValueWithData("cuelist");
            t2->targetId->setValue(target->id->intValue());
            t2->cuelistAction->setValueWithData("off");
        }
    }

    nextIsNewLine = true;
    for (auto it = Brain::getInstance()->effects.begin(); it != Brain::getInstance()->effects.end(); it.next()) {
        Effect* target = it.getValue();
        if (target->soloPool->intValue() == poolId) {
            const MessageManagerLock mmLock;
            String name = target->userName->stringValue();
            Cue* c = new Cue();
            cuesToAdd.add(c);
            c->commands.clear();
            c->editorIsCollapsed = true;
            c->setNiceName(name);
            c->loadWindowBreakLine->setValue(nextIsNewLine);
            c->id->setValue(id);
            id++;
            nextIsNewLine = false;
            Task* t = c->tasks.addItem();
            t->targetType->setValueWithData("effect");
            t->targetId->setValue(target->id->intValue());
            t->effectAction->setValueWithData("start");
            Task* t2 = c->tasksOffCue.addItem();
            t2->targetType->setValueWithData("effect");
            t2->targetId->setValue(target->id->intValue());
            t2->effectAction->setValueWithData("stop");
        }
    }

    nextIsNewLine = true;
    for (auto it = Brain::getInstance()->carousels.begin(); it != Brain::getInstance()->carousels.end(); it.next()) {
        Carousel* target = it.getValue();
        if (target->soloPool->intValue() == poolId) {
            const MessageManagerLock mmLock;
            String name = target->userName->stringValue();
            Cue* c = new Cue();
            cuesToAdd.add(c);
            c->commands.clear();
            c->editorIsCollapsed = true;
            c->setNiceName(name);
            c->loadWindowBreakLine->setValue(nextIsNewLine);
            c->id->setValue(id);
            id++;
            nextIsNewLine = false;
            Task* t = c->tasks.addItem();
            t->targetType->setValueWithData("carousel");
            t->targetId->setValue(target->id->intValue());
            t->carouselAction->setValueWithData("start");
            Task* t2 = c->tasksOffCue.addItem();
            t2->targetType->setValueWithData("carousel");
            t2->targetId->setValue(target->id->intValue());
            t2->carouselAction->setValueWithData("stop");
        }
    }

    cl->cues.addItems(cuesToAdd);

    Brain::getInstance()->usingCollections.exit();

    cl->selectThis();
    LOG("Palette created :)");

}

void Assistant::importAscii()
{
    FileChooser fc("Import an ASCII file", File::getCurrentWorkingDirectory(), "*.asc");
    if (!fc.browseForFileToOpen()) return;


    File f = fc.getResult();
    String content = f.loadFileAsString();

    StringArray lines = StringArray::fromLines(content);

    String currentPrimary = "";
    String currentSecondary = "";

    int mainCuelistId = asciiCuelistId->getValue();
    LOG("importing your ascii... please wait :)");

    Array<DMXInterface*> universes = InterfaceManager::getInstance()->getItemsWithType<DMXInterface>();
    Cuelist* cuelist = nullptr;
    if (asciiCues) {
        const MessageManagerLock mmlock;
        cuelist = Brain::getInstance()->getCuelistById(mainCuelistId);
        if (cuelist == nullptr) {
            cuelist = CuelistManager::getInstance()->addItem();
            cuelist->userName->setValue("ASCII Cuelist");
        }
        cuelist->kill();
        if (asciiEraseCuelist->boolValue()) {
            cuelist->cues.clear();
        }
    }

    HashMap<int, Cue*> idToCue;
    if (!asciiEraseCuelist->boolValue()) {
        for (Cue* c : cuelist->cues.items) {
            idToCue.set(c->id->floatValue()*100000, c);
        }
    }

    Cue* currentCue = nullptr;
    Cue* previousCue = nullptr;
    Group* currentGroup = nullptr;
    Preset* currentPreset = nullptr;
    Cuelist* currentSub = nullptr;
    Cue* currentSubCue = nullptr;
    FixtureType* currentFixtureType = nullptr;
    Fixture* currentFixture = nullptr;

    Array<Cue*> cuesToAdd;
    Array<Cuelist*> cuelistsToAdd;
    Array<Group*> groupsToAdd;
    Array<Preset*> presetsToAdd;
    Array<Fixture*> fixturesToAdd;
    
    HashMap<int, String> intToChannelFam;
    intToChannelFam.set(1, "Dimmer");
    intToChannelFam.set(2, "Position");
    intToChannelFam.set(3, "Color");
    intToChannelFam.set(4, "Gobo");
    intToChannelFam.set(5, "Beam");
    intToChannelFam.set(6, "Focus");
    intToChannelFam.set(7, "Control");

    HashMap<String, String> newChannelTypeNames;
    newChannelTypeNames.set("Intens", "Intensity");

    HashMap<int, ChannelType*> idToChannelType;
    HashMap<int, FixtureType*> idToFixtureType;

    HashMap<String, Preset*> idToPreset;

    FixtureType* ft = dynamic_cast<FixtureType*>(asciiChannelFixtureType->targetContainer.get());
    for (int i = 0; i < lines.size(); i++) {
        String line = lines[i];
        const MessageManagerLock mmlock;
        String originalLine = line;
        line = line.replaceCharacters(" ,/;<=>@", "        ").trim();
        while (line.indexOf("  ") != -1) {
            line = line.replace("  ", " ");
        }
        if (line == "") {
            // empty line
        }
        else if (line.startsWith("!")) {
            // comment
        }
        else {
            StringArray words = StringArray::fromTokens(line.toUpperCase(), " ", "");
            StringArray wordsOriginal = StringArray::fromTokens(line, " ", "");
            //int currentWord = 1;
            if (words[0] == "ENDDATA") {
                //return;
            }
            else if (words[0] == "CLEAR" || words[0] == "CONSOLE" || words[0] == "IDENT" || words[0] == "MANUFACTURER" || words[0] == "PATCH" || words[0] == "SET")
            {
                currentPrimary = words[0];
                currentSecondary = words[0];
                // ajouter ici une verif si l'objet d'avant est clean (subs, groupes)
            }
            else if (words[0] == "CUE" || words[0] == "GROUP" || words[0] == "SUB") 
            {
                currentPrimary = words[0];
                currentSecondary = words[0];
            }
            else if (words[0] == "CHAN" || words[0] == "DOWN" || words[0] == "FOLLOWON" || words[0] == "LINK" || words[0] == "PART" || words[0] == "TEXT" || words[0] == "UP" || words[0] == "$$WAIT")
            {
                currentSecondary = words[0];
            }
            else if (words[0] == "$PERSONALITY")
            {
                currentPrimary = words[0];
                currentSecondary = words[0];
                currentFixtureType = FixtureTypeManager::getInstance()->addItem();
                idToFixtureType.set(words[1].getIntValue(), currentFixtureType);
            }
            else if (words[0] == "$PARAMTYPE")
            {
                String famName = intToChannelFam.contains(words[2].getIntValue()) ? intToChannelFam.getReference(words[2].getIntValue()) : "Control";
                ChannelFamily* fam = nullptr;
                for (ChannelFamily* f : ChannelFamilyManager::getInstance()->items) {
                    if (fam == nullptr && f->niceName == famName) {
                        fam = f;
                    }
                }
                if (fam == nullptr) {
                    fam = ChannelFamilyManager::getInstance()->addItem();
                    fam->setNiceName(famName);
                }

                String ctName = newChannelTypeNames.contains(wordsOriginal[3]) ? newChannelTypeNames.getReference(wordsOriginal[3]) : wordsOriginal[3];
                ChannelType* channelType = nullptr;
                for (ChannelType* ct : fam->definitions.items) {
                    if (channelType == nullptr && ct->niceName == ctName) {
                        channelType = ct;
                    }
                }

                if (channelType == nullptr) {
                    channelType = fam->definitions.addItem();
                    channelType->setNiceName(ctName);
                }
                idToChannelType.set(words[1].getIntValue(), channelType);

            }
            else if (words[0] == "$PATCH") {
                int id = words[1].getIntValue();
                int ftid = words[2].getIntValue();
                int address = words[3].getIntValue();
                int universe = address / 512;
                address = address % 512;
                Fixture * f = Brain::getInstance()->getFixtureById(id);
                if (f == nullptr) {
                    f = FixtureManager::getInstance()->addItem();
                    f->id->setValue(id);
                    if (idToFixtureType.contains(ftid)) {
                        f->devTypeParam->setValueFromTarget(idToFixtureType.getReference(ftid));
                    }
                }
                currentFixture = f;
                while (universes.size() <= universe) {
                    InterfaceManager::getInstance()->addItem(new DMXInterface());
                    universes = InterfaceManager::getInstance()->getItemsWithType<DMXInterface>();
                }
                DMXInterface* targetInterface = universes[universe];
                if (targetInterface->channelToFixturePatch[address] == nullptr) {
                    FixturePatch* p = f->patchs.addItem();
                    p->targetInterface->setTarget(targetInterface);
                    p->address->setValue(address);
                }
                currentPrimary = words[0];
                currentSecondary = words[0];
            }
            else if (words[0] == "$INTENSITYPALETTE") {
                if (idToPreset.contains("IP" + words[1])) {
                    currentPreset = idToPreset.getReference("IP" + words[1]);
                }
                else {
                    currentPreset = PresetManager::getInstance()->addItem();
                    currentPreset->subFixtureValues.clear();

                }
                currentPrimary = "$PALETTE";
                currentSecondary = "$PALETTE";
            }
            else if (words[0] == "$FOCUSPALETTE") {
                if (idToPreset.contains("FP" + words[1])) {
                    currentPreset = idToPreset.getReference("FP" + words[1]);
                }
                else {
                    currentPreset = PresetManager::getInstance()->addItem();
                    currentPreset->subFixtureValues.clear();

                }
                currentPrimary = "$PALETTE";
                currentSecondary = "$PALETTE";
            }
            else if (words[0] == "$COLORPALETTE") {
                if (idToPreset.contains("CP" + words[1])) {
                    currentPreset = idToPreset.getReference("CP" + words[1]);
                }
                else {
                    currentPreset = PresetManager::getInstance()->addItem();
                    currentPreset->subFixtureValues.clear();

                }
                currentPrimary = "$PALETTE";
                currentSecondary = "$PALETTE";
            }
            else if (words[0] == "$BEAMPALETTE") {
                if (idToPreset.contains("BP" + words[1])) {
                    currentPreset = idToPreset.getReference("BP" + words[1]);
                }
                else {
                    currentPreset = PresetManager::getInstance()->addItem();
                    currentPreset->subFixtureValues.clear();

                }
                currentPrimary = "$PALETTE";
                currentSecondary = "$PALETTE";
            }

            else if (words[0].startsWith("$$"))
            {
                currentSecondary = words[0];
            }
            else if (words[0].startsWith("$"))
            {
                currentPrimary = words[0];
                currentSecondary = words[0];
            }
            else {
                currentSecondary = "";
            }
            
            if (currentPrimary == "PATCH" && currentSecondary == "PATCH" && asciiPatch->boolValue()) {
                for (int iWord = 2; iWord < words.size()-2; iWord+=3) {
                    int channel = words[iWord].getIntValue();
                    int address = words[iWord + 1].getIntValue();
                    int universe = address/512;
                    address = address%512;
                    //float level = asciiLevelToFloat(words[iWord + 2]);
                    if (channel > 0) {
                        Fixture* fixt = Brain::getInstance()->getFixtureById(channel);
                        if (fixt == nullptr) {
                            fixt = new Fixture();
                            fixturesToAdd.add(fixt);
                            if (ft != nullptr) {
                                fixt->devTypeParam->setValueFromTarget(ft);
                            }
                            fixt->id->setValue(channel);
                        }

                        while (universes.size() <= universe) {
                            InterfaceManager::getInstance()->addItem(new DMXInterface());
                            universes = InterfaceManager::getInstance()->getItemsWithType<DMXInterface>();
                        }
                        DMXInterface* targetInterface = universes[universe];
                        FixturePatch* p = fixt->patchs.addItem();
                        p->targetInterface->setTarget(targetInterface);
                        p->address->setValue(address);
                    }
                }
            }
            else if (currentPrimary == "CUE" && asciiCues->boolValue()) {
                if (currentSecondary == "CUE") {
                    if (words.size() == 1) {
                        LOGERROR("invalid file, CUE word must have an id in parameter");
                    }
                    previousCue = currentCue;
                    int cueId = words[1].getFloatValue() * 100000;

                    if (!asciiEraseCuelist->boolValue() && idToCue.contains(cueId)) {
                        currentCue = idToCue.getReference(cueId);
                        currentCue->commands.clear();
                    } else {
                        currentCue = new Cue();
                        cuesToAdd.add(currentCue);
                        currentCue->editorIsCollapsed = true;
                        String cueName = "Cue " + words[1];
                        currentCue->setNiceName(cueName);
                        currentCue->commands.clear();
                        if (asciiRespectCueNumbers->boolValue()) {
                            currentCue->id->setValue(words[1].getFloatValue());
                        }
                        idToCue.set(currentCue->id->floatValue() * 100000, currentCue);
                    }
                }
                else if (currentSecondary == "TEXT") {
                    if (cuesToAdd.contains(currentCue)) {
                        String text = currentCue->cueText->stringValue() + originalLine.trim().substring(5);
                        currentCue->cueText->setValue(text);
                        currentCue->goText->setValue(text);
                        currentCue->setNiceName(text);
                    }
                }
                else if (currentSecondary == "$$CUENOTES") {
                    if (cuesToAdd.contains(currentCue)) {
                        String text = originalLine.trim().substring(11);
                        currentCue->cueText->setValue(text);
                        currentCue->setNiceName(text);
                    }
                }
                else if (currentSecondary == "FOLLOWON") {
                    if (words.size() == 1) {
                        LOGERROR("invalid file, FOLLOWON word must have at least one parameter");
                    }
                    currentCue->autoFollow->setValueWithKey("Immediate");
                    currentCue->autoFollowTiming->setValue(words[1].getFloatValue());
                }
                else if (currentSecondary == "$$WAIT") {
                    if (words.size() == 1) {
                        LOGERROR("invalid file, WAIT word must have at least one parameter");
                    }
                    if (previousCue != nullptr && words[1].getFloatValue()>0) {
                        previousCue->autoFollow->setValueWithKey("End of transitions");
                        previousCue->autoFollowTiming->setValue(words[1].getFloatValue());
                    }
                }
                else if (currentSecondary == "UP") {
                    if (words.size() == 1) {
                        LOGERROR("invalid file, UP word must have at least one parameter");
                    }
                    float fade = words[1].getFloatValue();
                    currentCue->htpInFade->setValue(fade);
                    currentCue->ltpFade->setValue(fade);
                    if (words.size() > 2) {
                        float delay = words[2].getFloatValue();
                        currentCue->htpInDelay->setValue(delay);
                        currentCue->ltpDelay->setValue(delay);
                    }
                }
                else if (currentSecondary == "DOWN") {
                    if (words.size() == 1) {
                        LOGERROR("invalid file, DOWN word must have at least one parameter");
                    }
                    float fade = words[1].getFloatValue();
                    currentCue->htpOutFade->setValue(fade);
                    if (words.size() > 2) {
                        float delay = words[2].getFloatValue();
                        currentCue->htpOutDelay->setValue(delay);
                    }
                }
                else if (currentSecondary == "CHAN") {
                    for (int iChan = 1; iChan < words.size() - 1; iChan += 2) {
                        int fixt = words[iChan].getIntValue();
                        float level = asciiLevelToFloat(words[iChan + 1]);
                        Command* com = currentCue->commands.addItem();
                        com->selection.items[0]->valueFrom->setValue(fixt);
                        com->values.items[0]->channelType->setValue(asciiDimmerChannel->getValue());
                        com->values.items[0]->valueFrom->setValue(level);
                    }
                }
                else if (currentSecondary == "$$CHANMOVE") {
                    for (int iChan = 1; iChan < words.size() - 1; iChan += 2) {
                        int fixt = words[iChan].getIntValue();
                        float level = asciiLevelToFloat(words[iChan + 1]);
                        Array<String> starts = { "IP", "FP", "CP", "BP", "PR" };
                        String begin = words[iChan + 1].substring(0, 2);
                        Command* com = currentCue->commands.addItem();
                        com->selection.items[0]->valueFrom->setValue(fixt);
                        if (starts.contains(begin)) {
                            com->values.items[0]->presetOrValue->setValueWithData("preset");
                            if (!idToPreset.contains(words[iChan + 1])) {
                                Preset* p = PresetManager::getInstance()->addItem();
                                p->userName->setValue(words[iChan + 1]);
                            }
                            Preset* target = idToPreset.getReference(words[iChan + 1]);
                            com->values.items[0]->channelType->setValue(asciiDimmerChannel->getValue());
                            com->values.items[0]->presetIdFrom->setValue(target->id->intValue());
                        }
                        else {
                            com->values.items[0]->channelType->setValue(asciiDimmerChannel->getValue());
                            com->values.items[0]->valueFrom->setValue(level);
                        }
                    }
                }
                else if (currentSecondary == "$$PARAM") {
                    Fixture* fixt = Brain::getInstance()->getFixtureById(words[1].getIntValue());
                    Command* fixtCommand = nullptr;
                    if (fixt != nullptr) {
                        for (int iChan = 2; iChan < words.size() - 1; iChan += 2) {
                            int paramId = words[iChan].getIntValue();
                            ChannelType* param = idToChannelType.getReference(paramId);
                            float level = asciiLevelToFloat(words[iChan + 1]);
                            Array<String> starts = { "IP", "FP", "CP", "BP", "PR" };
                            String begin = words[iChan + 1].substring(0, 2);
                            if (fixtCommand == nullptr) {
                                fixtCommand = currentCue->commands.addItem();
                                fixtCommand->selection.items[0]->valueFrom->setValue(fixt->id->intValue());
                                fixtCommand->values.clear();
                                fixtCommand->editorIsCollapsed = true;
                            }
                            CommandValue* commandValue = fixtCommand->values.addItem();
                            if (starts.contains(begin)) {
                                commandValue->presetOrValue->setValueWithData("preset");
                                if (!idToPreset.contains(words[iChan + 1])) {
                                    Preset* p = PresetManager::getInstance()->addItem();
                                    p->userName->setValue(words[iChan + 1]);
                                    idToPreset.set(words[iChan + 1], p);
                                }
                                Preset* target = idToPreset.getReference(words[iChan + 1]);
                                commandValue->channelType->setValueFromTarget(param);
                                commandValue->presetIdFrom->setValue(target->id->intValue());
                            }
                            else {
                                commandValue->channelType->setValueFromTarget(param);
                                commandValue->valueFrom->setValue(level);
                            }
                        }
                    }
                }

            }
            else if (currentPrimary == "SUB" && asciiSubs->boolValue()) {
                if (currentSecondary == "SUB") {
                    if (words.size() == 1) {
                        LOGERROR("invalid file, SUB word must have an id in parameter");
                    }
                    int subId = words[1].getIntValue();
                    if (subId == mainCuelistId) {
                        mainCuelistId++;
                        while (Brain::getInstance()->getCuelistById(mainCuelistId) != nullptr) {
                            mainCuelistId++;
                        }
                        cuelist->id->setValue(mainCuelistId);
                    }

                    currentSub = Brain::getInstance()->getCuelistById(subId);
                    if (currentSub == nullptr) {
                        currentSub = new Cuelist();
                        cuelistsToAdd.add(currentSub);
                        currentSub->id->setValue(subId);
                    }
                    currentSub->cues.clear();
                    currentSubCue = currentSub->cues.addItem();
                    currentSubCue->commands.clear();
                    String subName = "Sub " + words[1];
                    currentSub->userName->setValue(subName);
                }
                else if (currentSecondary == "TEXT") {
                    String text = originalLine.trim().substring(5);
                    currentSub->userName->setValue(text);
                }
                else if (currentSecondary == "DOWN") {
                    if (words.size() == 1) {
                        LOGERROR("invalid file, DOWN word must have at least one parameter");
                    }
                    float fade = words[1].getFloatValue();
                    currentSubCue->htpOutFade->setValue(fade);
                    if (words.size() > 2) {
                        float delay = words[2].getFloatValue();
                        currentSubCue->htpOutDelay->setValue(delay);
                    }
                }
                else if (currentSecondary == "CHAN") {
                    for (int iChan = 1; iChan < words.size() - 1; iChan += 2) {
                        int fixt = words[iChan].getIntValue();
                        float level = asciiLevelToFloat(words[iChan + 1]);
                        Command* com = currentSubCue->commands.addItem();
                        com->selection.items[0]->valueFrom->setValue(fixt);
                        com->values.items[0]->channelType->setValue(asciiDimmerChannel->getValue());
                        com->values.items[0]->valueFrom->setValue(level);
                    }
                }
            }
            else if (currentPrimary == "GROUP" && (asciiGroups->boolValue() || asciiGroupValuesAsPreset->boolValue())) {
                if (currentSecondary == "GROUP") {
                    if (words.size() == 1) {
                        LOGERROR("invalid file, GROUP word must have an id in parameter");
                    }
                    int groupId = words[1].getIntValue();
                    if (asciiGroups->boolValue()) {
                        if (asciiGroupValuesAsPreset->boolValue()) {
                            currentPreset = Brain::getInstance()->getPresetById(groupId);
                            if (currentPreset == nullptr) {
                                currentPreset = new Preset();
                                presetsToAdd.add(currentPreset);
                                currentPreset->id->setValue(groupId);
                            }
                            currentPreset->userName->setValue("ASCII Group " + words[1]);
                            currentPreset->subFixtureValues.clear();
                        }
                        else {
                            currentGroup = Brain::getInstance()->getGroupById(groupId);
                            if (currentGroup == nullptr) {
                                currentGroup = new Group();
                                groupsToAdd.add(currentGroup);
                                currentGroup->id->setValue(groupId);
                            }
                            currentGroup->userName->setValue("ASCII Group " + words[1]);
                            currentGroup->selection.clear();

                        }
                    }

                }
                else if (currentSecondary == "TEXT") {
                    String text = originalLine.trim().substring(5);
                    if (currentGroup != nullptr) {
                        currentGroup->userName->setValue(text);
                    }
                    if (currentPreset != nullptr) {
                        currentPreset->userName->setValue(text);
                    }

                }
                else if (currentSecondary == "CHAN") {
                    for (int iChan = 1; iChan < words.size() - 1; iChan += 2) {
                        int fixt = words[iChan].getIntValue();
                        float level = asciiLevelToFloat(words[iChan + 1]);
                        if (currentGroup != nullptr) {
                            CommandSelection* s = currentGroup->selection.addItem();
                            s->valueFrom->setValue(fixt);
                        }
                        if (currentPreset != nullptr){
                            PresetSubFixtureValues * v = currentPreset->subFixtureValues.addItem();
                            v->targetFixtureId->setValue(fixt);
                            v->values.items[0]->param->setValue(asciiDimmerChannel->getValue());
                            v->values.items[0]->paramValue->setValue(level);
                        }
                    }
                }
                else if (currentSecondary == "$$PARAM") {
                    Fixture* fixt = Brain::getInstance()->getFixtureById(words[1].getIntValue());

                    if (fixt != nullptr) {
                        for (int iChan = 2; iChan < words.size() - 1; iChan += 2) {
                            int paramId = words[iChan].getIntValue();
                            float level = words[iChan + 1].getFloatValue();
                            float div = level > 255 ? 65535 : 255;
                            ChannelType* param = nullptr;
                            if (idToChannelType.contains(paramId)) {
                                param = idToChannelType.getReference(paramId);
                            }
                            if (div == 255 && fixt->subFixtures.contains(0)) {
                                if (fixt->subFixtures.getReference(0)->channelsMap.contains(param)) {
                                    auto c = fixt->subFixtures.getReference(0)->channelsMap.getReference(param);
                                    if (c->resolution == "16bits") {
                                        div = 65535;
                                    }
                                }
                            }
                            level = level / div;
                            PresetSubFixtureValues* com = nullptr;
                            for (PresetSubFixtureValues* psfv : currentPreset->subFixtureValues.items) {
                                if (psfv->targetFixtureId->intValue() == fixt->id->intValue()) {
                                    com = psfv;
                                }
                            }
                            if (com == nullptr) {
                                com = currentPreset->subFixtureValues.addItem();
                                com->targetFixtureId->setValue(fixt->id->intValue());
                                com->values.clear();
                            }
                            PresetValue* pv = com->values.addItem();
                            pv->param->setValueFromTarget(param);
                            pv->paramValue->setValue(level);
                        }
                    }
                }
            }
            else if (currentPrimary == "$PERSONALITY") {
                if (currentSecondary == "$$MODEL") {
                    currentFixtureType->setNiceName(wordsOriginal[1]);
                }
                else if (currentSecondary == "$$PERSCHAN") {
                    FixtureTypeChannel* ftc = currentFixtureType->chansManager.addItem();
                    if (idToChannelType.contains(words[1].getIntValue())) {
                        ftc->channelType->setValueFromTarget(idToChannelType.getReference(words[1].getIntValue()));
                    }
                    if (words[2] == "2") {
                        ftc->resolution->setValueWithData("16bits");
                    }
                }
            }
            else if (currentPrimary == "$PATCH") {
                if (currentSecondary == "TEXT") {
                    String text = originalLine.trim().substring(5);
                    currentFixture->userName->setValue(text);
                }
                else if (currentSecondary == "$$OPTIONS") {
                    if (words[1].contains("P")) {
                        auto c = currentFixture->patchs.items[0]->corrections.addItem();
                        BKEngine* engine = dynamic_cast<BKEngine*>(BKEngine::mainEngine);
                        ChannelType* panChanType = dynamic_cast<ChannelType*>(engine->TPanChannel->targetContainer.get());
                        c->channelType->setValueFromTarget(panChanType);
                        c->invertChannel->setValue(true);
                    }
                    if (words[1].contains("T")) {
                        auto c = currentFixture->patchs.items[0]->corrections.addItem();
                        BKEngine* engine = dynamic_cast<BKEngine*>(BKEngine::mainEngine);
                        ChannelType* tiltChanType = dynamic_cast<ChannelType*>(engine->TTiltChannel->targetContainer.get());
                        c->channelType->setValueFromTarget(tiltChanType);
                        c->invertChannel->setValue(true);
                    }
                }
            }
            else if (currentPrimary == "$PALETTE") {
                if (currentSecondary == "TEXT") {
                    String text = originalLine.trim().substring(5);
                    currentPreset->userName->setValue(text);
                }
                else if (currentSecondary == "$$PARAM") {
                    Fixture * fixt = Brain::getInstance()->getFixtureById(words[1].getIntValue());
                    if (fixt != nullptr) {
                        for (int iChan = 2; iChan < words.size() - 1; iChan += 2) {
                            int paramId = words[iChan].getIntValue();
                            float level = words[iChan + 1].getFloatValue();
                            float div = level > 255 ? 65535 : 255;
                            ChannelType* param = nullptr;
                            if (idToChannelType.contains(paramId)) {
                                param = idToChannelType.getReference(paramId);
                            }
                            if (div == 255 && fixt->subFixtures.contains(0)) {
                                if (fixt->subFixtures.getReference(0)->channelsMap.contains(param)) {
                                    auto c = fixt->subFixtures.getReference(0)->channelsMap.getReference(param);
                                    if (c->resolution == "16bits") {
                                        div = 65535;
                                    }
                                }
                            }
                            level = level/div;
                            PresetSubFixtureValues* com = nullptr;
                            for (PresetSubFixtureValues* psfv : currentPreset->subFixtureValues.items) {
                                if (psfv->targetFixtureId->intValue() == fixt->id->intValue()) {
                                    com = psfv;
                                }
                            }
                            if (com == nullptr) {
                                com = currentPreset->subFixtureValues.addItem();
                                com->targetFixtureId->setValue(fixt->id->intValue());
                                com->values.clear();
                            }
                            PresetValue* pv = com->values.addItem();
                            pv->param->setValueFromTarget(param);
                            pv->paramValue->setValue(level);
                        }
                    }
                }
            }
            else
            {
                //LOG("" + line + " ");
                //LOG(words.size());
            }

        }
    }

    cuelist->cues.addItems(cuesToAdd, juce::var(), false);
    CuelistManager::getInstance()->addItems(cuelistsToAdd, juce::var(), false);
    GroupManager::getInstance()->addItems(groupsToAdd, juce::var(), false);
    PresetManager::getInstance()->addItems(presetsToAdd, juce::var(), false);
    FixtureManager::getInstance()->addItems(fixturesToAdd, juce::var(), false);
}

float Assistant::asciiLevelToFloat(String asciiLevel) {
    float level = 0;
    if (asciiLevel.startsWith("H") && asciiLevel.length() == 3) {
        level = asciiLevel.substring(1).getHexValue32() / 255.0;
    }
    else if (asciiLevel.startsWith("H") && asciiLevel.length() == 5) {
        level = asciiLevel.substring(1).getHexValue32() / 65535.0;
    }
    else
    {
        level = asciiLevel.getIntValue() / 100.0;
    }
    return level;
}

void Assistant::exportAscii()
{
    const MessageManagerLock mmLock;
    FileChooser fc("Select export destination", File::getCurrentWorkingDirectory(), "*.asc");
    if (fc.browseForFileToSave(true))
    {
        File f = fc.getResult();
        //f.replaceWithText(s);
    }

}



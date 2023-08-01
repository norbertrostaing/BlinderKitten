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

juce_ImplementSingleton(Assistant)

Assistant::Assistant() :
	BaseItem("Offline Lighting General Assistant"),
    patcherCC("Patch Helper"),
    paletteMakerCC("Palette maker"),
    masterMakerCC("Masters maker"),
    fixtureSwapperCC("Fixture Type Swapper"),
    midiMapperCC("Midi mappings"),
    asciiCC("ASCII import / export"),
    controlsCC("Generic controls"),
    Thread("Assistant")
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
    paletteBtn = paletteMakerCC.addTrigger("Create Palette", "create a new cuelist with selected group and presets");
    addChildControllableContainer(&paletteMakerCC);

    masterFirstGroupId = masterMakerCC.addIntParameter("First group ID", "", 1, 1);
    masterLastGroupId = masterMakerCC.addIntParameter("Last group ID", "", 1, 1);
    masterMakerCC.addChildControllableContainer(&masterValue);
    masterBtn = masterMakerCC.addTrigger("Create Masters", "create a master cuelist for each group with these values");
    addChildControllableContainer(&masterMakerCC);

    swapperOld = fixtureSwapperCC.addTargetParameter("Replace fixtures", "", FixtureTypeManager::getInstance());
    swapperOld->targetType = TargetParameter::CONTAINER;
    swapperOld->maxDefaultSearchLevel = 0;
    swapperNew = fixtureSwapperCC.addTargetParameter("With this", "", FixtureTypeManager::getInstance());
    swapperNew->targetType = TargetParameter::CONTAINER;
    swapperNew->maxDefaultSearchLevel = 0;
    swapperBtn = fixtureSwapperCC.addTrigger("Change fixtures", "Replace all fixtures of first type with the second one");
    addChildControllableContainer(&fixtureSwapperCC);


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
    asciiChannelFixtureType = asciiCC.addTargetParameter("Channel Fixture type", "Fixture used for ascii import/export", FixtureTypeManager::getInstance());
    asciiChannelFixtureType->targetType = TargetParameter::CONTAINER;
    asciiChannelFixtureType->maxDefaultSearchLevel = 0;
    asciiDimmerChannel = asciiCC.addTargetParameter("Intensity Channel", "Channel used as intensity for ascii import/export", ChannelFamilyManager::getInstance());
    asciiDimmerChannel->targetType = TargetParameter::CONTAINER;
    asciiDimmerChannel->maxDefaultSearchLevel = 2;
    asciiCuelistId = asciiCC.addIntParameter("Main Sequence ID", "Sequence to export or to import in", 1, 1);
    importAsciiBtn = asciiCC.addTrigger("Import", "Click to import a file");
    //exportAsciiBtn = asciiCC.addTrigger("Export", "Click to export selected cuelist in file");

    addChildControllableContainer(&controlsCC);
    offCuelistsBtn = controlsCC.addTrigger("Off all cuelists", "");
    killCuelistsBtn = controlsCC.addTrigger("Kill all cuelists", "");
    stopEffectsBtn = controlsCC.addTrigger("Stop all effects", "");
    stopCarouselsBtn = controlsCC.addTrigger("Stop all carousels", "");
    randomSeed = controlsCC.addIntParameter("Random Seed", "", 0, 0);
    resetRandomBtn = controlsCC.addTrigger("Reset Random Seed", "");

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
        importAscii();
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
        pleaseImportAscii = true;
        startThread();
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
        Brain::getInstance()->resetRandomSeed(randomSeed->getValue());
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

    for (int i = 0; i < amount; i++) {
        const MessageManagerLock mmLock;
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
    // supprimer premier cue
    for (int i = presetFrom; i != presetTo + delta; i += delta) {
        const MessageManagerLock mmLock;
        Preset* p = Brain::getInstance()->getPresetById(i);
        if (p != nullptr) {
            String name = p->userName->getValue().toString();
            Cue* c = cl->cues.addItem();
            c->editorIsCollapsed = true;

            c->setNiceName(name);
            c->id->setValue(i);
            c->commands.items[0]->selection.items[0]->targetType->setValueWithKey("Group");
            c->commands.items[0]->selection.items[0]->valueFrom->setValue(groupId);
            c->commands.items[0]->values.items[0]->presetOrValue->setValueWithKey("Preset");
            c->commands.items[0]->values.items[0]->presetIdFrom->setValue(i);
            if (timePreset != 0) {
                c->commands.items[0]->timing.presetOrValue->setValueWithKey("Preset");
                c->commands.items[0]->timing.presetId->setValue(timePreset);
            }
            LOG("Cue " + name + " Created");
            //wait(10);
        }
    }

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
        }
        cuelist->kill();
        cuelist->cues.clear();
        cuelist->userName->setValue("ASCII Cuelist");
    }
    Cue* currentCue = nullptr;
    Group* currentGroup = nullptr;
    Preset* currentPreset = nullptr;
    Cuelist* currentSub = nullptr;
    Cue* currentSubCue = nullptr;

    FixtureType* ft = dynamic_cast<FixtureType*>(asciiChannelFixtureType->targetContainer.get());
    int totLines = lines.size();
    for (int i = 0; i < lines.size(); i++) {
        String line = lines[i];
        LOG((i+1)<<"/"<<totLines<<" : "<<line);
        wait(50);
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
            //int currentWord = 1;
            if (words[0] == "ENDDATA") {
                return;
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
            else if (words[0] == "CHAN" || words[0] == "DOWN" || words[0] == "FOLLOWON" || words[0] == "LINK" || words[0] == "PART" || words[0] == "TEXT" || words[0] == "UP") 
            {
                currentSecondary = words[0];
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
            
            if (currentPrimary == "PATCH" && currentSecondary == "PATCH" && asciiPatch->getValue()) {
                for (int iWord = 2; iWord < words.size()-2; iWord+=3) {
                    int channel = words[iWord].getIntValue();
                    int address = words[iWord + 1].getIntValue();
                    int universe = address/512;
                    address = address%512;
                    //float level = asciiLevelToFloat(words[iWord + 2]);
                    if (channel > 0) {
                        Fixture* fixt = Brain::getInstance()->getFixtureById(channel);
                        if (fixt == nullptr) {
                            fixt = FixtureManager::getInstance()->addItem();
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
            else if (currentPrimary == "CUE" && asciiCues->getValue()) {
                if (currentSecondary == "CUE") {
                    if (words.size() == 1) {
                        LOGERROR("invalid file, CUE word must have an id in parameter");
                    }
                    currentCue = cuelist->cues.addItem();
                    String cueName = "Cue " + words[1];
                    currentCue->setNiceName(cueName);
                    currentCue->commands.clear();
                    if (asciiRespectCueNumbers->getValue()) {
                        currentCue->id->setValue(words[1].getFloatValue());
                    }
                }
                else if (currentSecondary == "TEXT") {
                    String text = originalLine.trim().substring(5);
                    currentCue->cueText->setValue(text);
                }
                else if (currentSecondary == "FOLLOWON") {
                    if (words.size() == 1) {
                        LOGERROR("invalid file, UP word must have at least one parameter");
                    }
                    currentCue->autoFollow->setValueWithData("Immediate");
                    currentCue->autoFollowTiming->setValue(words[1].getFloatValue());
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
            }
            else if (currentPrimary == "SUB" && asciiSubs->getValue()) {
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
                        currentSub = CuelistManager::getInstance()->addItem();
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
            else if (currentPrimary == "GROUP" && (asciiGroups->getValue() || asciiGroupValuesAsPreset->getValue())) {
                if (currentSecondary == "GROUP") {
                    if (words.size() == 1) {
                        LOGERROR("invalid file, GROUP word must have an id in parameter");
                    }
                    int groupId = words[1].getIntValue();
                    if (asciiGroups->getValue()) {
                        if (asciiGroupValuesAsPreset->getValue()) {
                            currentPreset = Brain::getInstance()->getPresetById(groupId);
                            if (currentPreset == nullptr) {
                                currentPreset = PresetManager::getInstance()->addItem();
                                currentPreset->id->setValue(groupId);
                            }
                            currentPreset->userName->setValue("ASCII Group " + words[1]);
                            currentPreset->subFixtureValues.clear();
                        }
                        else {
                            currentGroup = Brain::getInstance()->getGroupById(groupId);
                            if (currentGroup == nullptr) {
                                currentGroup = GroupManager::getInstance()->addItem();
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
            }
            else
            {
                LOG("" + line + " ");
                LOG(words.size());
            }

        }
    }

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



/*
  ==============================================================================

    FixtureMultiEditor.cpp
    Created: 29 Jan 2019 3:52:46pm
    Author:  no

  ==============================================================================
*/
#include "JuceHeader.h"
#include "FixtureMultiEditor.h"
#include "Fixture.h"
#include "../FixtureType/FixtureType.h"
#include "../FixtureType/FixtureTypeManager.h"
#include "FixtureManager.h"
#include "FixturePatch.h"
#include "../Interface/InterfaceIncludes.h"

juce_ImplementSingleton(FixtureMultiEditor)

FixtureMultiEditor::FixtureMultiEditor() :
	BaseItem("Fixture Multi Editor"),
    renamer("Rename fixtures"),
    repatcher("Repatch fixtures"),
    renumberer("Change fixture IDs"),
    typeChanger("Change Fixture type")
{    
    newName = renamer.addStringParameter("New Name", "if ends with a space then a number, this number will be used to increment name", "");
    renameBtn = renamer.addTrigger("Rename Fixtures", "");
    addChildControllableContainer(&renamer);

    targetInterface = repatcher.addTargetParameter("Interface", "output interface", InterfaceManager::getInstance());
    targetInterface->targetType = TargetParameter::CONTAINER;
    targetInterface->customGetTargetContainerFunc = &InterfaceManager::showAndGetInterfaceOfType<DMXInterface>;
    targetInterface->maxDefaultSearchLevel = 0;
    firstAddress = repatcher.addIntParameter("Address", "First address ", 1, 1, 512);
    adressesInterval = repatcher.addIntParameter("Interval", "Fixtures will be patched every X", 1);
    addPatchBtn = repatcher.addTrigger("Add patchs", "add the patch to fixtures");
    unpatchBtn = repatcher.addTrigger("Reset patch", "Delete all patchs of selected fixtures");
    addChildControllableContainer(&repatcher);

    firstId = renumberer.addIntParameter("First ID", "ID of the fist fixture", 1, 1);
    renumberBtn = renumberer.addTrigger("Renumber fixtures", "renumber selected fixtures");
    addChildControllableContainer(&renumberer);

    newType = typeChanger.addTargetParameter("Fixture type", "Type of Fixture", FixtureTypeManager::getInstance());
    newType->targetType = TargetParameter::CONTAINER;
    newType->maxDefaultSearchLevel = 0;
    changeTypeBtn = typeChanger.addTrigger("Change Fixtures type", "Change the type al all these fixtures");
    addChildControllableContainer(&typeChanger);
    // updateDisplay();
}

void FixtureMultiEditor::updateDisplay() {

}

void FixtureMultiEditor::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
    if (c == renameBtn) {
        goRename();
    }
    else if(c == unpatchBtn) {
        goUnpatch();
    }
    else if(c == addPatchBtn) {
        goAddPatch();
    }
    else if (c == renumberBtn) {
        goRenumber();
    }
    else if (c == changeTypeBtn) {
        goChangeType();
    }
}

FixtureMultiEditor::~FixtureMultiEditor()
{
}


void FixtureMultiEditor::goRename() {
    String name = newName->getValue();
    name = name.trim();
    int num = 1;
    int lastSpace = name.lastIndexOf(" ");
    if (lastSpace > 0) {
        String maybeDigit = name.substring(lastSpace+1);
        if (maybeDigit.containsOnly("1234567890")) {
            num = maybeDigit.getIntValue();
            name = name.substring(0,lastSpace);
        }
    }

    name += " ";
    for (int i = 0; i < selectionManager->currentInspectables.size(); i++) {
        Fixture* f = dynamic_cast<Fixture*>(selectionManager->currentInspectables[i].get());
        if (f->objectType == "Fixture") {
            String localName = name + String(num);
            f->userName->setValue(localName);
            num++;
        }
        else {
            LOG(f->objectType);
        }
    }
    FixtureManager::getInstance()->queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, FixtureManager::getInstance()));
}

void FixtureMultiEditor::goUnpatch() {
    for (int i = 0; i < selectionManager->currentInspectables.size(); i++) {
        Fixture* f = dynamic_cast<Fixture*>(selectionManager->currentInspectables[i].get());
        if (f->objectType == "Fixture") {
            f->patchs.clear();
        }
    }
    FixtureManager::getInstance()->queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, FixtureManager::getInstance()));
}

void FixtureMultiEditor::goAddPatch() {
    int address = firstAddress->getValue();
    int interval = adressesInterval->getValue();
    Interface* outInterface = dynamic_cast<Interface*>(targetInterface->targetContainer.get());
    if (outInterface == nullptr) { LOGWARNING("you must specify a target interface"); return; }
    bool continuePatch = true;
    for (int i = 0; i < selectionManager->currentInspectables.size() && continuePatch; i++) {
        Fixture* f = dynamic_cast<Fixture*>(selectionManager->currentInspectables[i].get());

        if (f->objectType == "Fixture") {
            FixtureType* ft = dynamic_cast<FixtureType*>(f->devTypeParam->targetContainer.get());
            if (ft != nullptr) {
                FixturePatch* p = f->patchs.addItem();
                p->targetInterface->setValueFromTarget(outInterface);
                p->address->setValue(address);
                int delta = abs(interval);

                for (int cn = 0; cn < ft->chansManager.items.size(); cn++) {
                    int chanSize = ft->chansManager.items[cn]->resolution->getValue().toString()=="16bits" ? 1 : 0;
                    delta = jmax(delta, (int)ft->chansManager.items[cn]->dmxDelta->getValue()+chanSize);
                }
                if (interval >= 0) address += delta;
                else address -= delta;
                if (address > 512 || address < 1) continuePatch = false;
            }
            else {
                LOGWARNING("Fixture type not valid");
            }
            
        }
    }
    FixtureManager::getInstance()->queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, FixtureManager::getInstance()));
}

void FixtureMultiEditor::goRenumber() {
    int num = firstId->getValue();
    for (int i = 0; i < selectionManager->currentInspectables.size(); i++) {
        Fixture* f = dynamic_cast<Fixture*>(selectionManager->currentInspectables[i].get());
        if (f->objectType == "Fixture") {
            f->id->setValue(num);
            num++;
        }
        else {
            LOG(f->objectType);
        }
    }
    FixtureManager::getInstance()->queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, FixtureManager::getInstance()));
}

void FixtureMultiEditor::goChangeType() {
    FixtureType* t = dynamic_cast<FixtureType* >(newType->targetContainer.get());
    for (int i = 0; i < selectionManager->currentInspectables.size(); i++) {
        Fixture* f = dynamic_cast<Fixture*>(selectionManager->currentInspectables[i].get());
        if (f->objectType == "Fixture") {
            if (t != nullptr) {
                f->devTypeParam->setValueFromTarget(t);
            }
            else {
                f->devTypeParam->resetValue();
            }
        }
    }
    FixtureManager::getInstance()->queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, FixtureManager::getInstance()));
}

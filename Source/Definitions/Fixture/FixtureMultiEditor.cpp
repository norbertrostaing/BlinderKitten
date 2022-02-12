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
#include "FixtureManager.h"
#include "../Interface/InterfaceIncludes.h"

juce_ImplementSingleton(FixtureMultiEditor)

FixtureMultiEditor::FixtureMultiEditor() :
	BaseItem("Fixture Multi Editor"),
    renamer("Rename fixtures"),
    repatcher("Repatch fixtures"),
    renumberer("Change fixture IDs")
{    
    newName = renamer.addStringParameter("New Name", "if ends with a space then a number, this number will be used to increment name", "");
    renameBtn = renamer.addTrigger("Rename Fixtures", "");
    addChildControllableContainer(&renamer);

    targetInterface = repatcher.addTargetParameter("Interface", "output interface", InterfaceManager::getInstance());
    targetInterface->targetType = TargetParameter::CONTAINER;
    targetInterface->maxDefaultSearchLevel = 0;
    firstAddress = repatcher.addIntParameter("Address", "First address ", 1, 1, 512);
    adressesInterval = repatcher.addIntParameter("Interval", "Fixtures will be patched every X", 1, 1, 512);
    addPatchBtn = repatcher.addTrigger("Add patchs", "add the patch to fixtures");
    unpatchBtn = repatcher.addTrigger("Reset patch", "Delete all patchs of selected fixtures");
    addChildControllableContainer(&repatcher);

    firstId = renumberer.addIntParameter("First ID", "ID of the fist fixture",1,1);
    renumberBtn  = renumberer.addTrigger("Renumber fixtures","renumber selected fixtures");
    addChildControllableContainer(&renumberer);
    // updateDisplay();
}

void FixtureMultiEditor::updateDisplay() {

}

void FixtureMultiEditor::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
    if (c == renameBtn) {
        goRename();
    }
}

FixtureMultiEditor::~FixtureMultiEditor()
{
}


void FixtureMultiEditor::goRename() {
    String name = newName->getValue();
    name += " ";
    int num = 1;
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

void FixtureMultiEditor::goRepatch() {
    String name = newName->getValue();
    name += " ";
    int num = 1;
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

void FixtureMultiEditor::goRenumber() {
    String name = newName->getValue();
    name += " ";
    int num = 1;
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


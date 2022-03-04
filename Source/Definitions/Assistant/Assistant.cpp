/*
  ==============================================================================

    Assistant.cpp
    Created: 29 Jan 2019 3:52:46pm
    Author:  no

  ==============================================================================
*/
#include "JuceHeader.h"
#include "Assistant.h"

juce_ImplementSingleton(Assistant)

Assistant::Assistant() :
	BaseItem("Offline Lighting General Assistant"),
    paletteMakerCC("Palette maker")
{
    updateDisplay(); 
    addChildControllableContainer(&paletteMakerCC);

	paletteGroupId = paletteMakerCC.addIntParameter("Group ID", "", 1,0);
	paletteFirstPresetId = paletteMakerCC.addIntParameter("First Preset ID", "", 1,0);
	paletteLastPresetId = paletteMakerCC.addIntParameter("Last Preset ID", "", 0,0);
	paletteTimingPresetId = paletteMakerCC.addIntParameter("Timing Preset ID", "0 means none", 0,0);
    paletteBtn = paletteMakerCC.addTrigger("Create Palette", "create a new cuelist with selected group and presets");

}

void Assistant::updateDisplay() {
    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void Assistant::onContainerParameterChangedInternal(Parameter* p) {
    updateDisplay();
}

Assistant::~Assistant()
{
}

void Assistant::triggerTriggered(Trigger* t) {
}

void Assistant::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
    if ((Trigger*)c == paletteBtn) {
        createPalette();
    }
}

void Assistant::createPalette()
{
    LOG("yepaaaaaaaaaaa");
}



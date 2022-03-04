/*
  ==============================================================================

    Assistant.h
    Created: 29 Jan 2019 3:52:46pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class Assistant :
	public BaseItem
{
public:
	juce_DeclareSingleton(Assistant, true);

	Assistant();
	~Assistant();

	ControllableContainer paletteMakerCC;
	IntParameter * paletteGroupId;
	IntParameter * paletteFirstPresetId;
	IntParameter * paletteLastPresetId;
	IntParameter * paletteTimingPresetId;
	Trigger * paletteBtn;

	void triggerTriggered(Trigger* t);
	void onContainerParameterChangedInternal(Parameter* p);
	void updateDisplay();
	void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c);

	void createPalette();

};
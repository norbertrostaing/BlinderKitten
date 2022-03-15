/*
  ==============================================================================

    Assistant.h
    Created: 29 Jan 2019 3:52:46pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Definitions/Command/CommandValueManager.h"

class Assistant :
	public BaseItem,
	Thread
{
public:
	juce_DeclareSingleton(Assistant, true);

	Assistant();
	~Assistant();

	void run() override;

	ControllableContainer patcherCC;
	TargetParameter * patcherFixtureType;
	IntParameter* patcherAmount;
	StringParameter * patcherName;
	IntParameter* patcherFirstId;
	TargetParameter* patcherInterface;
	IntParameter* patcherFirstAddress;
	IntParameter* patcherAddressInterval;
	Trigger* patcherBtn;

	ControllableContainer paletteMakerCC;
	IntParameter* paletteGroupId;
	IntParameter* paletteFirstPresetId;
	IntParameter* paletteLastPresetId;
	IntParameter* paletteTimingPresetId;
	Trigger* paletteBtn;

	ControllableContainer masterMakerCC;
	IntParameter* masterFirstGroupId;
	IntParameter* masterLastGroupId;
	CommandValueManager masterValue;
	Trigger* masterBtn;

	ControllableContainer midiMapperCC;
	TargetParameter* midiMapperTargetInterface;
	EnumParameter* midiMapperTargetType = nullptr;
	IntParameter* midiMapperTargetId;
	IntParameter* midiMapperPageNumber;
	Trigger* midiMapperBtn;


	bool pleasePatchFixtures = false;
	bool pleaseCreatePalette = false;
	bool pleaseCreateMasters = false;
	bool pleaseCreateMidiMappings = false;

	void triggerTriggered(Trigger* t);
	void onContainerParameterChangedInternal(Parameter* p);
	void updateDisplay();
	void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c);

	void patchFixtures();
	void createPalette();
	void createMasters();
	void createMidiMappings();

};
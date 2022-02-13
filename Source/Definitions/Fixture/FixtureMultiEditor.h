/*
  ==============================================================================

    FixtureMultiEditor.h
    Created: 29 Jan 2019 3:52:46pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class FixtureMultiEditor :
	public BaseItem
{
public:
	juce_DeclareSingleton(FixtureMultiEditor, true);

	EnumParameter* sourceType;
	Trigger* go;

	FixtureMultiEditor();
	~FixtureMultiEditor();

	ControllableContainer renamer;
	StringParameter* newName;
	Trigger* renameBtn;

	ControllableContainer repatcher;
	TargetParameter* targetInterface;

	IntParameter* firstAddress;
	IntParameter* adressesInterval;

	Trigger* addPatchBtn;
	Trigger* unpatchBtn;

	ControllableContainer renumberer;
	IntParameter* firstId;
	Trigger* renumberBtn;

	ControllableContainer typeChanger;
	TargetParameter* newType;
	Trigger* changeTypeBtn;

	void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c);
	void updateDisplay();
	void goChangeType();
	void goRename();
	void goUnpatch();
	void goAddPatch();
	void goRenumber();

};
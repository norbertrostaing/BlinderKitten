/*
  ==============================================================================

    DataTransferManager.h
    Created: 29 Jan 2019 3:52:46pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class DataTransferManager :
	public BaseItem
{
public:
	juce_DeclareSingleton(DataTransferManager, true);

	EnumParameter* sourceType;
	IntParameter* sourceId;
	EnumParameter* targetType;
	IntParameter* targetId;
	EnumParameter* groupMergeMode;
	EnumParameter* presetMergeMode;
	EnumParameter* cuelistMergeMode;
	EnumParameter* programmerMergeMode;
	TargetParameter* paramfilter;
	Trigger* go;

	DataTransferManager();
	~DataTransferManager();

	void triggerTriggered(Trigger* t);
	void onContainerParameterChangedInternal(Parameter* p);
	void updateDisplay();
	void execute();

};
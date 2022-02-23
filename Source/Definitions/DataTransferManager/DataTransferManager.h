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
	IntParameter* targetUserId;
	EnumParameter* groupCopyMode;
	EnumParameter* presetCopyMode;
	EnumParameter* cuelistCopyMode;
	EnumParameter* programmerMergeMode;
	TargetParameter* paramfilter;
	Trigger* go;

	DataTransferManager();
	~DataTransferManager();

	void triggerTriggered(Trigger* t);
	void onContainerParameterChangedInternal(Parameter* p);
	void updateDisplay();
	void execute();

	void editObject(String type, int id);
	void deleteObject(String type, int id);
	void moveObject(String type, int id, int idTo);

};
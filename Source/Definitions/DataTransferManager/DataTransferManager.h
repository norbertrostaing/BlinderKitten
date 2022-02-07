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
	Trigger* go;

	DataTransferManager();
	~DataTransferManager();

	void triggerTriggered(Trigger* t);

};
/*
 ==============================================================================

 ChataigneEngine.h
 Created: 2 Apr 2016 11:03:21am
 Author:  Martin Hermant

 ==============================================================================
 */


#pragma once

#include "JuceHeader.h"
#include "Brain.h"
 // #include "Module/ModuleIncludes.h"

class BKEngine :
	public Engine
{
public:
	BKEngine();
	~BKEngine();

	//Global Settings
	//ControllableContainer defaultBehaviors;
	Brain* mainBrain;

	ControllableContainer uiParamsContainer;
	FloatParameter* gridScale;
	FloatParameter* panelScale;
	FloatParameter* encodersScale;
	IntParameter* encoderBigNumber;

	ControllableContainer virtualParamsContainer;
	IntParameter* virtualButtonGridCols;
	IntParameter* virtualButtonGridRows;

	IntParameter* virtualFaderCols;
	IntParameter* virtualFaderRotary;
	IntParameter* virtualFaderAbove;
	IntParameter* virtualFaderSize;
	IntParameter* virtualFaderBelow;

	void clearInternal() override;

	var getJSONData() override;
	void loadJSONDataInternalEngine(var data, ProgressTask* loadingTask) override;

	void childStructureChanged(ControllableContainer* cc) override;
	void controllableFeedbackUpdate(ControllableContainer* cc, Controllable* c) override;

	void handleAsyncUpdate() override;

	String getMinimumRequiredFileVersion() override;

	void importSelection(File f = File());
	void exportSelection();

	void parameterValueChanged(Parameter* p);
};
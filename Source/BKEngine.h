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

struct geometryBreaks {
	String name;
	int breakNum;
	int offset;
};


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
	IntParameter* encodersNumber;
	IntParameter* gridCols;
	FloatParameter* gridScale;
	FloatParameter* panelScale;
	FloatParameter* encodersScale;
	IntParameter* encoderBigNumber;

	ControllableContainer virtualParamsContainer;
	EnumParameter* faderSelectionMode;
	IntParameter* virtualButtonGridCols;
	IntParameter* virtualButtonGridRows;

	IntParameter* virtualFaderCols;
	IntParameter* virtualFaderRotary;
	IntParameter* virtualFaderAbove;
	IntParameter* virtualFaderSize;
	IntParameter* virtualFaderBelow;

	ControllableContainer conductorInfosContainer;
	IntParameter* conductorCuelistId;
	IntParameter* conductorTitleSize;
	IntParameter* conductorTextSize;
	ColorParameter* conductorCurrentCueColor;
	ColorParameter* conductorNextCueColor;

	ControllableContainer colorPickerContainer;
	TargetParameter* CPRedChannel;
	TargetParameter* CPGreenChannel;
	TargetParameter* CPBlueChannel;
	TargetParameter* CPCyanChannel;
	TargetParameter* CPMagentaChannel;
	TargetParameter* CPYellowChannel;

	ControllableContainer loadWindowContainer;
	IntParameter* loadWindowWidth;
	IntParameter* loadWindowHeight;
	IntParameter* loadWindowButtonPerLine;
	IntParameter* loadWindowButtonHeight;


	void createNewGraphInternal() override;

	void clearInternal() override;

	var getJSONData() override;
	void loadJSONDataInternalEngine(var data, ProgressTask* loadingTask) override;

	void childStructureChanged(ControllableContainer* cc) override;
	void controllableFeedbackUpdate(ControllableContainer* cc, Controllable* c) override;

	void handleAsyncUpdate() override;

	String getMinimumRequiredFileVersion() override;

	void importSelection();
	void importMochi(File f);
	void importMochi(var data);
	void importGDTF(File f);
	FixtureType* importGDTF(InputStream* stream, String modeName);
	FixtureType* importGDTFContent(InputStream* stream, String modeName);
	void importMVR(File f);
	void exportSelection();

	void getBreakOffset(XmlElement* tag, String geometryName, Array<geometryBreaks>* subs);

	void parameterValueChanged(Parameter* p);

	void autoFillColorPickerValues();
};

struct tempChannel {
	String attribute = "";
	String initialFunction = "";
	int subFixtId = 0;
	int resolution = 8;
	int virtualDimmer = -1;
};


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

class DMXChannelView;

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
	DMXChannelView* currentDMXChannelView;

	ControllableContainer uiParamsContainer;
	IntParameter* encodersNumber;
	IntParameter* gridCols;
	FloatParameter* gridScale;
	//FloatParameter* panelScale;
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

	ControllableContainer genericSettingsContainer;
	IntParameter* tapTempoHistory;
	IntParameter* defaultPresetId;


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
	TargetParameter* CPWhiteChannel;
	TargetParameter* CPAmberChannel;
	TargetParameter* CPUVChannel;
	TargetParameter* CPCyanChannel;
	TargetParameter* CPMagentaChannel;
	TargetParameter* CPYellowChannel;
	TargetParameter* CPHueChannel;
	TargetParameter* CPSaturationChannel;

	ControllableContainer trackerContainer;
	TargetParameter* IntensityChannel;
	TargetParameter* TPanChannel;
	TargetParameter* TTiltChannel;

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
	void importFixtureFromMVR(XmlElement* child,
		std::shared_ptr<ZipFile> archive,
		int& maxAddress,
		HashMap<String, FixtureType*>& fixtureTypesMap,
		HashMap<int, Fixture*>& fixturesMap,
		HashMap<int, std::shared_ptr<Array<int>> >& fixtureAddressesMap,
		Layout*& frontLayout,
		Layout*& topLayout,
		Layout*& sideLayout
	);
	void importGroupObjectFromMVR(XmlElement* child,
		std::shared_ptr<ZipFile> archive,
		int& maxAddress,
		HashMap<String, FixtureType*>& fixtureTypesMap,
		HashMap<int, Fixture*>& fixturesMap,
		HashMap<int, std::shared_ptr<Array<int>> >& fixtureAddressesMap,
		Layout*& frontLayout,
		Layout*& topLayout,
		Layout*& sideLayout
	);
	void exportSelection();

	void getBreakOffset(XmlElement* tag, String geometryName, int dmxBreak, Array<geometryBreaks>* subs);

	void parameterValueChanged(Parameter* p);

	void autoFillDefaultChannels();

	void showLabelAndTime();
	Array<Cue*> selectedCues;
	enum selectionMode{SET, ADD, ADDMULTIPLE};
	void selectCue(Cue* c, selectionMode s);
	void selectAllCuesOfCuelist(Cuelist* c);
	void clearCueSelection();
};

struct tempChannel {
	String attribute = "";
	String initialFunction = "";
	int subFixtId = 0;
	int resolution = 8;
	int virtualDimmer = -1;
	float physicalFrom = 0;
	float physicalTo = 0;
};


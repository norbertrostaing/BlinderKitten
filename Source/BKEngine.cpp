/*
 ==============================================================================

 Engine.cpp
 Created: 2 Apr 2016 11:03:21am
 Author:  Martin Hermant

 ==============================================================================
 */
#include "BKEngine.h"
#include "JuceHeader.h"


// #include "Common/InputSystem/InputSystemManager.h"

// #include "UI/ChataigneAssetManager.h"

// #include "CustomVariables/CustomVariablesIncludes.h"
// #include "Guider/GuiderIncludes.h"
// #include "TimeMachine/TimeMachineIncludes.h"
// #include "StateMachine/StateMachineIncludes.h"
// #include "Common/CommonIncludes.h"

#include "Brain.h"
#include "UserInputManager.h"
#include "./Definitions/Interface/InterfaceManager.h"
#include "./Definitions/Interface/InterfaceIncludes.h"
#include "./Definitions/ChannelFamily/ChannelFamilyManager.h"
#include "./Definitions/FixtureType/FixtureTypeManager.h"
#include "./Definitions/Fixture/FixtureManager.h"
#include "./Definitions/Group/GroupManager.h"
#include "./Definitions/Preset/PresetManager.h"
#include "./Definitions/Command/CommandManager.h"
#include "./Definitions/Cuelist/CuelistManager.h"
#include "./Definitions/Programmer/ProgrammerManager.h"
#include "./Definitions/TimingPreset/TimingPresetManager.h"
#include "./Definitions/CurvePreset/CurvePresetManager.h"
#include "./Definitions/Effect/EffectManager.h"
#include "./Definitions/Carousel/CarouselManager.h"
#include "./Definitions/Mapper/MapperManager.h"
#include "./Definitions/Tracker/TrackerManager.h"
#include "./Definitions/Multiplicator/MultiplicatorManager.h"
#include "./Definitions/Layout/LayoutManager.h"

#include "./Definitions/DataTransferManager/DataTransferManager.h"
#include "./Definitions/Fixture/FixtureMultiEditor.h"

#include "./Common/MIDI/MIDIDevice.h"
#include "./Common/MIDI/MIDIManager.h"
#include "./Common/DMX/DMXManager.h"
#include "./Common/DMX/device/DMXDevice.h"
#include "./Common/Serial/SerialDevice.h"
#include "./Common/Serial/SerialManager.h"

#include "./Common/Action/Action.h"
#include "./Common/Action/ActionManager.h"

#include "UI/CommandLine.h"
#include "UI/Encoders.h"
#include "UI/EncodersMult/EncodersMult.h"
#include "UI/InputPanel.h"
#include "UI/LayoutViewer.h"

#include "UI/GridView/FixtureGridView.h"
#include "UI/GridView/GroupGridView.h"
#include "UI/GridView/PresetGridView.h"
#include "UI/GridView/CuelistGridView.h"
#include "UI/GridView/EffectGridView.h"
#include "UI/GridView/CarouselGridView.h"
#include "UI/GridView/MapperGridView.h"

#include "UI/VirtualButtons/VirtualButtonManager.h"
#include "UI/VirtualButtons/VirtualButtonGrid.h"

#include "UI/VirtualFaders/VirtualFaderColManager.h"
#include "UI/VirtualFaders/VirtualFaderColGrid.h"

#include "UI/ConductorInfos.h"
#include "UI/BKColorPicker.h"

#include "UserInputManager.h"
#include "UI/DMXChannelView.h"
#include "Assistant/Assistant.h"
#include "UI/CuelistLoadWindow.h"

ControllableContainer* getAppSettings();

BKEngine::BKEngine() :
	Engine("BlinderKitten", ".olga"),
	conductorInfosContainer("Conductor infos Settings"),
	colorPickerContainer("Color Picker Settings"),
	trackerContainer("Tracker Settings"),
	virtualParamsContainer("Virtual Playbacks Settings"),
	uiParamsContainer("UI Settings"),
	loadWindowContainer("Cuelist load window")
	//defaultBehaviors("Test"),
	//ossiaFixture(nullptr)
{
	convertURL = "http://hazlab.fr/";
	
	//Communication
	OSCRemoteControl::getInstance()->addRemoteControlListener(UserInputManager::getInstance());
	//init here
	Engine::mainEngine = this;
	BKEngine::mainEngine = this;

	GlobalSettings::getInstance()->altScaleFactor->setDefaultValue(0.002);

	ProjectSettings::getInstance()->addChildControllableContainer(&conductorInfosContainer);
	GlobalSettings::getInstance()->addChildControllableContainer(&colorPickerContainer);
	GlobalSettings::getInstance()->addChildControllableContainer(&trackerContainer);
	ProjectSettings::getInstance()->addChildControllableContainer(&virtualParamsContainer);
	GlobalSettings::getInstance()->addChildControllableContainer(&uiParamsContainer);
	ProjectSettings::getInstance()->addChildControllableContainer(&loadWindowContainer);

	faderSelectionMode = virtualParamsContainer.addEnumParameter("Faders selection mode", "Single copies elements on only one leement, column assign target to all elements in selected column");
	faderSelectionMode->addOption("Single", "single")->addOption("Column", "column");

	virtualButtonGridCols = virtualParamsContainer.addIntParameter("Button cols", "Number of cols in playback button grid", 5, 1);
	virtualButtonGridCols->addParameterListener(this);
	virtualButtonGridRows = virtualParamsContainer.addIntParameter("Button rows", "Number of rows in playback button grid", 5, 1);
	virtualButtonGridRows->addParameterListener(this);

	virtualFaderCols = virtualParamsContainer.addIntParameter("Fader cols", "Number of cols in playback button grid", 8, 1);
	virtualFaderCols->addParameterListener(this);
	virtualFaderRotary = virtualParamsContainer.addIntParameter("Rotary number", "Number of rotary in each fader column", 1, 0);
	virtualFaderRotary->addParameterListener(this);
	virtualFaderAbove = virtualParamsContainer.addIntParameter("Above button numbers", "Number of buttons above the fader in each fader column", 1, 0);
	virtualFaderAbove->addParameterListener(this);
	virtualFaderSize = virtualParamsContainer.addIntParameter("Fader Size", "Size of the fader (unit is button size)", 3, 1);
	virtualFaderSize->addParameterListener(this);
	virtualFaderBelow = virtualParamsContainer.addIntParameter("Below button numbers", "Number of buttons below the fader in each fader column", 1, 0);
	virtualFaderBelow->addParameterListener(this);

	encodersNumber = uiParamsContainer.addIntParameter("Encoders number", "How many encoders do you want ?", 10, 1);
	encodersNumber->addParameterListener(this);
	//panelScale = uiParamsContainer.addFloatParameter("Input Panel scale", "scale Input panel view", 1, 0.1, 3);
	//panelScale->addParameterListener(this);
	encodersScale = uiParamsContainer.addFloatParameter("Encoders scale", "scale the encoders view", 1, 0.1, 3);
	encodersScale->addParameterListener(this);
	gridCols= uiParamsContainer.addIntParameter("Grid Columns", "Number of columns for grid viewss", 10, 1);
	gridCols->addParameterListener(this);
	gridScale = uiParamsContainer.addFloatParameter("Grid scale", "scale the grid view", 1, 0.1, 3);
	gridScale->addParameterListener(this);
	encoderBigNumber = uiParamsContainer.addIntParameter("Encoder big offset", "Offset of encoders when << or >> pressed",4,2);

	conductorCuelistId = conductorInfosContainer.addIntParameter("Conductor ID", "ID of the conductor cuelist", 1, 1);
	conductorCuelistId->addParameterListener(this);
	conductorTitleSize = conductorInfosContainer.addIntParameter("Main font size", "Text size of cue names and go action", 21, 1);;
	conductorTitleSize->addParameterListener(this);
	conductorTextSize = conductorInfosContainer.addIntParameter("Text font size", "text size of cue content", 14, 1);;
	conductorTextSize->addParameterListener(this);
	conductorCurrentCueColor = conductorInfosContainer.addColorParameter("Current cue color", "Text color for your current cue", Colour(127, 127, 127));
	conductorCurrentCueColor->addParameterListener(this);
	conductorNextCueColor = conductorInfosContainer.addColorParameter("Next cue color", "Text color for your next cue", Colour(196, 0, 0));
	conductorNextCueColor->addParameterListener(this);
	//conductorCurrentCueColor->addParameterListener(ConductorInfos::getInstance());

	colorPickerContainer.saveAndLoadRecursiveData = true;

	CPRedChannel = colorPickerContainer.addTargetParameter("Red channel", "", ChannelFamilyManager::getInstance());
	CPRedChannel->targetType = TargetParameter::CONTAINER;
	CPRedChannel->maxDefaultSearchLevel = 2;

	CPGreenChannel = colorPickerContainer.addTargetParameter("Green channel", "", ChannelFamilyManager::getInstance());
	CPGreenChannel->targetType = TargetParameter::CONTAINER;
	CPGreenChannel->maxDefaultSearchLevel = 2;

	CPBlueChannel = colorPickerContainer.addTargetParameter("Blue channel", "", ChannelFamilyManager::getInstance());
	CPBlueChannel->targetType = TargetParameter::CONTAINER;
	CPBlueChannel->maxDefaultSearchLevel = 2;

	CPCyanChannel = colorPickerContainer.addTargetParameter("Cyan channel", "", ChannelFamilyManager::getInstance());
	CPCyanChannel->targetType = TargetParameter::CONTAINER;
	CPCyanChannel->maxDefaultSearchLevel = 2;

	CPMagentaChannel = colorPickerContainer.addTargetParameter("Magenta channel", "", ChannelFamilyManager::getInstance());
	CPMagentaChannel->targetType = TargetParameter::CONTAINER;
	CPMagentaChannel->maxDefaultSearchLevel = 2;

	CPYellowChannel = colorPickerContainer.addTargetParameter("Yellow channel", "", ChannelFamilyManager::getInstance());
	CPYellowChannel->targetType = TargetParameter::CONTAINER;
	CPYellowChannel->maxDefaultSearchLevel = 2;

	CPHueChannel = colorPickerContainer.addTargetParameter("Hue channel", "", ChannelFamilyManager::getInstance());
	CPHueChannel->targetType = TargetParameter::CONTAINER;
	CPHueChannel->maxDefaultSearchLevel = 2;

	CPSaturationChannel = colorPickerContainer.addTargetParameter("Saturation channel", "", ChannelFamilyManager::getInstance());
	CPSaturationChannel->targetType = TargetParameter::CONTAINER;
	CPSaturationChannel->maxDefaultSearchLevel = 2;

	TPanChannel = trackerContainer.addTargetParameter("Pan channel", "", ChannelFamilyManager::getInstance());
	TPanChannel->targetType = TargetParameter::CONTAINER;
	TPanChannel->maxDefaultSearchLevel = 2;

	TTiltChannel = trackerContainer.addTargetParameter("Tilt channel", "", ChannelFamilyManager::getInstance());
	TTiltChannel->targetType = TargetParameter::CONTAINER;
	TTiltChannel->maxDefaultSearchLevel = 2;


	loadWindowWidth = loadWindowContainer.addIntParameter("Window Width", "", 810,100);
	loadWindowHeight = loadWindowContainer.addIntParameter("Windows Height", "", 610,100);
	loadWindowButtonPerLine = loadWindowContainer.addIntParameter("Buttons per line", "", 5,1);
	loadWindowButtonHeight = loadWindowContainer.addIntParameter("Button height", "", 40,30);

	mainBrain = Brain::getInstance();
	currentDMXChannelView = nullptr;

	addChildControllableContainer(InterfaceManager::getInstance());
	addChildControllableContainer(ChannelFamilyManager::getInstance());
	addChildControllableContainer(FixtureTypeManager::getInstance());
	addChildControllableContainer(FixtureManager::getInstance());
	addChildControllableContainer(GroupManager::getInstance());
	addChildControllableContainer(PresetManager::getInstance());
	addChildControllableContainer(CommandManager::getInstance());
	addChildControllableContainer(CuelistManager::getInstance());
	addChildControllableContainer(ProgrammerManager::getInstance());
	addChildControllableContainer(CurvePresetManager::getInstance());
	addChildControllableContainer(TimingPresetManager::getInstance());
	addChildControllableContainer(EffectManager::getInstance());
	addChildControllableContainer(CarouselManager::getInstance());
	addChildControllableContainer(MapperManager::getInstance());
	addChildControllableContainer(TrackerManager::getInstance());
	addChildControllableContainer(MultiplicatorManager::getInstance());
	addChildControllableContainer(LayoutManager::getInstance());

	addChildControllableContainer(VirtualButtonManager::getInstance());
	addChildControllableContainer(VirtualFaderColManager::getInstance());


	Encoders::getInstance()->engine = this;
	//EncodersMult::getInstance()->engine = this;
	InputPanel::getInstance()->engine = this;
	FixtureGridView::getInstance()->engine = this;
	GroupGridView::getInstance()->engine = this;
	PresetGridView::getInstance()->engine = this;
	CuelistGridView::getInstance()->engine = this;
	EffectGridView::getInstance()->engine = this;
	CarouselGridView::getInstance()->engine = this;
	MapperGridView::getInstance()->engine = this;

	VirtualButtonGrid::getInstance()->engine = this;
	VirtualButtonGrid::getInstance()->initCells();

	VirtualFaderColGrid::getInstance()->engine = this;
	VirtualFaderColGrid::getInstance()->initCells();

	ConductorInfos::getInstance()->engine = this;
	BKColorPicker::getInstance()->engine = this;

	// MIDIManager::getInstance(); //Trigger constructor, declare settings

	// getAppSettings()->addChildControllableContainer(&defaultBehaviors);
}

BKEngine::~BKEngine()
{
	//Application-end cleanup, nothing should be recreated after this
	//delete singletons here

	isClearing = true;

#if JUCE_WINDOWS
	//WindowsHooker::deleteInstance();
#endif

	// ZeroconfManager::deleteInstance();
	// CommunityModuleManager::deleteInstance();
	// ModuleRouterManager::deleteInstance();

	// ChataigneSequenceManager::deleteInstance();
	// StateManager::deleteInstance();
	// ModuleManager::deleteInstance();

	// MIDIManager::deleteInstance();
	// DMXManager::deleteInstance();
	// SerialManager::deleteInstance();
	// WiimoteManager::deleteInstance();

	// InputSystemManager::deleteInstance();
	// StreamDeckManager::deleteInstance();

	// ChataigneAssetManager::deleteInstance();

	// CVGroupManager::deleteInstance();

	// Guider::deleteInstance();
	Brain::getInstance()->stopThread(100);
	Brain::getInstance()->clear();

	if (currentDMXChannelView != nullptr) {
		currentDMXChannelView->setCurrentInterface(nullptr);
	}

	ArtnetSocket::getInstance()->stopThread(100);

	DataTransferManager::deleteInstance();
	FixtureMultiEditor::deleteInstance();

	VirtualButtonManager::deleteInstance();
	VirtualButtonGrid::deleteInstance();
	VirtualFaderColManager::deleteInstance();
	VirtualFaderColGrid::deleteInstance();

	EncodersMult::deleteInstance();
	LayoutManager::deleteInstance();
	MultiplicatorManager::deleteInstance();
	EffectManager::deleteInstance();
	CarouselManager::deleteInstance();
	TrackerManager::deleteInstance();
	MapperManager::deleteInstance();
	ProgrammerManager::deleteInstance();
	CuelistManager::deleteInstance();
	CommandManager::deleteInstance();
	PresetManager::deleteInstance();
	GroupManager::deleteInstance();
	FixtureManager::deleteInstance();
	FixtureTypeManager::deleteInstance();
	ChannelFamilyManager::deleteInstance();
	TimingPresetManager::deleteInstance();
	CurvePresetManager::deleteInstance();

	ConductorInfos::deleteInstance();
	BKColorPicker::deleteInstance();

	InterfaceManager::deleteInstance();
	MIDIManager::deleteInstance();
	DMXManager::deleteInstance();
	SerialManager::deleteInstance();

	ActionFactory::deleteInstance();
	UserInputManager::deleteInstance();

	FixtureGridView::deleteInstance();
	GroupGridView::deleteInstance();
	PresetGridView::deleteInstance();
	CuelistGridView::deleteInstance();
	EffectGridView::deleteInstance();
	CarouselGridView::deleteInstance();
	MapperGridView::deleteInstance();

	CommandLine::deleteInstance();
	Encoders::deleteInstance();
	InputPanel::deleteInstance();
	Assistant::deleteInstance();
	CuelistLoadWindow::deleteInstance();

	ArtnetSocket::getInstance()->deleteInstance();
	Brain::deleteInstance();
}


void BKEngine::createNewGraphInternal()
{
	ProgressTask* loadTask = addTask("loading");
	loadTask->start();
	loadJSONData(JSON::parse(BinaryData::newFileContent_olga), loadTask);
	loadTask->end();
}

void BKEngine::clearInternal()
{
	//clear
	// StateManager::getInstance()->clear();
	// ChataigneSequenceManager::getInstance()->clear();

	// ModuleRouterManager::getInstance()->clear();
	// ModuleManager::getInstance()->clear();
	Brain::getInstance()->skipLoop = true;
	Brain::getInstance()->stopThread(1);
	Brain::getInstance()->clear();

	//DMXChannelView::getInstance()->setCurrentInterface(nullptr);
	if (currentDMXChannelView != nullptr) {
		currentDMXChannelView->setCurrentInterface(nullptr);
	}

	VirtualFaderColManager::getInstance()->clear();
	VirtualButtonManager::getInstance()->clear();
	TrackerManager::getInstance()->clear();
	MapperManager::getInstance()->clear();
	MultiplicatorManager::getInstance()->clear();
	EffectManager::getInstance()->clear();
	CarouselManager::getInstance()->clear();
	EncodersMult::getInstance()->clear();
	ProgrammerManager::getInstance()->clear();
	CuelistManager::getInstance()->clear();
	CommandManager::getInstance()->clear();
	PresetManager::getInstance()->clear();
	GroupManager::getInstance()->clear();
	FixtureManager::getInstance()->clear();
	FixtureTypeManager::getInstance()->clear();
	ChannelFamilyManager::getInstance()->clear();
	InterfaceManager::getInstance()->clear();
	TimingPresetManager::getInstance()->clear();
	CurvePresetManager::getInstance()->clear();
	LayoutManager::getInstance()->clear();

	VirtualButtonGrid::getInstance()->initCells();
	VirtualFaderColGrid::getInstance()->initCells();
	FixtureGridView::getInstance()->updateCells();
	GroupGridView::getInstance()->updateCells();
	PresetGridView::getInstance()->updateCells();
	CuelistGridView::getInstance()->updateCells();
	EffectGridView::getInstance()->updateCells();
	CarouselGridView::getInstance()->updateCells();
	MapperGridView::getInstance()->updateCells();
	Brain::getInstance()->clear();
	Brain::getInstance()->startThread();
	Brain::getInstance()->skipLoop = false;

	autoFillDefaultChannels();
}

var BKEngine::getJSONData()
{
	var data = Engine::getJSONData();

	//var mData = ModuleManager::getInstance()->getJSONData();
	//if (!mData.isVoid() && mData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(ModuleManager::getInstance()->shortName, mData);

	var iData = InterfaceManager::getInstance()->getJSONData();
	if (!iData.isVoid() && iData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(InterfaceManager::getInstance()->shortName, iData);

	var dtData = FixtureTypeManager::getInstance()->getJSONData();
	if (!dtData.isVoid() && dtData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(FixtureTypeManager::getInstance()->shortName, dtData);

	var fptData = ChannelFamilyManager::getInstance()->getJSONData();
	if (!fptData.isVoid() && fptData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(ChannelFamilyManager::getInstance()->shortName, fptData);

	var gData = GroupManager::getInstance()->getJSONData();
	if (!gData.isVoid() && gData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(GroupManager::getInstance()->shortName, gData);

	var pData = PresetManager::getInstance()->getJSONData();
	if (!pData.isVoid() && pData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(PresetManager::getInstance()->shortName, pData);

	var dData = FixtureManager::getInstance()->getJSONData();
	if (!dData.isVoid() && dData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(FixtureManager::getInstance()->shortName, dData);

	var cData = CommandManager::getInstance()->getJSONData();
	if (!cData.isVoid() && cData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(CommandManager::getInstance()->shortName, cData);

	var clData = CuelistManager::getInstance()->getJSONData();
	if (!clData.isVoid() && clData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(CuelistManager::getInstance()->shortName, clData);

	var prData = ProgrammerManager::getInstance()->getJSONData();
	if (!prData.isVoid() && prData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(ProgrammerManager::getInstance()->shortName, prData);

	var cpData = CurvePresetManager::getInstance()->getJSONData();
	if (!cpData.isVoid() && cpData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(CurvePresetManager::getInstance()->shortName, cpData);

	var tpData = TimingPresetManager::getInstance()->getJSONData();
	if (!tpData.isVoid() && tpData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(TimingPresetManager::getInstance()->shortName, tpData);

	var fxData = EffectManager::getInstance()->getJSONData();
	if (!fxData.isVoid() && fxData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(EffectManager::getInstance()->shortName, fxData);

	var carData = CarouselManager::getInstance()->getJSONData();
	if (!carData.isVoid() && carData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(CarouselManager::getInstance()->shortName, carData);

	var tData = MapperManager::getInstance()->getJSONData();
	if (!tData.isVoid() && tData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(MapperManager::getInstance()->shortName, tData);

	var trackData = TrackerManager::getInstance()->getJSONData();
	if (!trackData.isVoid() && trackData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(TrackerManager::getInstance()->shortName, trackData);

	var multData = MultiplicatorManager::getInstance()->getJSONData();
	if (!multData.isVoid() && multData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(MultiplicatorManager::getInstance()->shortName, multData);

	var vbData = VirtualButtonManager::getInstance()->getJSONData();
	if (!vbData.isVoid() && vbData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(VirtualButtonManager::getInstance()->shortName, vbData);

	var vfData = VirtualFaderColManager::getInstance()->getJSONData();
	if (!vfData.isVoid() && vfData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(VirtualFaderColManager::getInstance()->shortName, vfData);

	var layData = LayoutManager::getInstance()->getJSONData();
	if (!layData.isVoid() && layData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(LayoutManager::getInstance()->shortName, layData);

	//var sData = StateManager::getInstance()->getJSONData();
	//if (!sData.isVoid() && sData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(StateManager::getInstance()->shortName, sData);

	//var seqData = ChataigneSequenceManager::getInstance()->getJSONData();
	//if (!seqData.isVoid() && seqData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(ChataigneSequenceManager::getInstance()->shortName, seqData);

	//var rData = ModuleRouterManager::getInstance()->getJSONData();
	//if (!rData.isVoid() && rData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(ModuleRouterManager::getInstance()->shortName, rData);

	return data;
}

void BKEngine::loadJSONDataInternalEngine(var data, ProgressTask* loadingTask)
{
	Brain::getInstance()->loadingIsRunning = true;
	Brain::getInstance()->stopThread(1);
	//ProgressTask* moduleTask = loadingTask->addTask("Modules");
	ProgressTask* iTask = loadingTask->addTask("Interfaces");
	ProgressTask* fptTask = loadingTask->addTask("SubFixture Param Types");
	ProgressTask* dtTask = loadingTask->addTask("Fixture Types");
	ProgressTask* dTask = loadingTask->addTask("Fixtures");
	// ProgressTask* fTask = loadingTask->addTask("SubFixtures");
	ProgressTask* gTask = loadingTask->addTask("Groups");
	ProgressTask* pTask = loadingTask->addTask("Presets");
	ProgressTask* cTask = loadingTask->addTask("Commands");
	ProgressTask* clTask = loadingTask->addTask("Cuelists");
	ProgressTask* prTask = loadingTask->addTask("Programmers");
	ProgressTask* cpTask = loadingTask->addTask("Curve Presets");
	ProgressTask* tpTask = loadingTask->addTask("Timing Presets");
	ProgressTask* fxTask = loadingTask->addTask("Effects");
	ProgressTask* carTask = loadingTask->addTask("Carousels");
	ProgressTask* mapTask = loadingTask->addTask("Mappers");
	ProgressTask* trackTask = loadingTask->addTask("Trackers");
	ProgressTask* multTask = loadingTask->addTask("Multiplicators");
	ProgressTask* layTask = loadingTask->addTask("Layouts");
	ProgressTask* vbTask = loadingTask->addTask("Virtual buttons");
	ProgressTask* vfTask = loadingTask->addTask("Virtual faders");
	//ProgressTask* stateTask = loadingTask->addTask("States");
	//ProgressTask* sequenceTask = loadingTask->addTask("Sequences");
	//ProgressTask* routerTask = loadingTask->addTask("Router");

	//moduleTask->start();
	//ModuleManager::getInstance()->loadJSONData(data.getProperty(ModuleManager::getInstance()->shortName, var()));
	//moduleTask->setProgress(1);
	//moduleTask->end();

	iTask->start();
	InterfaceManager::getInstance()->loadJSONData(data.getProperty(InterfaceManager::getInstance()->shortName, var()));
	iTask->setProgress(1);
	iTask->end();

	fptTask->start();
	ChannelFamilyManager::getInstance()->loadJSONData(data.getProperty(ChannelFamilyManager::getInstance()->shortName, var()));
	fptTask->setProgress(1);
	fptTask->end();

	dtTask->start();
	FixtureTypeManager::getInstance()->loadJSONData(data.getProperty(FixtureTypeManager::getInstance()->shortName, var()));
	dtTask->setProgress(1);
	dtTask->end();

	gTask->start();
	GroupManager::getInstance()->loadJSONData(data.getProperty(GroupManager::getInstance()->shortName, var()));
	gTask->setProgress(1);
	gTask->end();

	pTask->start();
	PresetManager::getInstance()->loadJSONData(data.getProperty(PresetManager::getInstance()->shortName, var()));
	pTask->setProgress(1);
	pTask->end();

	dTask->start();
	FixtureManager::getInstance()->loadJSONData(data.getProperty(FixtureManager::getInstance()->shortName, var()));
	dTask->setProgress(1);
	dTask->end();

	cTask->start();
	CommandManager::getInstance()->loadJSONData(data.getProperty(CommandManager::getInstance()->shortName, var()));
	cTask->setProgress(1);
	cTask->end();

	clTask->start();
	CuelistManager::getInstance()->loadJSONData(data.getProperty(CuelistManager::getInstance()->shortName, var()));
	clTask->setProgress(1);
	clTask->end();

	prTask->start();
	ProgrammerManager::getInstance()->loadJSONData(data.getProperty(ProgrammerManager::getInstance()->shortName, var()));
	prTask->setProgress(1);
	prTask->end();

	cpTask->start();
	CurvePresetManager::getInstance()->loadJSONData(data.getProperty(CurvePresetManager::getInstance()->shortName, var()));
	cpTask->setProgress(1);
	cpTask->end();

	tpTask->start();
	TimingPresetManager::getInstance()->loadJSONData(data.getProperty(TimingPresetManager::getInstance()->shortName, var()));
	tpTask->setProgress(1);
	tpTask->end();

	fxTask->start();
	EffectManager::getInstance()->loadJSONData(data.getProperty(EffectManager::getInstance()->shortName, var()));
	fxTask->setProgress(1);
	fxTask->end();

	carTask->start();
	CarouselManager::getInstance()->loadJSONData(data.getProperty(CarouselManager::getInstance()->shortName, var()));
	carTask->setProgress(1);
	carTask->end();

	mapTask->start();
	MapperManager::getInstance()->loadJSONData(data.getProperty(MapperManager::getInstance()->shortName, var()));
	mapTask->setProgress(1);
	mapTask->end();

	trackTask->start();
	TrackerManager::getInstance()->loadJSONData(data.getProperty(TrackerManager::getInstance()->shortName, var()));
	trackTask->setProgress(1);
	trackTask->end();

	multTask->start();
	MultiplicatorManager::getInstance()->loadJSONData(data.getProperty(MultiplicatorManager::getInstance()->shortName, var()));
	multTask->setProgress(1);
	multTask->end();

	layTask->start();
	LayoutManager::getInstance()->loadJSONData(data.getProperty(LayoutManager::getInstance()->shortName, var()));
	layTask->setProgress(1);
	layTask->end();

	vbTask->start();
	VirtualButtonManager::getInstance()->loadJSONData(data.getProperty(VirtualButtonManager::getInstance()->shortName, var()));
	vbTask->setProgress(1);
	vbTask->end();

	vfTask->start();
	VirtualFaderColManager::getInstance()->loadJSONData(data.getProperty(VirtualFaderColManager::getInstance()->shortName, var()));
	vfTask->setProgress(1);
	vfTask->end();

	//stateTask->start();
	//StateManager::getInstance()->loadJSONData(data.getProperty(StateManager::getInstance()->shortName, var()));
	//stateTask->setProgress(1);
	//stateTask->end();

	//sequenceTask->start();
	//ChataigneSequenceManager::getInstance()->loadJSONData(data.getProperty(ChataigneSequenceManager::getInstance()->shortName, var()));
	//sequenceTask->setProgress(1);
	//sequenceTask->end();

	//routerTask->start();
	//ModuleRouterManager::getInstance()->loadJSONData(data.getProperty(ModuleRouterManager::getInstance()->shortName, var()));
	//routerTask->setProgress(1);
	//routerTask->end();

	FixtureGridView::getInstance()->updateCells();
	GroupGridView::getInstance()->updateCells();
	PresetGridView::getInstance()->updateCells();
	CuelistGridView::getInstance()->updateCells();
	EffectGridView::getInstance()->updateCells();
	CarouselGridView::getInstance()->updateCells();
	MapperGridView::getInstance()->updateCells();

	VirtualButtonGrid::getInstance()->page = 1;
	VirtualButtonGrid::getInstance()->initCells();

	VirtualFaderColGrid::getInstance()->page = 1;
	VirtualFaderColGrid::getInstance()->initCells();

	Brain::getInstance()->clearUpdates();
	Brain::getInstance()->loadingIsRunning = false;
	Brain::getInstance()->startThread();

	VirtualButtonGrid::getInstance()->resetFeedbacks();
	VirtualFaderColGrid::getInstance()->resetFeedbacks();
	Brain::getInstance()->updateAllChannels();
	//EncodersMult::getInstance()->updateChannels();

	autoFillDefaultChannels();
	Encoders::getInstance()->updateChannels();
	EncodersMult::getInstance()->targetChanged();

	ConductorInfos::getInstance()->linkFadeSlider();

	for (int i = 0; i < FixtureManager::getInstance()->items.size(); i++) {
		Fixture* f = FixtureManager::getInstance()->items[i];
		for (int p = 0; p < f->patchs.items.size(); p++) {
			if (f->patchs.items[p]->enabled->boolValue()) {
				f->patchs.items[p]->tryToEnablePatch();
			}
		}
	}

	for (int i = 0; i < VirtualButtonManager::getInstance()->items.size(); i++) {
		VirtualButtonManager::getInstance()->items[i]->updateDisplay();
	}
	for (int i = 0; i < VirtualFaderColManager::getInstance()->items.size(); i++) {
		VirtualFaderColManager::getInstance()->items[i]->updateDisplay();
	}

}

void BKEngine::childStructureChanged(ControllableContainer* cc)
{
	Engine::childStructureChanged(cc);
}

void BKEngine::controllableFeedbackUpdate(ControllableContainer* cc, Controllable* c)
{
	if (isClearing || isLoadingFile) return;
	if (cc == &conductorInfosContainer) {
		ConductorInfos::getInstance()->repaint();
	}
}

void BKEngine::handleAsyncUpdate()
{
	Engine::handleAsyncUpdate();
}

String BKEngine::getMinimumRequiredFileVersion()
{
	return "1.0.0";
}

void BKEngine::importSelection()
{
	const MessageManagerLock mmLock;
	FileChooser fc("Load some files", File::getCurrentWorkingDirectory(), "*.mochi,*.gdtf,*.mvr");
	if (!fc.browseForMultipleFilesToOpen()) return;
	Array<File> f = fc.getResults();
	for (int i = 0; i < f.size(); i++) {
		if (f[i].hasFileExtension("mochi")) {
			importMochi(f[i]);
		}
		else if (f[i].hasFileExtension("gdtf")) {
			importGDTF(f[i]);
		}
		else if (f[i].hasFileExtension("mvr")) {
			importMVR(f[i]);
		}
	}
}

void BKEngine::importMochi(File f) {
	var data = JSON::parse(f);
	importMochi(data);
}

void BKEngine::importMochi(var data)
{
	if (!data.isObject()) return;

	InterfaceManager::getInstance()->addItemsFromData(data.getProperty(InterfaceManager::getInstance()->shortName, var()));
	ChannelFamilyManager::getInstance()->importData(data.getProperty(ChannelFamilyManager::getInstance()->shortName, var()));
	FixtureTypeManager::getInstance()->addItemsFromData(data.getProperty(FixtureTypeManager::getInstance()->shortName, var()));
	FixtureManager::getInstance()->addItemsFromData(data.getProperty(FixtureManager::getInstance()->shortName, var()));
	GroupManager::getInstance()->addItemsFromData(data.getProperty(GroupManager::getInstance()->shortName, var()));
	PresetManager::getInstance()->addItemsFromData(data.getProperty(PresetManager::getInstance()->shortName, var()));
	CommandManager::getInstance()->addItemsFromData(data.getProperty(CommandManager::getInstance()->shortName, var()));
	CuelistManager::getInstance()->addItemsFromData(data.getProperty(CuelistManager::getInstance()->shortName, var()));
	ProgrammerManager::getInstance()->addItemsFromData(data.getProperty(ProgrammerManager::getInstance()->shortName, var()));
	CurvePresetManager::getInstance()->addItemsFromData(data.getProperty(CurvePresetManager::getInstance()->shortName, var()));
	TimingPresetManager::getInstance()->addItemsFromData(data.getProperty(TimingPresetManager::getInstance()->shortName, var()));
	CarouselManager::getInstance()->addItemsFromData(data.getProperty(CarouselManager::getInstance()->shortName, var()));
	MapperManager::getInstance()->addItemsFromData(data.getProperty(MapperManager::getInstance()->shortName, var()));
	TrackerManager::getInstance()->addItemsFromData(data.getProperty(TrackerManager::getInstance()->shortName, var()));
	MultiplicatorManager::getInstance()->addItemsFromData(data.getProperty(MultiplicatorManager::getInstance()->shortName, var()));
	EffectManager::getInstance()->addItemsFromData(data.getProperty(EffectManager::getInstance()->shortName, var()));
	VirtualButtonManager::getInstance()->addItemsFromData(data.getProperty(VirtualButtonManager::getInstance()->shortName, var()));
	VirtualFaderColManager::getInstance()->addItemsFromData(data.getProperty(VirtualFaderColManager::getInstance()->shortName, var()));

	FixtureGridView::getInstance()->updateCells();
	GroupGridView::getInstance()->updateCells();
	PresetGridView::getInstance()->updateCells();
	CuelistGridView::getInstance()->updateCells();
	EffectGridView::getInstance()->updateCells();
	CarouselGridView::getInstance()->updateCells();
	MapperGridView::getInstance()->updateCells();

	autoFillDefaultChannels();
}

void BKEngine::importGDTF(File f)
{
	std::shared_ptr<ZipFile> archive = std::make_shared<ZipFile>(f);
	int descIndex = archive->getIndexOfFileName("description.xml");
	if (descIndex == -1) {
		LOGERROR("the file "+f.getFileName()+" is not a valid GDTF File (no description.xml in the archive)");
		return;
	}

	InputStream* stream = archive->createStreamForEntry(descIndex);
	importGDTFContent(stream, "");
	stream->~InputStream();
}

FixtureType* BKEngine::importGDTF(InputStream* stream, String modeName)
{
	std::shared_ptr<ZipFile> archive = std::make_shared<ZipFile>(stream, false);
	int descIndex = archive->getIndexOfFileName("description.xml");
	if (descIndex == -1) {
		LOGERROR("the file is not a valid GDTF File (no description.xml in the archive)");
		return nullptr;
	}

	return importGDTFContent(archive->createStreamForEntry(descIndex), modeName);
}

FixtureType* BKEngine::importGDTFContent(InputStream* stream, String importModeName)
{
	HashMap<String, String> changedNames;
	changedNames.set("Dimmer", "Intensity");
	changedNames.set("ColorAdd_R", "Red");
	changedNames.set("ColorAdd_G", "Green");
	changedNames.set("ColorAdd_B", "Blue");
	changedNames.set("ColorAdd_W", "White");
	changedNames.set("ColorAdd_A", "Amber");
	changedNames.set("ColorSub_C", "Cyan");
	changedNames.set("ColorSub_M", "Magenta");
	changedNames.set("ColorSub_Y", "Yellow");

	XmlDocument descriptionXml = stream->readString();
	auto rootElmt = descriptionXml.getDocumentElement();
	int nChildren = rootElmt->getNumChildElements();
	HashMap<int, int> breakOffsets;
	HashMap<String, ChannelType*> nameToChannelType;

	FixtureType* ret = nullptr;

	for (int iFam = 0; iFam < ChannelFamilyManager::getInstance()->items.size(); iFam++) {
		for (int iChan = 0; iChan < ChannelFamilyManager::getInstance()->items[iFam]->definitions.items.size(); iChan++) {
			ChannelType* ct = ChannelFamilyManager::getInstance()->items[iFam]->definitions.items[iChan];
			nameToChannelType.set(ct->niceName, ct);
		}
	}

	for (int indexChild = 0; indexChild < nChildren; indexChild++) {
		auto fixtureTypeNode = rootElmt->getChildElement(indexChild);
		if (fixtureTypeNode->getTagName().toLowerCase() == "fixturetype") {
			String fixtureName = "";
			String manufacturer = "";
			fixtureName += fixtureTypeNode->getStringAttribute("Name");
			manufacturer += fixtureTypeNode->getStringAttribute("Manufacturer");

			XmlElement* attributesNode = fixtureTypeNode->getChildByName("AttributeDefinitions");
			if (attributesNode == nullptr) { LOGERROR("import not finished, the fixture has no attributes tag"); }
			auto attributes = attributesNode->getChildByName("Attributes");
			for (int i = 0; i < attributes->getNumChildElements(); i++) {
				auto attr = attributes->getChildElement(i);
				String attrName = attr->getStringAttribute("Name");
				if (changedNames.contains(attrName)) { attrName = changedNames.getReference(attrName); }
				String attrFamilyName = StringArray::fromTokens(attr->getStringAttribute("Feature"), ".", "")[0];

				if (!nameToChannelType.contains(attrName)) {
					ChannelFamily* cf = nullptr;
					ChannelType* ct = nullptr;
					for (int iFam = 0; iFam < ChannelFamilyManager::getInstance()->items.size(); iFam++) {
						if (ChannelFamilyManager::getInstance()->items[iFam]->niceName.toLowerCase() == attrFamilyName.toLowerCase()) {
							cf = ChannelFamilyManager::getInstance()->items[iFam];
						}
					}
					if (cf == nullptr) {
						cf = ChannelFamilyManager::getInstance()->addItem();
						cf->setNiceName(attrFamilyName);
					}

					for (int iChan = 0; iChan < cf->definitions.items.size(); iChan++) {
						if (cf->definitions.items[iChan]->niceName.toLowerCase() == attrName.toLowerCase()) {
							ct = cf->definitions.items[iChan];
						}
					}
					if (ct == nullptr) {
						ct = cf->definitions.addItem();
						nameToChannelType.set(attrName, ct);
						ct->setNiceName(attrName);
					}
				}
			}

			XmlElement* geometriesNode = fixtureTypeNode->getChildByName("Geometries");
			if (geometriesNode == nullptr) { LOGERROR("import not finished, the fixture has no geometries tag"); }
			HashMap<String, XmlElement* > mainGeometries;
			for (int i = 0; i < geometriesNode->getNumChildElements(); i++) {
				auto mainGeo = geometriesNode->getChildElement(i);
				String geoName = mainGeo->getStringAttribute("Name");
				mainGeometries.set(geoName, mainGeo);
			}

			XmlElement* dmxModesNode = fixtureTypeNode->getChildByName("DMXModes");
			if (dmxModesNode == nullptr) { LOGERROR("import not finished, the fixture has no DMXModes tag"); }
			for (int iMode = 0; iMode < dmxModesNode->getNumChildElements(); iMode++) {
				auto modeNode = dmxModesNode->getChildElement(iMode);
				String modeName = modeNode->getStringAttribute("Name");
				if (importModeName == "" || importModeName == modeName) {

					FixtureType* ft = FixtureTypeManager::getInstance()->addItem();
					ret = ft;
					ft->setNiceName(fixtureName + " - " + modeName);

					Array<tempChannel> tempChannels;
					Array<String> getMasterDimmer;
					HashMap<int, FixtureTypeVirtualChannel*> subIdToVirtDimmer;
					auto modeRelations = modeNode->getChildByName("Relations");
					if (modeRelations != nullptr) {
						for (int iRel = 0; iRel < modeRelations->getNumChildElements(); iRel++) {
							getMasterDimmer.add(modeRelations->getChildElement(iRel)->getStringAttribute("Follower"));
						}
					}
					XmlElement* modeGeometry = mainGeometries.getReference(modeNode->getStringAttribute("Geometry"));
					Array<String> subFixtureNames;
					auto modeChannels = modeNode->getChildByName("DMXChannels");
					if (modeChannels != nullptr) {
						for (int iChan = 0; iChan < modeChannels->getNumChildElements(); iChan++) {
							auto dmxChannelNode = modeChannels->getChildElement(iChan);
							auto logicalChannelNode = dmxChannelNode->getChildByName("LogicalChannel");
							String DMXOffset = dmxChannelNode->getStringAttribute("Offset");
							String attribute = logicalChannelNode->getStringAttribute("Attribute");
							String geometry = dmxChannelNode->getStringAttribute("Geometry");
							String initialFunction = dmxChannelNode->getStringAttribute("InitialFunction");
							int dmxBreak = dmxChannelNode->getIntAttribute("DMXBreak");
							int dmxAdress = 0;
							int resolution = 0;
							if (DMXOffset == "") {
								// virtual
							}
							else if (DMXOffset.indexOf(",") != -1) {
								dmxAdress = StringArray::fromTokens(DMXOffset, ",", "")[0].getIntValue();
								resolution = 2;
							}
							else {
								dmxAdress = DMXOffset.getIntValue();
								resolution = 1;
							}
							Array<geometryBreaks> breaks;

							getBreakOffset(modeGeometry, geometry, dmxBreak, &breaks);
							if (dmxAdress > 0) {
								if (breaks.size() > 0) {
									for (int i = 0; i < breaks.size(); i++) {
										subFixtureNames.addIfNotAlreadyThere(breaks[i].name);
										tempChannel tc;
										tc.attribute = attribute;
										tc.resolution = resolution;
										tc.subFixtId = subFixtureNames.indexOf(breaks[i].name) + 1;
										tc.initialFunction = initialFunction;
										int index = dmxAdress;
										index += breaks[i].offset;
										index -= 2;
										while (tempChannels.size() < index) { tempChannels.add(tempChannel()); }
										tempChannels.set(index, tc);
									}
								}
								else {
									tempChannel tc;
									tc.attribute = attribute;
									tc.resolution = resolution;
									tc.initialFunction = initialFunction;
									if (subFixtureNames.indexOf(geometry) == -1) { subFixtureNames.add(geometry); }
									tc.subFixtId = subFixtureNames.indexOf(geometry) + 1;
									int index = dmxAdress - 1;
									while (tempChannels.size() < index) { tempChannels.add(tempChannel()); }
									tempChannels.set(index, tc);
								}
							}
						}
					}

					// got all channels
					for (int i = 0; i < tempChannels.size(); i++) {
						if (tempChannels[i].attribute != "") {
							String attrName = tempChannels[i].attribute;
							FixtureTypeChannel* ftc = ft->chansManager.addItem(nullptr, var(), false, false);
							if (changedNames.contains(attrName)) { attrName = changedNames.getReference(attrName); }

							ftc->channelType->setValueFromTarget(nameToChannelType.getReference(attrName));
							ftc->subFixtureId->setValue(tempChannels[i].subFixtId);
							if (tempChannels[i].resolution == 2)
							{
								ftc->resolution->setValue("16bits");
							}
							if (getMasterDimmer.contains(tempChannels[i].initialFunction)) {
								if (!subIdToVirtDimmer.contains(tempChannels[i].subFixtId)) {
									FixtureTypeVirtualChannel* virtDim = ft->virtualChansManager.addItem(nullptr, var(),false, false);
									subIdToVirtDimmer.set(tempChannels[i].subFixtId, virtDim);
									virtDim->channelType->setValueFromTarget(nameToChannelType.getReference("Intensity"));
									virtDim->subFixtureId->setValue(tempChannels[i].subFixtId);
									virtDim->setNiceName("Dimmer " + String(tempChannels[i].subFixtId));
								}
								ftc->virtualMaster->setValueFromTarget(subIdToVirtDimmer.getReference(tempChannels[i].subFixtId));
							}
						}
					}
				}
			}
			LOG("ok");
		}
	}
	LOG("Import done !");
	return ret;
}

void BKEngine::getBreakOffset(XmlElement* tag, String geometryName, int dmxBreak, Array<geometryBreaks>* breaks)
{
	if (tag->getTagName() == "Geometry") { 
		for (int i = 0; i < tag->getNumChildElements(); i++) {
			getBreakOffset(tag->getChildElement(i), geometryName, dmxBreak, breaks);
		}
	}
	else if (tag->getTagName() == "GeometryReference" && tag->getStringAttribute("Geometry") == geometryName) {
		for (int i = 0; i < tag->getNumChildElements(); i++) {
			auto br = tag->getChildElement(i);
			if (br->getTagName() == "Break" && dmxBreak == br->getStringAttribute("DMXBreak").getIntValue()) {
				geometryBreaks temp;
				temp.name = tag->getStringAttribute("Name");
				temp.offset = br->getStringAttribute("DMXOffset").getIntValue();
				breaks->add(temp);
			}
		}
	}
}

void BKEngine::importMVR(File f)
{
	std::shared_ptr<ZipFile> archive = std::make_shared<ZipFile>(f);
	int descIndex = archive->getIndexOfFileName("GeneralSceneDescription.xml");
	if (descIndex == -1) {
		LOGERROR("the file " + f.getFileName() + " is not a valid MVVR File (no GeneralSceneDescription.xml in the archive)");
		return;
	}

	XmlDocument descriptionXml = archive->createStreamForEntry(descIndex)->readString();
	auto rootElmt = descriptionXml.getDocumentElement();

	int nLayers = rootElmt->getNumChildElements();
	
	HashMap<String, FixtureType*> fixtureTypesMap;
	HashMap<int, Fixture*> fixturesMap;
	HashMap<int, std::shared_ptr<Array<int>> > fixtureAddressesMap;

	int maxAddress = 0;

	Layout* frontLayout = nullptr;
	Layout* topLayout = nullptr;
	Layout* sideLayout = nullptr;

	for (int indexScene = 0; indexScene < nLayers; indexScene++) {
		auto sceneNode = rootElmt->getChildElement(indexScene);
		if (sceneNode->getTagName().toLowerCase() == "scene") {
			for (int indexScene2 = 0; indexScene2 < sceneNode->getNumChildElements(); indexScene2++) {
				auto layersNode = sceneNode->getChildElement(indexScene2);
				if (layersNode->getTagName().toLowerCase() == "layers") {
					for (int indexLayer = 0; indexLayer < layersNode->getNumChildElements(); indexLayer++) {
						auto layerNode = layersNode->getChildElement(indexLayer);
						if (layerNode->getTagName().toLowerCase() == "layer") {
							for (int indexLayer2 = 0; indexLayer2 < layerNode->getNumChildElements(); indexLayer2++) {
								auto childListNode = layerNode->getChildElement(indexLayer2);
								if (childListNode != nullptr && childListNode->getTagName().toLowerCase() == "childlist") {
									int nChildren = childListNode->getNumChildElements();
									for (int indexChild = 0; indexChild < nChildren; indexChild++) {
										auto child = childListNode->getChildElement(indexChild);
										String childType = child->getTagName().toLowerCase();
										if (childType == "fixture") {
											bool valid = true;
											int id = 0;
											String spec = "";
											String mode = "";
											if (child->getChildByName("GDTFSpec") != nullptr) { spec = child->getChildByName("GDTFSpec")->getAllSubText().trim(); }
											else { valid = false; }
											if (child->getChildByName("GDTFMode") != nullptr) { mode = child->getChildByName("GDTFMode")->getAllSubText().trim(); }
											else { valid = false; }
											String name = child->getStringAttribute("name");

											id = child->getChildByName("FixtureID")->getAllSubText().trim().getIntValue();
											if (id == 0) {
												id = child->getChildByName("UnitNumber")->getAllSubText().trim().getIntValue();
											}

											if (valid) {

												if (id == 0) {
													id = 1000;
													while (fixturesMap.contains(id)) {
														id++;
													}
												}

												String ftName = spec + " - " + mode;
												FixtureType* ft = nullptr;
												if (!fixtureTypesMap.contains(ftName)) {
													int gdtfIndex = archive->getIndexOfFileName(spec);
													if (gdtfIndex != -1) {
														InputStream* s = archive->createStreamForEntry(gdtfIndex);
														ft = importGDTF(s, mode);
														s->~InputStream();

													}
													if (ft == nullptr) {
														ft = FixtureTypeManager::getInstance()->addItem();
														ft->setNiceName(ftName);
													}
													fixtureTypesMap.set(ftName, ft);
												}
												ft = fixtureTypesMap.getReference(ftName);

												Fixture* fixt;
												if (!fixturesMap.contains(id)) {
													fixt = FixtureManager::getInstance()->addItem();
													fixturesMap.set(id, fixt);
													fixtureAddressesMap.set(id, std::make_shared<Array<int>>());
													fixt->id->setValue(id);
													fixt->userName->setValue(name);
													fixt->devTypeParam->setValueFromTarget(ft);
												}

												if (child->getChildByName("Addresses") != nullptr) {
													auto addressesNode = child->getChildByName("Addresses");
													std::shared_ptr<Array<int>> fixtAddresses = fixtureAddressesMap.getReference(id);
													for (int indexAddress = 0; indexAddress < addressesNode->getNumChildElements(); indexAddress++) {
														auto tag = addressesNode->getChildElement(indexAddress);
														int address = tag->getAllSubText().trim().getIntValue();
														maxAddress = jmax(address, maxAddress);
														fixtAddresses->addIfNotAlreadyThere(address);
													}
												}

												if (child->getChildByName("Matrix") != nullptr) {
													auto matrixNode = child->getChildByName("Matrix");
													String matrixValue = matrixNode->getAllSubText();
													matrixValue = matrixValue.replace("}{", ",");
													matrixValue = matrixValue.replace("}", "");
													matrixValue = matrixValue.replace("{", "");
													auto matVals = StringArray::fromTokens(matrixValue, ",", "");

													if (matVals.size() == 12 && fixt != nullptr) {
														if (frontLayout == nullptr) {
															frontLayout = LayoutManager::getInstance()->addItem();
															frontLayout->userName->setValue("MVR Front layout");
														}
														if (topLayout == nullptr) {
															topLayout = LayoutManager::getInstance()->addItem();
															topLayout->userName->setValue("MVR Top layout");
														}
														if (sideLayout == nullptr) {
															sideLayout = LayoutManager::getInstance()->addItem();
															sideLayout->userName->setValue("MVR Side layout");
														}
														float x = matVals[9].getFloatValue()/1000;
														float y = matVals[10].getFloatValue()/1000;
														float z = matVals[11].getFloatValue()/1000;

														frontLayout->createPathForFixture(fixt, x, z);
														topLayout->createPathForFixture(fixt, x, y);
														sideLayout->createPathForFixture(fixt, y, z);
													}
												}
											}
										}
									}
								}
							}
						}
					}

				}
			}
		}
	}
	Array<DMXInterface*> universes;
	int currentUniverse = 1;

	for (int i = 0; i < maxAddress; i = i + 512) {
		DMXInterface* u = new DMXInterface();
		InterfaceManager::getInstance()->addItem(u);
		universes.add(u);
		u->setNiceName("MVR universe "+String(currentUniverse));
		currentUniverse++;
	}

	for (auto it = fixturesMap.begin(); it != fixturesMap.end(); it.next()) {
		int id = it.getKey();
		Fixture* fixt = it.getValue();
		
		std::shared_ptr<Array<int>> addresses = fixtureAddressesMap.getReference(id);
		for (int i = 0; i < addresses->size(); i++) {
			int globalAddress = addresses->getReference(i) - 1;
			int dmxAddress = globalAddress%512;
			int universeIndex = globalAddress/512;
			DMXInterface* u = universes[universeIndex];
			FixturePatch* p = fixt->patchs.addItem();
			p->targetInterface->setValueFromTarget(u);
			p->address->setValue(dmxAddress+1);
		}
	}

	if (frontLayout != nullptr) { frontLayout->fitToContent(); }
	if (topLayout != nullptr) { topLayout->fitToContent(); }
	if (sideLayout != nullptr) { sideLayout->fitToContent(); }

	LOG("Import done !");
}

void BKEngine::exportSelection()
{
	var data(new DynamicObject());

	data.getDynamicObject()->setProperty(InterfaceManager::getInstance()->shortName, InterfaceManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(ChannelFamilyManager::getInstance()->shortName, ChannelFamilyManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(FixtureTypeManager::getInstance()->shortName, FixtureTypeManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(FixtureManager::getInstance()->shortName, FixtureManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(GroupManager::getInstance()->shortName, GroupManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(PresetManager::getInstance()->shortName, PresetManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(CommandManager::getInstance()->shortName, CommandManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(CuelistManager::getInstance()->shortName, CuelistManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(ProgrammerManager::getInstance()->shortName, ProgrammerManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(CurvePresetManager::getInstance()->shortName, CurvePresetManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(TimingPresetManager::getInstance()->shortName, TimingPresetManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(CarouselManager::getInstance()->shortName, CarouselManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(MapperManager::getInstance()->shortName, MapperManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(MultiplicatorManager::getInstance()->shortName, MultiplicatorManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(EffectManager::getInstance()->shortName, EffectManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(VirtualButtonManager::getInstance()->shortName, VirtualButtonManager::getInstance()->getExportSelectionData());
	data.getDynamicObject()->setProperty(VirtualFaderColManager::getInstance()->shortName, VirtualFaderColManager::getInstance()->getExportSelectionData());

	String s = JSON::toString(data);

	FileChooser fc("Save a mochi", File::getCurrentWorkingDirectory(), "*.mochi");
	if (fc.browseForFileToSave(true))
	{
		File f = fc.getResult();
		f.replaceWithText(s);
	}
}

void BKEngine::parameterValueChanged(Parameter* p) {
	Engine::parameterValueChanged(p);
	/*if (p == panelScale) {
		InputPanel::getInstance()->resized();
	}
	else */
	if (p == encodersScale) {
		Encoders::getInstance()->resized();
		//EncodersMult::getInstance()->resized();
	}
	else if (p == gridScale || p == gridCols) {
		FixtureGridView::getInstance()->resized();
		GroupGridView::getInstance()->resized();
		PresetGridView::getInstance()->resized();
		CuelistGridView::getInstance()->resized();
		EffectGridView::getInstance()->resized();
		CarouselGridView::getInstance()->resized();
		MapperGridView::getInstance()->resized();
	}
	else if (p == virtualButtonGridCols || p == virtualButtonGridRows) {
		VirtualButtonGrid::getInstance()->initCells();
	}
	else if (p == virtualFaderCols || p == virtualFaderRotary || p == virtualFaderAbove || p == virtualFaderSize || p == virtualFaderBelow) {
		VirtualFaderColGrid::getInstance()->initCells();
	}
	else if (p == conductorCuelistId || p == conductorTextSize || p == conductorTitleSize || p == conductorCurrentCueColor || p == conductorNextCueColor) {
		if (p == conductorCuelistId) {
			ConductorInfos::getInstance()->linkFadeSlider();
		}
		ConductorInfos::getInstance()->repaint();
	} else if (p == encodersNumber) {
		Encoders::getInstance()->initEncoders();
	}

}

void BKEngine::autoFillDefaultChannels()
{	
	ChannelFamily* col = ChannelFamilyManager::getInstance()->getItemWithName("Color");
	if (col != nullptr) {
		auto Red = col->definitions.getItemWithName("Red");
		if (Red!= nullptr) {
			CPRedChannel->setValueFromTarget(Red);
		}
		auto Green = col->definitions.getItemWithName("Green");
		if (Green!= nullptr) {
			CPGreenChannel->setValueFromTarget(Green);
		}
		auto Blue = col->definitions.getItemWithName("Blue");
		if (Blue!= nullptr) {
			CPBlueChannel->setValueFromTarget(Blue);
		}
		auto Cyan = col->definitions.getItemWithName("Cyan");
		if (Cyan!= nullptr) {
			CPCyanChannel->setValueFromTarget(Cyan);
		}
		auto Magenta = col->definitions.getItemWithName("Magenta");
		if (Magenta!= nullptr) {
			CPMagentaChannel->setValueFromTarget(Magenta);
		}
		auto Yellow = col->definitions.getItemWithName("Yellow");
		if (Yellow!= nullptr) {
			CPYellowChannel->setValueFromTarget(Yellow);
		}
		auto Hue = col->definitions.getItemWithName("Hue");
		if (Hue!= nullptr) {
			CPHueChannel->setValueFromTarget(Hue);
		}
		auto Sat = col->definitions.getItemWithName("Sat");
		if (Sat!= nullptr) {
			CPSaturationChannel->setValueFromTarget(Sat);
		}
	}

	ChannelFamily* pos = ChannelFamilyManager::getInstance()->getItemWithName("Position");
	if (pos != nullptr) {
		auto Pan = pos->definitions.getItemWithName("Pan");
		if (Pan != nullptr) {
			TPanChannel->setValueFromTarget(Pan);
		}
		auto Tilt = pos->definitions.getItemWithName("Tilt");
		if (Tilt != nullptr) {
			TTiltChannel->setValueFromTarget(Tilt);
		}
	}

}

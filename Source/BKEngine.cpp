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
#include "./Definitions/Tracker/TrackerManager.h"

#include "./Definitions/DataTransferManager/DataTransferManager.h"
#include "./Definitions/Fixture/FixtureMultiEditor.h"

#include "./Common/MIDI/MIDIDevice.h"
#include "./Common/MIDI/MIDIManager.h"
#include "./Common/DMX/DMXManager.h"
#include "./Common/DMX/Device/DMXDevice.h"
#include "./Common/Serial/SerialDevice.h"
#include "./Common/Serial/SerialManager.h"

#include "./Common/Action/Action.h"
#include "./Common/Action/ActionManager.h"

#include "UI/CommandLine.h"
#include "UI/Encoders.h"
#include "UI/InputPanel.h"

#include "UI/GridView/FixtureGridView.h"
#include "UI/GridView/GroupGridView.h"
#include "UI/GridView/PresetGridView.h"
#include "UI/GridView/CuelistGridView.h"
#include "UI/GridView/EffectGridView.h"
#include "UI/GridView/CarouselGridView.h"
#include "UI/GridView/TrackerGridView.h"

#include "UI/VirtualButtons/VirtualButtonManager.h"
#include "UI/VirtualButtons/VirtualButtonGrid.h"

#include "UI/VirtualFaders/VirtualFaderColManager.h"
#include "UI/VirtualFaders/VirtualFaderColGrid.h"

#include "UserInputManager.h"

ControllableContainer* getAppSettings();

BKEngine::BKEngine() :
	Engine("BlinderKitten", ".olga"),
	virtualParamsContainer("Virtual Playbacks Settings"),
	uiParamsContainer("UI Settings")
	//defaultBehaviors("Test"),
	//ossiaFixture(nullptr)
{
	convertURL = "http://hazlab.fr/";
	
	//Communication
	OSCRemoteControl::getInstance()->addRemoteControlListener(UserInputManager::getInstance());
	//init here
	Engine::mainEngine = this;

	GlobalSettings::getInstance()->addChildControllableContainer(&virtualParamsContainer);
	GlobalSettings::getInstance()->addChildControllableContainer(&uiParamsContainer);

	virtualButtonGridCols = virtualParamsContainer.addIntParameter("Button cols", "Number of cols in playback button grid", 5, 1);
	virtualButtonGridCols->addParameterListener(this);
	virtualButtonGridRows = virtualParamsContainer.addIntParameter("Button rows", "Number of rows in playback button grid", 5, 1);
	virtualButtonGridRows->addParameterListener(this);

	virtualFaderCols = virtualParamsContainer.addIntParameter("Fader cols", "Number of cols in playback button grid", 5, 1);
	virtualFaderCols->addParameterListener(this);
	virtualFaderRotary = virtualParamsContainer.addIntParameter("Rotary number", "Number of rotary in each fader column", 1, 0);
	virtualFaderRotary->addParameterListener(this);
	virtualFaderAbove = virtualParamsContainer.addIntParameter("Above button numbers", "Number of buttons above the fader in each fader column", 1, 0);
	virtualFaderAbove->addParameterListener(this);
	virtualFaderSize = virtualParamsContainer.addIntParameter("Fader Size", "Size of the fader (unit is button size)", 3, 1);
	virtualFaderSize->addParameterListener(this);
	virtualFaderBelow = virtualParamsContainer.addIntParameter("Below button numbers", "Number of buttons below the fader in each fader column", 1, 0);
	virtualFaderBelow->addParameterListener(this);

	panelScale = uiParamsContainer.addFloatParameter("Input Panel scale", "scale Input panel view", 1, 0.1, 3);
	panelScale->addParameterListener(this);
	encodersScale = uiParamsContainer.addFloatParameter("Encoders scale", "scale the encoders view", 1, 0.1, 3);
	encodersScale->addParameterListener(this);
	gridCols= uiParamsContainer.addIntParameter("Grid Columns", "Number of columns for grid viewss", 10, 1);
	gridCols->addParameterListener(this);
	gridScale = uiParamsContainer.addFloatParameter("Grid scale", "scale the grid view", 1, 0.1, 3);
	gridScale->addParameterListener(this);
	encoderBigNumber = addIntParameter("Encoder big offset", "Offset of encoders when << or >> pressed",4,2);


	mainBrain = Brain::getInstance();
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
	addChildControllableContainer(CarouselManager::getInstance());
	addChildControllableContainer(TrackerManager::getInstance());
	addChildControllableContainer(EffectManager::getInstance());

	addChildControllableContainer(VirtualButtonManager::getInstance());
	addChildControllableContainer(VirtualFaderColManager::getInstance());


	Encoders::getInstance()->engine = this;
	InputPanel::getInstance()->engine = this;
	FixtureGridView::getInstance()->engine = this;
	GroupGridView::getInstance()->engine = this;
	PresetGridView::getInstance()->engine = this;
	CuelistGridView::getInstance()->engine = this;
	EffectGridView::getInstance()->engine = this;
	CarouselGridView::getInstance()->engine = this;
	TrackerGridView::getInstance()->engine = this;

	VirtualButtonGrid::getInstance()->engine = this;
	VirtualButtonGrid::getInstance()->initCells();

	VirtualFaderColGrid::getInstance()->engine = this;
	VirtualFaderColGrid::getInstance()->initCells();
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


	DataTransferManager::deleteInstance();
	FixtureMultiEditor::deleteInstance();

	VirtualButtonManager::deleteInstance();
	VirtualButtonGrid::deleteInstance();
	VirtualFaderColManager::deleteInstance();
	VirtualFaderColGrid::deleteInstance();

	EffectManager::deleteInstance();
	CarouselManager::deleteInstance();
	TrackerManager::deleteInstance();
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

	CommandLine::deleteInstance();
	Encoders::deleteInstance();
	InputPanel::deleteInstance();

	Brain::deleteInstance();
}


void BKEngine::clearInternal()
{
	//clear
	// StateManager::getInstance()->clear();
	// ChataigneSequenceManager::getInstance()->clear();

	// ModuleRouterManager::getInstance()->clear();
	// ModuleManager::getInstance()->clear();
	Brain::getInstance()->stopThread(1);
	Brain::getInstance()->clear();

	VirtualFaderColManager::getInstance()->clear();
	VirtualButtonManager::getInstance()->clear();
	TrackerManager::getInstance()->clear();
	EffectManager::getInstance()->clear();
	CarouselManager::getInstance()->clear();
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

	VirtualButtonGrid::getInstance()->initCells();
	VirtualFaderColGrid::getInstance()->initCells();
	FixtureGridView::getInstance()->updateCells();
	GroupGridView::getInstance()->updateCells();
	PresetGridView::getInstance()->updateCells();
	CuelistGridView::getInstance()->updateCells();
	EffectGridView::getInstance()->updateCells();
	CarouselGridView::getInstance()->updateCells();
	TrackerGridView::getInstance()->updateCells();
	Brain::getInstance()->startThread();


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

	var tData = TrackerManager::getInstance()->getJSONData();
	if (!tData.isVoid() && tData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(TrackerManager::getInstance()->shortName, tData);

	var vbData = VirtualButtonManager::getInstance()->getJSONData();
	if (!vbData.isVoid() && vbData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(VirtualButtonManager::getInstance()->shortName, vbData);

	var vfData = VirtualFaderColManager::getInstance()->getJSONData();
	if (!vfData.isVoid() && vfData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(VirtualFaderColManager::getInstance()->shortName, vfData);

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
	ProgressTask* trackTask = loadingTask->addTask("Trackers");
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

	trackTask->start();
	TrackerManager::getInstance()->loadJSONData(data.getProperty(TrackerManager::getInstance()->shortName, var()));
	trackTask->setProgress(1);
	trackTask->end();

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
	TrackerGridView::getInstance()->updateCells();

	VirtualButtonGrid::getInstance()->page = 1;
	VirtualButtonGrid::getInstance()->initCells();

	VirtualFaderColGrid::getInstance()->page = 1;
	VirtualFaderColGrid::getInstance()->initCells();
	Brain::getInstance()->clearUpdates();
	Brain::getInstance()->loadingIsRunning = false;
	Brain::getInstance()->startThread();

}

void BKEngine::childStructureChanged(ControllableContainer* cc)
{
	Engine::childStructureChanged(cc);
}

void BKEngine::controllableFeedbackUpdate(ControllableContainer* cc, Controllable* c)
{
	if (isClearing || isLoadingFile) return;
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
	FileChooser fc("Load some mochis", File::getCurrentWorkingDirectory(), "*.mochi");
	if (!fc.browseForMultipleFilesToOpen()) return;
	Array<File> f = fc.getResults();
	for (int i = 0; i < f.size(); i++) {
		importMochi(f[i]);
	}
}

void BKEngine::importMochi(File f) {
	var data = JSON::parse(f);

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
	TrackerManager::getInstance()->addItemsFromData(data.getProperty(TrackerManager::getInstance()->shortName, var()));
	EffectManager::getInstance()->addItemsFromData(data.getProperty(EffectManager::getInstance()->shortName, var()));
	VirtualButtonManager::getInstance()->addItemsFromData(data.getProperty(VirtualButtonManager::getInstance()->shortName, var()));
	VirtualFaderColManager::getInstance()->addItemsFromData(data.getProperty(VirtualFaderColManager::getInstance()->shortName, var()));

	FixtureGridView::getInstance()->updateCells();
	GroupGridView::getInstance()->updateCells();
	PresetGridView::getInstance()->updateCells();
	CuelistGridView::getInstance()->updateCells();
	EffectGridView::getInstance()->updateCells();
	CarouselGridView::getInstance()->updateCells();
	TrackerGridView::getInstance()->updateCells();
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
	data.getDynamicObject()->setProperty(TrackerManager::getInstance()->shortName, TrackerManager::getInstance()->getExportSelectionData());
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
	if (p == panelScale) {
		InputPanel::getInstance()->resized();
	}
	else if (p == encodersScale) {
		Encoders::getInstance()->resized();
	}
	else if (p == gridScale || p == gridCols) {
		FixtureGridView::getInstance()->resized();
		GroupGridView::getInstance()->resized();
		PresetGridView::getInstance()->resized();
		CuelistGridView::getInstance()->resized();
		EffectGridView::getInstance()->resized();
		CarouselGridView::getInstance()->resized();
		TrackerGridView::getInstance()->resized();
	}
	else if (p == virtualButtonGridCols || p == virtualButtonGridRows) {
		VirtualButtonGrid::getInstance()->initCells();
	}
	else if (p == virtualFaderCols || p == virtualFaderRotary || p == virtualFaderAbove || p == virtualFaderSize || p == virtualFaderBelow) {
		VirtualFaderColGrid::getInstance()->initCells();
	}
}
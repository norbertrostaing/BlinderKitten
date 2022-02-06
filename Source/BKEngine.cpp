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

#include "./Definitions/Interface/InterfaceManager.h"
#include "./Definitions/FixtureParamType/FixtureParamTypeManager.h"
#include "./Definitions/DeviceType/DeviceTypeManager.h"
#include "./Definitions/Device/DeviceManager.h"
#include "./Definitions/Fixture/FixtureManager.h"
#include "./Definitions/Group/GroupManager.h"
#include "./Definitions/Preset/PresetManager.h"
#include "./Definitions/Command/CommandManager.h"
#include "./Definitions/Cuelist/CuelistManager.h"
#include "./Definitions/Programmer/ProgrammerManager.h"


ControllableContainer* getAppSettings();

BKEngine::BKEngine() :
	Engine("BlinderKitten", ".olga"),
	defaultBehaviors("Test")
	//ossiaDevice(nullptr)
{
	convertURL = "http://hazlab.fr/";

	//init here
	Engine::mainEngine = this;
	mainBrain = Brain::getInstance();
	addChildControllableContainer(InterfaceManager::getInstance());
	addChildControllableContainer(FixtureParamTypeManager::getInstance());
	addChildControllableContainer(DeviceTypeManager::getInstance());
	addChildControllableContainer(DeviceManager::getInstance());
	addChildControllableContainer(FixtureManager::getInstance());
	addChildControllableContainer(GroupManager::getInstance());
	addChildControllableContainer(PresetManager::getInstance());
	addChildControllableContainer(CommandManager::getInstance());
	addChildControllableContainer(CuelistManager::getInstance());
	addChildControllableContainer(ProgrammerManager::getInstance());
	// addChildControllableContainer(StateManager::getInstance());
	// addChildControllableContainer(ChataigneSequenceManager::getInstance());
	// addChildControllableContainer(ModuleRouterManager::getInstance());
	// addChildControllableContainer(CVGroupManager::getInstance());

	// MIDIManager::getInstance(); //Trigger constructor, declare settings

	getAppSettings()->addChildControllableContainer(&defaultBehaviors);
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
	ProgrammerManager::deleteInstance();
	CuelistManager::deleteInstance();
	CommandManager::deleteInstance();
	PresetManager::deleteInstance();
	GroupManager::deleteInstance();
	FixtureManager::deleteInstance();
	DeviceManager::deleteInstance();
	DeviceTypeManager::deleteInstance();
	FixtureParamTypeManager::deleteInstance();
	InterfaceManager::deleteInstance();
}


void BKEngine::clearInternal()
{
	//clear
	// StateManager::getInstance()->clear();
	// ChataigneSequenceManager::getInstance()->clear();

	// ModuleRouterManager::getInstance()->clear();
	// ModuleManager::getInstance()->clear();
	ProgrammerManager::getInstance()->clear();
	CuelistManager::getInstance()->clear();
	CommandManager::getInstance()->clear();
	PresetManager::getInstance()->clear();
	GroupManager::getInstance()->clear();
	FixtureManager::getInstance()->clear();
	DeviceManager::getInstance()->clear();
	DeviceTypeManager::getInstance()->clear();
	FixtureParamTypeManager::getInstance()->clear();
	InterfaceManager::getInstance()->clear();
}

var BKEngine::getJSONData()
{
	var data = Engine::getJSONData();

	//var mData = ModuleManager::getInstance()->getJSONData();
	//if (!mData.isVoid() && mData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(ModuleManager::getInstance()->shortName, mData);

	var iData = InterfaceManager::getInstance()->getJSONData();
	if (!iData.isVoid() && iData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(InterfaceManager::getInstance()->shortName, iData);

	var dtData = DeviceTypeManager::getInstance()->getJSONData();
	if (!dtData.isVoid() && dtData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(DeviceTypeManager::getInstance()->shortName, dtData);

	var fptData = FixtureParamTypeManager::getInstance()->getJSONData();
	if (!fptData.isVoid() && fptData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(FixtureParamTypeManager::getInstance()->shortName, fptData);

	var fData = FixtureManager::getInstance()->getJSONData();
	if (!fData.isVoid() && fData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(FixtureManager::getInstance()->shortName, fData);

	var gData = GroupManager::getInstance()->getJSONData();
	if (!gData.isVoid() && gData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(GroupManager::getInstance()->shortName, gData);

	var pData = PresetManager::getInstance()->getJSONData();
	if (!pData.isVoid() && pData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(PresetManager::getInstance()->shortName, pData);

	var dData = DeviceManager::getInstance()->getJSONData();
	if (!dData.isVoid() && dData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(DeviceManager::getInstance()->shortName, dData);

	var cData = CommandManager::getInstance()->getJSONData();
	if (!cData.isVoid() && cData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(CommandManager::getInstance()->shortName, cData);

	var clData = CuelistManager::getInstance()->getJSONData();
	if (!clData.isVoid() && clData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(CuelistManager::getInstance()->shortName, clData);

	var prData = ProgrammerManager::getInstance()->getJSONData();
	if (!prData.isVoid() && prData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(ProgrammerManager::getInstance()->shortName, prData);

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
	//ProgressTask* moduleTask = loadingTask->addTask("Modules");
	ProgressTask* iTask = loadingTask->addTask("Interfaces");
	ProgressTask* fptTask = loadingTask->addTask("Fixture Param Types");
	ProgressTask* dtTask = loadingTask->addTask("Device Types");
	ProgressTask* dTask = loadingTask->addTask("Devices");
	ProgressTask* fTask = loadingTask->addTask("Fixtures");
	ProgressTask* gTask = loadingTask->addTask("Groups");
	ProgressTask* pTask = loadingTask->addTask("Presets");
	ProgressTask* cTask = loadingTask->addTask("Commands");
	ProgressTask* clTask = loadingTask->addTask("Cuelists");
	ProgressTask* prTask = loadingTask->addTask("Programmers");
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
	FixtureParamTypeManager::getInstance()->loadJSONData(data.getProperty(FixtureParamTypeManager::getInstance()->shortName, var()));
	fptTask->setProgress(1);
	fptTask->end();

	dtTask->start();
	DeviceTypeManager::getInstance()->loadJSONData(data.getProperty(DeviceTypeManager::getInstance()->shortName, var()));
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

	fTask->start();
	FixtureManager::getInstance()->loadJSONData(data.getProperty(FixtureManager::getInstance()->shortName, var()));
	fTask->setProgress(1);
	fTask->end();

	dTask->start();
	DeviceManager::getInstance()->loadJSONData(data.getProperty(DeviceManager::getInstance()->shortName, var()));
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

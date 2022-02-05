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

	//ChataigneGenericModule module;
	//MultiplexModule multiplexModule;

	//Global Settings
	ControllableContainer defaultBehaviors;
	Brain* mainBrain;

	void clearInternal() override;

	var getJSONData() override;
	void loadJSONDataInternalEngine(var data, ProgressTask* loadingTask) override;

	void childStructureChanged(ControllableContainer* cc) override;
	void controllableFeedbackUpdate(ControllableContainer* cc, Controllable* c) override;

	void handleAsyncUpdate() override;

	void importSelection(File f = File());
	void exportSelection();

	String getMinimumRequiredFileVersion() override;

};
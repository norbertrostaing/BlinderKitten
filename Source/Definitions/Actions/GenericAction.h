/*
  ==============================================================================

    GenericAction.h
    Created: 8 Oct 2020 3:33:47pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include"../../Common/Action/Action.h"

class GenericAction :
	public Action,
	public EngineListener
{
public:
	GenericAction(var params);
	~GenericAction();

	enum ActionType { SET_VALUE, TRIGGER };
	ActionType actionType;
	TargetParameter* target;
	WeakReference<Parameter> value;
	var dataToLoad;
	var ghostValueData; // to keep when target is lost

	bool isLoading = true;

	void setValueParameter(Parameter* p);

	virtual void triggerInternal() override;
	void setValueInternal(var value, String origin, int incrementIndex, bool isRelative) override;

	void onContainerParameterChanged(Parameter*) override;

	virtual void loadJSONDataInternal(var data) override;
	virtual void endLoadFile() override;

	static GenericAction* create(var params) { return new GenericAction(params); }
};
/*
  ==============================================================================

    SelectionMasterManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "SelectionMaster.h"

class SelectionMasterManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<SelectionMaster>, SelectionMaster, BaseItemUI<SelectionMaster>>
{
public:
	SelectionMasterManagerUI(const String &contentName);
	~SelectionMasterManagerUI();

	static SelectionMasterManagerUI * create(const String &name) { return new SelectionMasterManagerUI(name); }
};
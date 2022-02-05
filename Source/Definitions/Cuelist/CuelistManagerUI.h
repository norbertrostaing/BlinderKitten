/*
  ==============================================================================

    CVGroupManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Cuelist.h"

class CuelistManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<Cuelist>, Cuelist, BaseItemUI<Cuelist>>
{
public:
	CuelistManagerUI(const String &contentName);
	~CuelistManagerUI();

	static CuelistManagerUI * create(const String &name) { return new CuelistManagerUI(name); }
};
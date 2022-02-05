/*
  ==============================================================================

    CVGroupManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Group.h"

class GroupManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<Group>, Group, BaseItemUI<Group>>
{
public:
	GroupManagerUI(const String &contentName);
	~GroupManagerUI();

	static GroupManagerUI * create(const String &name) { return new GroupManagerUI(name); }
};
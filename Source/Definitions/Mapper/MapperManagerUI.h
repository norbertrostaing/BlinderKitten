/*
  ==============================================================================

    MapperManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Mapper.h"

class MapperManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<Mapper>, Mapper, BaseItemUI<Mapper>>
{
public:
	MapperManagerUI(const String &contentName);
	~MapperManagerUI();

	static MapperManagerUI * create(const String &name) { return new MapperManagerUI(name); }
};
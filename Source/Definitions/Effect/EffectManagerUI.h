/*
  ==============================================================================

    EffectManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Effect.h"

class EffectManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<Effect>, Effect, BaseItemUI<Effect>>
{
public:
	EffectManagerUI(const String &contentName);
	~EffectManagerUI();

	static EffectManagerUI * create(const String &name) { return new EffectManagerUI(name); }
};
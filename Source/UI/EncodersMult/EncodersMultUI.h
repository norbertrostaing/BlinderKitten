/*
  ==============================================================================

    EncodersMultMult.h
    Created: 8 Oct 2020 6:59:51pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
class EncodersMultUI : public ShapeShifterContent
{
public:
    EncodersMultUI(const String& contentName);
    ~EncodersMultUI();

    static EncodersMultUI* create(const String& name) { return new EncodersMultUI(name); }

};

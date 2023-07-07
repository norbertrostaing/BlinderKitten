/*
  ==============================================================================

    EffectGridView.h
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GridView.h"
//==============================================================================
/*
*/
class EffectGridViewUI : public ShapeShifterContent
{
public:
    EffectGridViewUI(const String& contentName);
    ~EffectGridViewUI();

    static EffectGridViewUI* create(const String& name) { return new EffectGridViewUI(name); }


};


class EffectGridView  : 
    public GridView
{
public:
    juce_DeclareSingleton(EffectGridView, true);
    EffectGridView();
    ~EffectGridView() override;
    
    void updateCells() override;
    void updateButtons();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectGridView)
};



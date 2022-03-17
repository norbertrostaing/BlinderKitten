/*
  ==============================================================================

    CuelistGridView.h
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
class CuelistGridViewUI : public ShapeShifterContent
{
public:
    CuelistGridViewUI(const String& contentName);
    ~CuelistGridViewUI();

    static CuelistGridViewUI* create(const String& name) { return new CuelistGridViewUI(name); }


};


class CuelistGridView  : 
    public GridView
{
public:
    juce_DeclareSingleton(CuelistGridView, true);
    CuelistGridView();
    ~CuelistGridView() override;
    
    void updateCells() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CuelistGridView)
};



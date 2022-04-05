/*
  ==============================================================================

    TrackerGridView.h
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
class TrackerGridViewUI : public ShapeShifterContent
{
public:
    TrackerGridViewUI(const String& contentName);
    ~TrackerGridViewUI();

    static TrackerGridViewUI* create(const String& name) { return new TrackerGridViewUI(name); }


};


class TrackerGridView  : 
    public GridView
{
public:
    juce_DeclareSingleton(TrackerGridView, true);
    TrackerGridView();
    ~TrackerGridView() override;
    
    void updateCells() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackerGridView)
};



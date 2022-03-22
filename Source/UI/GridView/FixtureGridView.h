/*
  ==============================================================================

    FixtureGridView.h
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
class FixtureGridViewUI : public ShapeShifterContent
{
public:
    FixtureGridViewUI(const String& contentName);
    ~FixtureGridViewUI();

    int numberOfCells = 5000;
    static FixtureGridViewUI* create(const String& name) { return new FixtureGridViewUI(name); }


};


class FixtureGridView :
    public GridView
{
public:
    juce_DeclareSingleton(FixtureGridView, true);
    FixtureGridView();
    ~FixtureGridView() override;

    void updateCells() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FixtureGridView)
};



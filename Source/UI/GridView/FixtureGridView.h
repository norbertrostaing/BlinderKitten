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
#include "Definitions/Fixture/FixtureManager.h"

//==============================================================================
/*
*/
class FixtureGridViewUI : public ShapeShifterContent
{
public:
    FixtureGridViewUI(const String& contentName);
    ~FixtureGridViewUI();

    static FixtureGridViewUI* create(const String& name) { return new FixtureGridViewUI(name); }


};


class FixtureGridView :
    public GridView,
    public FixtureManager::AsyncListener
{
public:
    juce_DeclareSingleton(FixtureGridView, true);
    FixtureGridView();
    ~FixtureGridView() override;

    int numberOfCells = 0;
    HashMap<int, TextButton*> hashMapButtons;
    CriticalSection cs;
    int fixtIdMax = 0;
    void initArrays() override;
    void resized() override;
    void updateCells() override;
    void buttonClicked(juce::Button* button) override;

    void newMessage(const FixtureManager::ManagerEvent &e ) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FixtureGridView)
};



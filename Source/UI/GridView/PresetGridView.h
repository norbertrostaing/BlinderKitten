/*
  ==============================================================================

    PresetGridView.h
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GridView.h"
#include "Preset/PresetManager.h"
//==============================================================================
/*
*/
class PresetGridViewUI : public ShapeShifterContent
{
public:
    PresetGridViewUI(const String& contentName);
    ~PresetGridViewUI();

    static PresetGridViewUI* create(const String& name) { return new PresetGridViewUI(name); }


};


class PresetGridView  : 
    public GridView,
    public PresetManager::AsyncListener
{
public:
    juce_DeclareSingleton(PresetGridView, true);
    PresetGridView();
    ~PresetGridView() override;
    
    void updateCells() override;
    void newMessage(const PresetManager::ManagerEvent& e) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetGridView)
};



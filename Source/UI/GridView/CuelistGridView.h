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
#include "Cuelist/CuelistManager.h"
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
    public GridView,
    public CuelistManager::AsyncListener
{
public:
    juce_DeclareSingleton(CuelistGridView, true);
    CuelistGridView();
    ~CuelistGridView() override;
    
    void updateCells() override;
    void updateButtons();
    void showContextMenu(int id) override;
    void newMessage(const CuelistManager::ManagerEvent& e) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CuelistGridView)
};



/*
  ==============================================================================

    GroupGridView.h
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GridView.h"
#include "Group/GroupManager.h"

//==============================================================================
/*
*/
class GroupGridViewUI : public ShapeShifterContent
{
public:
    GroupGridViewUI(const String& contentName);
    ~GroupGridViewUI();

    static GroupGridViewUI* create(const String& name) { return new GroupGridViewUI(name); }


};


class GroupGridView  : 
    public GridView,
    public GroupManager::AsyncListener
{
public:
    juce_DeclareSingleton(GroupGridView, true);
    GroupGridView();
    ~GroupGridView() override;
    
    void updateCells() override;
    void newMessage(const GroupManager::ManagerEvent& e) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GroupGridView)
};



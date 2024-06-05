/*
  ==============================================================================

    MapperGridView.h
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
class MapperGridViewUI : public ShapeShifterContent
{
public:
    MapperGridViewUI(const String& contentName);
    ~MapperGridViewUI();

    static MapperGridViewUI* create(const String& name) { return new MapperGridViewUI(name); }


};


class MapperGridView  : 
    public GridView
{
public:
    juce_DeclareSingleton(MapperGridView, true);
    MapperGridView();
    ~MapperGridView() override;
    
    void updateCells() override;
    void showContextMenu(int id);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapperGridView)
};



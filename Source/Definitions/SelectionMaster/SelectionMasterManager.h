/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "SelectionMaster.h"

class SelectionMasterManager :
    public BaseManager<SelectionMaster>
{
public:
    juce_DeclareSingleton(SelectionMasterManager, true);

    SelectionMasterManager();
    ~SelectionMasterManager();

    void addItemInternal(SelectionMaster* o, var data) override;
    void removeItemInternal(SelectionMaster* o) override;

    void onContainerParameterChanged(Parameter* p) override;

    void recomputeAllSelectionMasters();

};
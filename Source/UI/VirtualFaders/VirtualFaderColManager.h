/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "VirtualFaderCol.h"

class VirtualFaderColManager :
    public BaseManager<VirtualFaderCol>
{
public:
    juce_DeclareSingleton(VirtualFaderColManager, true);

    VirtualFaderColManager();
    ~VirtualFaderColManager();

    void addItemInternal(VirtualFaderCol* o, var data) override;
    void removeItemInternal(VirtualFaderCol* o) override;

    void onContainerParameterChanged(Parameter* p) override;

    HashMap<String, VirtualFaderButton*> buttonLibrary;
    HashMap<String, VirtualFaderSlider*> sliderLibrary;
    CriticalSection usingLibraries;
    void reconstructLibraries();
    // static int sort(VirtualFaderCol* A, VirtualFaderCol* B) {return (int)A->id->getValue() - (int)B->id->getValue();};
    void setAboveButtonValue(int page, int col, int row, float value, String origin);
    void setBelowButtonValue(int page, int col, int row, float value, String origin);
    void setRotaryValue(int page, int col, int row, float value, String origin);
    void setFaderValue(int page, int col, float value, String origin);


};
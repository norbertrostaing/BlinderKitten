/*
  ==============================================================================

    OSCMapping.h
    Created: 12 Oct 2020 11:07:59am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class OSCMapping :
    public BaseItem
{
public:
    OSCMapping();
    ~OSCMapping();

    StringParameter* inputAddress;
    Point2DParameter* inputRange;
    Point2DParameter* outputRange;

    BoolParameter* learnMode;
    BoolParameter* learnRange;

    bool nextInputReinitRange = false;

    bool isValid;
    bool wasInRange;

    ActionManager actionManager;

    void processMessage(const OSCMessage& m);

    void handleValue(float value, String origin, int incrementIndex, bool isRelative);

    void onContainerParameterChangedInternal(Parameter* p);

    void updateDisplay();

    //InspectableEditor* getEditorInternal(bool isRoot, Array<Inspectable*> inspectables);
};
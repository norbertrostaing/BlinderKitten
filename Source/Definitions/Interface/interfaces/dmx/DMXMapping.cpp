/*
  ==============================================================================

    DMXMapping.cpp
    Created: 12 Oct 2020 11:07:59am
    Author:  bkupe

  ==============================================================================
*/

#include "Definitions/Interface/InterfaceIncludes.h"

DMXMapping::DMXMapping() :
    BaseItem("DMX Mapping"),
    isValid(false),
    wasInRange(false)
{
    saveAndLoadRecursiveData = true;

    // mode = addEnumParameter("Mode", "Set the mode of this mapping.");
    // mode->addOption("Continuous", CONTINUOUS)->addOption("Trigger", TRIGGER)->addOption("On / Off", ONOFF)->addOption("Toggle", TOGGLE);
    

    pitchOrNumber = addIntParameter("Channel Number", "Desired channel", 1, 1, 512);
    
    //inputRange = addPoint2DParameter("Input Range", "The range to get from input",false);
    //inputRange->setBounds(0, 0, 127, 127);
    //inputRange->setPoint(0, 127);
    
    //outputRange = addPoint2DParameter("Output Range", "The range to remap the value to.",false);
    //outputRange->setPoint(0, 1);

    addChildControllableContainer(&actionManager);

    showInspectorOnSelect = false;
}

DMXMapping::~DMXMapping()
{
}

void DMXMapping::handleChannel(int channel, int value, String origin)
{
    if (!enabled->boolValue()) return;

    if (pitchOrNumber->intValue() != channel) return;
    handleValue(value, origin);
}

void DMXMapping::handleValue(int value, String origin)
{
    if (!enabled->boolValue()) return;
    // m = mode->getValueDataAsEnum<MappingMode>();

    //float minInput = jmin(inputRange->x, inputRange->y);
    //float maxInput = jmax(inputRange->x, inputRange->y);
    float relVal = jmap<float>(jlimit<float>(0, 255, value), 0, 255, 0, 1);
    actionManager.setValueAll(relVal, origin);
    return;
    /*
    if (m != CONTINUOUS)
    {
        bool isInRange = value >= inputRange->x && value <= inputRange->y;
        if (wasInRange != isInRange) //state change
        {
            bool valid = isInRange;
            if (m == TRIGGER || m == ONOFF)
            {
                isValid = valid;
                if (m == TRIGGER && isValid) actionManager.triggerAll();
            }
            else if (m == TOGGLE)
            {
                if (valid) isValid = !isValid;
            }

            if (m == ONOFF || m == TOGGLE)
            {
                actionManager.setValueAll(isValid ? outputRange->y : outputRange->x);
            }

            wasInRange = isInRange;
        }
    }else
    {
        //float minInput = jmin(inputRange->x, inputRange->y);
        //float maxInput = jmax(inputRange->x, inputRange->y);
        float minInput = 0;
        float maxInput = 127;
        float relVal = jmap<float>(jlimit<float>(minInput, maxInput, value), minInput, maxInput, 0, 1);
        if (inputRange->x > inputRange->y) relVal = 1 - relVal;
        float targetVal = jmap<float>(relVal, outputRange->x, outputRange->y);
        actionManager.setValueAll(targetVal);
    }
    */
}


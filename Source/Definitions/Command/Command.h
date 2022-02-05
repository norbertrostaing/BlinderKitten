/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "CommandValue.h"
#include "CommandTiming.h"

class CommandSelectionManager;

class CommandTiming;
class ChannelValue;
class FixtureChannel;

class Command:
    public BaseItem
{
public:
    Command(var params = var());
    virtual ~Command();

    String objectType;
    var objectData;

    TargetParameter* devTypeParam;

    CommandSelectionManager* selection;
    std::unique_ptr<BaseManager<CommandValue>> values;
    CommandTiming timing;

    HashMap<FixtureChannel*, ChannelValue*> computedValues;

    String getTypeString() const override { return objectType; }
    void parameterValueChanged(Parameter* p);
    void afterLoadJSONDataInternal();
    void updateDisplay();

    int64 maxTiming;
    void computeValues();

    static Command* create(var params) { return new Command(params); }

};


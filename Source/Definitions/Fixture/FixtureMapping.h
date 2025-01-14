/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
class ChannelType;

class FixtureMapping:
    public BaseItem
{
    public:
    FixtureMapping(var params = var());
    ~FixtureMapping();

    String objectType;
    var objectData;

    String outputInterfaceType = "";

    TargetParameter* outputInterface;
    IntParameter* subFixtureId;
    TargetParameter* channelType;

    enum MidiType { NOTE, CONTROLCHANGE, PITCHWHEEL, TEXT };
    EnumParameter* midiType;
    IntParameter* midiChannel;
    IntParameter* midiPitchOrNumber;
    Point2DParameter* midiOutputRange7b;
    Point2DParameter* midiOutputRange14b;

    StringParameter* oscOutputAddress;
    Point2DParameter* oscOutputRange;


    void onContainerParameterChangedInternal(Parameter* p);
    void updateDisplay();

    void process(int subId, ChannelType* type, double val);

};

class FixtureMappingManager :
    public BaseManager<FixtureMapping>
{
public:
    juce_DeclareSingleton(FixtureMappingManager, true);

    FixtureMappingManager();
    FixtureMappingManager(const juce::String& name);
    ~FixtureMappingManager();

    void askForMoveBefore(BaseItem*) override;
    void askForMoveAfter(BaseItem*) override;
    void addItemInternal(FixtureMapping* c, var data);
    void askForDuplicateItem(BaseItem* item);
    void setItemIndex(FixtureMapping* item, int newIndex, bool addToUndo = true);

};
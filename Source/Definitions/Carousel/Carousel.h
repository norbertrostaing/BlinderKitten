/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================

*/

#pragma once
#include "JuceHeader.h"
#include "../Command/CommandSelectionManager.h"
#include "CarouselRow.h"
class CarouselStep;
class SubFixtureChannel;

class Carousel :
    public BaseItem
{
public:
    Carousel(var params = var());
    virtual ~Carousel();

    String objectType;
    var objectData;

    IntParameter* id;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p);
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c);
    void updateName();

    BoolParameter* isCarouselOn;
    bool isOn = false;

    Trigger* startBtn;
    Trigger* stopBtn;

    BoolParameter* autoStartAndStop;
    FloatParameter* speed;
    FloatParameter* sizeValue;
    BaseManager<CarouselRow> rows;

    FloatParameter* currentPosition;
    double TSLastUpdate;
    double totalElapsed;

    HashMap<SubFixtureChannel*, Array<CarouselRow*>*> chanToCarouselRow;

    double lastTapTempo;
    Trigger* tapTempoBtn;
    IntParameter* beatPerCycle;

    String getTypeString() const override { return objectType; }

    void triggerTriggered(Trigger* t);
    void update(double now);
    void computeData();
    bool computed = false;
    bool computing = false;
    void pleaseComputeIfRunning();
    void start();
    void stop();
    float applyToChannel(SubFixtureChannel* fc, float currentVal, double now);
    void tapTempo();

    static Carousel* create(var params) { return new Carousel(params); }
    
};

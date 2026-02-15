/*
  ==============================================================================
 
    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "BundleSelectionManager.h"

class BundleSelectionManager;
class BundleSelection;
class Cuelist;
class Effect;
class Carousel;

class Bundle:
    public BaseItem
{
public:
    Bundle(var params = var());
    virtual ~Bundle();

    String objectType;
    var objectData;

    IntParameter* id;
    int registeredId = 0;
    StringParameter* userName;
    void updateName();
    BundleSelectionManager selection;

    Array<Cuelist*> computedCuelists;
    Array<Effect*> computedEffects;
    Array<Carousel*> computedCarousels;
    Array<Mapper*> computedMappers;
    Array<Tracker*> computedTrackers;

    CriticalSection isComputing;

    float lastHTP = 0;
    float lastLTP = 0;
    float lastSize = 0;
    float lastFlash = 0;
    float lastSpeed = 0;

    bool toDelete = false;

    String getTypeString() const override { return objectType; }
    void onContainerParameterChangedInternal(Parameter* p) override;
    void afterLoadJSONDataInternal() override;
    void updateDisplay();

    double TSLastUpdate = 0;
    void computeValues();

    void start();
    void stop(float forcedFade = -1);
    void tapTempo();
    void setSize(float val, bool size, bool HTP, bool LTP, bool flash);
    void setSpeed(float val, bool tapTempoDivide = false);
    void setSizeRel(float val, bool size, bool HTP, bool LTP, bool flash);
    void setSpeedRel(float val, bool tapTempoDivide = false);
    void speedMult(float mult);

    void onControllableFeedbackUpdate(ControllableContainer*, Controllable*) override;
    void triggerTriggered(Trigger* t) override;
    static Bundle* create(var params) { return new Bundle(params); }

    void flash(bool on, bool timing, bool swop = false);

};


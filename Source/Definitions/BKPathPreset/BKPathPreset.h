/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Definitions/Multiplicator/MultiplicatorLinkManager.h"

// #include "../Command/CommandSelectionManager.h"
class CommandSelectionManager;

class BKPathPreset :
    public BaseItem
{
public:
    BKPathPreset(var params = var());
    virtual ~BKPathPreset();

    String objectType;
    var objectData;

    IntParameter* id;
    int registeredId = 0;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();

    IntParameter* useAnotherId;

    std::unique_ptr<ControllableContainer> pointsManager;

    void updateDisplay();
    void afterLoadJSONDataInternal() override;

    String getTypeString() const override { return objectType; }
    static void createPoint(ControllableContainer* cc);

    static BKPathPreset* create(var params) { return new BKPathPreset(params); }
};

/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "PresetFixtureValues.h"
#include "PresetValue.h"
#include "../Fixture/Fixture.h"
#include "../FixtureParamType/FixtureParamDefinition/FixtureParamDefinition.h"
#include "../DeviceType/DeviceType.h"

class Preset:
    public BaseItem
{
public:
    Preset(var params = var());
    virtual ~Preset();

    void onContainerNiceNameChanged() override;
    void afterLoadJSONDataInternal() override;

    String objectType;
    var objectData;

    IntParameter* id;
    TargetParameter* devTypeParam;
    EnumParameter* presetType;

    HashMap<Fixture*, HashMap<FixtureParamDefinition*, float>*> computedFixtureValues;
    HashMap<DeviceType*, HashMap<FixtureParamDefinition*, float>*> computedDeviceTypeValues;
    HashMap<DeviceType*, HashMap<String, HashMap<FixtureParamDefinition*, float>*>*> computedFixtureTypeValues;
    HashMap<FixtureParamDefinition*, float> computedUniversalValues;

    HashMap<FixtureParamDefinition*, float>* getFixtureValues(Fixture* f);

    std::unique_ptr<BaseManager<PresetFixtureValues>> fixtureValues;
    String getTypeString() const override { return objectType; }
    void parameterValueChanged(Parameter* p);

    static Preset* create(var params) { return new Preset(params); }

    void computeValues();
};

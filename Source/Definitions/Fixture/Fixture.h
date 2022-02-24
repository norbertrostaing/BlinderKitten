/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "FixturePatch.h"
#include "../SubFixture/SubFixture.h"

class Fixture:
    public BaseItem
{
public:
    Fixture(var params = var());
    virtual ~Fixture();

    void onContainerNiceNameChanged() override;
    void afterLoadJSONDataInternal() override;

    String objectType;
    var objectData;

    IntParameter* id;
    int registeredId = 0;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    TargetParameter* devTypeParam;

    HashMap<int, SubFixture*> subFixtures;
    BaseManager<FixturePatch> patchs;
    String getTypeString() const override { return objectType; }
    Array<SubFixtureChannel*> channels;

    void applyPatchs();
    void checkChildrenSubFixtures();

    static Fixture* create(var params) { return new Fixture(params); }

    Array<SubFixture*> getAllSubFixtures();
    SubFixture* getSubFixture(int id);

    void selectThis(bool addToSelection=false, bool notify = true);
    void autoName();
};

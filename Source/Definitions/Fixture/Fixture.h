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
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c);
    void updateName();
    void updateSubFixtureNames();
    void updateDisplay();

    TargetParameter* devTypeParam;

    BoolParameter* useCustomLayoutStrokeColor;
    ColorParameter* layoutStrokeColor;
    BoolParameter* useCustomLayoutFillColor;
    ColorParameter* layoutFillColor;

    Point3DParameter* position;
    Point3DParameter* rotation;

    OwnedArray<SubFixture> subFixturesContainer;
    HashMap<int, SubFixture*> subFixtures;
    FixturePatchManager patchs;
    String getTypeString() const override { return objectType; }

    void applyPatchs();
    void checkChildrenSubFixtures();

    static Fixture* create(var params) { return new Fixture(params); }

    Array<SubFixture*> getAllSubFixtures();
    SubFixture* getSubFixture(int id);
    Colour getLayoutStrokeColor();
    Colour getLayoutFillColor();

    void selectThis(bool addToSelection=false, bool notify = true);
    void autoName();

};

/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "BKPath/BKPath.h"

class SubFixture;
class Fixture;

class Layout :
    public ChangeBroadcaster,
    public BaseItem
    // public ChainVizTarget
{
public:
    Layout(var params = var());
    virtual ~Layout();

    String objectType;
    var objectData;

    HashMap<Fixture*, std::shared_ptr<Point<float>>> fixtToPos;
    HashMap<SubFixture*, std::shared_ptr<Point<float>>> subFixtToPos;
    CriticalSection isComputing;

    IntParameter* id;
    int registeredId = 0;
    StringParameter* userName;
    void updateName();

    Point2DParameter* dimensionsX;
    Point2DParameter* dimensionsY;
    Point2DParameter* tilesScale;
    FloatParameter* textScale;

    BoolParameter* viewOutput;

    BaseManager<BKPath> paths;

    void onContainerParameterChangedInternal(Parameter* p) override;
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;

    void computeData();
    void fitToContent();

    void createPathForFixture(Fixture* f, float x, float y);

    std::shared_ptr<HashMap<SubFixture*, float>> getSubfixturesRatioFromDirection(float angle);
    std::shared_ptr<HashMap<SubFixture*, float>> getSubfixturesRatioFromOriginAndAngle(Point<float>* vect, float angle, bool normalize, bool clockwise);
    std::shared_ptr<HashMap<SubFixture*, float>> getSubfixturesRatioFromOrigin(Point<float>* vect);
    std::shared_ptr<HashMap<SubFixture*, float>> getSubfixturesRatioPerlin(float scale, int seed);

    String getTypeString() const override { return objectType; }
    static Layout* create(var params) { return new Layout(params); }
};


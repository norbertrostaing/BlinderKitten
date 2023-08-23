/*
  ==============================================================================

    BKPath.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Definitions/Command/CommandSelectionManager.h"

class BKPath:
    public BaseItem
{
    public:
    BKPath(var params = var());
    ~BKPath();

    String objectType;
    var objectData;

    enum PathType { PATH_POINT, PATH_LINE, PATH_ROD, PATH_GRID, PATH_CIRCLE };
    enum GridOrentation { 
        GRID_LR, GRID_RL, GRID_TB, GRID_BT
    };

    EnumParameter* pathType;
    Point2DParameter* position;

    Point2DParameter* lineEndPosition;

    FloatParameter* rodSize;
    FloatParameter* rodAngle;
    
    Point2DParameter* gridSize;
    FloatParameter* gridAngle;
    IntParameter* gridNumberOfElements;
    EnumParameter* gridOrientation;
    BoolParameter* gridZigZag;
    BoolParameter* gridInverseRows;
    Array<std::shared_ptr<Point<float>>> gridPath;
    Point<float> gridTL, gridTR, gridBL, gridBR;

    FloatParameter* circleRadius;
    FloatParameter* circleFrom;
    FloatParameter* circleTo;

    BoolParameter* spreadSubFixtures;
    CommandSelectionManager selection;

    CriticalSection isComputing;
    HashMap<Fixture*, std::shared_ptr<Point<float>>> fixtToPos;
    HashMap<SubFixture*, std::shared_ptr<Point<float>>> subFixtToPos;
    void computeData();

    void onContainerParameterChangedInternal(Parameter*);
    void updateDisplay();

    static void rotateVect(Point<float>* vect, float angleInDegrees);
    static float getVectAngle(Point<float>* vect);

    String getTypeString() const override { return objectType; }
    static BKPath* create(var params) { return new BKPath(params); }

};
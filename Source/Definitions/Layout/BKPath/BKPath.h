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

    enum PathType { PATH_POINT, PATH_LINE, PATH_GRID, PATH_CIRCLE };
    enum GridOrentation { 
        GRID_LR, GRID_RL, GRID_TB, GRID_BT
    };

    EnumParameter* pathType;
    Point2DParameter* position;

    Point2DParameter* lineEndPosition;
    
    Point2DParameter* gridSize;
    FloatParameter* gridAngle;
    IntParameter* gridNumberOfElements;
    EnumParameter* gridOrientation;
    BoolParameter* gridZigZag;
    BoolParameter* gridInverseRows;
    Array<std::shared_ptr<Point<float>>> gridPath;

    FloatParameter* circleRadius;
    FloatParameter* circleFrom;
    FloatParameter* circleTo;

    BoolParameter* spreadSubFixtures;
    CommandSelectionManager selection;

    CriticalSection isComputing;
    HashMap<Fixture*, std::shared_ptr<Vector3D<float>>> fixtToPos;
    HashMap<SubFixture*, std::shared_ptr<Vector3D<float>>> subFixtToPos;
    void computeData();

    
    void onContainerParameterChangedInternal(Parameter*);
    void updateDisplay();

    static void rotateVect(Vector3D<float>* vect, float angleInDegrees);
    static float getVectAngle(Vector3D<float>* vect);
};
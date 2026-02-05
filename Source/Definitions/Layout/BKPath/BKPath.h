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
#include "Common/CommonIncludes.h"

class BKPath:
    public BaseItem
{
    public:
    BKPath(var params = var());
    ~BKPath();

    String objectType;
    var objectData;

    enum PathType { PATH_POINT, PATH_LINE, PATH_ROD, PATH_GRID, PATH_CIRCLE, PATH_PRESET};
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

    FloatParameter* fixturesAngleFrom;
    FloatParameter* fixturesAngleTo;

    IntParameter* presetId;
    FloatParameter* presetScale;
    FloatParameter* presetAngle;

    BoolParameter* spreadSubFixtures;
    Point2DParameter* tilesSize;
    FloatParameter* textSize;
    BoolParameter* overrideStrokeColor;
    ColorParameter* strokeColor;
    BoolParameter* overrideFillColor;
    ColorParameter* fillColor;
    StringParameter* customText;
    EnumParameter* labelPosition;
    enum LabelPosition {CENTER, TOP, RIGHT, BOTTOM, LEFT, HIDDEN};
    CommandSelectionManager selection;
    ActionManager actionManager;

    CriticalSection isComputing;
    HashMap<Fixture*, std::shared_ptr<Point<float>>> fixtToPos;
    HashMap<SubFixture*, std::shared_ptr<Point<float>>> subFixtToPos;
    HashMap<Fixture*, AffineTransform> fixtTransform;
    HashMap<Fixture*, Image> fixtImageBorder;
    HashMap<Fixture*, Image> fixtImageContent;
    HashMap<Fixture*, Image> fixtImageSelected;

    void computeData();

    void onContainerParameterChangedInternal(Parameter*) override;
    void updateDisplay();

    void clicked();
    void clearFixtImages();

    static void rotateVect(Point<float>* vect, float angleInDegrees);
    static float getVectAngle(Point<float>* vect);

    String getTypeString() const override { return objectType; }
    static BKPath* create(var params) { return new BKPath(params); }

};

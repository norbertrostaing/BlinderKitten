/*
  ==============================================================================

    BKPath.cpp
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#include "BKPath.h"
#include "Definitions/Fixture/Fixture.h"
#include "Definitions/SubFixture/SubFixture.h"

BKPath::BKPath(var params) :
    BaseItem(params.getProperty("name", "Path")),
    objectType(params.getProperty("type", "BKPath").toString()),
    objectData(params),
    subFixtToPos(4096, SubFixture::MyHashGenerator())
{
    saveAndLoadRecursiveData = true;
    var d; d.append(0); d.append(0);
    pathType = addEnumParameter("Type", "Type of path");
    pathType->addOption("Point", PATH_POINT)->addOption("Line", PATH_LINE)->addOption("Rod", PATH_ROD)->addOption("Grid", PATH_GRID)->addOption("Circle", PATH_CIRCLE);
    position = addPoint2DParameter("Position", "Position in your layout");

    d[0] = 5;    d[1] = 0;
    lineEndPosition = addPoint2DParameter("End position", "");
    lineEndPosition->setDefaultValue(d);

    rodSize = addFloatParameter("Rod size", "", 1,0);
    rodAngle = addFloatParameter("Rod Angle", "Angle of your rod", 0, -360, 360);

    d[0] = 4;    d[1] = 4;
    gridSize = addPoint2DParameter("Size", "Size of your grid");
    gridSize->setDefaultValue(d);
    gridSize->setBounds(0,0, (float)INT32_MAX, (float)INT32_MAX);
    gridAngle = addFloatParameter("Grid Angle", "Angle of your grid", 0,-360,360);
    gridNumberOfElements = addIntParameter("Number per line", "Change direction after N elements",8,2);
    gridOrientation = addEnumParameter("Orientation", "Grid orientation");
    gridOrientation->addOption("Left to right", GRID_LR)
        ->addOption("Right to left", GRID_RL)
        ->addOption("Top to bottom", GRID_TB)
        ->addOption("Bottom to top", GRID_BT)
        ;
    gridZigZag = addBoolParameter("Zig zag", "", false);
    gridInverseRows = addBoolParameter("Reverse lines", "", false);

    circleRadius = addFloatParameter("Radius", "Radius of the circle", 2, 0);
    circleFrom = addFloatParameter("From angle", "Angle of first element", 0, -360, 360);
    circleTo = addFloatParameter("To angle", "Angle of the last element", 360, -360, 360);

    d[0] = 20;    d[1] = 20;
    tilesSize = addPoint2DParameter("Tiles size", "Size of your tiles in px");
    tilesSize->setDefaultValue(d);
    textSize = addFloatParameter("Text size", "", 10, 0);
    spreadSubFixtures = addBoolParameter("Spread Subfixts", "if checked, subfixtures will be spread along the path, if not, only fixture wil be", false);

    overrideColor = addBoolParameter("Override color", "", false);
    pathColor = addColorParameter("Tiles color", "", juce::Colours::lightcyan);
    customText = addStringParameter("Custom Text", "Write your own text on your tile", "");
    fixturesAngleFrom = addFloatParameter("Fixture rotation from", "Angle of first element", 0, -360, 360);
    fixturesAngleTo = addFloatParameter("Fixture rotation to", "Angle of the last element", 0, -360, 360);
    labelPosition = addEnumParameter("Label position", "Position of your labels for this path");
    labelPosition->addOption("Centered", CENTER)->addOption("Top", TOP)->addOption("Bottom", BOTTOM)->addOption("Left", LEFT)->addOption("Right", RIGHT);

    addChildControllableContainer(&selection);
    addChildControllableContainer(&actionManager);

    updateDisplay();
};

BKPath::~BKPath()
{
};

void BKPath::computeData()
{
    isComputing.enter();
    Array<Fixture*> fixts;
    Array<SubFixture*> subFixts;
    subFixtToPos.clear();
    fixtToPos.clear();
    selection.computeSelection();
    for (int isf = 0; isf < selection.computedSelectedSubFixtures.size(); isf++) {
        SubFixture* sf = selection.computedSelectedSubFixtures[isf];
        if (sf != nullptr) {
            subFixts.addIfNotAlreadyThere(sf);
            if (sf->parentFixture != nullptr) {
                fixts.addIfNotAlreadyThere(sf->parentFixture);
            }
        }
    }

    Point<float> origin;
    origin.x = float(position->getValue()[0]);
    origin.y = float(position->getValue()[1]);

    PathType type = pathType->getValueDataAsEnum<PathType>();
    if (type == PATH_POINT) {
        for (int i = 0; i < subFixts.size(); i++) {
            subFixtToPos.set(subFixts[i], std::make_shared<Point<float>>(position->getValue()[0], position->getValue()[1]));
        }
        for (int i = 0; i < fixts.size(); i++) {
            fixtToPos.set(fixts[i], std::make_shared<Point<float>>(position->getValue()[0], position->getValue()[1]));
        }
    }
    else if (type == BKPath::PATH_LINE) {
        Point<float> temp;
        Point<float> delta;

        delta.x = float(lineEndPosition->getValue()[0]) - float(position->getValue()[0]);
        delta.y = float(lineEndPosition->getValue()[1]) - float(position->getValue()[1]);
        int test = subFixts.size();
        for (int i = 0; i < subFixts.size(); i++) {
            Fixture* f = subFixts[i]->parentFixture;
            //std::shared_ptr<Point<float>> vect = std::make_shared<Point<float>>();

            float ratio = subFixts.size() > 1 ? (float)i / ((float)subFixts.size() - 1) : 0.5;
            if (!spreadSubFixtures->boolValue()) {
                ratio = fixts.size() > 1 ? (float)fixts.indexOf(f) / ((float)fixts.size() - 1) : 0.5;
            }
            temp.x = 0; temp.y = 0;
            temp += delta;
            temp *= ratio;
            temp.x += origin.x;
            temp.y += origin.y;
            subFixtToPos.set(subFixts[i], std::make_shared<Point<float>>(temp.x, temp.y));

            // LOG(f->id->stringValue()+" "+String(subFixts[i]->subId)+" - x:"+String(temp.x)+"  y:" + String(temp.y));

            if (!fixtToPos.contains(f)) {
                fixtToPos.set(f, std::make_shared<Point<float>>(temp.x, temp.y));
            }
        }

    }
    else if (type == BKPath::PATH_ROD) {
        Point<float> start;
        Point<float> end;
        Point<float> delta;
        Point<float> temp;

        Point<float> halfRod(rodSize->floatValue()/2, 0);
        rotateVect(&halfRod, rodAngle->floatValue());
        end = halfRod + origin;
        rotateVect(&halfRod, 180.0);
        start = halfRod+origin;

        gridTL = start;
        gridTR = end;

        delta = end-start;

        int test = subFixts.size();
        for (int i = 0; i < subFixts.size(); i++) {
            Fixture* f = subFixts[i]->parentFixture;
            //std::shared_ptr<Point<float>> vect = std::make_shared<Point<float>>();

            float ratio = subFixts.size() > 1 ? (float)i / ((float)subFixts.size() - 1) : 0.5;
            if (!spreadSubFixtures->boolValue()) {
                ratio = fixts.size() > 1 ? (float)fixts.indexOf(f) / ((float)fixts.size() - 1) : 0.5;
            }
            temp.x = 0; temp.y = 0;
            temp += delta;
            temp *= ratio;
            temp.x += start.x;
            temp.y += start.y;
            subFixtToPos.set(subFixts[i], std::make_shared<Point<float>>(temp.x, temp.y));

            // LOG(f->id->stringValue()+" "+String(subFixts[i]->subId)+" - x:"+String(temp.x)+"  y:" + String(temp.y));

            if (!fixtToPos.contains(f)) {
                fixtToPos.set(f, std::make_shared<Point<float>>(temp.x, temp.y));
            }
        }

    }
    else if (type == BKPath::PATH_GRID) {
        Point<float> currentPos(0, 0);
        Point<float> deltaCol(0, 0);
        Point<float> deltaRow(0, 0);
        Point<float> deltaOrigin(0, 0);
        Point<float> lineOrigin(0, 0);

        int nElements = spreadSubFixtures->boolValue() ? subFixts.size() : fixts.size();
        int nPerRow = gridNumberOfElements->intValue();
        int nRows = ceil(nElements/(float)nPerRow);

        float gridWidth = gridSize->getValue()[0];
        float gridHeight = gridSize->getValue()[1];

        deltaOrigin.x -= gridWidth / 2;
        deltaOrigin.y += gridHeight / 2;

        GridOrentation o = gridOrientation->getValueDataAsEnum<GridOrentation>();
        if (o == GRID_LR) {
            deltaCol.x = gridWidth / (float)(nPerRow - 1);
            deltaRow.y = nRows > 1 ? gridHeight / (float)(nRows-1) : 0;
        }
        else if (o == GRID_RL) {
            deltaCol.x = -gridWidth / (float)(nPerRow - 1);
            deltaRow.y = nRows > 1 ? gridHeight / (float)(nRows-1) : 0;
            deltaOrigin.x += gridWidth;
        }
        else if (o == GRID_TB) {
            deltaRow.x = nRows > 1 ? gridWidth / (float)(nRows-1) : 0;
            deltaCol.y = -gridHeight / (float)(nPerRow - 1);
        }
        else if(o == GRID_BT) {
            deltaRow.x = nRows > 1 ? gridWidth / (float)(nRows-1) : 0;
            deltaCol.y = gridHeight / (float)(nPerRow - 1);
            deltaOrigin.y -= gridHeight;
        }

        deltaRow.y *= -1;
        if (gridInverseRows->boolValue()) {
            if (o == GRID_LR || o == GRID_RL) {
                deltaOrigin.y -= gridHeight;
                deltaRow.y *= -1;
            }
            else if (o == GRID_TB || o == GRID_BT) {
                deltaOrigin.x += gridWidth;
                deltaRow.x *= -1;
            }
        }


        rotateVect(&deltaOrigin, gridAngle->floatValue());
        rotateVect(&deltaRow, gridAngle->floatValue());
        rotateVect(&deltaCol, gridAngle->floatValue());

        gridTL.setXY(-gridWidth / 2, gridHeight / 2);
        gridTR.setXY(gridWidth / 2, gridHeight / 2);
        gridBL.setXY(-gridWidth / 2, -gridHeight / 2);
        gridBR.setXY(gridWidth / 2, -gridHeight / 2);
        rotateVect(&gridTL, gridAngle->floatValue());
        rotateVect(&gridTR, gridAngle->floatValue());
        rotateVect(&gridBL, gridAngle->floatValue());
        rotateVect(&gridBR, gridAngle->floatValue());
        gridTL += origin;
        gridTR += origin;
        gridBL += origin;
        gridBR += origin;

        // apply rotations here
        currentPos.x = origin.x + deltaOrigin.x;
        currentPos.y = origin.y + deltaOrigin.y;
        lineOrigin.x = currentPos.x;
        lineOrigin.y = currentPos.y;

        int currentCol = 0;
        int currentRow = 0;
        String test = niceName;
        gridPath.clear();

        for (int i = 0; i < nElements; i++) {
            if (spreadSubFixtures->boolValue()) {
                Fixture* f = subFixts[i]->parentFixture;
                subFixtToPos.set(subFixts[i], std::make_shared<Point<float>>(currentPos.x, currentPos.y));
            }
            else {
                Fixture* f = fixts[i];
                fixtToPos.set(f, std::make_shared<Point<float>>(currentPos.x, currentPos.y));
            }
            //LOG(f->id->stringValue()+" "+String(subFixts[i]->subId)+" - x:"+String(currentPos.x)+"  y:" + String(currentPos.y));
            gridPath.add(std::make_shared<Point<float>>(currentPos.x, currentPos.y));
            if (currentCol == nPerRow - 1) {
                currentCol = 0;
                currentRow += 1;
                if (!gridZigZag->boolValue()) {
                    currentPos.x = lineOrigin.x;
                    currentPos.y = lineOrigin.y;
                }
                else {
                    deltaCol = -deltaCol;
                }
                currentPos += deltaRow;
                lineOrigin = currentPos;
            }
            else {
                currentCol += 1;
                currentPos += deltaCol;
            }
        }

    }
    else if (type == BKPath::PATH_CIRCLE)
    {
        Point<float> offset(circleRadius->floatValue(), 0);
        Point<float> temp(0, 0);
        float angleTo = circleTo->floatValue();
        float angleFrom = circleFrom->floatValue();

        int nElements = spreadSubFixtures->boolValue() ? subFixts.size() : fixts.size();

        float deltaAngle = angleTo - angleFrom;
        if (nElements > 1) {
            if (abs(deltaAngle) >= 360) {
                deltaAngle = 360;
                deltaAngle /= (float)(nElements);
            }
            else {
                deltaAngle /= (float)(nElements - 1);
            }
        }

        for (int i = 0; i < nElements; i++) {
            temp = offset;
            rotateVect(&temp, angleFrom + (i*deltaAngle));
            temp += origin;
            if (spreadSubFixtures->boolValue()) {
                Fixture* f = subFixts[i]->parentFixture;
                subFixtToPos.set(subFixts[i], std::make_shared<Point<float>>(temp.x, temp.y));
            }
            else {
                Fixture* f = fixts[i];
                fixtToPos.set(f, std::make_shared<Point<float>>(temp.x, temp.y));
            }
        }

    }

    if (!spreadSubFixtures->boolValue()) {
        for (auto it = fixtToPos.begin(); it != fixtToPos.end(); it.next()) {
            Fixture* f = it.getKey();
            for (auto it2 = f->subFixtures.begin(); it2 != f->subFixtures.end(); it2.next()) {
                SubFixture* sf = it2.getValue();
                subFixtToPos.set(sf, it.getValue());
            }
        }
    }
    isComputing.exit();
}

void BKPath::onContainerParameterChangedInternal(Parameter* c) {
    if (c == pathType || c == overrideColor) {
        updateDisplay();
    }
    clearFixtImages();
}

void BKPath::updateDisplay() {

    lineEndPosition->hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_LINE;

    rodSize->hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_ROD;
    rodAngle->hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_ROD;

    gridSize -> hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_GRID;
    gridAngle -> hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_GRID;
    gridNumberOfElements -> hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_GRID;
    gridOrientation -> hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_GRID;
    gridZigZag -> hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_GRID;
    gridInverseRows->hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_GRID;
    
    circleRadius->hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_CIRCLE;
    circleFrom->hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_CIRCLE;
    circleTo->hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_CIRCLE;

    pathColor->hideInEditor = !overrideColor->boolValue();

    actionManager.hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_POINT;
    customText->hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_POINT;

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void BKPath::clicked()
{
    actionManager.setValueAll(1.f);
    actionManager.setValueAll(0.f);
}

void BKPath::clearFixtImages()
{
    fixtTransform.clear();
    fixtImageBorder.clear();
    fixtImageContent.clear();
}

void BKPath::rotateVect(Point<float>* vect, float angleInDegrees)
{
    float angle = -angleInDegrees / 360.0;
    angle = angle * 2 * MathConstants<float>::pi;
    float x1 = vect->x;
    float y1 = vect->y;
    vect->x = (cos(angle) * x1) - (sin(angle) * y1);
    vect->y = (sin(angle) * x1) + (cos(angle) * y1);
}

float BKPath::getVectAngle(Point<float>* vect)
{
    Point<float> temp(vect->x, vect->y);
    temp /= temp.getDistanceFromOrigin();
    float angle = acosf(temp.x);
    if (vect->y > 0) {
        angle = -angle;
    }
    return angle;
}

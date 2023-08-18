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
    objectData(params)
{
    saveAndLoadRecursiveData = true;
    pathType = addEnumParameter("Type", "Type of path");
    pathType->addOption("Point", PATH_POINT)->addOption("Line", PATH_LINE)->addOption("Grid", PATH_GRID)->addOption("Circle", PATH_CIRCLE);
    position = addPoint2DParameter("Position", "Position in pexiels in your layout");

    lineEndPosition = addPoint2DParameter("End position", "");

    gridSize = addPoint2DParameter("Size", "Size of yout grid");
    gridSize->setBounds(0,0, (float)INT32_MAX, (float)INT32_MAX);
    gridAngle = addFloatParameter("Angle", "Angle of your grid", 0,-360,360);
    gridNumberOfElements = addIntParameter("Number per line", "Change direction after N elements",2,2);
    gridOrientation = addEnumParameter("Orientation", "Grid orientation");
    gridOrientation->addOption("Left to right", GRID_LR)
        ->addOption("Right to left", GRID_RL)
        ->addOption("Top to bottom", GRID_TB)
        ->addOption("Bottom to top", GRID_BT)
        ;
    gridZigZag = addBoolParameter("Zig zag", "", false);
    gridInverseRows = addBoolParameter("Reverse lines", "", false);

    circleRadius = addFloatParameter("Radius", "Radius of the circle", 0, 0);
    circleFrom = addFloatParameter("From angle", "Angle of first element", 0, -360, 360);
    circleTo = addFloatParameter("To angle", "Angle of the last element", 0, -360, 360);

    addChildControllableContainer(&selection);
    spreadSubFixtures = addBoolParameter("Spread Subfixts", "if checked, subfixtures will be spread along the path, if not, only fixture wil be", true);

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

    Vector3D<float> origin;
    origin.x = float(position->getValue()[0]);
    origin.y = float(position->getValue()[1]);
    origin.z = 0;

    PathType type = pathType->getValueDataAsEnum<PathType>();
    if (type == PATH_POINT) {
        for (int i = 0; i < subFixts.size(); i++) {
            subFixtToPos.set(subFixts[i], std::make_shared<Vector3D<float>>(position->getValue()[0], position->getValue()[1],0));
        }
        for (int i = 0; i < fixts.size(); i++) {
            fixtToPos.set(fixts[i], std::make_shared<Vector3D<float>>(position->getValue()[0], position->getValue()[1],0));
        }
    }
    else if (type == BKPath::PATH_LINE) {
        Vector3D<float> temp;
        Vector3D<float> delta;

        delta.x = float(lineEndPosition->getValue()[0]) - float(position->getValue()[0]);
        delta.y = float(lineEndPosition->getValue()[1]) - float(position->getValue()[1]);
        delta.z = 0;

        for (int i = 0; i < subFixts.size(); i++) {
            Fixture* f = subFixts[i]->parentFixture;
            //std::shared_ptr<Vector3D<float>> vect = std::make_shared<Vector3D<float>>();

            float ratio = subFixts.size() > 1 ? (float)i / ((float)subFixts.size()-1) : 0.5;
            if (!spreadSubFixtures->boolValue()) {
                ratio = fixts.size() > 1 ? (float)fixts.indexOf(f) / ((float)fixts.size() - 1) : 0.5;
            }
            temp.x = 0; temp.y = 0; temp.z = 0;
            temp += delta;
            temp *= ratio;
            temp.x += origin.x;
            temp.y += origin.y;
            subFixtToPos.set(subFixts[i], std::make_shared<Vector3D<float>>(temp.x, temp.y, 0));

            // LOG(f->id->stringValue()+" "+String(subFixts[i]->subId)+" - x:"+String(temp.x)+"  y:" + String(temp.y));

            if (!fixtToPos.contains(f)) {
                fixtToPos.set(f, std::make_shared<Vector3D<float>>(temp.x, temp.y, 0));
            }
        }

    }
    else if (type == BKPath::PATH_GRID) {
        Vector3D<float> currentPos(0, 0, 0);
        Vector3D<float> deltaCol(0,0,0);
        Vector3D<float> deltaRow(0, 0, 0);
        Vector3D<float> deltaOrigin(0, 0, 0);
        Vector3D<float> lineOrigin(0, 0, 0);

        int nElements = spreadSubFixtures->boolValue() ? subFixts.size() : fixts.size();
        int nPerRow = gridNumberOfElements->intValue();
        int nRows = ceil(nElements/(float)nPerRow);

        float gridWidth = gridSize->getValue()[0];
        float gridHeight = gridSize->getValue()[1];

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
            deltaCol.y = gridHeight / (float)(nPerRow - 1);
        }
        else if(o == GRID_BT) {
            deltaRow.x = nRows > 1 ? gridWidth / (float)(nRows-1) : 0;
            deltaCol.y = -gridHeight / (float)(nPerRow - 1);
            deltaOrigin.y += gridHeight;
        }

        if (gridInverseRows->boolValue()) {
            if (o == GRID_LR || o == GRID_RL) {
                deltaOrigin.y += gridHeight;
            }
            else if (o == GRID_TB || o == GRID_BT) {
                deltaOrigin.x += gridWidth;
            }
            deltaRow *= -1;
        }

        rotateVect(&deltaOrigin, gridAngle->floatValue());
        rotateVect(&deltaRow, gridAngle->floatValue());
        rotateVect(&deltaCol, gridAngle->floatValue());

        // apply rotations here
        currentPos.x = origin.x + deltaOrigin.x;
        currentPos.y = origin.y + deltaOrigin.y;
        lineOrigin.x = currentPos.x;
        lineOrigin.y = currentPos.y;

        int currentCol = 0;
        int currentRow = 0;

        gridPath.clear();

        for (int i = 0; i < nElements; i++) {
            if (spreadSubFixtures->boolValue()) {
                Fixture* f = subFixts[i]->parentFixture;
                subFixtToPos.set(subFixts[i], std::make_shared<Vector3D<float>>(currentPos.x, currentPos.y, 0));
            }
            else {
                Fixture* f = fixts[i];
                fixtToPos.set(f, std::make_shared<Vector3D<float>>(currentPos.x, currentPos.y, 0));
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
        Vector3D<float> offset(circleRadius->floatValue(), 0, 0);
        Vector3D<float> temp(0, 0, 0);
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
                subFixtToPos.set(subFixts[i], std::make_shared<Vector3D<float>>(temp.x, temp.y, 0));
            }
            else {
                Fixture* f = fixts[i];
                fixtToPos.set(f, std::make_shared<Vector3D<float>>(temp.x, temp.y, 0));
            }
        }

    }
    isComputing.exit();
}

void BKPath::onContainerParameterChangedInternal(Parameter* c) {
    if (c == pathType) {
        updateDisplay();
    }
}

void BKPath::updateDisplay() {

    lineEndPosition -> hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_LINE;

    gridSize -> hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_GRID;
    gridAngle -> hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_GRID;
    gridNumberOfElements -> hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_GRID;
    gridOrientation -> hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_GRID;
    gridZigZag -> hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_GRID;
    gridInverseRows->hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_GRID;
    
    circleRadius->hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_CIRCLE;
    circleFrom->hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_CIRCLE;
    circleTo->hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_CIRCLE;

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void BKPath::rotateVect(Vector3D<float>* vect, float angleInDegrees)
{
    float angle = -angleInDegrees / 360.0;
    angle = angle * 2 * MathConstants<float>::pi;
    float x1 = vect->x;
    float y1 = vect->y;
    vect->x = (cos(angle) * x1) - (sin(angle) * y1);
    vect->y = (sin(angle) * x1) + (cos(angle) * y1);
}

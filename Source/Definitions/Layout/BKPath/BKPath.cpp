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
    pathType->addOption("Point", PATH_POINT)->addOption("Line", PATH_LINE)->addOption("Grid", PATH_GRID);
    position = addPoint2DParameter("Position", "Position in pexiels in your layout");

    lineEndPosition = addPoint2DParameter("End position", "");

    gridSize = addPoint2DParameter("Size", "Size of yout grid");
    gridAngle = addFloatParameter("Angle", "Angle of your grid", 0,0,360);
    gridNumberOfElements = addIntParameter("Number per line", "Change direction after N elements",1,1);
    gridOrientation = addEnumParameter("Orientation", "Grid orientation");
    gridOrientation->addOption("Left to right", GRID_LR)
        ->addOption("Right to left", GRID_RL)
        ->addOption("Top to bottom", GRID_TB)
        ->addOption("Bottom to top", GRID_BT)
        ;
    gridZigZag = addBoolParameter("Zig zag", "", false);
    gridInverseRows = addBoolParameter("Reverse lines", "", false);

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
        subFixts.addIfNotAlreadyThere(sf);
        fixts.addIfNotAlreadyThere(sf->parentFixture);
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
            std::shared_ptr<Vector3D<float>> vect = std::make_shared<Vector3D<float>>();

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

            LOG(f->id->stringValue()+" "+String(subFixts[i]->subId)+" - x:"+String(temp.x)+"  y:" + String(temp.y));

            if (!fixtToPos.contains(f)) {
                fixtToPos.set(f, std::make_shared<Vector3D<float>>(temp.x, temp.y, 0));
            }
        }

    }
    else if (type == BKPath::PATH_GRID) {

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
    gridInverseRows -> hideInEditor = pathType->getValueDataAsEnum<PathType>() != PATH_GRID;

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}
/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Layout.h"
#include "LayoutManager.h"
#include "BKPath/BKPath.h"
#include "Definitions/Fixture/Fixture.h"
#include "Definitions/SubFixture/SubFixture.h"
#include "Brain.h"
#include "UI/LayoutViewer.h"
#include "UserInputManager.h"


Layout::Layout(var params) :
	BaseItem(params.getProperty("name", "Layout")),
	objectType(params.getProperty("Layouts", "Layout").toString()),
	// parameters("Parameters"),
	paths("Paths"),
	objectData(params),
	subFixtToPos(4096, SubFixture::MyHashGenerator())
	// previousID(-1),
	// slideManipParameter(nullptr)
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = true;

	itemDataType = "Layout";
	canBeDisabled = false;

	// definitions->addBaseManagerListener(this);

	id = addIntParameter("ID", "ID of this layout", 1, 1);
	userName = addStringParameter("Name", "Name of this layout", "New layout");
	updateName();

	dimensionsX = addPoint2DParameter("From To X", "");
	var dx = -10; dx.append(10);
	dimensionsX->setDefaultValue(dx);
	dimensionsY = addPoint2DParameter("From To Y", "");
	var dy = -10; dy.append(10);
	dimensionsY->setDefaultValue(dy);

	addChildControllableContainer(&paths);
	paths.selectItemWhenCreated = false;

	var objectsData = params.getProperty("objects", var());
	Brain::getInstance()->registerLayout(this, id->getValue());

}

Layout::~Layout()
{
	Brain::getInstance()->unregisterLayout(this);
}

void Layout::updateName()
{
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<LayoutManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
}

void Layout::onContainerParameterChangedInternal(Parameter* p)
{
	if (p == userName || p == id) {
		updateName();
	}
	sendChangeMessage();
	
}

void Layout::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	computeData();
	sendChangeMessage();
}

void Layout::computeData()
{
	isComputing.enter();

	subFixtToPos.clear();
	fixtToPos.clear();
	for (int iPath = 0; iPath < paths.items.size(); iPath++) {
		BKPath* p = paths.items[iPath];
		if (p->enabled->boolValue()) {
			p->computeData();
			p->isComputing.enter();
			int test = p->subFixtToPos.size();
			for (auto it = p->subFixtToPos.begin(); it != subFixtToPos.end(); it.next()) {
				subFixtToPos.set(it.getKey(), it.getValue());
			}
			p->isComputing.exit();
		}
	}

	isComputing.exit();
}

std::shared_ptr<HashMap<SubFixture*, float>> Layout::getSubfixturesRatioFromDirection(float angle)
{
	std::shared_ptr<HashMap<SubFixture*, float>> ret = std::make_shared<HashMap<SubFixture*, float>>();
	Vector3D<float> xAxis(1,0,0);
	BKPath::rotateVect(&xAxis, angle);

	float minDot = (float)INT32_MAX;
	float maxDot = -(float)INT32_MAX;
	computeData();
	isComputing.enter();
	for (auto it = subFixtToPos.begin(); it != subFixtToPos.end(); it.next()) {
		Vector3D<float> sfAxis(it.getValue()->x, it.getValue()->y, 0);
		float dot = xAxis*sfAxis;
		minDot = jmin(minDot, dot);
		maxDot = jmax(maxDot, dot);
		ret->set(it.getKey(), dot);
	}
	isComputing.exit();
	for (auto it = ret->begin(); it != ret->end(); it.next()) {
		ret->set(it.getKey(), jmap(it.getValue(), minDot, maxDot, (float)0, (float)1));
	}
	return ret;
}

std::shared_ptr<HashMap<SubFixture*, float>> Layout::getSubfixturesRatioFromOriginAndAngle(Vector3D<float>* vect, float angle, bool normalize, bool clockwise)
{
	float twoPi = 2 * MathConstants<float>::pi;
	float fourPi = 4 * MathConstants<float>::pi;
	angle = (angle / 360.0)*twoPi;
	angle = fmod(angle + fourPi, twoPi);
	std::shared_ptr<HashMap<SubFixture*, float>> ret = std::make_shared<HashMap<SubFixture*, float>>();
	Vector3D<float> origin(vect->x, vect->y, 0);
	computeData();
	isComputing.enter();
	float minRatio = 1;
	float maxRatio = 0;
	for (auto it = subFixtToPos.begin(); it != subFixtToPos.end(); it.next()) {
		Vector3D<float> sfAxis(it.getValue()->x, it.getValue()->y, 0);
		sfAxis -= origin;
		float sfAngle = BKPath::getVectAngle(&sfAxis); // angle between -PI and PI
		sfAngle -= angle;
		sfAngle = fmod(sfAngle + fourPi, twoPi);
		float ratio = jmap(sfAngle, 0.0f, twoPi, 0.0f, 1.0f);
		minRatio = jmin(minRatio, ratio);
		maxRatio = jmax(maxRatio, ratio);
		ret->set(it.getKey(), ratio);
	}
	isComputing.exit();

	if (normalize) {
		for (auto it = ret->begin(); it != ret->end(); it.next()) {
			ret->set(it.getKey(), jmap(it.getValue(), minRatio, maxRatio, (float)0, (float)1));
		}
	}

	if (clockwise) {
		for (auto it = ret->begin(); it != ret->end(); it.next()) {
			ret->set(it.getKey(), 1.0-it.getValue());
		}
	}

	return ret;
}

std::shared_ptr<HashMap<SubFixture*, float>> Layout::getSubfixturesRatioFromOrigin(Vector3D<float>* vect)
{
	std::shared_ptr<HashMap<SubFixture*, float>> ret = std::make_shared<HashMap<SubFixture*, float>>();
	Vector3D<float> origin(vect->x, vect->y, 0);
	computeData();
	isComputing.enter();
	float minDist = UINT16_MAX;
	float maxDist = 0;
	for (auto it = subFixtToPos.begin(); it != subFixtToPos.end(); it.next()) {
		Vector3D<float> sfAxis(it.getValue()->x, it.getValue()->y, 0);

		float deltaX = sfAxis.x - vect->x;
		float deltaY = sfAxis.y - vect->y;
		float dist = sqrtf((deltaX*deltaX)+(deltaY*deltaY));
		minDist = jmin(minDist, dist);
		maxDist = jmax(maxDist, dist);
		ret->set(it.getKey(), dist);
	}
	isComputing.exit();

	for (auto it = ret->begin(); it != ret->end(); it.next()) {
		ret->set(it.getKey(), jmap(it.getValue(), minDist, maxDist, (float)0, (float)1));
	}

	return ret;
}



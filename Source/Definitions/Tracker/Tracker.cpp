/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Tracker.h"
#include "../Command/CommandSelectionManager.h"
#include "../SubFixture/SubFixtureChannel.h"
#include "../../Brain.h"
#include "TrackerManager.h"
#include "../CurvePreset/CurvePreset.h"
#include "../ChannelValue.h"
#include "BKEngine.h"

Tracker::Tracker(var params) :
	BaseItem(params.getProperty("name", "Tracker")),
	objectType(params.getProperty("type", "Tracker").toString()),
	objectData(params),
	selection()
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;

	itemDataType = "Tracker";

	id = addIntParameter("ID", "ID of this Tracker", 1, 1);
	userName = addStringParameter("Name", "Name of this Tracker", "New Tracker");
	layerId = addIntParameter("Layer", "Higher layer, higer priority", 1, 1);
	updateName();

	isTrackerOn = addBoolParameter("is ON", "Enable or disable this Tracker",false);
	isTrackerOn->isControllableFeedbackOnly;
	isTrackerOn->setEnabled(false);
	isTrackerOn->isSavable = false;
	isOn = false;

	startBtn = addTrigger("Start", "Start this Tracker");
	stopBtn = addTrigger("Stop", "Stop this Tracker");

	autoStartAndStop = addBoolParameter("Auto Start / Stop", "Start and stop the Tracker when size is modified", true);
	sizeValue = addFloatParameter("Size", "Master of this Tracker", 1, 0, 1);

	targetPosition = addPoint3DParameter("Target position", "");

	addChildControllableContainer(&selection);

	Brain::getInstance()->registerTracker(this, id->getValue());

}

Tracker::~Tracker()
{
	Brain::getInstance()->unregisterTracker(this);
	Brain::getInstance()->usingCollections.enter();
	Brain::getInstance()->trackerPoolWaiting.removeAllInstancesOf(this);
	Brain::getInstance()->trackerPoolUpdating.removeAllInstancesOf(this);
	Brain::getInstance()->usingCollections.exit();
	isComputing.enter();
	for (auto it = chanToVal.begin(); it != chanToVal.end(); it.next()) {
		SubFixtureChannel* sfc = it.getKey();
		sfc->trackerOutOfStack(this);
		Brain::getInstance()->pleaseUpdate(sfc);
	}
	isComputing.exit();
}

void Tracker::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
	pleaseComputeIfRunning();
}


void Tracker::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerTracker(this, id->getValue(), true);
	}
	if (p == isTrackerOn) {
		Brain::getInstance()->virtualButtonsNeedUpdate = true;
		Brain::getInstance()->virtualFaderButtonsNeedUpdate = true;
	}
	if (p == userName || p == id) {
		updateName();
	}
	if (p == sizeValue) {
		if (autoStartAndStop->getValue()) {
			if (isOn && (float)sizeValue->getValue() == 0) {
				stop();
			}
			else if(!isOn && (float)sizeValue->getValue() > 0 && lastSize == 0) {
				start();
			}
		}
		lastSize = p->getValue();
		currentSizeController = nextSizeController;
		nextSizeController = "";
		Brain::getInstance()->virtualFadersNeedUpdate = true;
		updateAllSubFixtures();
	}
	if (p == targetPosition) {
		pleaseComputeIfRunning();
	}
}

void Tracker::triggerTriggered(Trigger* t) {
	if (t == startBtn) {
		start();
	}
	else if (t == stopBtn) {
		stop();
	}
	else {}
}

void Tracker::start() {
	isOn = true;
	isTrackerOn->setValue(true);
	computeData();
}

void Tracker::stop() {
	isOn = false;
	isTrackerOn->setValue(false);
	for (auto it = chanToVal.begin(); it != chanToVal.end(); it.next()) {
		if (it.getKey() != nullptr) {
			it.getKey()->trackerOutOfStack(this);
			Brain::getInstance()->pleaseUpdate(it.getKey());
		}
	}
}

void Tracker::update(double now) {
	if (computed == false) {
		computeData();
	}
}

void Tracker::pleaseComputeIfRunning() {
	if (isOn) {
		computed = false;
		Brain::getInstance()->pleaseUpdate(this);
	}
}

Vector3D<float> multiply(Vector3D<float> vec, Matrix3D<float> mat) {
	Vector3D<float> ret;

	// Ajoute une composante w égale à 1 au vecteur
	float w = 1.0f;

	ret.x = vec.x * mat.mat[0] + vec.y * mat.mat[4] + vec.z * mat.mat[8] + w * mat.mat[12];
	ret.y = vec.x * mat.mat[1] + vec.y * mat.mat[5] + vec.z * mat.mat[9] + w * mat.mat[13];
	ret.z = vec.x * mat.mat[2] + vec.y * mat.mat[6] + vec.z * mat.mat[10] + w * mat.mat[14];

	return ret;
}

void Tracker::computeData() {
	if (computing) {return;}
	computed = true;
	computing = true;

	updateAllSubFixtures();

	BKEngine* engine = dynamic_cast<BKEngine*>(BKEngine::mainEngine);
	ChannelType* panChanType = dynamic_cast<ChannelType*>(engine->TPanChannel->targetContainer.get());
	ChannelType* tiltChanType = dynamic_cast<ChannelType*>(engine->TTiltChannel->targetContainer.get());

	if (panChanType != nullptr && tiltChanType != nullptr) {
		selection.computeSelection();
		chanToVal.clear();

		for (int i = 0; i < selection.computedSelectedSubFixtures.size(); i++) {
			SubFixture * sf = selection.computedSelectedSubFixtures[i];
			SubFixtureChannel* sfcPan = sf->channelsMap.contains(panChanType) ? sf->channelsMap.getReference(panChanType) : nullptr;
			SubFixtureChannel* sfcTilt = sf->channelsMap.contains(tiltChanType) ? sf->channelsMap.getReference(tiltChanType) : nullptr;

			if (sfcPan != nullptr && sfcTilt != nullptr && sfcPan->parentFixtureTypeChannel != nullptr&& sfcTilt->parentFixtureTypeChannel != nullptr) {
				
				Fixture* f = sf->parentFixture;

				Vector3D<float> fixtPosition = f->position->getVector(); 
				Vector3D<float> fixtRotation = f->rotation->getVector(); 
				fixtRotation.x = degreesToRadians(fixtRotation.x);
				fixtRotation.y = degreesToRadians(fixtRotation.y);
				fixtRotation.z = degreesToRadians(fixtRotation.z);
				Vector3D<float> target = targetPosition->getVector(); 


				Matrix3D<float> rotate = Matrix3D<float>::rotation(fixtRotation); 
				//Matrix3D<float> translate = Matrix3D<float>::fromTranslation(fixtPosition);

				target -= fixtPosition;
				Vector3D<float> rotated = multiply(target, rotate);
				//Vector3D<float> translated = multiply(rotated, translate);

				//LOG("translated x : " << rotated.x << "    y : " << rotated.y);

				Point<float> panPos = Point<float>(rotated.x, rotated.y);
				float dist = (sqrt((rotated.x * rotated.x) + (rotated.y * rotated.y)));
				float panAngle  = radiansToDegrees( std::atan2(rotated.y, rotated.x));
				float tiltAngle = 90+radiansToDegrees( std::atan2(rotated.z, dist));
				float minPan = sfcPan->parentFixtureTypeChannel->physicalRange->x;
				float maxPan = sfcPan->parentFixtureTypeChannel->physicalRange->y;
				float minTilt = sfcTilt->parentFixtureTypeChannel->physicalRange->x;
				float maxTilt = sfcTilt->parentFixtureTypeChannel->physicalRange->y;

				//LOG("origin pan : " << panAngle << "    origin tilt : "<< tiltAngle);
				if (minPan < maxPan) {
					while (panAngle < minPan) (panAngle += 360.0f);
					while (panAngle > maxPan) (panAngle -= 360.0f);
				}
				else {
					while (panAngle > minPan) (panAngle -= 360.0f);
					while (panAngle < maxPan) (panAngle += 360.0f);
				}

				if (minTilt < maxTilt) {
					while (tiltAngle < minTilt) (tiltAngle += 360.0f);
					while (tiltAngle > maxTilt) (tiltAngle -= 360.0f);
				}
				else {
					while (tiltAngle > minTilt) (tiltAngle -= 360.0f);
					while (tiltAngle < maxTilt) (tiltAngle += 360.0f);
				}
				//LOG("pan : " << panAngle << "    tilt : " << tiltAngle);

				float panValue = jmap(panAngle, minPan, maxPan, 0.0f, 1.0f);
				float tiltValue = jmap(tiltAngle, minTilt, maxTilt, 0.0f, 1.0f);
				//LOG("pan val : " << panValue << "    tilt  val: " << tiltValue);
				chanToVal.set(sfcPan, panValue);
				chanToVal.set(sfcTilt, tiltValue);
			}

		}
		//LOG("x : " << translated.x << " - y : " << translated.y << " - z :  " << translated.z << " ");
	
		if (isOn) {
			for (auto it = chanToVal.begin(); it != chanToVal.end(); it.next()) {
				if (it.getKey() != nullptr) {
					it.getKey()->trackerOnTopOfStack(this);
					Brain::getInstance()->pleaseUpdate(it.getKey());
				}
			}
			Brain::getInstance()->pleaseUpdate(this);
		}
	}
	isComputing.exit();
	computing = false;
}

float Tracker::applyToChannel(SubFixtureChannel* fc, float currentVal, double now) {
	if (!chanToVal.contains(fc)) {return currentVal; }
	if (computing) { return currentVal; }

	float calcValue = chanToVal.getReference(fc);
	float s = sizeValue->getValue();
	currentVal = jmap(s, currentVal, calcValue);
	//LOG(currentVal);
	return currentVal;
}

void Tracker::updateAllSubFixtures()
{
	if (isOn) {
		for (auto it = chanToVal.begin(); it != chanToVal.end(); it.next()) {
			SubFixtureChannel* sfc = it.getKey();
			Brain::getInstance()->pleaseUpdate(sfc);
		}
	}
}


void Tracker::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<TrackerManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
}

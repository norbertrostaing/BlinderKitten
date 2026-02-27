/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Stamp.h"
#include "../Command/CommandSelectionManager.h"
#include "../SubFixture/SubFixtureChannel.h"
#include "../../Brain.h"
#include "StampManager.h"
#include "BKEngine.h"
#include "Definitions/Interface/InterfaceIncludes.h"
#include "Definitions/Layout/Layout.h"

Stamp::Stamp(var params) :
	BaseItem(params.getProperty("name", "Stamp")),
	objectType(params.getProperty("type", "Stamp").toString()),
	objectData(params),
	mappings("PixelMappings"),
	selection()
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;
	canBeDisabled = false;

	itemDataType = "Stamp";

	id = addIntParameter("ID", "ID of this Stamp", 1, 1);
	userName = addStringParameter("Name", "Name of this Stamp", "New Stamp");
	layerId = addIntParameter("Layer", "Higher layer, higer priority", 1, 1);
	updateName();

	ndiParam = new NDIDeviceParameter("NDI Source");
	addParameter(ndiParam);

	layoutId = addIntParameter("Layout ID", "Wich layout do you want to use for placing pixels ?", 0, 0);

	videoPosition = addPoint2DParameter("Video position", "Position of the video (center point)");
	videoSize = addPoint2DParameter("Video size", "Size of the video");
	videoSize->setBounds(0,0,999,999);
	videoSize->setDefaultPoint(1,1);

	isStampOn = addBoolParameter("is ON", "Enable or disable this Stamp",false);
	isStampOn->isControllableFeedbackOnly;
	isStampOn->setEnabled(false);
	isStampOn->isSavable = false;
	isOn = false;

	startBtn = addTrigger("Start", "Start this Stamp");
	stopBtn = addTrigger("Stop", "Stop this Stamp");

	autoStartAndStop = addBoolParameter("Auto Start / Stop", "Start and stop the Stamp when size is modified", true);
	sizeValue = addFloatParameter("Size", "Master of this Stamp", 1, 0, 1);
	flashValue = addFloatParameter("Flash", "Flash master of this Stamp", 1, 0, 1);

	fadeInTime = addFloatParameter("Fade in", "Fade in time in seconds", 0, 0);
	fadeOutTime = addFloatParameter("Fade out", "Fade out time in seconds", 0, 0);

	soloPool = addIntParameter("Solo pool", "If greater than zero, only one element can be activated at a time with this number", 0, 0);

	addChildControllableContainer(&selection);

	mappings.selectItemWhenCreated = false;
	addChildControllableContainer(&mappings);

	initImage(10,10);

	Brain::getInstance()->registerStamp(this, id->getValue());

}

Stamp::~Stamp()
{
	if (ndiDevice != nullptr) ndiDevice->removeNDIInputListener(this);

	isComputing.enter();
	Brain::getInstance()->unregisterStamp(this);
	Brain::getInstance()->usingCollections.enter();
	Brain::getInstance()->stampPoolWaiting.removeAllInstancesOf(this);
	Brain::getInstance()->stampPoolUpdating.removeAllInstancesOf(this);
	Brain::getInstance()->usingCollections.exit();
	
	//for (auto sfc : Brain::getInstance()->allSubfixtureChannels) {
	//	if (sfc->stampStack.contains(this)) {
	//		sfc->stampOutOfStack(this);
	//		Brain::getInstance()->pleaseUpdate(sfc);
	//	}
	//}
	//for (int i = 0; i < values.items.size(); i++) {
	//	values.items[i]->parentStamp = nullptr;
	//	for (int j = 0; j < values.items[i]->paramContainer.items.size(); j++) {
	//		values.items[i]->paramContainer.items[j]->parentStamp = nullptr;

	//	}
	//}
	//isComputing.exit();
	//if (currentMidiClockSyncInterface != nullptr) {
	//	currentMidiClockSyncInterface->removeClockListener(this);
	//}
}

void Stamp::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
	pleaseComputeIfRunning();
}


void Stamp::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerStamp(this, id->getValue(), true);
	}
	if (p == isStampOn) {
		Brain::getInstance()->virtualButtonsNeedUpdate = true;
		Brain::getInstance()->virtualFaderButtonsNeedUpdate = true;
	}
	if (p == userName || p == id) {
		updateName();
	}
	if (p == sizeValue) {
		if (autoStartAndStop->getValue()) {
			if (isOn && (float)sizeValue->getValue() == 0) {
				kill();
			}
			else if(!isOn && (float)sizeValue->getValue() > 0 && lastSize == 0 && !Engine::mainEngine->isLoadingFile) {
				userStart(false);
			}
		}
		lastSize = p->getValue();
		currentSizeController = nextSizeController;
		nextSizeController = "";
		Brain::getInstance()->virtualFadersNeedUpdate = true;
	}
	if (p == ndiParam) {
		updateDevice();
	}
}

void Stamp::triggerTriggered(Trigger* t) {
	if (t == startBtn) {
		userStart();
	}
	else if (t == stopBtn) {
		stop();
	}
	else {}
}


void Stamp::userStart(float forcedFade) {
	userPressedGo = true;
	start(forcedFade);
}

void Stamp::start(float forcedFade) {
	double now = Time::getMillisecondCounterHiRes();
	float current = isOn ? 1 : 0;
	if (TSStartFadeIn < now && TSEndFadeIn > now)   {
		current = jmap(now, TSStartFadeIn, TSEndFadeIn, 0.0, 1.0);	
	}
	if (TSStartFadeOut < now && TSEndFadeOut > now)  {
		current = jmap(now, TSStartFadeOut, TSEndFadeOut, 1.0, 0.0);
	}
	TSEndFadeOut = 0;
	TSStartFadeOut = 0;
	float fade = forcedFade != -1 ? forcedFade : fadeInTime->floatValue();
	float delta = (current*fade)*1000;

	if (fade>0) {
		TSStartFadeIn = now - delta;
		TSEndFadeIn = now+(fade*1000)-delta;
	}
	else {
		TSStartFadeIn = 0;
		TSEndFadeIn = 0;
	}
	TSEndFadeOut = 0;
	TSStartFadeOut = 0;
	if (!isOn) {
		isOn = true;
		isStampOn->setValue(true);
		//totalElapsed = 0;
		computeData();
	}
	isComputing.enter();
	for (auto it = sfcToStampMappings.begin(); it != sfcToStampMappings.end(); it.next()) {
		it.getKey()->stampOnTopOfStack(this);
		Brain::getInstance()->pleaseUpdate(it.getKey());
	}
	isComputing.exit();
	Brain::getInstance()->pleaseUpdate(this);
	if (soloPool->intValue() > 0) Brain::getInstance()->soloPoolStampStarted(soloPool->intValue(), this);
}

void Stamp::stop(float forcedFade) {
	double now = Time::getMillisecondCounterHiRes();
	float current = isOn ? 1 : 0;
	if (TSStartFadeIn < now && TSEndFadeIn > now) {
		current = jmap(now, TSStartFadeIn, TSEndFadeIn, 0.0, 1.0);
	}
	if (TSStartFadeOut < now && TSEndFadeOut > now) {
		current = jmap(now, TSStartFadeOut, TSEndFadeOut, 1.0, 0.0);
	}
	//TSLastUpdate = now;
	TSEndFadeIn = 0;
	TSStartFadeIn = 0;
	float fade = forcedFade != -1 ? forcedFade : fadeOutTime->floatValue();
	float delta = ((1-current) * fade) * 1000;

	if (fade > 0) {
		TSStartFadeOut = now - delta;
		TSEndFadeOut = TSStartFadeOut + (fade * 1000.0);
	}
	else {
		kill();
	}
}

void Stamp::kill() {
	userPressedGo = false;
	isOn = false;
	isStampOn->setValue(false);
	isComputing.enter();
	for (auto it = sfcToStampMappings.begin(); it != sfcToStampMappings.end(); it.next()) {
		it.getKey()->stampOutOfStack(this);
		Brain::getInstance()->pleaseUpdate(it.getKey());
	}
	isComputing.exit();
	TSStartFadeIn = 0;
	TSStartFadeOut = 0;
	TSEndFadeIn = 0;
	TSEndFadeOut = 0;
}

void Stamp::update(double now) {
	if (computed == false) {
		computeData();
	}
	if (isOn) {
		Brain::getInstance()->pleaseUpdate(this);
		if (TSEndFadeOut > 0 && TSEndFadeOut < now) {
			kill();
		}
	}
	else {
	}
}

void Stamp::pleaseComputeIfRunning() {
	if (isOn) {
		computed = false;
		Brain::getInstance()->pleaseUpdate(this);
	}
}

void Stamp::computeData() {
	//LOG("computing");
	ScopedLock lock(isComputing);
	Layout* l = Brain::getInstance()->getLayoutById(layoutId->intValue());
	if (l == nullptr) return;
	l->computeData();
	selection.computeSelection();
	subFixtureToPixelPosition.clear();
	sfcToStampMappings.clear();

	Point<float> min = videoPosition->getPoint() - (videoSize->getPoint() / 2);
	Point<float> max = videoPosition->getPoint() + (videoSize->getPoint() / 2);
	Range<float> rangeX(min.x, max.x);
	Range<float> rangeY(min.y, max.y);

	for (SubFixture* sf : selection.computedSelectedSubFixtures) {
		if (!l->subFixtToPos.contains(sf)) continue;
		std::shared_ptr<Point<float>> layoutPos = l->subFixtToPos.getReference(sf);
		if (!rangeX.contains(layoutPos->x)) continue;
		if (!rangeY.contains(layoutPos->y)) continue;
		double x = jmap(layoutPos->x, min.x, max.x,0.f,1.f);
		double y = jmap(layoutPos->y, min.y, max.y,1.f,0.f);
		subFixtureToPixelPosition.set(sf, {x,y});

		for (StampMapping* map : mappings.items) {
			ChannelType* ct = dynamic_cast<ChannelType*>(map->paramType->targetContainer.get());
			if (sf->channelsMap.contains(ct)) {

				SubFixtureChannel* sfc = sf->channelsMap.getReference(ct);
				if (sfc == nullptr) continue;

				if (!sfcToStampMappings.contains(sfc))
				{
					sfcToStampMappings.set(sfc, {});
				}

				auto& arr = sfcToStampMappings.getReference(sfc);
				arr.addIfNotAlreadyThere(map);
			}
		}
	}
}

float Stamp::applyToChannel(SubFixtureChannel* fc, float currentVal, double now) {
	SubFixture* sf = fc->parentSubFixture;
	if (sf == nullptr) return currentVal;
	if (!subFixtureToPixelPosition.contains(sf)) return currentVal;
	Point<double> pixelPos = subFixtureToPixelPosition.getReference(sf);
	Colour pixel = bitmapData->getPixelColour(pixelPos.x*(image.getWidth()-1), pixelPos.y * (image.getHeight()-1));
	float pixelVal = 0;

	if (!sfcToStampMappings.contains(fc)) return currentVal;

	float fadeSize = 1;
	if (TSStartFadeIn < now && TSEndFadeIn > now) fadeSize = jmap(now, TSStartFadeIn, TSEndFadeIn, 0.0, 1.0);
	if (TSStartFadeOut < now && TSEndFadeOut > now) fadeSize = jmap(now, TSStartFadeOut, TSEndFadeOut, 1.0, 0.0);
	
	fadeSize *= sizeValue->floatValue();

	for (StampMapping* map : sfcToStampMappings.getReference(fc)) {
		switch (map->pixelVal->getValueDataAsEnum<StampMapping::PixelVal>()) {
			case StampMapping::PixelVal::R: pixelVal = pixel.getFloatRed(); break;
			case StampMapping::PixelVal::G: pixelVal = pixel.getFloatGreen(); break;
			case StampMapping::PixelVal::B: pixelVal = pixel.getFloatBlue(); break;
			case StampMapping::PixelVal::Luma: pixelVal = pixel.getBrightness(); break;
		}
		pixelVal *= fadeSize;
		pixelVal = jmap(pixelVal, 0.f, 1.f, map->outRange->x, map->outRange->y);

		float baseValue = currentVal;
		if (map->stampMode->stringValue() == "absolute") baseValue = map->baseValue->floatValue();
		else if (map->stampMode->stringValue() == "attracted") baseValue = jmap(fadeSize,0.f,1.f, currentVal, map->baseValue->floatValue());

		currentVal = baseValue + pixelVal;
	}

	return currentVal;
}


void Stamp::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<StampManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
	Brain::getInstance()->reconstructVirtuals = true;
}


void Stamp::flash(bool on, bool swop)
{
	if (on) {
		if (!isOn) {
			start(false);
		}
		isFlashing = true;
		if (swop) {
			isSwopping = true;
			Brain::getInstance()->swoppedStamp(this);
		}
	}
	else {
		isFlashing = false;
		if (isSwopping) {
			isSwopping = false;
			Brain::getInstance()->unswoppedStamp(this);
		}
		if (!userPressedGo) {
			stop();
		}
	}
}


void Stamp::initImage(int width, int height)
{
	initImage(Image(Image::ARGB, width, height, true));
}

void Stamp::initImage(const Image & newImage)
{
	GenericScopedLock lock(imageLock);

	if (!newImage.isValid())
	{
		image = Image();
		return;
	}

	bool imageUpdated = false;
	if (newImage.getWidth() != image.getWidth() || newImage.getHeight() != image.getHeight())
	{
		image = newImage.convertedToFormat(Image::ARGB);

		imageUpdated = true;
		bitmapData.reset();

	}

	if (bitmapData == nullptr)
	{
		bitmapData.reset(new Image::BitmapData(image, Image::BitmapData::readWrite));
	}

}

void Stamp::updateDevice()
{
	if (isClearing) return;

	if (ndiParam->inputDevice != ndiDevice)
	{
		if (ndiDevice != nullptr) ndiDevice->removeNDIInputListener(this);
		ndiDevice = ndiParam->inputDevice;

		if (ndiDevice != nullptr)
		{
			ndiDevice->addNDIInputListener(this);
			NLOG(niceName, "Now listening to NDI Device : " << ndiDevice->name);
		}
	}

}

void Stamp::videoFrameReceived(NDIlib_video_frame_v2_t* frame)
{
	ScopedLock lock(imageLock);
	const uint8_t* frameData = frame->p_data;
	int width = frame->xres;
	int height = frame->yres;

	// Créer une image JUCE et copier les données

	if (image.getWidth() != width || image.getHeight() != height) {
		initImage(width, height);
	}

	std::memcpy(bitmapData->data, frameData, width * height * 4);

}
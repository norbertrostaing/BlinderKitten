/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "SubFixture.h"
#include "../../Brain.h"
#include "Fixture/Fixture.h"
#include "UserInputManager.h"
#include "BKEngine.h"

SubFixture::SubFixture() :
	channelsMap()
{

	// Logger::writeToLog("I'm a new SubFixture !");
	
	channelsMap.clear();
	Brain::getInstance()->allSubfixtures.add(this);
	//Brain::getInstance()->registerSubFixture(this, id->getValue());
}

SubFixture::~SubFixture()
{
	Brain::getInstance()->allSubfixtures.removeAllInstancesOf(this);
	channelsMap.clear();
	channelsContainer.clear();
	if (parentFixture != nullptr) {
		parentFixture->subFixtures.removeValue(this);
	}
	
	if (UserInputManager::getInstance()->currentProgrammer != nullptr) {
		if (UserInputManager::getInstance()->currentProgrammer->currentUserCommand != nullptr) {
			UserInputManager::getInstance()->currentProgrammer->currentUserCommand->computeValues();
		}
	}

	for (auto it = Brain::getInstance()->layouts.begin(); it != Brain::getInstance()->layouts.end(); it.next()) {
		Layout* l = it.getValue();
		l->isComputing.enter();
		if (l->subFixtToPos.contains(this)) l->subFixtToPos.remove(this);
		l->isComputing.exit();
		for (BKPath* p : l->paths.items) {
			p->subFixtToPos.remove(this);
		}
	}

	Brain::getInstance()->layoutViewerNeedRefresh = true;
}

Colour SubFixture::getOutputColor()
{
	BKEngine* e = dynamic_cast<BKEngine*>(BKEngine::mainEngine);
	double r = 0;
	double g = 0;
	double b = 0;

	ChannelType* intensity = dynamic_cast<ChannelType*>(e->IntensityChannel->targetContainer.get());
	ChannelType* red = dynamic_cast<ChannelType*>(e->CPRedChannel->targetContainer.get());
	ChannelType* green = dynamic_cast<ChannelType*>(e->CPGreenChannel->targetContainer.get());
	ChannelType* blue = dynamic_cast<ChannelType*>(e->CPBlueChannel->targetContainer.get());
	ChannelType* white = dynamic_cast<ChannelType*>(e->CPWhiteChannel->targetContainer.get());
	ChannelType* amber = dynamic_cast<ChannelType*>(e->CPAmberChannel->targetContainer.get());
	ChannelType* uv = dynamic_cast<ChannelType*>(e->CPUVChannel->targetContainer.get());
	ChannelType* cyan = dynamic_cast<ChannelType*>(e->CPCyanChannel->targetContainer.get());
	ChannelType* magenta = dynamic_cast<ChannelType*>(e->CPMagentaChannel->targetContainer.get());
	ChannelType* yellow = dynamic_cast<ChannelType*>(e->CPYellowChannel->targetContainer.get());
	ChannelType* hue = dynamic_cast<ChannelType*>(e->CPHueChannel->targetContainer.get());
	ChannelType* sat = dynamic_cast<ChannelType*>(e->CPSaturationChannel->targetContainer.get());

	bool colorIsSet = false;

	if (hue != nullptr && sat != nullptr) {
		if (channelsMap.contains(hue) && channelsMap.contains(sat)) {
			float h = channelsMap.getReference(hue)->currentValue;
			float s = channelsMap.getReference(sat)->currentValue;
			Colour c = Colour::fromHSV(h,s,1,1);
			r = c.getFloatRed();
			g = c.getFloatGreen();
			b = c.getFloatBlue();
			colorIsSet = true;
		}
	}

	if (red != nullptr && green != nullptr && blue != nullptr) {
		if (channelsMap.contains(red)) { colorIsSet = true; r += channelsMap.getReference(red)->currentValue; }
		if (channelsMap.contains(green)) { colorIsSet = true; g += channelsMap.getReference(green)->currentValue; }
		if (channelsMap.contains(blue)) { colorIsSet = true; b += channelsMap.getReference(blue)->currentValue; }
	}

	if (white != nullptr && channelsMap.contains(white)) {
		colorIsSet = true;
		double v = channelsMap.getReference(white)->currentValue;
		r += v * 1.0;
		g += v * 1.0;
		b += v * 1.0;
	}
	if (amber != nullptr && channelsMap.contains(amber)) {
		colorIsSet = true;
		double v = channelsMap.getReference(amber)->currentValue;
		r += v * 1.0;
		g += v * 0.5;
		b += v * 0.0;
	}
	if (uv != nullptr && channelsMap.contains(uv)) {
		colorIsSet = true;
		double v = channelsMap.getReference(uv)->currentValue;
		r += v * 0.5;
		g += v * 0;
		b += v * 1.0;
	}

	if (r > 1 || g > 1 || b > 1) {
		double max = jmax(r, g,b);
		r = jmap(r, 0.0, max, 0.0, 1.0);
		g = jmap(g, 0.0, max, 0.0, 1.0);
		b = jmap(b, 0.0, max, 0.0, 1.0);
	}

	if (!colorIsSet && cyan != nullptr && magenta != nullptr && yellow != nullptr) {
		r = 1; g = 1; b= 1;
		if (channelsMap.contains(cyan)) { colorIsSet = true; r = 1-channelsMap.getReference(cyan)->currentValue; }
		if (channelsMap.contains(magenta)) { colorIsSet = true; g = 1-channelsMap.getReference(magenta)->currentValue; }
		if (channelsMap.contains(yellow)) { colorIsSet = true; b = 1-channelsMap.getReference(yellow)->currentValue; }
	}

	if (intensity != nullptr) {
		if (!colorIsSet) {r = 1; g = 1; b = 1;}
		if (channelsMap.contains(intensity)) {
			double v = channelsMap.getReference(intensity)->currentValue;
			colorIsSet = true;
			r *= v;
			g *= v;
			b *= v;
		}
	}

	if (parentFixture != nullptr && parentFixture->useCustomLayoutFillColor->boolValue()) {
		Colour max = parentFixture->layoutFillColor->getColor();
		r *= max.getFloatRed();
		g *= max.getFloatGreen();
		b *= max.getFloatBlue();
	}

	if (!colorIsSet) {r = 0; g = 0; b = 0;}



	return Colour(255*r, 255*g, 255*b);
}

int SubFixture::MyHashGenerator::generateHash(SubFixture* key, int upperLimit) const
{
	return ((key->parentFixture->id->intValue() * 512) + key->subId) % upperLimit;
}

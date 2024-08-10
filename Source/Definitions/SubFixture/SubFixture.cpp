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
	float r = 0;
	float g = 0;
	float b = 0;

	ChannelType* intensity = dynamic_cast<ChannelType*>(e->IntensityChannel->targetContainer.get());
	ChannelType* red = dynamic_cast<ChannelType*>(e->CPRedChannel->targetContainer.get());
	ChannelType* green = dynamic_cast<ChannelType*>(e->CPGreenChannel->targetContainer.get());
	ChannelType* blue = dynamic_cast<ChannelType*>(e->CPBlueChannel->targetContainer.get());
	ChannelType* cyan = dynamic_cast<ChannelType*>(e->CPCyanChannel->targetContainer.get());
	ChannelType* magenta = dynamic_cast<ChannelType*>(e->CPMagentaChannel->targetContainer.get());
	ChannelType* yellow = dynamic_cast<ChannelType*>(e->CPYellowChannel->targetContainer.get());

	bool colorIsSet = false;

	if (red != nullptr && green != nullptr && blue != nullptr) {
		if (channelsMap.contains(red)) { colorIsSet = true; r = channelsMap.getReference(red)->value; }
		if (channelsMap.contains(green)) { colorIsSet = true; g = channelsMap.getReference(green)->value; }
		if (channelsMap.contains(blue)) { colorIsSet = true; b = channelsMap.getReference(blue)->value; }
	}
	
	if (!colorIsSet && cyan != nullptr && magenta != nullptr && yellow != nullptr) {
		r = 1; g = 1; b= 1;
		if (channelsMap.contains(cyan)) { colorIsSet = true; r = 1-channelsMap.getReference(cyan)->value; }
		if (channelsMap.contains(magenta)) { colorIsSet = true; g = 1-channelsMap.getReference(magenta)->value; }
		if (channelsMap.contains(yellow)) { colorIsSet = true; b = 1-channelsMap.getReference(yellow)->value; }
	}

	if (intensity != nullptr) {
		if (!colorIsSet) {r = 1; g = 1; b = 1;}
		if (channelsMap.contains(intensity)) {
			float v = channelsMap.getReference(intensity)->value;
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

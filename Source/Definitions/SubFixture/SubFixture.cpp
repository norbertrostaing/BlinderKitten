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

SubFixture::SubFixture() :
	channelsMap()
{

	// Logger::writeToLog("I'm a new SubFixture !");
	
	channelsMap.clear();
	//Brain::getInstance()->registerSubFixture(this, id->getValue());
}

SubFixture::~SubFixture()
{
	channelsMap.clear();
	channelsContainer.clear();
	if (UserInputManager::getInstance()->currentProgrammer != nullptr) {
		if (UserInputManager::getInstance()->currentProgrammer->currentUserCommand != nullptr) {
			UserInputManager::getInstance()->currentProgrammer->currentUserCommand->computeValues();
		}
	}
}

int SubFixture::MyHashGenerator::generateHash(SubFixture* key, int upperLimit) const
{
	return ((key->parentFixture->id->intValue() * 512) + key->subId) % upperLimit;
}

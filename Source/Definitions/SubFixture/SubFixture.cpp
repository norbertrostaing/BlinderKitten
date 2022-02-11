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

SubFixture::SubFixture() :
	channelsMap()
{

	Logger::writeToLog("I'm a new SubFixture !");
	
	channelsMap.clear();
	//Brain::getInstance()->registerSubFixture(this, id->getValue());
}

SubFixture::~SubFixture()
{
	for (auto it = channelsMap.begin(); it != channelsMap.end(); it.next()) {
		if (it.getValue() != nullptr) {
			it.getValue()->~SubFixtureChannel();
		}
	}
	channelsMap.clear();
}



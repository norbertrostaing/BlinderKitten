/*
  ==============================================================================

    UserInputManager.cpp
    Created: 14 Nov 2021 10:42:56pm
    Author:  No

  ==============================================================================
*/


#include "UserInputManager.h"
#include "Brain.h"
#include "Definitions/Cuelist/Cuelist.h"
#include "Definitions/Effect/Effect.h"
#include "Definitions/Carousel/Carousel.h"

juce_ImplementSingleton(UserInputManager);

UserInputManager::UserInputManager()
{
}

UserInputManager::~UserInputManager() {
}

void UserInputManager::processInput(String s) {
	s = s.toLowerCase();
	if (s == "backspace") {
		if (currentCommand.size() > 0) {
			currentCommand.remove(currentCommand.size()-1);
		}
	}
	else if (s == "clear") {
		currentCommand.clear();
	}
	else {
		currentCommand.add(s);
	}

	String test = "";
	for (int i = 0; i < currentCommand.size(); i++) {
		test = test + currentCommand[i] + " ";
	}
	LOG(test);
}

void UserInputManager::processMessage(const OSCMessage& m)
{
	StringArray aList;
	aList.addTokens(m.getAddressPattern().toString(), "/", "\"");
	LOG("Yeah baby" + m.getAddressPattern().toString());
	if (aList.size() < 3) return;

	String firstWord = aList[1].toLowerCase();
	if (firstWord == "key") {
		processInput(aList[2]);
	}
	else if (firstWord == "cuelist" && aList.size() > 3) {
		int targetNumber = (int)((var)aList[2]);
		Cuelist* target = Brain::getInstance()->getCuelistById(targetNumber);
		if (target != nullptr) {
			String action = aList[3].toLowerCase();
			if (action == "go") {
				target->go();
			}
			else if (action == "gorandom") {
				target->goRandom();
			}
			else if (action == "off") {
				target->off();
			}
		}
		else {
			LOGWARNING("Cuelist "+String(targetNumber)+ " doesn't exist");
		}
	}

	/*
	if (aList[1] == "model")
	{
		String modelName = OSCHelpers::getStringArg(m[0]);
		LightBlockModel* lm = LightBlockModelLibrary::getInstance()->getModelWithName(modelName);

		if (lm != nullptr)
		{
			if (aList[2] == "assign")
			{
				if (m.size() >= 2)
				{
					int id = OSCHelpers::getIntArg(m[1]);

					LightBlockModelPreset* mp = nullptr;
					if (m.size() >= 3)
					{
						String presetName = OSCHelpers::getStringArg(m[2]);
						mp = lm->presetManager.getItemWithName(presetName);
					}

					LightBlockColorProvider* providerToAssign = mp != nullptr ? mp : (LightBlockColorProvider*)lm;
					if (id == -1)
					{
						for (auto& p : PropManager::getInstance()->items)  p->activeProvider->setValueFromTarget(providerToAssign);
					}
					else
					{
						Prop* p = PropManager::getInstance()->getPropWithId(id);
						if (p != nullptr) p->activeProvider->setValueFromTarget(providerToAssign);
					}

				}


			}
		}

	}
	else if (aList[1] == "prop")
	{
		int id = aList[2] == "all" ? -1 : aList[2].getIntValue();

		String localAddress = "/" + aList.joinIntoString("/", 3);
		OSCMessage lm(localAddress);
		lm.addString(""); //fake ID
		for (auto& a : m) lm.addArgument(a);
		lm.setAddressPattern(localAddress);

		if (id == -1)
		{
			for (auto& p : PropManager::getInstance()->items)  p->handleOSCMessage(lm);
		}
		else
		{
			if (Prop* p = PropManager::getInstance()->getPropWithId(id)) p->handleOSCMessage(lm);
		}
	}
	*/
}

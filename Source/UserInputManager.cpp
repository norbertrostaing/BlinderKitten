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
#include "Definitions/Programmer/Programmer.h"
#include "Definitions/Programmer/ProgrammerManager.h"
#include "Definitions/Command/Command.h"
#include "Definitions/Command/CommandSelectionManager.h"
#include "Definitions/Command/CommandSelection.h"
#include "UI/CommandLine.h"

juce_ImplementSingleton(UserInputManager);

UserInputManager::UserInputManager()
{
}

UserInputManager::~UserInputManager() {
}

void UserInputManager::processInput(String s) {
	s = s.toLowerCase();
	int size = currentCommand.size();
	if (s == "backspace") {
		if (currentCommand.size() > 0) {
			String s = currentCommand[size - 1];
			if (s.containsOnly("1234567890") && s.length()>1) {
				currentCommand.set(size - 1, s.substring(0, s.length()-1));
			} 
			else {
				currentCommand.remove(size - 1);
			}
		}
	}
	else if (s == "clear") {
		currentCommand.clear();
	}
	else if (s.containsOnly("1234567890") && size>0 && currentCommand[size-1].containsOnly("1234567890")) {
		currentCommand.set(size-1, currentCommand[size-1]+s);
	}
	else {
		currentCommand.add(s);
	}

	String test = "";
	for (int i = 0; i < currentCommand.size(); i++) {
		test = test + currentCommand[i] + " ";
	}
	LOG(test);

	CommandLine::getInstance()->repaint();

	processCommand();
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

void UserInputManager::processCommand() {

	Programmer* prog = Brain::getInstance()->getProgrammerById(1);
	Command* targetCommand;

	if (prog == nullptr) {
		prog = ProgrammerManager::getInstance()->addItem();
		prog -> id -> setValue(1);
	}

	if (prog->commands.items.size() == 0) {
		targetCommand = prog->commands.addItem();
	}
	else {
		targetCommand = prog->commands.items[0];
	}

	String currentSelectionType = "fixture";
	int cmdSize = currentCommand.size();
	int selectionIndex = 0;
	bool currentSelectionFilled = false;
	CommandSelection* currentSelection;
	LOG(targetCommand->selection.items.size());
	if (targetCommand->selection.items.size() > 0) {
		currentSelection = targetCommand->selection.items[0];
	}
	else {
		currentSelection = targetCommand->selection.addItem();
	}

	for (int i = 0; i < currentCommand.size(); i++) {
		String w = currentCommand.getReference(i);
		bool lastWord = i == currentCommand.size()-1;

		if (w == "+" || w == "-") {
			if (lastWord || currentCommand[i + 1] == "group" || currentCommand[i + 1] == "fixture" || currentCommand[i + 1].containsOnly("1234567890")) {
				if (currentSelectionFilled) {
					currentSelection = targetCommand->selection.addItem();
				}
				targetCommand->selection.items[selectionIndex]->plusOrMinus->setValue(w);
			}
			else {
				LOGERROR(currentCommand[i + 1]+" is not allowed after the "+w+" command");
				return;
			}
		}
		else if (w == "fixture") {
			if (lastWord || currentCommand[i + 1].containsOnly("1234567890")) {
				currentSelectionType = "fixture";
				if (!lastWord) {
					currentSelection->valueFrom->setValue((int)(var)currentCommand[i+1]);
				}
				if (cmdSize > i + 2 && currentCommand[i + 2] == "thru") {
					currentSelection->thru->setValue(true);
					if (cmdSize > i + 3 && currentCommand[i + 3].containsOnly("1234567890")) {
						currentSelection->valueTo ->setValue(currentCommand[i + 3]);
					}
				}
				else {
					currentSelection->thru->setValue(false);
				}
			}
			else {
				LOGERROR(currentCommand[i + 1] + " is not allowed after the - command");
			}
		}
	}

}


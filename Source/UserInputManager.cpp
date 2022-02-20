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
#include "Definitions/Command/CommandValue.h"
#include "Definitions/ChannelFamily/ChannelType/ChannelType.h"
#include "Definitions/ChannelFamily/ChannelFamily.h"
#include "Definitions/ChannelFamily/ChannelFamilyManager.h"
#include "UI/CommandLine.h"
#include "UI/Encoders.h"

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
		else {
			currentCommand.clear();
			if (targetCommand != nullptr) {
				targetCommand->selection.clear();
				targetCommand->values.clear();
				Brain::getInstance()->getProgrammerById(1)->go();
			}
		}
	}
	else if (s == "clear") {
		currentCommand.clear();
		if (targetCommand != nullptr) {
			targetCommand->selection.clear();
			targetCommand->values.clear();
			Brain::getInstance()->getProgrammerById(1)->go();
		}
	}
	else if (s.containsOnly("1234567890") && size>0 && currentCommand[size-1].containsOnly("1234567890")) {
		currentCommand.set(size-1, currentCommand[size-1]+s);
	}
	else {
		currentCommand.add(s.toLowerCase());
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

	bool currentSelectionFilled = false;
	CommandSelection* currentSelection = nullptr;
	bool currentValueFilled = false;
	CommandValue* currentValue = nullptr;


	if (targetCommand->selection.items.size() > 0) {
		currentSelection = targetCommand->selection.items[0];
	}
	else {
		currentSelection = targetCommand->selection.addItem();
	}

	for (int i = 0; i < targetCommand->values.items.size() && currentValue == nullptr; i++) {
		if (targetCommand->values.items[i]->presetOrValue->getValue() == "value") {
			if (targetCommand->values.items[i]->channelType->targetContainer == nullptr) {
				currentValue = targetCommand->values.items[i];
			}
		}
		else if(targetCommand->values.items[i]->presetOrValue->getValue() == "preset") {
			currentValue = targetCommand->values.items[i];
		}
	}
	if (currentValue == nullptr) {
		currentValue = targetCommand->values.addItem();
	}

	for (int i = 0; i < currentCommand.size(); i++) {
		String w = currentCommand.getReference(i);
		bool lastWord = i == currentCommand.size()-1;

		if (w == "+" || w == "-") {
			if (lastWord || currentCommand[i + 1] == "group" || currentCommand[i + 1] == "fixture" || currentCommand[i + 1].containsOnly("1234567890")) {
				if (currentSelectionFilled) {
					currentSelectionFilled = false;
					int index = targetCommand->selection.items.indexOf(currentSelection) + 1;
					if (targetCommand->selection.items.size() == index) {
						targetCommand->selection.addItem();
					}
					currentSelection = targetCommand->selection.items[index];
				}
				currentSelection->plusOrMinus->setValue(w);
			}
			else {
				LOGERROR(currentCommand[i + 1]+" is not allowed after the "+w+" command");
				return;
			}
		}
		else if (w == "fixture" || w == "group") {
			currentSelection->subSel->setValue(false);
			if (lastWord || currentCommand[i + 1].containsOnly("1234567890")) {
				currentSelectionType = w;
				if (!lastWord) {
					i++;
					currentSelection->targetType->setValueWithData(w);
					currentSelection->valueFrom->setValue((int)(var)currentCommand[i]);
					currentSelectionFilled = true;
				}
				if (cmdSize > i + 1 && currentCommand[i + 1] == "thru") {
					currentSelection->thru->setValue(true);
					i++;
					if (cmdSize > i + 1) {
						if (currentCommand[i + 1].containsOnly("1234567890")) {
							currentSelection->valueTo->setValue(currentCommand[i + 1]);
						}
						else {
							LOGERROR(currentCommand[i + 1] + " cannot be called after Thru command");
							return;
						}
					}
				}
				else {
					currentSelection->thru->setValue(false);
				}
			}
			else {
				LOGERROR(currentCommand[i + 1] + " is not allowed after the "+w+" command");
			}
		}
		else if (w == "subfixture") {
			if (lastWord || currentCommand[i + 1].containsOnly("1234567890")) {
				currentSelection->subSel->setValue(true);
				if (!lastWord) {
					i++;
					currentSelection->subFrom->setValue((int)(var)currentCommand[i]);
				}
				if (cmdSize > i + 1 && currentCommand[i + 1] == "thru") {
					currentSelection->subThru->setValue(true);
					i++;
					if (cmdSize > i + 1) {
						if (currentCommand[i + 1].containsOnly("1234567890")) {
							currentSelection->subTo->setValue(currentCommand[i + 1]);
						}
						else {
							LOGERROR(currentCommand[i + 1] + " cannot be called after Thru command");
							return;
						}
					}
				}
				else {
					currentSelection->thru->setValue(false);
				}
			}
			else {
				LOGERROR(currentCommand[i + 1] + " is not allowed after the subfixture command");
			}
		}
		else if (w == "preset") {
			if (lastWord || currentCommand[i + 1].containsOnly("1234567890")) {
				if (currentValueFilled) {
					currentValueFilled = false;
					int index = targetCommand->values.items.indexOf(currentValue) + 1;
					if (targetCommand->values.items.size() == index) {
						targetCommand->values.addItem();
					}
					currentValue = targetCommand->values.items[index];
				}
				currentValue->presetOrValue->setValueWithData("preset");
				if (!lastWord) {
					i++;
					currentValue->presetIdFrom->setValue((int)(var)currentCommand[i]);
					currentValueFilled = true;
				}
				if (cmdSize > i + 1 && currentCommand[i + 1] == "thru") {
					currentValue->thru->setValue(true);
					i++;
					if (cmdSize > i + 1) {
						if (currentCommand[i + 1].containsOnly("1234567890")) {
							currentValue->presetIdTo->setValue(currentCommand[i + 1]);
						}
						else {
							LOGERROR(currentCommand[i + 1] + " cannot be called after Thru command");
							return;
						}
					}
				}
				else {
					currentSelection->thru->setValue(false);
				}
			}
			else {
				LOGERROR(currentCommand[i + 1] + " is not allowed after the Preset command");
			}
		}
	}

}

void UserInputManager::commandSelectionChanged(Command* c) {
	if (c == targetCommand) {
		targetCommand->selection.computeSelection();
		Array<ChannelType* >chans = targetCommand->selection.getControllableChannelsTypes();
		encoderChannels.clear();
	
		int currentIndex = 0;

		for (int i = 0; i < ChannelFamilyManager::getInstance()->items.size(); i++) {
			for (int j = 0; j < ChannelFamilyManager::getInstance()->items[i]->definitions->items.size(); j++) {
				if (chans.contains(ChannelFamilyManager::getInstance()->items[i]->definitions->items[j])) {
					encoderChannels.set(currentIndex, ChannelFamilyManager::getInstance()->items[i]->definitions->items[j]);
					currentIndex++;
				}
			}
		}
	redrawEncoders();
	}
}

void UserInputManager::redrawEncoders() {
	for (int i = 0; i < 8; i++) {
		if (encoderChannels.getReference(i) != nullptr) {

			Encoders::getInstance()->labels[i]->setText(String(encoderChannels.getReference(i)->niceName), juce::sendNotification);
			Encoders::getInstance()->encoders[i]->setEnabled(true);
			Encoders::getInstance()->encoders[i]->setColour(Slider::rotarySliderFillColourId, Colour(192, 192, 192));
		}
		else {
			Encoders::getInstance()->labels[i]->setText("", juce::dontSendNotification);
			Encoders::getInstance()->encoders[i]->setEnabled(false);
			Encoders::getInstance()->encoders[i]->setColour(Slider::rotarySliderFillColourId, Colour(63, 63, 63));
			Encoders::getInstance()->labels[i]->repaint();
		}
	}
	Encoders::getInstance()->resized();

}

void UserInputManager::commandValueChanged(Command* c) {
	for (int i = 0; i < c->values.items.size(); i++) {
		CommandValue *cv = c->values.items[i];
		if (cv->presetOrValue->getValue() == "value") {
			ChannelType* c = dynamic_cast<ChannelType*>(cv->channelType->targetContainer.get());
			for (auto it = encoderChannels.begin(); it != encoderChannels.end(); it.next()) {
				if (it.getValue() == c) {
					Encoders::getInstance()->encoders[it.getKey()]->setValue(cv->valueFrom->getValue(), juce::dontSendNotification);
				}
			}
		}
	}
}

void UserInputManager::encoderValueChanged(int index, float newValue) {
	ChannelType* c = encoderChannels.getReference(index);
	if (c != nullptr) {
		if (targetCommand == nullptr) {
			return;
		}
		CommandValue* t = nullptr;
		CommandValue* empty = nullptr;
		for (int i = 0; t == nullptr && i < targetCommand->values.items.size(); i++) {
			CommandValue* temp = targetCommand->values.items[i];
			if (temp->presetOrValue->getValue() == "value") {
				ChannelType* valCT = dynamic_cast<ChannelType*>(temp->channelType->targetContainer.get());
				if (valCT == c) {
					t = temp;
				}
				else if (empty == nullptr && valCT == nullptr) {
					empty = temp;
				}
			}
		}
		if (t == nullptr) {
			if (empty != nullptr) {
				t = empty;
				t->channelType->setValueFromTarget(c);
			}
			else {
				t = targetCommand->values.addItem();
			}
		}
		
		t->valueFrom->setValue(newValue, false);
	}
}


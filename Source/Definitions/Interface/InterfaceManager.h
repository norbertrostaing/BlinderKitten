/*
  ==============================================================================

    InterfaceManager.h
    Created: 26 Sep 2020 1:50:28pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "Interface.h"

class InterfaceManager :
    public BaseManager<Interface>
{
public:
    juce_DeclareSingleton(InterfaceManager, true)

    InterfaceManager();
    ~InterfaceManager();

    Factory<Interface> factory;

	template <class T>
	static void showAndGetInterfaceOfType(ControllableContainer* startFromCC, std::function<void(ControllableContainer*)> returnFunc)
	{
		PopupMenu menu;
		Array<Interface*> validModules;
		for (auto& m : InterfaceManager::getInstance()->items)
		{
			T* mt = dynamic_cast<T*>(m);
			if (mt == nullptr) continue;
			validModules.add(m);
			menu.addItem(validModules.indexOf(m) + 1, m->niceName);
		}

		menu.showMenuAsync(PopupMenu::Options(), [validModules, returnFunc](int result)
			{

				if (result == 0) return;
		returnFunc(validModules[result - 1]);
			}
		);
	}

	template <class T>
	static Array<T*> getInterfacesOfType()
	{
		Array<T*> ret;
		for (auto& m : InterfaceManager::getInstance()->items)
		{
			T* mt = dynamic_cast<T*>(m);
			if (mt == nullptr) continue;
			ret.add(mt);
		}
		return ret;

	}

	void feedback(String address, double value);
};
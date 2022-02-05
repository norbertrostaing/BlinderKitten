/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "CommandSelection.h"
//#include "../Fixture/Fixture.h"
class Fixture;

class CommandSelectionManager :
    public BaseManager<CommandSelection>
{
public:
    CommandSelectionManager();
    ~CommandSelectionManager();

    Array<Fixture*> computedSelectedFixtures;
    void computeSelection();
    void computeSelection(Array<int> groupHistory);
};
/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Cue.h"
//#include "../Fixture/Fixture.h"
class Fixture;

class CueManager :
    public BaseManager<Cue>
{
public:
    CueManager();
    ~CueManager();

    void askForMoveBefore(BaseItem*) override;
    void askForMoveAfter(BaseItem*) override;
    void addItemInternal(Cue* c, var data);

    

};
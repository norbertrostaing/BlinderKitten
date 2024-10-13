/*
  ==============================================================================

    MoveInBlackManager.h
    Created: 13 Oct 2024 1:33:27am
    Author:  Norb

  ==============================================================================
*/
#include "JuceHeader.h"

#pragma once
class ChannelFamily;

class MoveInBlackManager :
    public ControllableContainer
{
    public:
    MoveInBlackManager();
    ~MoveInBlackManager();

    Array<ChannelFamily*> getFamilies();

    static void createItem(ControllableContainer* cc);
    void afterLoadJSONDataInternal();
};
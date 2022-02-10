/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
//#include "../Fixture/Fixture.h"
#include "DeviceTypeChannel.h"
class Fixture;

class DeviceTypeChannelManager :
    public BaseManager<DeviceTypeChannel>
{
public:
    DeviceTypeChannelManager();
    ~DeviceTypeChannelManager();

    void calcDmxChannels();
    void addItemInternal(DeviceTypeChannel* c, var data);
    void askForRemoveBaseItem(BaseItem* item);
    void askForDuplicateItem(BaseItem* item);
    void askForPaste();
    void askForMoveBefore(BaseItem* i);
    void askForMoveAfter(BaseItem* i);


};
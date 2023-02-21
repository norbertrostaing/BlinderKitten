/*
  ==============================================================================

    DMXMappingManager.h
    Created: 12 Oct 2020 11:07:45am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "DMXMapping.h"

class DMXMappingManager :
    public BaseManager<DMXMapping>
{
public:
    DMXMappingManager();
    ~DMXMappingManager();

    void handleChannel(int channel, int value, String origin);
};
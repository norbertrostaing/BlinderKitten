/*
  ==============================================================================

    OSCMappingManager.h
    Created: 12 Oct 2020 11:07:45am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class OSCMappingManager :
    public BaseManager<OSCMapping>
{
public:
    OSCMappingManager();
    ~OSCMappingManager();

    HashMap<String, int> paramToIncrement;

};
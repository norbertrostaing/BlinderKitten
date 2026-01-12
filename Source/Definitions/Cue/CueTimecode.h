/*
  ==============================================================================

    CueTimecode.h
    Created: 11 Jan 2026 12:11:35am
    Author:  Norb

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class Cuelist;

class CueTimecode :
    public BaseItem
{
public:
    CueTimecode(var params = var());
    virtual ~CueTimecode();

    String objectType;
    var objectData;

    Cuelist* parentCuelist = nullptr;

    IntParameter* hour;
    IntParameter* minute;
    IntParameter* second;
    IntParameter* frame;

    void checkParentCuelist();
    void onContainerParameterChangedInternal(Parameter* p) override;

};
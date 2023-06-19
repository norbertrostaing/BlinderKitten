/*
  ==============================================================================

    CuelistAction.h
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include"../../Common/Action/Action.h"

class EncoderAction :
    public Action
{
public:
    EncoderAction(var params);
    ~EncoderAction();

    enum ActionType { ENC_VALUE, ENC_SELECT, ENC_NEXTCOMMAND, ENC_PREVCOMMAND, ENC_TOGGLEFILTERNUM, ENC_TOGGLEFILTERFAMILY, ENC_CLEARFILTERS  };
    ActionType actionType;
    IntParameter* targetEncoder;
    IntParameter* selectionDelta;
    IntParameter* filterNumber;
    TargetParameter* filterFamily;


    void triggerInternal() override;
    void setValueInternal(var value, String origin, bool isRelative);

    var getValue();

    static EncoderAction* create(var params) { return new EncoderAction(params); }

};
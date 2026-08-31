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

    enum ActionType { ENC_VALUE, ENC_SELECT, ENC_NEXTCOMMAND, ENC_PREVCOMMAND, ENC_TOGGLEFILTERNUM, ENC_TOGGLEFILTERFAMILY, ENC_CLEARFILTERS, 
        ENC_HL, ENC_BLIND, ENC_RANGE, ENC_THRU, ENC_EXPLODE, ENC_TYPE, ENC_RESET
        };
    ActionType actionType;
    IntParameter* targetEncoder;
    IntParameter* selectionDelta;
    IntParameter* filterNumber;
    TargetParameter* filterFamily;
    TargetParameter* encoderType;
    BoolParameter* soloMode;


    void triggerInternal() override;
    void setValueInternal(var value, String origin, int incrementIndex, bool isRelative) override;

    var getValue() override;

    static EncoderAction* create(var params) { return new EncoderAction(params); }

};

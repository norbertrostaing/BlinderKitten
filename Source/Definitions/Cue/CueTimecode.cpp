/*
  ==============================================================================

    CueTimecode.cpp
    Created: 11 Jan 2026 12:11:35am
    Author:  Norb

  ==============================================================================
*/

#include "CueTimecode.h"
#include "Definitions/Cuelist/Cuelist.h"

CueTimecode::CueTimecode(var params) :
	BaseItem(params.getProperty("name", "Timecode trigger 1")),
	objectType(params.getProperty("type", "CueTimecode").toString()),
	objectData(params)
{
	hour = addIntParameter("Hour", "Hour", 0, 0,24);
	minute = addIntParameter("Minute", "Minute", 0,0, 59);
	second = addIntParameter("Second", "Second", 0,0, 59);
	frame = addIntParameter("Frame", "Frame", 0,0, 30);
}

CueTimecode::~CueTimecode()
{
}

void CueTimecode::onContainerParameterChangedInternal(Parameter* p)
{
	
}

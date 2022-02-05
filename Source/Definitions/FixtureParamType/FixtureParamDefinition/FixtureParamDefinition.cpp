/*
  ==============================================================================

    FixtureParamDefinition.cpp
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#include "FixtureParamDefinition.h"

FixtureParamDefinition::FixtureParamDefinition() :
    BaseItem("Channel type")
{
    priority = addEnumParameter("Channel mode", "Type of channel", true);
    priority -> addOption("Highest Takes Precedence", "HTP");
    priority -> addOption("Latest Takes Precedence", "LTP");

    reactGM = addBoolParameter("React to grand master", "If checked, this channel will be dimmed by the grand master", false);
    
    snapOrFade = addEnumParameter("Snap or fade", "Is ths channel allowed to fade or should it jump to his new value ?");
    snapOrFade->addOption("Fade", "fade");
    snapOrFade->addOption("Snap", "snap");

};

FixtureParamDefinition::~FixtureParamDefinition()
{
};


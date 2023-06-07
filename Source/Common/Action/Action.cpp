/*
  ==============================================================================

    Action.cpp
    Created: 8 Oct 2020 3:33:09pm
    Author:  bkupe

  ==============================================================================
*/
#include "Common/CommonIncludes.h"

Action::Action(var params) :
    BaseItem(params.getProperty("type","Action").toString())
{
    typeString = params.getProperty("type", "Action").toString();
    showInspectorOnSelect = false;
}

Action::~Action()
{

}

void Action::trigger()
{
    if (!enabled->boolValue()) return;
    triggerInternal();
}

void Action::setValue(var value)
{
    if (!enabled->boolValue()) return;
    setValueInternal(value);
    previousValue = value;
}

void Action::setValue(var value, String origin, bool isRelative)
{
    if (!enabled->boolValue()) return;
    setValueInternal(value, origin, isRelative);
    previousValue = value;
}


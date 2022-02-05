/*
  ==============================================================================

    Action.cpp
    Created: 8 Oct 2020 3:33:09pm
    Author:  bkupe

  ==============================================================================
*/

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
}

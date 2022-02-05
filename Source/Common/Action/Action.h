/*
  ==============================================================================

    Action.h
    Created: 8 Oct 2020 3:33:09pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class Action :
    public BaseItem
{
public:
    Action(var params = var());
    virtual ~Action();

    String typeString;

    void trigger();
    virtual void triggerInternal() {}

    void setValue(var value);
    virtual void setValueInternal(var value) {}

    String getTypeString() const override { return typeString; }
};
/*
  ==============================================================================

    SerialInterface.h
    Created: 26 Sep 2020 1:53:17pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class SerialInterface :
    public Interface
{
public:
    SerialInterface();
    ~SerialInterface();

    String getTypeString() const override { return "Serial"; }
    static SerialInterface* create(var params) { return new SerialInterface(); };
};
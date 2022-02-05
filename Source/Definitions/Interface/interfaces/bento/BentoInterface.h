/*
  ==============================================================================

    BentoInterface.h
    Created: 29 Sep 2020 3:56:02pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class BentoInterface :
    public Interface
{
public:
    BentoInterface();
    ~BentoInterface();

    String getTypeString() const override { return "Bento"; }
    static BentoInterface* create(var params) { return new BentoInterface(); };
};
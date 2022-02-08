/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================


  l'effet de base
  vitesse (cycles par secondes)
  tap tempo
  tap tempo divider (taps par cycles

  EffectLine ? = >
  curve courbe ou preset ?
  curve origin (float 0-1)
  curve width (ratio to effect width)

  Selection
  repart from
  repart to
  channel Type
  size
  baseValue





*/

#pragma once
#include "JuceHeader.h"
#include "../Command/CommandSelectionManager.h"

class Effect :
    public BaseItem
{
public:
    Effect(var params = var());
    virtual ~Effect();

    String objectType;
    var objectData;

    IntParameter* id;
    CommandSelectionManager selection;

    String getTypeString() const override { return objectType; }

    void parameterValueChanged(Parameter* p);

    static Effect* create(var params) { return new Effect(params); }
};

/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Mapper.h"

class MapperManager :
    public BaseManager<Mapper>
{
public:
    juce_DeclareSingleton(MapperManager, true);

    MapperManager();
    ~MapperManager();

    void addItemInternal(Mapper* o, var data) override;
    void removeItemInternal(Mapper* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};
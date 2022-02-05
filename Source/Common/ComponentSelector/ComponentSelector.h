/*
  ==============================================================================

    ComponentSelector.h
    Created: 3 Oct 2020 2:08:48pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class ComponentSelector
{
public:
    ComponentSelector();
    ~ComponentSelector();

    HashMap<ComponentType, bool> selectedComponents;
    Array<ComponentType> allowedComponents;

    var getJSONData();
    void loadJSONData(var data);
};
/*
  ==============================================================================

    PresetTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "PresetValue.h"
#include "../ChannelFamily/ChannelFamilyManager.h"

PresetValue::PresetValue(var params) :
    BaseItem(params.getProperty("name", "Value")),
    objectType(params.getProperty("type", "PresetValue").toString()),
    objectData(params)
{
    itemDataType = "PresetValue";
    param  = addTargetParameter("Channel type", "Type of data of this channel", ChannelFamilyManager::getInstance());
    param ->targetType = TargetParameter::CONTAINER;
    param ->maxDefaultSearchLevel = 2;
    param ->typesFilter.add("ChannelType");
    canBeDisabled = false;

    paramValue = addFloatParameter("Value", "Value of this channel",0,0,1);
};

PresetValue::~PresetValue()
{
};



/*
  ==============================================================================

    ChannelValue.cpp
    Created: 18 Nov 2021 10:10:30pm
    Author:  No

  ==============================================================================
*/

#include "ChannelValue.h"

ChannelValue::ChannelValue() 
{
    values.set(0, -1);
    values.set(1, -1);
};
ChannelValue::~ChannelValue() {}

float ChannelValue::startValue()
{
    return values[0];
}

float ChannelValue::endValue()
{
    return values[values.size()-1];
}

float ChannelValue::valueAt(double ratio, double baseVal)
{
    float v = baseVal;
    if (ratio == 0) {
        v = startValue();
        v = v == -1 ? baseVal : v;
    }
    else if(ratio == 1) {
        v = endValue();
        v = v == -1 ? baseVal : v;
    }
    else {
        double mainRatio = ratio * (values.size()-1);
        int index = floor(mainRatio);
        double subRatio = mainRatio - index;
        float from = values[index] != -1 ? values[index] : baseVal;
        float to = values[index+1] != -1 ? values[index+1] : baseVal;
        v = jmap(subRatio, 0., 1. , (double)from, (double)to);
    }
    return v;
}
;

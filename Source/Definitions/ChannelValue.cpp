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
        if (values.size() == 2) {
            v = jmap(ratio, 0., 1., (double)values[0], (double)values[1]);
        }
        else {
            double pathRatio = ratio * pathLength;
            int currentIndex = 0;
            double currentPathFrom = 0;
            double currentPathTo = stepSizes[currentIndex];
            while (pathRatio > currentPathTo) {
                currentIndex++;
                currentPathFrom = currentPathTo;
                currentPathTo += stepSizes[currentIndex];
            }
            double from = values[currentIndex] != -1 ? values[currentIndex] : baseVal;
            double to = values[currentIndex + 1] != -1 ? values[currentIndex + 1] : baseVal;
            v = jmap(pathRatio, currentPathFrom, currentPathTo, (double)from, (double)to);
        }
    }
    return v;
}
;

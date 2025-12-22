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
ChannelValue::~ChannelValue() 
{
    cs.enter();
    cs.exit();
}

float ChannelValue::startValue()
{
    if (values.size()>0) return values[0];
    else return -1;
}

float ChannelValue::endValue()
{
    if (values.size() > 0) return values[values.size() - 1];
    else return -1;
}

float ChannelValue::valueAt(double ratio, double baseVal)
{
    cs.enter();
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
            double from = values[0] == -1 ? baseVal : values[0];
            double to = values[1] == -1 ? baseVal : values[1];
            v = jmap(ratio, 0., 1., from, to);
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
    cs.exit();
    return v;
}
;

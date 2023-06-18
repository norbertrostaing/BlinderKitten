/*
  ==============================================================================

    TapTempoMultiple.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "TapTempoMultiple.h"
#include "../Carousel/Carousel.h"
#include "../../Brain.h"

TapTempoMultiple::TapTempoMultiple(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", TAPTEMPO_FX);

    targetIdFrom = addIntParameter("First ID", "Id of the first target", 0, 0);
    targetIdTo = addIntParameter("Last ID", "Id of the last target", 0, 0);

}

TapTempoMultiple::~TapTempoMultiple()
{
}

void TapTempoMultiple::triggerInternal()
{
}

void TapTempoMultiple::setValueInternal(var value, String origin, bool isRelative) {
    if ((float)value == 1) {
        int from = targetIdFrom->getValue();
        int to = targetIdTo->getValue();

        int first = jmin(from, to);
        int last = jmax(from, to);

        for (int i = first; i <= last; i++) {
            if (actionType == TAPTEMPO_CUELIST) {
                Cuelist* t = Brain::getInstance()->getCuelistById(i);
                if (t != nullptr) {
                    t->tapTempo();
                }
            }
            else if (actionType == TAPTEMPO_FX) {
                Effect* t = Brain::getInstance()->getEffectById(i);
                if (t != nullptr) {
                    t->tapTempo();
                }
            }
            else if (actionType == TAPTEMPO_CAR) {
                Carousel* t = Brain::getInstance()->getCarouselById(i);
                if (t != nullptr) {
                    t->tapTempo();
                }
            } 
        }
    }

}

var TapTempoMultiple::getValue()
{
    return var();
}

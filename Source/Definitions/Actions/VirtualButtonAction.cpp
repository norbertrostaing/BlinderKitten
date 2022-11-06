/*
  ==============================================================================

    VirtualButtonAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "VirtualButtonAction.h"
#include "../Cuelist/Cuelist.h"
#include "../../Brain.h"
#include "UI/VirtualButtons/VirtualButtonGrid.h"
#include "UI/VirtualButtons/VirtualButtonManager.h"

VirtualButtonAction::VirtualButtonAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", VB_PRESS);

    if (actionType == VB_GOTOPAGE) {
        goToPageId = addIntParameter("Target page", "go to this page", 1, 1);
    }
    else if (actionType == VB_PRESS) {
        pageNumber = addIntParameter("Page", "Target Page 0 means current page", 0, 0);
        rowNumber = addIntParameter("Row", "", 1, 1);
        colNumber = addIntParameter("Col", "", 1, 1);
    }

}

VirtualButtonAction::~VirtualButtonAction()
{
}

void VirtualButtonAction::triggerInternal()
{   
    //if (target == nullptr) return;
}

void VirtualButtonAction::setValueInternal(var value)
{
    float val = value;
    if (actionType == VB_PAGEUP && val >0) {
        VirtualButtonGrid::getInstance()->goToPage(VirtualButtonGrid::getInstance()->page + 1);
    }
    else if (actionType == VB_PAGEDOWN && val > 0) {
        VirtualButtonGrid::getInstance()->goToPage(VirtualButtonGrid::getInstance()->page - 1);
    }
    else if (actionType == VB_GOTOPAGE && val > 0) {
        VirtualButtonGrid::getInstance()->goToPage(goToPageId->getValue());
    }
    else if (actionType == VB_PRESS) {

        int row = rowNumber->getValue();
        int col = colNumber->getValue();
        int page = pageNumber->getValue();
        if (page == 0) {
            page = VirtualButtonGrid::getInstance()->page;
        }

        for (int i = 0; i < VirtualButtonManager::getInstance()->items.size(); i++) {
            VirtualButton* target = VirtualButtonManager::getInstance()->items[i];
            if ((int)target->pageNumber->getValue() == page && (int)target->rowNumber->getValue() == row && (int)target->colNumber->getValue() == col) {
                switch (actionType) {
                case VB_PRESS:
                    if (val >0) {
                        target->pressed();
                    } else {
                        target->released();
                    }
                    break;

                //case CL_FLASHLEVEL:
                //    target->setFlashLevel(val);
                //    break;
                //}
                }
            }
        }
    }

}
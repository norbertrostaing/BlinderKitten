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

void VirtualButtonAction::setValueInternal(var value, String origin, int incrementIndex, bool isRelative)
{
    float val = value;
    if (actionType == VB_PAGEUP && val == 1) {
        VirtualButtonGrid::getInstance()->goToPage(VirtualButtonGrid::getInstance()->page + 1);
    }
    else if (actionType == VB_PAGEDOWN && val == 1) {
        VirtualButtonGrid::getInstance()->goToPage(VirtualButtonGrid::getInstance()->page - 1);
    }
    else if (actionType == VB_GOTOPAGE && val == 1) {
        VirtualButtonGrid::getInstance()->goToPage(goToPageId->getValue());
    }
    else if (actionType == VB_PRESS) {

        int row = rowNumber->getValue();
        int col = colNumber->getValue();
        int page = pageNumber->getValue();
        if (page == 0) {
            page = VirtualButtonGrid::getInstance()->page;
        }

        if (val == 1) {
            VirtualButtonManager::getInstance()->setButtonValue(page, col, row, 1, origin);
        }
        else {
            VirtualButtonManager::getInstance()->setButtonValue(page, col, row, 0, origin);
        }
    }

}

var VirtualButtonAction::getValue()
{
    return var();
}

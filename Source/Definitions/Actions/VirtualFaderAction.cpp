/*
  ==============================================================================

    VirtualFaderAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "VirtualFaderAction.h"
#include "../Cuelist/Cuelist.h"
#include "../../Brain.h"
#include "UI/VirtualFaders/VirtualFaderColGrid.h"
#include "UI/VirtualFaders/VirtualFaderColManager.h"


VirtualFaderAction::VirtualFaderAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", VF_FADER);
    
    if (actionType == VF_GOTOPAGE) {
        goToPageId = addIntParameter("Target page", "go to this page", 1, 1);
    }
    else if (actionType == VF_PAGEUP || actionType == VF_PAGEDOWN) {
    }
    else {
        pageNumber = addIntParameter("Page", "Target Page 0 means current page", 0,0);
        colNumber = addIntParameter("Column", "", 1,1);
        if (actionType != VF_FADER) {
            elementNumber = addIntParameter("Number", "", 1, 1);
        }
    }
}

VirtualFaderAction::~VirtualFaderAction()
{
}

void VirtualFaderAction::triggerInternal()
{   
    //if (target == nullptr) return;
}

void VirtualFaderAction::setValueInternal(var value, String origin, bool isRelative)
{
    float val = value;
    int number = 0;
    if (actionType == VF_PAGEUP) {
        if (val > 0) {
            VirtualFaderColGrid::getInstance()->goToPage(VirtualFaderColGrid::getInstance()->page + 1);
        }
        return;
    }
    else if(actionType == VF_PAGEDOWN) {
        if (val > 0) {
            VirtualFaderColGrid::getInstance()->goToPage(VirtualFaderColGrid::getInstance()->page - 1);
        }
        return;
    }
    else if (actionType == VF_GOTOPAGE) {
        if (val > 0) {
            VirtualFaderColGrid::getInstance()->goToPage(goToPageId->getValue());
        }
        return;
    }

    if (actionType != VF_FADER) {
        number = elementNumber->getValue();
    }
    int col = colNumber->getValue();
    int page = pageNumber->getValue();

    if (page == 0) {
        page = VirtualFaderColGrid::getInstance()->page;
    }

    //bool currentPage = page == VirtualFaderColGrid::getInstance()->page;


    switch (actionType) {
    case VF_ROTARY:
        VirtualFaderColManager::getInstance()->setRotaryValue(page, col, number, val, origin, isRelative);
        break;

    case VF_ABOVEBUTTON:
        VirtualFaderColManager::getInstance()->setAboveButtonValue(page, col, number, val, origin);
       break;

    case VF_FADER: {
        VirtualFaderColManager::getInstance()->setFaderValue(page, col, val, origin, isRelative);
        break;
    }
    case VF_BELOWBUTTON:
        VirtualFaderColManager::getInstance()->setBelowButtonValue(page, col, number, val, origin);
        break;
    }



    
}

var VirtualFaderAction::getValue()
{
    var val = var();

    int number = 0;
    if (actionType == VF_PAGEUP) {
        return val;
    }
    else if (actionType == VF_PAGEDOWN) {
        return val;
    }
    else if (actionType == VF_GOTOPAGE) {
        return val;
    }

    if (actionType != VF_FADER) {
        number = elementNumber->getValue();
    }
    int col = colNumber->getValue();
    int page = pageNumber->getValue();

    if (page == 0) {
        page = VirtualFaderColGrid::getInstance()->page;
    }

    // bool currentPage = page == VirtualFaderColGrid::getInstance()->page;

    switch (actionType) {
    case VF_ROTARY:
        if (col < VirtualFaderColGrid::getInstance()->rotaries.size()) {
            if (number < VirtualFaderColGrid::getInstance()->rotaries.getRawDataPointer()[col]->size()) {
                Slider* s = VirtualFaderColGrid::getInstance()->rotaries.getRawDataPointer()[col]->getRawDataPointer()[number];
                if (VirtualFaderColGrid::getInstance()->sliderToVFS.contains(s)) {
                    VirtualFaderSlider* vfs = VirtualFaderColGrid::getInstance()->sliderToVFS.getReference(s);
                    val = vfs->getTargetValue();
                }
            }
        }
        break;

    case VF_ABOVEBUTTON:
        break;

    case VF_FADER: {
        if (col < VirtualFaderColGrid::getInstance()->faders.size()) {
            Slider* s = VirtualFaderColGrid::getInstance()->faders.getRawDataPointer()[col];
            if (VirtualFaderColGrid::getInstance()->sliderToVFS.contains(s)) {
                VirtualFaderSlider* vfs = VirtualFaderColGrid::getInstance()->sliderToVFS.getReference(s);
                val = vfs->getTargetValue();
            }
        }
        break;
    }
    case VF_BELOWBUTTON:
        break;
    }


    return val;
}

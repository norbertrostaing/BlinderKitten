/*
  ==============================================================================

    DataTransferManager.cpp
    Created: 29 Jan 2019 3:52:46pm
    Author:  no

  ==============================================================================
*/
#include "JuceHeader.h"
#include "DataTransferManager.h"
#include "../../Brain.h"
#include "../Fixture/Fixture.h"
#include "../Group/GroupManager.h"
#include "../Group/Group.h"
#include "../Preset/Preset.h"
#include "../Cuelist/Cuelist.h"
#include "../Programmer/Programmer.h"

juce_ImplementSingleton(DataTransferManager)

DataTransferManager::DataTransferManager() :
	BaseItem("Data Transfer Manager")
{
    sourceType = addEnumParameter("Source Type", "Type of the data source");
    sourceType->addOption("Group", "Group");
    sourceType->addOption("Preset", "Preset");
    sourceType->addOption("Cuelist", "Cuelist");
    sourceType->addOption("Programmer", "Programmer");
    sourceId = addIntParameter("Source Id", "ID of the source", 0, 0);
    targetType = addEnumParameter("Target Type", "Type of the data target");
    targetType->addOption("Group", "Group");
    targetType->addOption("Preset", "Preset");
    targetType->addOption("Cuelist", "Cuelist");
    targetType->addOption("Programmer", "Programmer");
    targetId = addIntParameter("Target Id", "ID of the target", 0, 0);
    go = addTrigger("Transfer Data", "Run the data transfer");
}

DataTransferManager::~DataTransferManager()
{
}

void DataTransferManager::triggerTriggered(Trigger* t) {
    String srcType = sourceType->getValue();
    String trgType = targetType->getValue();

    if (srcType == "Programmer") {
        Programmer* source = Brain::getInstance() -> getProgrammerById(sourceId->getValue());
        if (source == nullptr) {LOG("Invalid Programmer ID"); return; }
        if (trgType == "Group") {
            Group* target = Brain::getInstance()->getGroupById(targetId->getValue());
            if (target == nullptr) {
                target = GroupManager::getInstance()->addItem(new Group());
                target->id->setValue(targetId->getValue());
                target->setNiceName("Group "+String(int(target->id->getValue())));
            } 
        }
    }
    else if (srcType == "Cuelist") {

    }
    else if (srcType == "Preset") {

    }
    else if (srcType == "Group") {

    }
    


}
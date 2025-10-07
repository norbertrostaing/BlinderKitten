#include "FixtureTypeChannelManager.h"
#include "../../Brain.h"

FixtureTypeChannelManager::FixtureTypeChannelManager() :
    BaseManager("Channels")
{
    itemDataType = "FixtureTypeChannel";
    selectItemWhenCreated = false;
    updateCanAddItems();
    saveAndLoadRecursiveData = false;
}

FixtureTypeChannelManager::~FixtureTypeChannelManager()
{
}

// TODO: change this to calculate/check ranges for collisions?
void FixtureTypeChannelManager::calcDmxChannels() {
	if (massiveImport) return;
//	int current = 1;
//	for (int i = 0; i < items.size(); i++) {
//		items[i]->setNiceName("temp "+String(i));
//	}
//	for (int i = 0; i < items.size(); i++) {
//		items[i]->dmxDelta->setValue(current);
//		String name = "Channel " + String(current);
//		if (items[i]->resolution->getValue() == "8bits") {
//			current += 1;
//		}
//		else if (items[i]->resolution->getValue() == "fine") {
//			current += 1;
//		}
//		else if (items[i]->resolution->getValue() == "16bits") {
//			name += " + "+String(current+1);
//			current += 2;
//		}
//		else {
//			LOG("no resolution !");
//		}
//
//		ChannelType* ct = dynamic_cast<ChannelType*>(items[i]->channelType->targetContainer.get());
//		if (ct != nullptr) {
//			name += " - "+ct->niceName;
//		}
//
//		items[i]->setNiceName(name);
//	}
}



/*
void FixtureTypeChannelManager::askForMoveBefore(BaseItem* c) {
    BaseManager::askForMoveBefore(c);
};
void FixtureTypeChannelManager::askForMoveAfter(BaseItem* c) {
    BaseManager::askForMoveAfter(c);
};
void FixtureTypeChannelManager::addItemInternal(FixtureTypeChannel* c, var data) {
};

void FixtureTypeChannelManager::askForMoveAfter(BaseItem* c) {
    BaseManager::askForMoveAfter(c);
};
void FixtureTypeChannelManager::askForMoveAfter(BaseItem* c) {
    BaseManager::askForMoveAfter(c);
};
*/

String FixtureTypeChannelManager::parentDMXChannelResoltion(){
    if (parentContainer != nullptr) {
        FixtureTypeDMXChannel* parent = dynamic_cast<FixtureTypeDMXChannel*>(parentContainer.get());
        if (parent != nullptr) {
            return parent->resolution->getValue();
        }
    }
    return "8bits";
}

void FixtureTypeChannelManager::updateCanAddItems() {
    // Allow adding items if:
    // 1. Parent DMX channel is 8-bit, OR
    // 2. No items exist yet (first item can always be added)
    String resolution = parentDMXChannelResoltion();
    bool canAdd = (resolution == "8bits") || (items.size() == 0);
    userCanAddItemsManually = canAdd;
}

void FixtureTypeChannelManager::addItemInternal(FixtureTypeChannel* c, var data) {
    if(!userCanAddItemsManually) {
        juce::AlertWindow::showMessageBox (
            juce::AlertWindow::WarningIcon,
            "Cannot split channel",
            "Only 8-bit channels can be split into multiple logical channels. Please remove extra logical channels.",
            "Ok",
            &ShapeShifterManager::getInstance()->mainContainer);
        // Can't actually remove it directly because the object needs to still exist for steps further down the call stack
        // askForRemoveBaseItem(c);
    }
    calcDmxChannels();
    updateCanAddItems();
}

void FixtureTypeChannelManager::askForRemoveBaseItem(BaseItem* item) {
	BaseManager::askForRemoveBaseItem(item);
	calcDmxChannels();
    updateCanAddItems();
}
void FixtureTypeChannelManager::askForDuplicateItem(BaseItem* item) {
	BaseManager::askForDuplicateItem(item);
	calcDmxChannels();
    updateCanAddItems();
}
void FixtureTypeChannelManager::askForPaste() {
	BaseManager::askForPaste();
	calcDmxChannels();
    updateCanAddItems();
}
void FixtureTypeChannelManager::askForMoveBefore(BaseItem* i) {
	BaseManager::askForMoveBefore(i);
	calcDmxChannels();
}
void FixtureTypeChannelManager::askForMoveAfter(BaseItem* i) {
	BaseManager::askForMoveAfter(i);
	calcDmxChannels();
}

void FixtureTypeChannelManager::setItemIndex(FixtureTypeChannel* item, int newIndex, bool addToUndo)
{
	BaseManager::setItemIndex(item, newIndex, addToUndo);
	calcDmxChannels();
}


/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "FixtureType.h"
#include "Brain.h"

FixtureType::FixtureType(var params) :
	BaseItem(params.getProperty("name", "FixtureType")),
	objectType(params.getProperty("type", "FixtureType").toString()),
	objectData(params),
    dmxChannelsManager(),
	virtualChansManager(),
	helpContainer("Editor Help")
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = true;
	canBeDisabled = false;
	
	itemDataType = "FixtureType";
	layoutStrokeColor = addColorParameter("Layout stroke color", "Default stroke color in layout", Colours::orange);
	layoutFillColor = addColorParameter("Layout fill color", "Default fill color in layout", Colours::white);
	layoutIconParameter = addFileParameter("Layout icon", "Load a custom png do display in the layout");
	
	templateId = helpContainer.addIntParameter("Template ID", "Use the subfixture with this id as template", 0, 0);
	copyToId = helpContainer.addIntParameter("Copy until ID", "Duplicate the template until the copy has this ID", 0, 0);
	copyTemplateButton = helpContainer.addTrigger("Copy template", "");
	//chansManager = new BaseManager<FixtureTypeChannel>("Channels");
	// ContainerAsyncListener* newListener = new ContainerAsyncListener();
	// chansManager->addAsyncContainerListener();
	addChildControllableContainer(&dmxChannelsManager);
	addChildControllableContainer(&virtualChansManager);
	addChildControllableContainer(&helpContainer);
	helpContainer.saveAndLoadRecursiveData = false;
}

FixtureType::~FixtureType()
{
}

void FixtureType::afterLoadJSONDataInternal() {
	updateVirtualLists();
}

void FixtureType::updateVirtualLists() {
    for (int x = 0; x < dmxChannelsManager.items.size(); x++) {
        FixtureTypeDMXChannel* dmxChannel = dmxChannelsManager.items[x];
        
        for (int i=0; i < dmxChannel->chansManager.items.size(); i++) {
            String value = dmxChannel->chansManager.items[i]->virtualMaster->getValue();
            dmxChannel->chansManager.items[i]->virtualMaster->setRootContainer(&virtualChansManager);
            dmxChannel->chansManager.items[i]->virtualMaster->setValue(value);
        }
	}
}

void FixtureType::onContainerParameterChangedInternal(Parameter* p)
{
	if (p == layoutIconParameter) {
		File f = layoutIconParameter->getFile();
		useLayoutIcon = false;
		if (f.exists()) {
			layoutIconImage = ImageFileFormat::loadFrom(f);
			if (layoutIconImage.isValid()) {
				int limit = 100;
				int w = layoutIconImage.getWidth();
				int h = layoutIconImage.getHeight();
				if (w > limit || h > limit) {
					if (w > h) {
						h = limit * h / w;
						w = limit;
					}
					else {
						w = limit * w / h;
						h = limit;
					}
					layoutBorderImage = layoutIconImage.rescaled(w,h);
					layoutContentImage = layoutIconImage.rescaled(w, h);

				}
				else {
					layoutBorderImage = layoutIconImage.createCopy();
					layoutContentImage = layoutIconImage.createCopy();
				}
				Colour c(1.0, 1.0, 1.0);
				for (int x = 0; x < layoutContentImage.getWidth(); x++) {
					for (int y = 0; y < layoutContentImage.getHeight(); y++) {
						Colour pixel = layoutContentImage.getPixelAt(x, y);
						layoutBorderImage.setPixelAt(x, y, c.withAlpha(pixel.getFloatRed() * pixel.getFloatAlpha()));
						layoutContentImage.setPixelAt(x, y, c.withAlpha(pixel.getFloatGreen() * pixel.getFloatAlpha()));
					}
				}
				useLayoutIcon = true;
			}
		}
		Brain::getInstance()->layoutViewerNeedRefresh = true;
	}
	if (p == layoutFillColor || p == layoutStrokeColor) {
		Brain::getInstance()->layoutViewerNeedRefresh = true;
	}
}

void FixtureType::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	if (c == copyTemplateButton) {
		copyTemplate();
		copyToId->setValue(0);
	}
}

void FixtureType::copyTemplate()
{
	int from = templateId->intValue();
	int to = copyToId->intValue();
	if (to <= from)
	{
		LOG("Copy to must be higher than template ID");
		return;
	}

	Array<FixtureTypeVirtualChannel*> ftvcToAdd;
	Array<FixtureTypeDMXChannel*> ftdcToAdd;
	HashMap<FixtureTypeChannel*, FixtureTypeVirtualChannel*> vDimmers;

	for (int subId = from + 1; subId <= to; subId++) {
		HashMap<FixtureTypeVirtualChannel*, FixtureTypeVirtualChannel*> virtualChans;

		// Copy virtual channels that belong to the 'from' subfixture
		Array<FixtureTypeVirtualChannel*> virtToCopy;
		for (FixtureTypeVirtualChannel* c : virtualChansManager.items) {
			if (c->subFixtureId->intValue() == from) {
				virtToCopy.add(c);
			}
		}
		for (FixtureTypeVirtualChannel* c : virtToCopy) {
			FixtureTypeVirtualChannel* newC = new FixtureTypeVirtualChannel();
			ftvcToAdd.add(newC);
			newC->loadJSONData(c->getJSONData());
			newC->subFixtureId->setValue(subId);
			newC->editorIsCollapsed = true;
			virtualChans.set(c, newC);
		}

		// Find DMX channels that contain logical channels belonging to the 'from' subfixture
		Array<FixtureTypeDMXChannel*> dmxChannelsToCopy;
		for (FixtureTypeDMXChannel* dmxChannel : dmxChannelsManager.items) {
			bool hasCopyableChannels = false;
			for (FixtureTypeChannel* logicalChannel : dmxChannel->chansManager.items) {
				if (logicalChannel->subFixtureId->intValue() == from) {
					hasCopyableChannels = true;
					break;
				}
			}
			if (hasCopyableChannels) {
				dmxChannelsToCopy.add(dmxChannel);
			}
		}

		// Copy each DMX channel and update the subfixture IDs
		for (FixtureTypeDMXChannel* originalDMXChannel : dmxChannelsToCopy) {
			FixtureTypeDMXChannel* newDMXChannel = new FixtureTypeDMXChannel();
			ftdcToAdd.add(newDMXChannel);

			// Load the DMX channel data (this will create all logical channels too)
			newDMXChannel->loadJSONData(originalDMXChannel->getJSONData());
			newDMXChannel->editorIsCollapsed = true;

			// Update subfixture IDs on the logical channels
			for (FixtureTypeChannel* logicalChannel : newDMXChannel->chansManager.items) {
				if (logicalChannel->subFixtureId->intValue() == from) {
					logicalChannel->subFixtureId->setValue(subId);
				}

				// Update virtual master mappings
				FixtureTypeVirtualChannel* v = dynamic_cast<FixtureTypeVirtualChannel*>(logicalChannel->virtualMaster->targetContainer.get());
				if (v != nullptr && virtualChans.contains(v)) {
					vDimmers.set(logicalChannel, virtualChans.getReference(v));
				}
			}
		}
	}

	// Add all the copied items
	virtualChansManager.editorIsCollapsed = true;
	virtualChansManager.addItems(ftvcToAdd, var(), false);

	dmxChannelsManager.massiveImport = true;
	dmxChannelsManager.editorIsCollapsed = true;
	dmxChannelsManager.addItems(ftdcToAdd, var(), false);
	dmxChannelsManager.massiveImport = false;
	dmxChannelsManager.calcDmxChannels();

	updateVirtualLists();

	// Update virtual master references
	for (auto it = vDimmers.begin(); it != vDimmers.end(); it.next()) {
		it.getKey()->virtualMaster->setValueFromTarget(it.getValue());
	}

	virtualChansManager.editorIsCollapsed = false;
	dmxChannelsManager.editorIsCollapsed = false;
	queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

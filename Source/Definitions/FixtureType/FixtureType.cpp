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
#include "Definitions/Preset/PresetManager.h"

FixtureType::FixtureType(var params) :
	BaseItem(params.getProperty("name", "FixtureType")),
	objectType(params.getProperty("type", "FixtureType").toString()),
	objectData(params),
	chansManager(),
	virtualChansManager(),
	helpContainer("Editor Help"),
	presets("Presets"),
	presetsCC("Presets")
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
	addChildControllableContainer(&chansManager);
	addChildControllableContainer(&virtualChansManager);
	addChildControllableContainer(&helpContainer);
	addChildControllableContainer(&presetsCC);
	presetsCC.addChildControllableContainer(&presets);
	presetsCC.saveAndLoadRecursiveData = true;
	presets.selectItemWhenCreated = false;
	helpContainer.saveAndLoadRecursiveData = false;

	importFromGridBtn = presetsCC.addTrigger("Import data from grid", "");
	exportInExistingBtn = presetsCC.addTrigger("Export in existing grid", "");
	exportAllToGridBtn = presetsCC.addTrigger("Create missing in grid", "");

}

FixtureType::~FixtureType()
{
}

void FixtureType::afterLoadJSONDataInternal() {
	updateVirtualLists();
}

void FixtureType::updateVirtualLists() {
	for (int i = 0; i < chansManager.items.size(); i++) {
		String value = chansManager.items[i]->virtualMaster->getValue();
		chansManager.items[i]->virtualMaster->setRootContainer(&virtualChansManager);
		chansManager.items[i]->virtualMaster->setValue(value);
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
	else if (c == importFromGridBtn) { importFromGrid(); }
	else if (c == exportAllToGridBtn) { exportToGrid(true); }
	else if (c == exportInExistingBtn) { exportToGrid(false); }
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
	Array<FixtureTypeChannel*> ftcToAdd;
	HashMap<FixtureTypeChannel*, FixtureTypeVirtualChannel*> vDimmers;

	int temp = 1;

	for (int subId = from + 1; subId <= to; subId++) {
		HashMap<FixtureTypeVirtualChannel*, FixtureTypeVirtualChannel*> virtualChans;
		Array< FixtureTypeVirtualChannel*> virtToCopy;
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
		Array< FixtureTypeChannel*> toCopy;
		for (FixtureTypeChannel* c : chansManager.items) {
			if (c->subFixtureId->intValue() == from) {
				toCopy.add(c);
			}
		}
		for (FixtureTypeChannel* c : toCopy) {
			FixtureTypeChannel* newC = new FixtureTypeChannel();
			ftcToAdd.add(newC);
			newC->loadJSONData(c->getJSONData());
			newC->subFixtureId->setValue(subId);
			newC->editorIsCollapsed = true;
			newC->setNiceName("import "+String(temp));
			temp++;
			FixtureTypeVirtualChannel* v = dynamic_cast<FixtureTypeVirtualChannel*>(c->virtualMaster->targetContainer.get());
			if (v != nullptr && virtualChans.contains(v)) {
				vDimmers.set(newC, virtualChans.getReference(v));
			}
		}
	}
	virtualChansManager.editorIsCollapsed = true;
	virtualChansManager.addItems(ftvcToAdd, var(), false);
	chansManager.massiveImport = true;
	chansManager.editorIsCollapsed = true;
	chansManager.addItems(ftcToAdd, var(), false);
	chansManager.massiveImport = false;
	chansManager.calcDmxChannels();
	updateVirtualLists();
	for (auto it = vDimmers.begin(); it != vDimmers.end(); it.next()) {
		it.getKey()->virtualMaster->setValueFromTarget(it.getValue());
	}
	virtualChansManager.editorIsCollapsed = false;
	chansManager.editorIsCollapsed = false;
	queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void FixtureType::importFromGrid()
{
	Fixture* fixt = nullptr;
	for (auto it = Brain::getInstance()->fixtures.begin(); it != Brain::getInstance()->fixtures.end() && fixt == nullptr; it.next()) {
		Fixture* f = it.getValue();
		FixtureType* ft = dynamic_cast<FixtureType*>(f->devTypeParam->targetContainer.get());
		if (ft == this) {
			fixt = f;
		}
	}
	if (fixt == nullptr) return;
	for (auto itPresets = Brain::getInstance()->presets.begin(); itPresets != Brain::getInstance()->presets.end(); itPresets.next()) {
		Preset* p = itPresets.getValue();
		p->computeValues();
		bool valid = false;
		for (auto itSF = fixt->subFixtures.begin(); !valid && itSF != fixt->subFixtures.end(); itSF.next()) {
			SubFixture* sf = itSF.getValue();
			auto vals = p->getSubFixtureValues(sf);
			for (auto itCT = vals->begin(); !valid && itCT != vals->end(); itCT.next()) {
				ChannelType* ct = itCT.getKey();
				if (ct != nullptr && sf->channelsMap.contains(ct)) {
					valid = true;
				}
			}
		}
		if (valid) {
			FixtureTypePreset * ftp = nullptr;
			for (int i = 0; ftp == nullptr && i < presets.items.size(); i++) {
				if (p->userName->stringValue() == presets.items[i]->niceName) {
					ftp = presets.items[i];
				}
			}
			if (ftp == nullptr) {
				ftp = presets.addItem();
				ftp -> setNiceName(p->userName->stringValue());
			}
			ftp->importFromPresetGrid();
		}
	}
}

void FixtureType::exportToGrid(bool createIfNotHere)
{
	for (int i = 0; i < presets.items.size(); i++) {
		FixtureTypePreset * ftp = presets.items[i];
		ftp->exportInPresetGrid(createIfNotHere);
	}
}

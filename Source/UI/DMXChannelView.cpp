/*
  ==============================================================================

	DMXChannelView.cpp
	Created: 8 Oct 2020 6:59:51pm
	Author:  bkupe

  ==============================================================================
*/

#include "DMXChannelView.h"
#include "Definitions/Interface/interfaces/dmx/DMXInterface.h"
#include "Definitions/Fixture/FixtureManager.h"
#include "Definitions/FixtureType/FixtureTypeManager.h"
#include "BKEngine.h"

DMXChannelView::DMXChannelView() :
	ShapeShifterContentComponent("DMX Tester"),
    flashValue(nullptr),
    //testingUI(nullptr),
    engine(nullptr),
    currentInterface(nullptr)
{
	dmxList.setTextWhenNoChoicesAvailable("No DMX Interface");
	dmxList.setTextWhenNothingSelected("Select a DMX interface");
	dmxList.addListener(this);
	addAndMakeVisible(&dmxList);

	rebuildChannelItems();

	BKEngine* testEngine = dynamic_cast<BKEngine*>(Engine::mainEngine);
	if (testEngine != nullptr) {
		this->engine = testEngine;
	}

	if (this->engine != nullptr) {
		DMXInterface* i = engine->dmxTesterInterface->getTargetContainerAs<DMXInterface>();
		if (i != nullptr && currentInterface != i) {
			setCurrentInterface(i);
		}
	}

	rebuildDMXList();

	addAndMakeVisible(viewport);
	viewport.setViewedComponent(&channelContainer);
	addKeyListener(this);
	InterfaceManager::getInstance()->addAsyncManagerListener(this);
	engine = dynamic_cast<BKEngine*>(BKEngine::mainEngine);
	engine->currentDMXChannelView = this;
	engine->dmxTesterInterface->addParameterListener(this);
}

DMXChannelView::~DMXChannelView()
{
	engine->dmxTesterInterface->removeParameterListener(this);
	if (engine->currentDMXChannelView == this) {
		engine->currentDMXChannelView = nullptr;
	}
	if (InterfaceManager::getInstanceWithoutCreating()) InterfaceManager::getInstance()->removeAsyncManagerListener(this);
	setCurrentInterface(nullptr);
}

juce_ImplementSingleton(DMXChannelView);


void DMXChannelView::resized()
{
	Rectangle<int> r = getLocalBounds();
	Rectangle<int> hr = r.removeFromTop(20);

	dmxList.setBounds(hr.removeFromLeft(150).reduced(2));

	/*if (testingUI != nullptr)
	{
		hr.removeFromLeft(8);
		testingUI->setBounds(hr.removeFromLeft(100).reduced(2));
	}*/

	if (flashValue != nullptr)
	{
		hr.removeFromLeft(8);
		flashValue->setBounds(hr.removeFromLeft(150).reduced(1));
	}


	int numRows = 16;
	int itemSize = (r.getWidth() - 6) / numRows;
	int lowerY = 0;
	for (int i = 0; i < channelItems.size(); i++)
	{
		int tx = (i % numRows) * itemSize;
		int ty = floorf(i / numRows) * itemSize;
		Rectangle<int> ir = Rectangle<int>(tx, ty, itemSize, itemSize);
		channelItems[i]->setBounds(ir);
		lowerY = ir.getBottom();
	}

	channelContainer.setBounds(Rectangle<int>(0, 0, r.getWidth() - 10, lowerY));
	viewport.setBounds(r);
}

void DMXChannelView::setCurrentInterface(DMXInterface* i, bool clearSel)
{
	if (currentInterface == i) return;

	if (clearSel) {
		clearSelection();
	}

	if (currentInterface != nullptr && !currentInterface->isClearing)
	{
		currentInterface->channelTestingMode->setValue(false);
		currentInterface->removeInspectableListener(this);
		currentInterface->tester = nullptr;
		//removeChildComponent(testingUI.get());
		//testingUI.reset();
		removeChildComponent(flashValue.get());
		flashValue.reset();
	}

	currentInterface = i;

	if (currentInterface != nullptr)
	{
		currentInterface->tester = this;
		currentInterface->channelTestingMode->setValue(true);
		currentInterface->addInspectableListener(this);

		//testingUI.reset(currentInterface->channelTestingMode->createButtonToggle());
		//addAndMakeVisible(testingUI.get());
		flashValue.reset(currentInterface->channelTestingFlashValue->createSlider());
		flashValue->useCustomFGColor = true;
		flashValue->customFGColor = Colour(0,127,0);
		//flashValue->setColour(FloatSliderUI)
		addAndMakeVisible(flashValue.get());

		currentInterface->repaintChannels(1,512);

		resized();
	}
	else {
		for (int n = 0; n < 512; n++) {
			channelItems[n]->value = 0;
		}
		repaint();
	}
}

void DMXChannelView::rebuildDMXList()
{
	DMXInterface* tmp = currentInterface;

	int idToSelect = 0;

	dmxList.clear(dontSendNotification);
	dmxList.addItem("No DMX Interface selected", -1);
	for (auto& i : InterfaceManager::getInstance()->items)
	{
		if (DMXInterface* d = dynamic_cast<DMXInterface*>(i))
		{
			int id = InterfaceManager::getInstance()->items.indexOf(i) + 1;
			if (d == tmp) idToSelect = id;
			dmxList.addItem(i->niceName, id);
		}
	}

	dmxList.setSelectedId(idToSelect, dontSendNotification);
}

void DMXChannelView::rebuildChannelItems()
{
	for (auto& c : channelItems)  removeChildComponent(c);

	channelItems.clear();

	for (int i = 0; i < 512; i++)
	{
		DMXChannelItem* it = new DMXChannelItem(i + 1, this);
		channelContainer.addAndMakeVisible(it);
		channelItems.add(it);
	}
}

void DMXChannelView::sendDMXValue(int channel, float value)
{
	if (currentInterface != nullptr && currentInterface->channelTestingMode->boolValue()) currentInterface->sendDMXValue(channel, value);
}

void DMXChannelView::newMessage(const InterfaceManager::ManagerEvent& e)
{
	if (e.type == InterfaceManager::ManagerEvent::ITEM_REMOVED || e.type == InterfaceManager::ManagerEvent::ITEMS_REMOVED) {
		setCurrentInterface(nullptr);
	}
	rebuildDMXList();
}

void DMXChannelView::clearSelection()
{
	for (int i = 0; i < selectedItems.size(); i++) {
		selectedItems[i]->tmpFlash = false;
		selectedItems[i]->updateDMXValue(0);
	}
	selectedItems.clear();
}

void DMXChannelView::rangeOn(int from, int to)
{
	from = jmax(from, 0);
	to = jmax(to, 0);
	for (int i = jmin(from, to); i <= jmax(from, to); i++) {
		selectedItems.add(channelItems[i]);
		channelItems[i]->tmpFlash = true;
		channelItems[i]->updateDMXValue(getFlashValue());
	}
}

void DMXChannelView::rangeOff(int from, int to)
{
	from = jmax(from, 0);
	to = jmax(to, 0);
	for (int i = jmin(from, to); i <= jmax(from, to); i++) {
		selectedItems.removeAllInstancesOf(channelItems[i]);
		channelItems[i]->tmpFlash = false;
		channelItems[i]->updateDMXValue(0);
	}
}

void DMXChannelView::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	int id = dmxList.getSelectedId();
	if (id == -1) {
		engine->dmxTesterInterface->resetValue();
	} else {
		DMXInterface* d = (DMXInterface*)InterfaceManager::getInstance()->items[id - 1];
		engine->dmxTesterInterface->setValueFromTarget(d);
	}
}

float DMXChannelView::getFlashValue()
{
	return currentInterface != nullptr ? currentInterface->channelTestingFlashValue->floatValue() : 1;
}

bool DMXChannelView::keyPressed(const KeyPress& key, Component* originatingComponent)
{
	if (key.getKeyCode() == KeyPress::leftKey) {
		clearSelection();
		lastClickedId = lastClickedId - 1;
		lastClickedId = jlimit(-1,511,lastClickedId);
		if (!key.getModifiers().isShiftDown()) {
			keyboardStartSelection = lastClickedId;
		}
		if (lastClickedId > -1) {
			rangeOn(keyboardStartSelection, lastClickedId);
		}
	}
	if (key.getKeyCode() == KeyPress::rightKey) {
		clearSelection();
		lastClickedId = lastClickedId + 1;
		lastClickedId = jlimit(-1, 511, lastClickedId);
		if (!key.getModifiers().isShiftDown()) {
			keyboardStartSelection = lastClickedId;
		}
		if (lastClickedId > -1) {
			rangeOn(keyboardStartSelection, lastClickedId);
		}
	}
	return false;
}

void DMXChannelView::inspectableDestroyed(Inspectable* i)
{
	if (i == currentInterface) setCurrentInterface(nullptr);
	rebuildDMXList();
}

void DMXChannelView::parameterValueChanged(Parameter* p)
{
	if (p == engine->dmxTesterInterface) {
		DMXInterface* i = engine->dmxTesterInterface->getTargetContainerAs<DMXInterface>();
		if (currentInterface != i) {
			setCurrentInterface(i, true);
			rebuildDMXList();
		}
	}
}

DMXChannelItem::DMXChannelItem(int channel, DMXChannelView* v) :
	channel(channel),
	valueAtMouseDown(0),
	value(0),
	tmpFlash(false),
	channelView(v)
{
	setRepaintsOnMouseActivity(true);
}

DMXChannelItem::~DMXChannelItem()
{
}

void DMXChannelItem::mouseEnter(const MouseEvent& e)
{
	//tmpFlash = e.mods.isShiftDown();
	//valueAtMouseDown = value;
	//if (tmpFlash) updateDMXValue(channelView->getFlashValue());
}

void DMXChannelItem::mouseExit(const MouseEvent& e)
{
	//if (tmpFlash) updateDMXValue(valueAtMouseDown);
	//tmpFlash = false;
}

void DMXChannelItem::mouseDown(const MouseEvent& e)
{
	valueAtMouseDown = value;
	if (e.mods.isLeftButtonDown())
	{
		if (e.mods.isShiftDown() && !e.mods.isAltDown()) {
		if (!channelView->selectedItems.contains(this)) {
			channelView->rangeOn(channelView->lastClickedId, channelView->channelItems.indexOf(this));
		}
		else {
			channelView->rangeOff(channelView->lastClickedId, channelView->channelItems.indexOf(this));
		}
		}
		else if (channelView->selectedItems.contains(this) && !e.mods.isAltDown()) {
			updateDMXValue(savedValue);

			channelView->selectedItems.removeAllInstancesOf(this);
			tmpFlash = false;
		}
		else if (e.mods.isCtrlDown() && !e.mods.isAltDown()) {
			savedValue = value;
			updateDMXValue(channelView->getFlashValue());
			channelView->selectedItems.add(this);
			tmpFlash = true;
		}
		else {
			channelView->clearSelection();
			savedValue = value;
			if (!e.mods.isAltDown()) {
				updateDMXValue(channelView->getFlashValue());
			}
			channelView->selectedItems.add(this);
			tmpFlash = true;

		}
		channelView->lastClickedId = channelView->channelItems.indexOf(this);


		if (!e.mods.isShiftDown()) {
			channelView->keyboardStartSelection = channelView->channelItems.indexOf(this);
		}
			//updateDMXValue(channelView->getFlashValue());
	}
	else if (e.mods.isRightButtonDown())
	{
		DMXInterface* inter = channelView->currentInterface;

		if (inter != nullptr) {
			PopupMenu p;
			PopupMenu fixtureTypesMenu;
			PopupMenu addPatchMenu;
			PopupMenu replacePatchMenu;
			int address = channel;

			for (int i = 0; i < FixtureTypeManager::getInstance()->items.size(); i++) {
				FixtureType* ft = FixtureTypeManager::getInstance()->items[i];
				fixtureTypesMenu.addItem(ft->niceName, [this, ft, inter, address]() {
					Fixture* f = FixtureManager::getInstance()->addItem();
					f->devTypeParam->setValueFromTarget(ft);
					FixturePatch* p = f->patchs.addItem();
					p->targetInterface->setValueFromTarget(inter);
					p->address->setValue(address);
				});
			}

			for (int i = 0; i < FixtureManager::getInstance()->items.size(); i++) {
				Fixture* f = FixtureManager::getInstance()->items[i];
				addPatchMenu.addItem(f->niceName, [this, f, inter, address]() {
					FixturePatch* p = f->patchs.addItem();
					p->targetInterface->setValueFromTarget(inter);
					p->address->setValue(address);
				});
				replacePatchMenu.addItem(f->niceName, [this, f, inter, address]() {
					f->patchs.clear();
					FixturePatch* p = f->patchs.addItem();
					p->targetInterface->setValueFromTarget(inter);
					p->address->setValue(address);
				});

			}

			// p.addSubMenu();
			p.addSubMenu("Create fixture", fixtureTypesMenu);
			p.addSubMenu("Add fixture patch", addPatchMenu);
			p.addSubMenu("Replace fixture patch", replacePatchMenu);

			p.showMenuAsync(PopupMenu::Options(), [this](int result)
				{
					/*if (Object* o = ObjectManager::getInstance()->factory.createFromMenuResult(result))
					{
						o->targetInterface->setValueFromTarget(channelView->currentInterface);
						if (o->interfaceParameters != nullptr)
						{
							if (IntParameter* ch = dynamic_cast<IntParameter*>(o->interfaceParameters->getParameterByName("startChannel")))
							{
								ch->setValue(channel);
							}
						}
						ObjectManager::getInstance()->addItem(o);
					}
				*/
				}
			);

		}

	}
}

void DMXChannelItem::mouseDrag(const MouseEvent& e)
{
	if (e.mods.isShiftDown()) return;

	if (e.mods.isLeftButtonDown() && e.mods.isAltDown())
	{
		updateDMXValue(valueAtMouseDown - e.getOffsetFromDragStart().y * 1.0f / getHeight());
		tmpFlash = true;
	}
}

void DMXChannelItem::mouseUp(const MouseEvent& e)
{
	//if (tmpFlash) return;
	if (e.mods.isLeftButtonDown() && !e.mods.isAltDown())
	{
		// updateDMXValue(valueAtMouseDown);
	}
}

void DMXChannelItem::modifierKeysChanged(const ModifierKeys& keys)
{
	/*
	if (Desktop::getInstance().getMainMouseSource().getCurrentModifiers().isLeftButtonDown()) return;
	bool shift = keys.isShiftDown();
	if (tmpFlash != shift)
	{
		tmpFlash = shift;
		if (tmpFlash)
		{
			valueAtMouseDown = value;
			updateDMXValue(channelView->getFlashValue());
		}
		else updateDMXValue(valueAtMouseDown);
	}
	*/
}

void DMXChannelItem::updateDMXValue(float val)
{
	value = jlimit<float>(0, 1, val);
	channelView->sendDMXValue(channel, value * 255);
	repaint();
}


void DMXChannelItem::paint(Graphics& g)
{
	Rectangle<int> r = getLocalBounds().reduced(2);

	Colour c = BG_COLOR.darker();
	String fixtName = "";
	String channelType = "";
	//c = Colour(32,64,32);
	if (channelView->currentInterface != nullptr) {
		FixturePatch* p = channelView->currentInterface->channelToFixturePatch[channel];
		if (p != nullptr) {
			c = BG_COLOR.brighter(0.2f);
			if (p->address->intValue() == channel) {
				fixtName = String(p->getFixtureId());
			}
		}
		ChannelType * ct = channelView->currentInterface->channelToChannelType[channel];
		channelType = ct != nullptr ? ct->niceName : "";
	}

	if (isMouseOver()) c = c.brighter(.4f);
	g.setColour(c);
	g.fillRoundedRectangle(r.toFloat(), 4);


	c = Colour(203,166,66).darker();
	if (isMouseOver()) c = c.brighter(.2f);
	g.setColour(c);
	g.fillRoundedRectangle(r.withTrimmedTop((1 - value) * r.getHeight()).toFloat(), 4);

	g.setColour(BG_COLOR.darker(.2f));
	g.drawRoundedRectangle(r.toFloat(), 4, 1);

	g.setColour(Colours::white.withAlpha(.8f));
	g.setFont(jlimit<float>(12, 20, getHeight() - 30));
	g.drawText(String(channel), getLocalBounds().toFloat(), Justification::centred, false);

	if (fixtName != "") {
		g.setColour(Colours::white.withAlpha(.6f));
		g.setFont(jlimit<float>(10, 16, getHeight() - 30));
		g.drawText(fixtName, getLocalBounds().reduced(1).toFloat(), Justification::topLeft, false);
	}

	if (channelType != "") {
		g.setColour(Colours::white.withAlpha(.6f));
		g.setFont(jlimit<float>(10, 16, getHeight() - 30));
		g.drawText(channelType, getLocalBounds().reduced(1).toFloat(), Justification::bottomLeft, false);
	}

}


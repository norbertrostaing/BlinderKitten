/*
  ==============================================================================

	DMXChannelView.cpp
	Created: 8 Oct 2020 6:59:51pm
	Author:  bkupe

  ==============================================================================
*/

#include "DMXChannelView.h"
#include "Definitions/Interface/interfaces/dmx/DMXInterface.h"

DMXChannelView::DMXChannelView() :
	ShapeShifterContentComponent("DMX Tester"),
	currentInterface(nullptr),
	//testingUI(nullptr),
	flashValue(nullptr)
{
	dmxList.setTextWhenNoChoicesAvailable("No DMX Interface");
	dmxList.setTextWhenNothingSelected("Select a DMX interface");
	dmxList.addListener(this);
	addAndMakeVisible(&dmxList);

	rebuildDMXList();
	rebuildChannelItems();

	addAndMakeVisible(viewport);
	viewport.setViewedComponent(&channelContainer);
	addKeyListener(this);
	InterfaceManager::getInstance()->addAsyncManagerListener(this);
}

DMXChannelView::~DMXChannelView()
{
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
		flashValue->setBounds(hr.removeFromLeft(150).reduced(2));
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

void DMXChannelView::setCurrentInterface(DMXInterface* i)
{
	clearSelection();
	if (currentInterface == i) return;

	if (currentInterface != nullptr && !currentInterface->isClearing)
	{
		currentInterface->channelTestingMode->setValue(false);
		currentInterface->removeInspectableListener(this);

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
		addAndMakeVisible(flashValue.get());

		currentInterface->repaintChannels(1,512);

		resized();
	}
	else {
		for (int i = 0; i < 512; i++) {
			channelItems[i]->value = 0;
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
	if (e.type == InterfaceManager::ManagerEvent::ITEM_ADDED || e.type == InterfaceManager::ManagerEvent::ITEM_REMOVED)

	{
		if (DMXInterface* d = dynamic_cast<DMXInterface*>(e.getItem())) rebuildDMXList();
	}
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
	for (int i = jmin(from, to); i <= jmax(from, to); i++) {
		selectedItems.add(channelItems[i]);
		channelItems[i]->tmpFlash = true;
		channelItems[i]->updateDMXValue(getFlashValue());
	}
}

void DMXChannelView::rangeOff(int from, int to)
{
	for (int i = jmin(from, to); i <= jmax(from, to); i++) {
		selectedItems.removeAllInstancesOf(channelItems[i]);
		channelItems[i]->tmpFlash = false;
		channelItems[i]->updateDMXValue(0);
	}
}

void DMXChannelView::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	DMXInterface* d = (dmxList.getSelectedId() == -1) ? nullptr : (DMXInterface*)InterfaceManager::getInstance()->items[dmxList.getSelectedId() - 1];
	setCurrentInterface(d);
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
		if (lastClickedId > -1) {
			rangeOn(lastClickedId, lastClickedId);
		}
	}
	if (key.getKeyCode() == KeyPress::rightKey) {
		clearSelection();
		lastClickedId = lastClickedId + 1;
		lastClickedId = jlimit(-1, 511, lastClickedId);
		if (lastClickedId > -1) {
			rangeOn(lastClickedId, lastClickedId);
		}
	}
	return false;
}

void DMXChannelView::inspectableDestroyed(Inspectable* i)
{
	if (i == currentInterface) setCurrentInterface(nullptr);
	rebuildDMXList();
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
	if (e.mods.isShiftDown()) {
		if (!channelView->selectedItems.contains(this)) {
			channelView->rangeOn(channelView->lastClickedId, channelView->channelItems.indexOf(this));
		}
		else {
			channelView->rangeOff(channelView->lastClickedId, channelView->channelItems.indexOf(this));
		}
	}
	else if (channelView->selectedItems.contains(this)) {
		updateDMXValue(0);
		channelView->selectedItems.removeAllInstancesOf(this);
		tmpFlash = false;
	}
	else if (e.mods.isCtrlDown()) {
		updateDMXValue(channelView->getFlashValue());
		channelView->selectedItems.add(this);
		tmpFlash = true;
	}
	else {
		channelView->clearSelection();
		updateDMXValue(channelView->getFlashValue());
		channelView->selectedItems.add(this);
		tmpFlash = true;

	}
	channelView->lastClickedId = channelView->channelItems.indexOf(this);
	//valueAtMouseDown = value;
	if (e.mods.isLeftButtonDown() && !e.mods.isAltDown())
	{
		//updateDMXValue(channelView->getFlashValue());
	}
	else if (e.mods.isRightButtonDown())
	{
		PopupMenu p;
		/*
		p.addSubMenu("Create from this channel...", ObjectManager::getInstance()->factory.getMenu());
		p.showMenuAsync(PopupMenu::Options(), [this](int result)
			{
				if (Object* o = ObjectManager::getInstance()->factory.createFromMenuResult(result))
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
			}
		);*/
	}
}

void DMXChannelItem::mouseDrag(const MouseEvent& e)
{
	if (tmpFlash || e.mods.isShiftDown()) return;

	if (e.mods.isLeftButtonDown() && e.mods.isAltDown())
	{
		updateDMXValue(valueAtMouseDown - e.getOffsetFromDragStart().y * 1.0f / getHeight());
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
	if (isMouseOver()) c = c.brighter(.2f);
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
}


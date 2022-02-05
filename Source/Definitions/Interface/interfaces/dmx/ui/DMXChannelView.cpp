/*
  ==============================================================================

    DMXChannelView.cpp
    Created: 8 Oct 2020 6:59:51pm
    Author:  bkupe

  ==============================================================================
*/

// #include "Object/ObjectIncludes.h"

DMXChannelView::DMXChannelView(const String& name) :
    ShapeShifterContentComponent(name),
    currentInterface(nullptr),
    testingUI(nullptr),
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

    InterfaceManager::getInstance()->addAsyncManagerListener(this);
}

DMXChannelView::~DMXChannelView()
{
    if(InterfaceManager::getInstanceWithoutCreating()) InterfaceManager::getInstance()->removeAsyncManagerListener(this);
    setCurrentInterface(nullptr);
}

void DMXChannelView::resized()
{
    Rectangle<int> r = getLocalBounds();
    Rectangle<int> hr = r.removeFromTop(20);

    dmxList.setBounds(hr.removeFromLeft(150).reduced(2));

    if (testingUI != nullptr)
    {
        hr.removeFromLeft(8);
        testingUI->setBounds(hr.removeFromLeft(100).reduced(2));
    }

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
        int tx = (i%numRows) * itemSize;
        int ty = floorf(i / numRows) * itemSize;
        Rectangle<int> ir = Rectangle<int>(tx, ty, itemSize, itemSize);
        channelItems[i]->setBounds(ir);
        lowerY = ir.getBottom();
    }

    channelContainer.setBounds(Rectangle<int>(0, 0, r.getWidth() - 6, lowerY));
    viewport.setBounds(r);
}

void DMXChannelView::setCurrentInterface(DMXInterface* i)
{
    if (currentInterface == i) return;

    if (currentInterface != nullptr && !currentInterface->isClearing)
    {
        currentInterface->channelTestingMode->setValue(false);
        currentInterface->removeInspectableListener(this);

        removeChildComponent(testingUI.get());
        testingUI.reset();
        removeChildComponent(flashValue.get());
        flashValue.reset();
    }
    
    currentInterface = i;

    if(currentInterface != nullptr)
    {
        currentInterface->channelTestingMode->setValue(true);
        currentInterface->addInspectableListener(this);

        testingUI.reset(currentInterface->channelTestingMode->createButtonToggle());
        addAndMakeVisible(testingUI.get());
        flashValue.reset(currentInterface->channelTestingFlashValue->createSlider());
        addAndMakeVisible(flashValue.get());

        resized();
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
            int id = InterfaceManager::getInstance()->items.indexOf(i) +1;
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
        DMXChannelItem * it = new DMXChannelItem(i+1, this);
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
        if(DMXInterface * d = dynamic_cast<DMXInterface*>(e.getItem())) rebuildDMXList();
    }
}

void DMXChannelView::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    DMXInterface* d = (dmxList.getSelectedId() == -1) ? nullptr : (DMXInterface *)InterfaceManager::getInstance()->items[dmxList.getSelectedId()-1];
    setCurrentInterface(d);
}

float DMXChannelView::getFlashValue()
{
    return currentInterface != nullptr ? currentInterface->channelTestingFlashValue->floatValue() : 1;
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
    tmpFlash = e.mods.isShiftDown();
    valueAtMouseDown = value;
    if(tmpFlash) updateDMXValue(channelView->getFlashValue());
}

void DMXChannelItem::mouseExit(const MouseEvent& e)
{
    if(tmpFlash) updateDMXValue(valueAtMouseDown);
    tmpFlash = false;
}

void DMXChannelItem::mouseDown(const MouseEvent& e)
{
    if (tmpFlash || e.mods.isShiftDown()) return;
    
    valueAtMouseDown = value;
    if (e.mods.isLeftButtonDown() && !e.mods.isAltDown())
    {
        updateDMXValue(channelView->getFlashValue());
    }
    else if (e.mods.isRightButtonDown())
    {
        /*
        PopupMenu p;
        p.addSubMenu("Create from this channel...", ObjectManager::getInstance()->factory.getMenu());

        int result = p.show();

        //switch (result)
        //{
        //default:
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
        //    break;
        //}
        */
    }
}

void DMXChannelItem::mouseDrag(const MouseEvent& e)
{
    if (tmpFlash || e.mods.isShiftDown()) return;

    if (e.mods.isLeftButtonDown() && e.mods.isAltDown())
    {
        updateDMXValue(valueAtMouseDown - e.getOffsetFromDragStart().y *1.0f / getHeight());
    }
}

void DMXChannelItem::mouseUp(const MouseEvent& e)
{
    if (tmpFlash) return;
    if (e.mods.isLeftButtonDown() && !e.mods.isAltDown())
    {
        updateDMXValue(valueAtMouseDown);
    }
}

void DMXChannelItem::modifierKeysChanged(const ModifierKeys& keys)
{
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
    

    c = Colours::hotpink.darker();
    if (isMouseOver()) c = c.brighter(.2f);
    g.setColour(c);
    g.fillRoundedRectangle(r.withTrimmedTop((1-value) * r.getHeight()).toFloat(),4);

    g.setColour(BG_COLOR.darker(.2f));
    g.drawRoundedRectangle(r.toFloat(), 4, 1);
    
    g.setColour(Colours::white.withAlpha(.8f));
    g.setFont(jlimit<float>(12,20,getHeight() - 30));
    g.drawText(String(channel), getLocalBounds().toFloat(), Justification::centred, false);
}

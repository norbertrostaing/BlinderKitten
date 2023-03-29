/*
  ==============================================================================

    DMXChannelView.h
    Created: 8 Oct 2020 6:59:51pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Definitions/Interface/InterfaceManager.h"

class DMXInterface;
class DMXChannelView;

class DMXChannelItem :
    public Component{
public:
    DMXChannelItem(int channel, DMXChannelView * v);
    ~DMXChannelItem();
    
    int channel;
    float valueAtMouseDown;
    float value;

    bool tmpFlash;

    DMXChannelView* channelView;

    void mouseEnter(const MouseEvent& e) override;
    void mouseExit(const MouseEvent& e) override;
    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    void modifierKeysChanged(const ModifierKeys& keys) override;

    void updateDMXValue(float val);

    void paint(Graphics& g) override;
    bool keyPressed(const KeyPress& key, Component* originatingComponent);
};

class DMXChannelView :
    public ShapeShifterContentComponent,
    public InterfaceManager::AsyncListener,
    public ComboBox::Listener,
    public Inspectable::InspectableListener,
    public KeyListener
{
public:
    juce_DeclareSingleton(DMXChannelView, true);

    DMXChannelView();
    ~DMXChannelView();

    Viewport viewport;
    Component channelContainer;

    //std::unique_ptr<BoolButtonToggleUI> testingUI;
    std::unique_ptr<FloatSliderUI> flashValue;

    ComboBox dmxList;
    OwnedArray<DMXChannelItem> channelItems;
    Array<DMXChannelItem*> selectedItems;
    int lastClickedId = 0;

    DMXInterface* currentInterface;

    void resized() override;

    void setCurrentInterface(DMXInterface* i);

    void rebuildDMXList();
    void rebuildChannelItems();

    void sendDMXValue(int channel, float value);

    void newMessage(const InterfaceManager::ManagerEvent& e) override;
    void clearSelection();
    void rangeOn(int from, int to);
    void rangeOff(int from, int to);

    // Inherited via Listener
    virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

    float getFlashValue();
    bool keyPressed(const KeyPress& key, Component* originatingComponent);

    void inspectableDestroyed(Inspectable* i) override;

    static DMXChannelView* create(const String& name) { return new DMXChannelView(); }

};
/*
  ==============================================================================

    LayoutViewer.h
    Created: 8 Oct 2020 6:59:51pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Definitions/Layout/LayoutManager.h"

class LayoutViewer :
    public ShapeShifterContentComponent,
    public ComboBox::Listener,
    public LayoutManager::AsyncListener,
    public ChangeListener

{
public:
    LayoutViewer();
    ~LayoutViewer();

    ComboBox layoutsList;

    //void setCurrentLayout(Layout* l);
    void rebuildLayoutsList();
    void newMessage(const LayoutManager::ManagerEvent& e) override;

    virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
    Layout* selectedLayout = nullptr;

    void selectLayout(int id);

    void resized() override;
    //void mouseEnter(const MouseEvent& e) override;
    //void mouseExit(const MouseEvent& e) override;
    //void mouseDown(const MouseEvent& e) override;
    //void mouseDrag(const MouseEvent& e) override;
    //void mouseUp(const MouseEvent& e) override;
    //void modifierKeysChanged(const ModifierKeys& keys) override;
    //bool keyPressed(const KeyPress& key, Component* originatingComponent);

    void changeListenerCallback(ChangeBroadcaster* source) override;

    void paint(Graphics& g) override;

    static LayoutViewer* create(const String& name) {return new LayoutViewer(); }

};
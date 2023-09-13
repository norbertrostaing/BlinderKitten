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
#include "Definitions/Stamp/StampManager.h"

class LayoutViewer :
    public ShapeShifterContentComponent,
    public ComboBox::Listener,
    public LayoutManager::AsyncListener,
    public StampManager::AsyncListener,
    public ChangeListener,
    public Timer

{
public:
    LayoutViewer();
    ~LayoutViewer();

    ComboBox layoutsList;
    ComboBox stampsList;
    Image clicZones;
    enum ClicAction {CLIC_NOACTION, CLIC_DRAG, CLIC_ORIGIN, CLIC_END, CLIC_TL, CLIC_TR, CLIC_BL, CLIC_BR, CLIC_ROTATE};

    ClicAction currentMouseAction = CLIC_NOACTION;
    BKPath* currentMousePath = nullptr;
    BKPath* hoveredPath = nullptr;
    float deltaMouseX = 0;
    float deltaMouseY = 0;
    float fixedMouseX = 0;
    float fixedMouseY = 0;

    HashMap<uint32, BKPath*> colourToTarget;
    HashMap<uint32, ClicAction> colourToAction;
    uint32 currentClicColour = 0;

    //void setCurrentLayout(Layout* l);
    void rebuildLayoutsList();
    void newMessage(const LayoutManager::ManagerEvent& e) override;

    void rebuildStampsList();
    void newMessage(const StampManager::ManagerEvent& e) override;

    Colour getClickColour(BKPath* path, ClicAction action);
    void resetClickColour();

    virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
    Layout* selectedLayout = nullptr;
    Stamp* selectedStamp = nullptr;

    void selectLayout(int id);
    void selectStamp(int id); 

    void resized() override;
    //void mouseEnter(const MouseEvent& e) override;
    void mouseExit(const MouseEvent& e) override;
    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    void mouseMove(const MouseEvent& e) override;
    //void modifierKeysChanged(const ModifierKeys& keys) override;
    //bool keyPressed(const KeyPress& key, Component* originatingComponent);
    void changeListenerCallback(ChangeBroadcaster* source) override;

    void paint(Graphics& g) override;

    void timerCallback() override;
    
    float topLeftX = 0;
    float topLeftY = 0;
    float bottomRightX = 0;
    float bottomRightY = 0;

    static LayoutViewer* create(const String& name) {return new LayoutViewer(); }

};
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

class BKEngine;

class LayoutViewer :
    public ShapeShifterContentComponent,
    public ComboBox::Listener,
    public ToggleButton::Listener,
    public LayoutManager::AsyncListener,
    public ChangeListener,
    public Timer

{
public:
    LayoutViewer();
    ~LayoutViewer();

    ComboBox layoutsList;
    Image clicZones;
    enum ClicAction {CLIC_NOACTION, CLIC_DRAG, CLIC_ORIGIN, CLIC_END, CLIC_TL, CLIC_TR, CLIC_BL, CLIC_BR, CLIC_ROTATE, CLIC_SELECT};
    
    ToggleButton viewPaths;
    ToggleButton editMode;
    TextButton exportBtn;

    BKEngine* engine;

    ClicAction currentMouseAction = CLIC_NOACTION;
    BKPath* currentMousePath = nullptr;
    BKPath* hoveredPath = nullptr;
    float deltaMouseX = 0;
    float deltaMouseY = 0;
    float fixedMouseX = 0;
    float fixedMouseY = 0;

    HashMap<uint32, BKPath*> colourToPath;
    HashMap<uint32, ClicAction> colourToAction;
    HashMap<uint32, SubFixture*> colourToSubFixture;
    HashMap<uint32, Fixture*> colourToFixture;
    uint32 currentClicColour = 0;

    //void setCurrentLayout(Layout* l);
    void rebuildLayoutsList();
    void newMessage(const LayoutManager::ManagerEvent& e) override;

    Colour getClickColour(BKPath* path, ClicAction action);
    Colour getClickColour(SubFixture* sf);
    Colour getClickColour(Fixture* f);
    void resetClickColour();

    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
    Layout* selectedLayout = nullptr;
    void buttonStateChanged(Button*) override;
    void buttonClicked(Button*) override;

    void selectLayout(int id);

    void resized() override;
    int currentWidth = 0; 
    int currentHeight = 0;
    //void mouseEnter(const MouseEvent& e) override;
    void mouseExit(const MouseEvent& e) override;
    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    void mouseMove(const MouseEvent& e) override;
    //void modifierKeysChanged(const ModifierKeys& keys) override;
    //bool keyPressed(const KeyPress& key, Component* originatingComponent);
    void changeListenerCallback(ChangeBroadcaster* source) override;

    void drawMidArrow(Graphics& g, Point<float>& from, Point<float>& to);
    void drawMidArrow(Graphics& g, float fromX, float fromY, float toX, float toY);

    void paint(Graphics& g) override;
    void stopAndCheckTimer();

    void drawFixture(Graphics& g, Fixture* f, BKPath* path, float x, float y, float w, float h, float angle, Colour c, BKPath::LabelPosition pos);
    void drawSubFixture(Graphics& g, SubFixture* sf, float x, float y, float w, float h, Colour c, BKPath::LabelPosition pos);
    void drawName(Graphics& g, String &name, float x, float y, float w, float h, Colour c, BKPath::LabelPosition pos);
    void exportToPNG();

    float topLeftX = 0;
    float topLeftY = 0;
    float bottomRightX = 0;
    float bottomRightY = 0;

    void timerCallback();
    static LayoutViewer* create(const String& name) {return new LayoutViewer(); }

};
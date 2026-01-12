/*
  ==============================================================================

    Interface.h
    Created: 26 Sep 2020 1:50:32pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
class Object;
class ObjectComponent;
class InterfaceUI;

class Interface :
    public BaseItem
{
public:
    Interface(String name = "Interface");
    virtual ~Interface();

    BoolParameter* logIncomingData;
    BoolParameter* logOutgoingData;


    virtual void sendValuesForObject(Object* o) {}


    virtual ControllableContainer* getInterfaceParams() { return new ControllableContainer("Interface parameters"); }

    virtual InterfaceUI* createUI();

    static bool isMidiInterface(Interface* i);

    class TimecodeListener {
    public:
        /** Destructor. */
        virtual ~TimecodeListener() {}
        virtual void timecodeUpdated(int tc) {};
    };

    int lastFrameSent = -1;      // filtre doublons

    ListenerList<TimecodeListener> timecodeListeners;

    void addTimecodeListener(TimecodeListener* newListener) { timecodeListeners.add(newListener); }
    void removeTimecodeListener(TimecodeListener* listener) { timecodeListeners.remove(listener); }

};
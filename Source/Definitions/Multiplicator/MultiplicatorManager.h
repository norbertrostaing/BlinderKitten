/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Multiplicator.h"

class MultiplicatorManager :
    public BaseManager<Multiplicator>
    // public Multiplicator::ObjectListener,
    //public URL::DownloadTask::Listener
{
public:
    juce_DeclareSingleton(MultiplicatorManager, true);

    MultiplicatorManager();
    ~MultiplicatorManager();

    // Factory<Object> factory;

    // BoolParameter* blackOut;

    //ui
    // IntParameter* gridThumbSize;
    // FloatParameter* defaultFlashValue;
    // BoolParameter* filterActiveInScene;
    // BoolParameter* lockUI;

    // URL downloadURL;
    // std::unique_ptr<URL::DownloadTask> downloadTask;
     
    // void downloadObjects();
    // void updateFactoryDefinitions();
    void addItemInternal(Multiplicator* o, var data) override;
    void removeItemInternal(Multiplicator* o) override;

    // int getFirstAvailableObjectID(Object* excludeObject = nullptr);
    // Object* getObjectWithID(int id, Object* excludeObject = nullptr);

    // void objectIDChanged(Object* o, int previousID) override;

    void onContainerParameterChanged(Parameter* p) override;

};
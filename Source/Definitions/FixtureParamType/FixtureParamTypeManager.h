/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "FixtureParamType.h"

class FixtureParamTypeManager :
    public BaseManager<FixtureParamType>,
    // public FixtureParamType::ObjectListener,
    public Thread
    //public URL::DownloadTask::Listener
{
public:
    juce_DeclareSingleton(FixtureParamTypeManager, true);

    FixtureParamTypeManager();
    ~FixtureParamTypeManager();

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
    void addItemInternal(FixtureParamType* o, var data) override;
    void removeItemInternal(FixtureParamType* o) override;

    // int getFirstAvailableObjectID(Object* excludeObject = nullptr);
    // Object* getObjectWithID(int id, Object* excludeObject = nullptr);

    // void objectIDChanged(Object* o, int previousID) override;

    void onContainerParameterChanged(Parameter* p) override;

    // var getSceneData();
    // void updateSceneData(var& sceneData);
    // void lerpFromSceneData(var startData, var endData, float weight);

    void run() override;

    // virtual void progress(URL::DownloadTask* task, int64 downloaded, int64 total) override;
    // virtual void finished(URL::DownloadTask* task, bool success) override;

};
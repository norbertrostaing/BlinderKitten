/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "ChannelFamily.h"

class ChannelFamilyManager :
    public BaseManager<ChannelFamily>
    // public ChannelFamily::ObjectListener,
    //public URL::DownloadTask::Listener
{
public:
    juce_DeclareSingleton(ChannelFamilyManager, true);

    ChannelFamilyManager();
    ~ChannelFamilyManager();

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
    void addItemInternal(ChannelFamily* o, var data) override;
    void removeItemInternal(ChannelFamily* o) override;

    // int getFirstAvailableObjectID(Object* excludeObject = nullptr);
    // Object* getObjectWithID(int id, Object* excludeObject = nullptr);

    // void objectIDChanged(Object* o, int previousID) override;

    void onContainerParameterChanged(Parameter* p) override;

    void importData(var data = var());

    Array<ChannelType*> orderedElements;
    void updateOrderedElements();

    // var getSceneData();
    // void updateSceneData(var& sceneData);
    // void lerpFromSceneData(var startData, var endData, float weight);

    // virtual void progress(URL::DownloadTask* task, int64 downloaded, int64 total) override;
    // virtual void finished(URL::DownloadTask* task, bool success) override;

};
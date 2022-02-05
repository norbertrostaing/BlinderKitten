/*
  ==============================================================================

    SceneListener.h
    Created: 10 Sep 2021 12:21:49pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class Scene;
class SceneListener
{
public:
    /** Destructor. */
    virtual ~SceneListener() {}
    virtual void askForLoadScene(Scene* i, float loadTime) {}
};

class SceneManagerEvent
{
public:
    enum Type { SCENE_LOAD_START, SCENE_LOAD_END };

    SceneManagerEvent(Type t) : type(t) {}
    Type type;
};

typedef QueuedNotifier<SceneManagerEvent>::Listener AsyncSceneListener;
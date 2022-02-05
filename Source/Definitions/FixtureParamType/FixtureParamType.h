/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "FixtureParamDefinition\FixtureParamDefinition.h"

class FixtureParamType :
    public BaseItem
    // public ChainVizTarget
{
public:
    FixtureParamType(var params = var());
    virtual ~FixtureParamType();

    String objectType;
    var objectData;

    // ControllableContainer definitions;

    std::unique_ptr<BaseManager<FixtureParamDefinition>> definitions;
    // BaseManager<T> parameters;
    // TargetParameter* targetInterface;
    // std::unique_ptr<ControllableContainer> interfaceParameters;
    // var interfaceGhostData;

    // std::unique_ptr<ComponentManager> componentManager;
    // std::unique_ptr<EffectManager> effectManager;
    // std::unique_ptr<SubObjectManager> objectManager;

    // IntParameter* globalID;
    // int previousID;
    
    // BoolParameter* excludeFromScenes;
    StringParameter* categoryName;

    // Point3DParameter* stagePosition;

    //ui
    // EnumParameter* icon;
    // Parameter* slideManipParameter;
    // float slideManipValueRef;

    //chainviz
    // HashMap<Effect*, float, DefaultHashFunctions, CriticalSection> effectIntensityOutMap;

    void rebuildInterfaceParams();

    //template<class T>
    //T* getComponent();

    void onContainerParameterChangedInternal(Parameter* p) override;
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;

    // void checkAndComputeComponentValuesIfNeeded();
    // void computeComponentValues(ObjectComponent* c);

    var getSceneData();
    void updateSceneData(var& sceneData);
    void lerpFromSceneData(var startData, var endData, float weight);


    // Array<ChainVizTarget *> getEffectChain();

    // ChainVizComponent* createVizComponent(Object * o, ChainVizTarget::ChainVizType type) override;

    // ListenerList<ObjectListener> objectListeners;
    // void addObjectListener(ObjectListener* newListener) { objectListeners.add(newListener); }
    // void removeObjectListener(ObjectListener* listener) { objectListeners.remove(listener); }

    String getTypeString() const override { return objectType; }
    static FixtureParamType* create(var params) { return new FixtureParamType(params); }
};


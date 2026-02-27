/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================

*/

#pragma once
#include "JuceHeader.h"
#include "../Command/CommandSelectionManager.h"
#include "StampMapping.h"
#include "Definitions/Interface/InterfaceIncludes.h"

class StampMapping;
class SubFixtureChannel;
class CommandSelectionManager;

class Stamp :
    public BaseItem,
    public NDIInputDevice::NDIInputListener
{
public:
    Stamp(var params = var());
    virtual ~Stamp();

    String objectType;
    var objectData;

    IntParameter* id;
    IntParameter* layerId;
    int registeredId = 0;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p);
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c);
    void updateName();

    NDIDeviceParameter* ndiParam;
    NDIInputDevice* ndiDevice = nullptr;

    CriticalSection imageLock;
    Image image;
    std::shared_ptr<Image::BitmapData> bitmapData;

    CommandSelectionManager selection;

    void initImage(int width, int height);
    void initImage(const Image& newImage);
    void updateDevice();
    void videoFrameReceived(NDIlib_video_frame_v2_t* frame) override;

    IntParameter* layoutId;
    
    Point2DParameter* videoPosition;
    Point2DParameter* videoSize;

    BoolParameter* isStampOn;
    bool isOn = false;
    bool isFlashing = false;
    bool isSwopping = false;

    Trigger* startBtn;
    Trigger* stopBtn;
    BoolParameter* autoStartAndStop;
    FloatParameter* sizeValue;
    FloatParameter* flashValue;

    String currentSizeController = "";
    String nextSizeController = "";
    float lastSize = 0;

    FloatParameter* fadeInTime;
    FloatParameter* fadeOutTime;
    double TSStartFadeIn = 0;
    double TSStartFadeOut = 0;
    double TSEndFadeIn = 0;
    double TSEndFadeOut = 0;

    BaseManager<StampMapping> mappings;

    IntParameter* soloPool;

    CriticalSection isComputing;

    HashMap<SubFixture*, juce::Point<double>> subFixtureToPixelPosition;
    HashMap<SubFixtureChannel*, juce::Array<StampMapping*>> sfcToStampMappings;

    String getTypeString() const override { return objectType; }

    void triggerTriggered(Trigger* t);
    void update(double now);
    void computeData();
    bool computed = false;
    bool computing = false;
    void pleaseComputeIfRunning();
    void userStart(float forcedFade = -1);
    void start(float forcedFade = -1);
    void stop(float forcedFade = -1);
    void kill();
    float applyToChannel(SubFixtureChannel* fc, float currentVal, double now);
    void tapTempo();

    void flash(bool on, bool swop = false);
    bool userPressedGo = false;

    static Stamp* create(var params) { return new Stamp(params); }
    
};

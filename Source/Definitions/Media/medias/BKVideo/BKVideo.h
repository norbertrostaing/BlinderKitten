/*
  ==============================================================================

    BKVideo.h
    Created: 26 Sep 2020 1:51:42pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

//#include "../../Common/CommonIncludes.h"
#include "Definitions/Media/Media.h"

class FixturePatch;
struct libvlc_instance_t;
struct libvlc_media_player_t;
struct libvlc_media_t;

class BKVideo :
    public Media,
    public Thread
{
public:
    BKVideo(var params = var());
    ~BKVideo();

    FileParameter* filePath;

    void clearItem() override;
    Colour getColourAtCoord(Point<float>* point) override;
    void onContainerParameterChanged(Parameter* p) override;
    
    String getTypeString() const override { return "VideoFile"; }
    static BKVideo* create(var params) { return new BKVideo(); }

    libvlc_instance_t* VLCInstance;
    libvlc_media_player_t* VLCMediaPlayer;
    libvlc_media_t* VLCMedia;

    void play(); 
    void stop();

    void run() override;
    void threadLoop();

    static void* lock(void* opaque, void** planes);
    static void unlock(void* data, void* id, void* const* pixelData);
    static void display(void* data, void* id);

    static unsigned setupVideo(void** opaque, char* chroma, unsigned* width, unsigned* height, unsigned* pitches, unsigned* lines);
    static void cleanVideo(void* opaque);

    //virtual MediaUI* createUI() {return new BKVideo(); };
};
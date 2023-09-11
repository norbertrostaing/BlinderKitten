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
struct libvlc_media_list_player_t;
struct libvlc_media_list_t;
struct libvlc_media_t;

class BKVideo :
    public Media,
    public Thread
{
public:
    BKVideo(var params = var());
    ~BKVideo();

    FileParameter* filePath;

    Trigger* startBtn;
    Trigger* stopBtn;
    Trigger* restartBtn;
    Trigger* pauseBtn;
    FloatParameter* mediaVolume;

    void clearItem() override;
    Colour getColourAtCoord(Point<float>* point) override;
    void onContainerParameterChanged(Parameter* p) override;
    void triggerTriggered(Trigger* t);
    
    String getTypeString() const override { return "VideoFile"; }
    static BKVideo* create(var params) { return new BKVideo(); }

    libvlc_instance_t* VLCInstance = nullptr;
    libvlc_media_player_t* VLCMediaPlayer = nullptr;
    libvlc_media_list_player_t* VLCMediaListPlayer = nullptr;
    libvlc_media_list_t* VLCMediaList = nullptr;
    libvlc_media_t* VLCMedia = nullptr;

    int imageWidth = 0;
    int imageHeight = 0;
    int imagePitches = 0;
    int imageLines = 0;
    uint32_t* vlcData;
    bool vlcDataIsValid = false;
    CriticalSection useImageData;

    void play(); 
    void stop();

    void run() override;
    void threadLoop();

    void* lock(void** pixels);
    static void* lock(void* self, void** pixels) {  return static_cast<BKVideo*>(self)->lock(pixels);   };

    void unlock(void* oldBuffer, void* const* pixels);
    static void unlock(void* self, void* oldBuffer, void* const* pixels) { static_cast<BKVideo*>(self)->unlock(oldBuffer, pixels); };

    void display(void* nextBuffer);
    static void display(void* self, void* nextBuffer) { static_cast<BKVideo*>(self)->display(nextBuffer); };

    unsigned setup_video(char* chroma, unsigned* width, unsigned* height, unsigned* pitches, unsigned* lines);
    static unsigned setup_video(void** self, char* chroma, unsigned* width, unsigned* height, unsigned* pitches, unsigned* lines) {
        return static_cast<BKVideo*>(*self)->setup_video(chroma, width, height, pitches, lines);
    }

    void cleanup_video();
    static void cleanup_video(void* self) {
        static_cast<BKVideo*>(self)->cleanup_video();
    }

    //virtual MediaUI* createUI() {return new BKVideo(); };
};
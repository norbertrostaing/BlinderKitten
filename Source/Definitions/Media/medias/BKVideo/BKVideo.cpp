/*
  ==============================================================================

	BKVideo.cpp
	Created: 26 Sep 2020 1:51:42pm
	Author:  bkupe

  ==============================================================================
*/

#include "BKVideo.h"
#include "vlcpp/vlc.hpp"

BKVideo::BKVideo(var params) :
	Media(params),
    Thread("BKVideoFileMedia")
{
	filePath = addFileParameter("File path", "File path", "");

    startBtn = addTrigger("start", "");
    stopBtn = addTrigger("stop", "");
    restartBtn = addTrigger("restart", "");
    pauseBtn = addTrigger("pause", "");

    mediaVolume = addFloatParameter("Volume", "Media volume", 1, 0, 1);
    seek = addFloatParameter("Seek", "Manual seek", 1, 0, 1);
    seek->isSavable = false;
    pixelsAround = addIntParameter("Pixels around", "number of pixels to look around the targeted pixel. To have a medium value.",0,0);

    const char* argv[1] = { "-vvv" };
    VLCInstance = libvlc_new(1, argv);
}

BKVideo::~BKVideo()
{
    stop();
    if (VLCMediaListPlayer != nullptr) { libvlc_media_list_player_release(VLCMediaListPlayer); VLCMediaListPlayer = nullptr;}
    if (VLCMediaList != nullptr) { libvlc_media_list_release(VLCMediaList); VLCMediaList = nullptr; }
    if (VLCMediaPlayer != nullptr) {
        libvlc_event_detach(libvlc_media_player_event_manager(VLCMediaPlayer), libvlc_MediaPlayerPositionChanged, vlcSeek, this);
        libvlc_media_player_release(VLCMediaPlayer); VLCMediaPlayer = nullptr;
    }
    if (VLCMedia != nullptr) {  libvlc_media_release(VLCMedia);VLCMedia = nullptr; }
    libvlc_release(VLCInstance); VLCInstance = nullptr;
}

void BKVideo::clearItem()
{
    BaseItem::clearItem();
}

Colour BKVideo::getColourAtCoord(Point<float>* point)
{
    int delta = pixelsAround->intValue();
    useImageData.enter();
    int w = imageWidth;
    int h = imageHeight;

    int r = 0;
    int g = 0;
    int b = 0;
    int number = 0;

    if (vlcDataIsValid && w > 0 && h > 0 && abs(point->x) <= 1 && abs(point->y) <= 1) {
        int x = jmap(point->x, -1.0f, 1.0f, 0.0f, 1.0f) * w;
        int y = jmap(point->y, -1.0f, 1.0f, 0.0f, 1.0f) * h;

        for (int dx = -delta; dx <= delta; dx++) {
            for (int dy = -delta; dy <= delta; dy++) {
                int localX = x + dx;
                int localY = y + dy;
                if (localX >= 0 && localX < w && localY >= 0 && localY < h) {
                    int index = localX + (imageWidth * localY);

                    uint32_t pixel = vlcData[index];
                    //auto pixel = packed[x];
                    uint8_t blue = pixel;
                    uint8_t green = pixel >> 8;
                    uint8_t red = pixel >> 16;
                    uint8_t alpha = pixel >> 24;

                    r += red;
                    g += green;
                    b += blue;
                    number++;
                }
            }
        }
        useImageData.exit();
        r /= number;
        g /= number;
        b /= number;
        return Colour(r, g, b);

        //return Colour(*vlcData[index], *vlcData[index+1], *vlcData[index+2]);
        //return Colour(0, 0, 0);
    }
    else {
        useImageData.exit();
        return Colour(0, 0, 0);
    }
}

void BKVideo::onContainerParameterChanged(Parameter* p)
{
    // LIBVLC_API int libvlc_audio_set_volume( libvlc_media_player_t *p_mi, int i_volume );

    if (p == filePath) {
        String f = filePath->getFile().getFullPathName();
        stop();
        VLCMediaList = libvlc_media_list_new(VLCInstance);
        VLCMedia = libvlc_media_new_path(VLCInstance, f.toRawUTF8());
        libvlc_media_list_add_media(VLCMediaList, VLCMedia);

        VLCMediaPlayer = libvlc_media_player_new(VLCInstance); //libvlc_media_player_new_from_media(VLCMedia);
        libvlc_video_set_format_callbacks(VLCMediaPlayer, setup_video, cleanup_video);
        libvlc_video_set_callbacks(VLCMediaPlayer, lock, unlock, display, this);

        VLCMediaListPlayer = libvlc_media_list_player_new(VLCInstance);

        libvlc_media_list_player_set_media_list(VLCMediaListPlayer, VLCMediaList);
        libvlc_media_list_player_set_media_player(VLCMediaListPlayer, VLCMediaPlayer);
        libvlc_media_list_player_set_playback_mode(VLCMediaListPlayer, libvlc_playback_mode_loop);

        libvlc_event_attach(libvlc_media_player_event_manager(VLCMediaPlayer), libvlc_MediaPlayerPositionChanged, vlcSeek, this);
        libvlc_media_player_play(VLCMediaPlayer);

        libvlc_media_release(VLCMedia); VLCMedia = nullptr;
        play();
    }
    else if (p == mediaVolume) {
        int v = mediaVolume->floatValue()*100;
        libvlc_audio_set_volume(VLCMediaPlayer, v);
    }
    else if (p == seek) {
        if (!vlcSeekedLast) {
            libvlc_media_player_set_position(VLCMediaPlayer, seek->floatValue());
        }
        vlcSeekedLast = false;
    }
}

void BKVideo::triggerTriggered(Trigger* t)
{
    if (t == startBtn) {
        play();
    }
    else if (t == stopBtn) {
        stop();
    }
    else if (t == restartBtn) {
        stop();
        play();
    }
    else if (t == pauseBtn) {
        libvlc_media_list_player_pause(VLCMediaListPlayer);
    }

}

void BKVideo::play()
{
    if (VLCMediaPlayer != nullptr) {
        libvlc_media_list_player_play(VLCMediaListPlayer);
    }
}

void BKVideo::stop()
{
    if (VLCMediaPlayer != nullptr) {
        libvlc_media_list_player_stop(VLCMediaListPlayer);
    }
}

void BKVideo::run()
{
}

void BKVideo::threadLoop()
{
}

void* BKVideo::lock(void** pixels)
{
    useImageData.enter();
    pixels[0] = vlcData;
    //vlcData = pixels;
    return vlcData;
}

void BKVideo::unlock(void* oldBuffer, void* const* pixels)
{
    useImageData.exit();
}

void BKVideo::display(void* nextBuffer)
{
    //LOG("display");
    //LOG(String(imageLines)+" "+String(imagePitches));
    //vlcData = nextBuffer;
}

unsigned BKVideo::setup_video(char* chroma, unsigned* width, unsigned* height, unsigned* pitches, unsigned* lines)
{
    //LOG("Hello ");
    imageWidth = *width;
    imageHeight = *height;
    imagePitches = *pitches;
    imageLines = *lines;

    useImageData.enter();
    vlcData = (uint32_t*)malloc(imageWidth * imageHeight * sizeof(uint32_t));
    vlcDataIsValid = true;

    // setup vlc
    memcpy(chroma, "RV32", 4);
    (*pitches) = imageWidth * 4;
    (*lines) = imageHeight;
    useImageData.exit();

    //LOG(String(imageLines) + " " + String(imagePitches));
    return 1;
}

void BKVideo::cleanup_video()
{
    vlcDataIsValid = false;
    free(vlcData);
    //LOG("cleanup_video");

}

void BKVideo::vlcSeek()
{
    float pos = libvlc_media_player_get_position(VLCMediaPlayer);
    vlcSeekedLast = true;
    seek->setValue(pos);
}



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
    const char* argv[1] = { "-vvv" };
    VLCInstance = libvlc_new(1, argv);
}

BKVideo::~BKVideo()
{
    if (VLCMediaPlayer != nullptr) {
        libvlc_media_player_release(VLCMediaPlayer);
    }
    libvlc_release(VLCInstance);
}

void BKVideo::clearItem()
{
	BaseItem::clearItem();
}

Colour BKVideo::getColourAtCoord(Point<float>* point)
{
    int w = imageWidth;
    int h = imageHeight;
    if (w > 0 && h > 0 && abs(point->x) <= 1 && abs(point->y) <= 1) {
        int x = jmap(point->x, -1.0f, 1.0f, 0.0f, 1.0f) * w;
        int y = jmap(point->y, -1.0f, 1.0f, 0.0f, 1.0f) * h;
        int index = x + (imageWidth*y);

        uint32_t pixel = vlcData[index];
        //auto pixel = packed[x];
        uint8_t red = pixel;
        uint8_t green = pixel << 8;
        uint8_t blue = pixel << 16;
        uint8_t alpha = pixel << 24;

        return Colour(red, green, blue);
        
        //return Colour(*vlcData[index], *vlcData[index+1], *vlcData[index+2]);
        return Colour(0, 0, 0);
    }
    else {
        return Colour(0, 0, 0);
    }
}

void BKVideo::onContainerParameterChanged(Parameter* p)
{

    if (p == filePath) {
        String f = filePath->getFile().getFullPathName();
        VLCMedia = libvlc_media_new_path(VLCInstance, f.toRawUTF8());
        //VLCMedia = libvlc_media_new_location(VLCInstance, f.toRawUTF8());
        VLCMediaPlayer = libvlc_media_player_new_from_media(VLCMedia);
    
        // libvlc_video_set_callbacks(); ---- https://cpp.hotexamples.com/fr/examples/-/-/libvlc_video_set_format_callbacks/cpp-libvlc_video_set_format_callbacks-function-examples.html
        //libvlc_video_lock_cb;
        //libvlc_video_cleanup_cb;

        libvlc_video_set_format_callbacks(VLCMediaPlayer, setup_video, cleanup_video);
        libvlc_video_set_callbacks(VLCMediaPlayer, lock, unlock, display, this);
        //libvlc_video_set_callbacks(VLCMediaPlayer, lock, unlock, display, this);
        libvlc_media_release(VLCMedia);
        play();
    }

#if 0
    /* This is a non working code that show how to hooks into a window,
     * if we have a window around */
    libvlc_media_player_set_xwindow(mp, xid);
    /* or on windows */
    libvlc_media_player_set_hwnd(mp, hwnd);
    /* or on mac os */
    libvlc_media_player_set_nsobject(mp, view);
#endif

    /* play the media_player */

    //sleep(10); /* Let it play a bit */

    /* Stop playing */

}

void BKVideo::play()
{
    if (VLCMediaPlayer != nullptr) {
        libvlc_media_player_play(VLCMediaPlayer);
    }
}

void BKVideo::stop()
{
    if (VLCMediaPlayer != nullptr) {
        libvlc_media_player_stop(VLCMediaPlayer);
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

    vlcData = (uint32_t*)malloc(imageWidth * imageHeight * sizeof(uint32_t));

    // setup vlc
    memcpy(chroma, "RV32", 4);
    (*pitches) = imageWidth * 4;
    (*lines) = imageHeight;

    //LOG(String(imageLines) + " " + String(imagePitches));
    return 1;
}

void BKVideo::cleanup_video()
{
    free(vlcData);
    //LOG("cleanup_video");

}



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
	return Colour(0,0,0);
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
        libvlc_video_cleanup_cb;

        libvlc_video_set_callbacks(VLCMediaPlayer, NULL, unlock, display, this);
        libvlc_video_set_format_callbacks(VLCMediaPlayer, setupVideo, cleanVideo);
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



void* BKVideo::lock(void* opaque, void** planes)
{
    //LOG("oui");
    /*VideoSource* _this = static_cast<VideoSource*>(data);

    *pixelData = _this->pixelData;

    EnterCriticalSection(&_this->textureLock);
    */
    return 0;
}

void BKVideo::unlock(void* data, void* id, void* const* pixelData)
{
    LOG("oui");
    /*
    VideoSource* _this = static_cast<VideoSource*>(data);

    if (_this->isInScene) {
        _this->GetTexture()->SetImage(*pixelData, GS_IMAGEFORMAT_BGRA, _this->GetTexture()->Width() * 4);
    }

    LeaveCriticalSection(&_this->textureLock);

    assert(id == NULL); // picture identifier, not needed here 
    */
}

void BKVideo::display(void* opaque, void* picture)
{
    LOG("oui");
}

unsigned BKVideo::setupVideo(void** opaque, char* chroma, unsigned* width, unsigned* height, unsigned* pitches, unsigned* lines)
{
    LOG("ok");
    return 1;
}

void BKVideo::cleanVideo(void* opaque)
{
    LOG("meh");
}


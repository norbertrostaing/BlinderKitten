#include "Media.h"
#include "MediaManager.h"

#include "Media/medias/SolidColor/SolidColor.h"
#include "Media/medias/BKImage/BKImage.h"
#include "Media/medias/BKVideo/BKVideo.h"
#include "Media/medias/BKNDI/BKNDI.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(MediaManager);

int compare(Media* A, Media* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}

MediaManager::MediaManager() :
    BaseManager("Media")
{
    managerFactory = &factory;

    factory.defs.add(Factory<Media>::Definition::createDef("", "SolidColor", &SolidColor::create));
    factory.defs.add(Factory<Media>::Definition::createDef("", "Image", &BKImage::create));
    factory.defs.add(Factory<Media>::Definition::createDef("", "VideoFile", &BKVideo::create));
    factory.defs.add(Factory<Media>::Definition::createDef("", "NDI", &BKNDI::create));

    itemDataType = "Media";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;
    autoReorderOnAdd = true;
}

MediaManager::~MediaManager()
{
    // stopThread(1000);
}


void MediaManager::addItemInternal(Media* o, var data)
{
    reorderItems();
}

void MediaManager::removeItemInternal(Media* o)
{
}

void MediaManager::onContainerParameterChanged(Parameter* p)
{
}


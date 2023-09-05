/*
  ==============================================================================

	BKVideo.cpp
	Created: 26 Sep 2020 1:51:42pm
	Author:  bkupe

  ==============================================================================
*/

#include "BKVideo.h"

BKVideo::BKVideo(var params) :
	Media(params)
{
	filePath = addFileParameter("File path", "File path", "");
}

BKVideo::~BKVideo()
{
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
}


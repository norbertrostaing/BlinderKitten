/*
  ==============================================================================

	BKImage.cpp
	Created: 26 Sep 2020 1:51:42pm
	Author:  bkupe

  ==============================================================================
*/

#include "BKImage.h"

BKImage::BKImage(var params) :
	Media(params)
{
	filePath = addFileParameter("File path", "File path", "");
}

BKImage::~BKImage()
{
}

void BKImage::clearItem()
{
	BaseItem::clearItem();
}

Colour BKImage::getColourAtCoord(Point<float>* point)
{
	int w = imageContent.getWidth();
	int h = imageContent.getHeight();
	if (w > 0 && h > 0 && abs(point->x) <= 1 && abs(point->y) <= 1){
		int x = jmap(point->x, -1.0f, 1.0f, 0.0f, 1.0f) * w;
		int y = jmap(point->y, -1.0f, 1.0f, 0.0f, 1.0f) * h;
		return imageContent.getPixelAt(x, y);
	}
	else {
		return Colour(0, 0, 0);
	}
}

void BKImage::onContainerParameterChanged(Parameter* p)
{
	Media::onContainerParameterChanged(p);
	if (p == filePath) {
		File target = filePath->getFile();
		if (target.existsAsFile() && target.hasFileExtension("jpg;jpeg;png")) {
			String ext = target.getFileExtension();
			imageContent = ImageFileFormat::loadFrom(target);
		}
	}
}


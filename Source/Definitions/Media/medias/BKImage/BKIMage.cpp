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

Colour BKImage::getColourAtCoord(Point<float>* point, int pixelsAround)
{
	int w = image.getWidth();
	int h = image.getHeight();
	if (w > 0 && h > 0 && abs(point->x) <= 1 && abs(point->y) <= 1){
        int x = jmap(point->x, -1.0f, 1.0f, 0.0f, 1.0f) * w;
        int y = jmap(point->y, -1.0f, 1.0f, 0.0f, 1.0f) * h;
        int r = 0;
        int g = 0;
        int b = 0;
        int number = 0;

        for (int dx = -pixelsAround; dx <= pixelsAround; dx++) {
            for (int dy = -pixelsAround; dy <= pixelsAround; dy++) {
                int localX = x + dx;
                int localY = y + dy;
                if (localX >= 0 && localX < w && localY >= 0 && localY < h) {
                    Colour pixel = image.getPixelAt(localX, localY);

                    r += pixel.getRed();
                    g += pixel.getGreen();
                    b += pixel.getBlue();
                    number++;
                }
            }
        }
        useImageData.exit();
        if (number > 0) {
            r /= number;
            g /= number;
            b /= number;
            return Colour(r, g, b);
        }
        else {
            return Colour(0, 0, 0);
        }
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
			image = ImageFileFormat::loadFrom(target);
		}
	}
}


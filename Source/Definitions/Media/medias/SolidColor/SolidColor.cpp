/*
  ==============================================================================

	SolidColor.cpp
	Created: 26 Sep 2020 1:51:42pm
	Author:  bkupe

  ==============================================================================
*/

#include "SolidColor.h"

SolidColor::SolidColor(var params) :
	Media(params)
{
	color = addColorParameter("Color", "", Colour(255,0,0));
}

SolidColor::~SolidColor()
{
}

void SolidColor::clearItem()
{
	BaseItem::clearItem();
}

Colour SolidColor::getColourAtCoord(Point<float>* point, int pixelsAround)
{
	if (abs(point->x) <= 1 && abs(point->y) <= 1) {
		return color->getColor();
	}
	return Colour(0,0,0);

}

void SolidColor::onContainerParameterChanged(Parameter* p)
{
	Media::onContainerParameterChanged(p);
}


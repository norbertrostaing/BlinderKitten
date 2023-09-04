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

Colour SolidColor::getColourAtCoord(Point<float>* point)
{
	return color->getColor();
}

void SolidColor::onContainerParameterChanged(Parameter* p)
{
}


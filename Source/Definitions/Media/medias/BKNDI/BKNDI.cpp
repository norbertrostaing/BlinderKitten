/*
  ==============================================================================

	BKNDI.cpp
	Created: 26 Sep 2020 1:51:42pm
	Author:  bkupe

  ==============================================================================
*/

#include "BKNDI.h"

BKNDI::BKNDI(var params) :
	Media(params)
{
	color = addColorParameter("Color", "", Colour(255,0,0));
}

BKNDI::~BKNDI()
{
}

void BKNDI::clearItem()
{
	BaseItem::clearItem();
}

Colour BKNDI::getColourAtCoord(Point<float>* point)
{
	return color->getColor();
}

void BKNDI::onContainerParameterChanged(Parameter* p)
{
}


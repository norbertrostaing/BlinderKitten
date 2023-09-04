/*
  ==============================================================================

    CVMediaManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Media.h"
#include "MediaManager.h"

class MediaManagerUI :
	public BaseManagerShapeShifterUI<MediaManager, Media, MediaUI>
{
public:
	MediaManagerUI(const String &contentName);
	~MediaManagerUI();

	static MediaManagerUI * create(const String &name) { return new MediaManagerUI(name); }
};


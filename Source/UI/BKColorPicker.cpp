/*
  ==============================================================================

    BKColorPicker.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "BKColorPicker.h"
#include "UserInputManager.h"
#include "BKEngine.h"
//#include "Definitions/Command/Command.h"


//==============================================================================
BKColorPickerUI::BKColorPickerUI(const String& contentName):
    ShapeShifterContent(BKColorPicker::getInstance(), contentName)
{
    
}

BKColorPickerUI::~BKColorPickerUI()
{
}

juce_ImplementSingleton(BKColorPicker);

BKColorPicker::BKColorPicker()
{
    //addAndMakeVisible(background);
    //background.setImage(ImageCache::getFromMemory(BinaryData::colorpicker_png, BinaryData::colorpicker_pngSize));
}

BKColorPicker::~BKColorPicker()
{
}

void BKColorPicker::buttonClicked(juce::Button* button) {
}

void BKColorPicker::paint (juce::Graphics& g)
{
    //background.setBounds(getLocalBounds());
    Rectangle img = getLocalBounds();
    img.reduce(margin, margin);
    g.drawImage(ImageCache::getFromMemory(BinaryData::colorpicker_png, BinaryData::colorpicker_pngSize), img.toFloat());
    
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

}

void BKColorPicker::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}

void BKColorPicker::sliderValueChanged(Slider* slider)
{

}

void BKColorPicker::mouseDown(const MouseEvent& e)
{
    mouseSetColor(e.position.getX(), e.position.getY());
}

void BKColorPicker::mouseDrag(const MouseEvent& e)
{
    mouseSetColor(e.position.getX(), e.position.getY());
}

void BKColorPicker::mouseSetColor(double x, double y)
{
    x = x - margin;
    y = y - margin;

    double maxX = getLocalBounds().getWidth() - (2*margin);
    double maxY = getLocalBounds().getHeight() - (2*margin);

    x = jmax(0., x);
    y = jmax(0., y);

    x = jmin(maxX, x);
    y = jmin(maxY, y);
    

    x = x / (getLocalBounds().getWidth() - (2 * margin));
    y = y / (getLocalBounds().getHeight() - (2 * margin));

    double x6 = 6*x;

    double r = 0;
    double g = 0;
    double b = 0;
    
    if (x6 < 1) {
        r = 1;
        g = jmap(x6, 0., 1., 0., 1.);
    }
    else if (x6 < 2) {
        g = 1;
        r = jmap(x6, 1., 2., 1., 0.);
    }
    else if (x6 < 3) {
        g = 1;
        b = jmap(x6, 2., 3., 0., 1.);
    }
    else  if (x6 < 4) {
        b = 1;
        g = jmap(x6, 3., 4., 1., 0.);
    }
    else if (x6 < 5) {
        b = 1;
        r = jmap(x6, 4., 5., 0., 1.);
    }
    else  {
        r = 1;
        b = jmap(x6, 5., 6., 1., 0.);
    }

    r = jmap(r, 0., 1., y, 1.);
    g = jmap(g, 0., 1., y, 1.);
    b = jmap(b, 0., 1., y, 1.);

    ChannelType* redChannel = dynamic_cast<ChannelType*>(engine->CPRedChannel->targetContainer.get());
    ChannelType* greenChannel = dynamic_cast<ChannelType*>(engine->CPGreenChannel->targetContainer.get());
    ChannelType* blueChannel = dynamic_cast<ChannelType*>(engine->CPBlueChannel->targetContainer.get());
    ChannelType* cyanChannel = dynamic_cast<ChannelType*>(engine->CPCyanChannel->targetContainer.get());
    ChannelType* magentaChannel = dynamic_cast<ChannelType*>(engine->CPMagentaChannel->targetContainer.get());
    ChannelType* yellowChannel = dynamic_cast<ChannelType*>(engine->CPYellowChannel->targetContainer.get());

    UserInputManager::getInstance()->changeChannelValue(redChannel, r);
    UserInputManager::getInstance()->changeChannelValue(greenChannel, g);
    UserInputManager::getInstance()->changeChannelValue(blueChannel, b);
    UserInputManager::getInstance()->changeChannelValue(cyanChannel, 1-r);
    UserInputManager::getInstance()->changeChannelValue(magentaChannel, 1-g);
    UserInputManager::getInstance()->changeChannelValue(yellowChannel, 1-b);

    //LOG("color : "+ String(r) + " " + String(g) + " " + String(b) + " ");


}

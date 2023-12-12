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
    
    Command* c = UserInputManager::getInstance()->targetCommand;
    if (c == nullptr) return;
    float red = -1;
    float green = -1;
    float blue = -1;
    float cyan = -1;
    float magenta = -1;
    float yellow = -1;
    float hue = -1;
    float saturation = -1;

    for (int i = 0; i < c->values.items.size(); i++) {
        CommandValue* cv = c->values.items[i];
        if (cv->channelType->getValue() == engine->CPRedChannel->getValue()) {red = cv->valueFrom->getValue();}
        if (cv->channelType->getValue() == engine->CPGreenChannel->getValue()) {green = cv->valueFrom->getValue();}
        if (cv->channelType->getValue() == engine->CPBlueChannel->getValue()) {blue = cv->valueFrom->getValue();}
        if (cv->channelType->getValue() == engine->CPCyanChannel->getValue()) {cyan = cv->valueFrom->getValue();}
        if (cv->channelType->getValue() == engine->CPMagentaChannel->getValue()) {magenta = cv->valueFrom->getValue();}
        if (cv->channelType->getValue() == engine->CPYellowChannel->getValue()) {yellow = cv->valueFrom->getValue();}
        if (cv->channelType->getValue() == engine->CPHueChannel->getValue()) {hue = cv->valueFrom->getValue();}
        if (cv->channelType->getValue() == engine->CPSaturationChannel->getValue()) {saturation = cv->valueFrom->getValue();}
    }

    if (red != -1 && green != -1 && blue != -1) {
        Colour c = Colour(red * 255, green * 255, blue * 255);
        float x = c.getHue();
        float y = 1 - c.getSaturation();
        x *= getWidth() - (2 * margin);
        y *= getHeight() - (2 * margin);
        g.drawEllipse(margin + x - 5, margin + y - 5, 10.0f, 10.0f, 1);
    }
    else if (cyan != -1 && magenta != -1 && yellow != -1) {
        Colour c = Colour((1 - cyan) * 255, (1 - magenta) * 255, (1-yellow) * 255);
        float x = c.getHue();
        float y = 1 - c.getSaturation();
        x *= getWidth() - (2 * margin);
        y *= getHeight() - (2 * margin);
        g.drawEllipse(margin + x - 5, margin + y - 5, 10.0f, 10.0f, 1);
    }
    else if (hue != -1 && saturation != -1) {
        float x = hue;
        float y = 1 - saturation;
        x *= getWidth() - (2 * margin);
        y *= getHeight() - (2 * margin);
        g.drawEllipse(margin + x - 5, margin + y - 5, 10.0f, 10.0f, 1);

    }
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

    if (dynamic_cast<ChannelType*>(engine->CPRedChannel->targetContainer.get()) == nullptr) {
        engine -> autoFillColorPickerValues();
    }

    if (engine->CPRedChannel->stringValue() != "") {
        ChannelType* redChannel = dynamic_cast<ChannelType*>(engine->CPRedChannel->targetContainer.get());
        UserInputManager::getInstance()->changeChannelValue(redChannel, r);
    }
    if (engine->CPGreenChannel->stringValue() != "") {
        ChannelType* greenChannel = dynamic_cast<ChannelType*>(engine->CPGreenChannel->targetContainer.get());
        UserInputManager::getInstance()->changeChannelValue(greenChannel, g);
    }
    if (engine->CPBlueChannel->stringValue() != "") {
        ChannelType* blueChannel = dynamic_cast<ChannelType*>(engine->CPBlueChannel->targetContainer.get());
        UserInputManager::getInstance()->changeChannelValue(blueChannel, b);
    }
    if (engine->CPCyanChannel->stringValue() != "") {
        ChannelType* cyanChannel = dynamic_cast<ChannelType*>(engine->CPCyanChannel->targetContainer.get());
        UserInputManager::getInstance()->changeChannelValue(cyanChannel, 1 - r);
    }
    if (engine->CPMagentaChannel->stringValue() != "") {
        ChannelType* magentaChannel = dynamic_cast<ChannelType*>(engine->CPMagentaChannel->targetContainer.get());
        UserInputManager::getInstance()->changeChannelValue(magentaChannel, 1 - g);
    }
    if (engine->CPYellowChannel->stringValue() != "") {
        ChannelType* yellowChannel = dynamic_cast<ChannelType*>(engine->CPYellowChannel->targetContainer.get());
        UserInputManager::getInstance()->changeChannelValue(yellowChannel, 1 - b);
    }
    if (engine->CPHueChannel->stringValue() != "") {
        ChannelType* hueChannel = dynamic_cast<ChannelType*>(engine->CPHueChannel->targetContainer.get());
        UserInputManager::getInstance()->changeChannelValue(hueChannel, x);
    }
    if (engine->CPSaturationChannel->stringValue() != "") {
        ChannelType* saturationChannel = dynamic_cast<ChannelType*>(engine->CPSaturationChannel->targetContainer.get());
        UserInputManager::getInstance()->changeChannelValue(saturationChannel, y);
    }
    
    
    
    
    
    
    
    

    //LOG("color : "+ String(r) + " " + String(g) + " " + String(b) + " ");


}

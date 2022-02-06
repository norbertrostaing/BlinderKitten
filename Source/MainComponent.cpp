#include "MainComponent.h"
#include "BKEngine.h"

#include "Definitions/Interface/ui/InterfaceManagerUI.h"
#include "Definitions/FixtureParamType/FixtureParamTypeManagerUI.h"
#include "Definitions/DeviceType/DeviceTypeManagerUI.h"
#include "Definitions/Device/DeviceManagerUI.h"
#include "Definitions/Fixture/FixtureManagerUI.h"
#include "Definitions/Group/GroupManagerUI.h"
#include "Definitions/Preset/PresetManagerUI.h"
#include "Definitions/Command/CommandManagerUI.h"
#include "Definitions/Cuelist/CuelistManagerUI.h"
#include "Definitions/Programmer/ProgrammerManagerUI.h"


//==============================================================================
MainContentComponent::MainContentComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    // setSize (800, 600);
}

MainContentComponent::~MainContentComponent()
{
    // This shuts down the audio device and clears the audio source.
    //shutdownAudio();
}

//==============================================================================
/*
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
*/
void MainContentComponent::init()
{
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Interfaces", &InterfaceManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Fixture Type Params", &FixtureParamTypeManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Device Types", &DeviceTypeManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Devices", &DeviceManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Fixtures", &FixtureManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Groups", &GroupManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Presets", &PresetManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Commands", &CommandManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Cuelists", &CuelistManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Programmers", &ProgrammerManagerUI::create));
    OrganicMainContentComponent::init();

}

#include "JuceHeader.h"
#include "MainComponent.h"
#include "BKEngine.h"

#include "Definitions/Interface/ui/InterfaceManagerUI.h"
#include "Definitions/ChannelFamily/ChannelFamilyManagerUI.h"
#include "Definitions/FixtureType/FixtureTypeManagerUI.h"
#include "Definitions/Fixture/FixtureManagerUI.h"
#include "Definitions/Group/GroupManagerUI.h"
#include "Definitions/Preset/PresetManagerUI.h"
#include "Definitions/Command/CommandManagerUI.h"
#include "Definitions/Cuelist/CuelistManagerUI.h"
#include "Definitions/Programmer/ProgrammerManagerUI.h"
#include "Definitions/CurvePreset/CurvePresetManagerUI.h"
#include "Definitions/TimingPreset/TimingPresetManagerUI.h"
#include "Definitions/Effect/EffectManagerUI.h"
#include "Definitions/Carousel/CarouselManagerUI.h"
#include "Definitions/Mapper/MapperManagerUI.h"
#include "Definitions/Tracker/TrackerManagerUI.h"
#include "Definitions/Multiplicator/MultiplicatorManagerUI.h"
#include "Definitions/Layout/LayoutManagerUI.h"
#include "UI/CommandLine.h"
#include "UI/Encoders.h"
#include "UI/EncodersMult/EncodersMultUI.h"
#include "UI/InputPanel.h"

#include "UI/GridView/FixtureGridView.h"
#include "UI/GridView/GroupGridView.h"
#include "UI/GridView/PresetGridView.h"
#include "UI/GridView/CuelistGridView.h"
#include "UI/GridView/EffectGridView.h"
#include "UI/GridView/CarouselGridView.h"
#include "UI/GridView/MapperGridView.h"

#include "UI/VirtualButtons/VirtualButtonManagerUI.h"
#include "UI/VirtualButtons/VirtualButtonGrid.h"
#include "UI/VirtualFaders/VirtualFaderColGrid.h"
#include "UI/VirtualFaders/VirtualFaderColManagerUI.h"

#include "UI/ConductorInfos.h"
#include "UI/BKColorPicker.h"
#include "UI/DMXChannelView.h"
#include "UI/LayoutViewer.h"
#include "UI/CuelistSheet/CuelistSheet.h"

//==============================================================================
MainContentComponent::MainContentComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    // setSize (800, 600);
    getCommandManager().registerAllCommandsForTarget(this);
}

MainContentComponent::~MainContentComponent()
{
    // This shuts down the audio Fixture and clears the audio source.
    //shutdownAudio();
}

//==============================================================================
/*
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio Fixture is started, or when
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
    // This will be called when the audio Fixture stops, or when it is being
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
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Channels config", &ChannelFamilyManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Fixture Types", &FixtureTypeManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Fixtures", &FixtureManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Groups", &GroupManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Presets", &PresetManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Timing Presets", &TimingPresetManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Curve Presets", &CurvePresetManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Cuelists", &CuelistManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Programmers", &ProgrammerManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Effects", &EffectManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Carousels", &CarouselManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Mappers", &MapperManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Trackers", &TrackerManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Multiplicators", &MultiplicatorManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Layouts", &LayoutManagerUI::create));

    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Encoders", &EncodersUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Encoders Overview", &EncodersMultUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Input Panel", &InputPanelUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Fixture Grid", &FixtureGridViewUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Group Grid", &GroupGridViewUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Preset Grid", &PresetGridViewUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Cuelist Grid", &CuelistGridViewUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Effect Grid", &EffectGridViewUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Carousel Grid", &CarouselGridViewUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Mapper Grid", &MapperGridViewUI::create));

    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Virtual buttons", &VirtualButtonManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Virtual buttons grid", &VirtualButtonGridUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Virtual faders", &VirtualFaderColManagerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Virtual faders grid", &VirtualFaderColGridUI::create));

    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Conductor Infos", &ConductorInfosUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Color Picker", &BKColorPickerUI::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("DMX Tester", &DMXChannelView::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Layout Viewer", &LayoutViewer::create));
    ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Cuelist sheet", &CuelistSheetUI::create));

    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Interfaces", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Channels config", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Fixture Types", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Fixtures", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Groups", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Presets", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Timing Presets", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Curve Presets", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Cuelists", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Programmers", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Effects", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Carousels", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Mappers", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Trackers", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Multiplicators", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Layouts", "Lists");

    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Encoders", "Panels");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Encoders Overview", "Panels");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Input Panel", "Panels");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Fixture Grid", "Grids");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Group Grid", "Grids");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Preset Grid", "Grids");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Cuelist Grid", "Grids");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Effect Grid", "Grids");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Carousel Grid", "Grids");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Mapper Grid", "Grids");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Virtual buttons", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Virtual buttons grid", "Playback");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Virtual faders", "Lists");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Virtual faders grid", "Playback");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Conductor Infos", "Panels");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Color Picker", "Panels");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("DMX Tester", "Panels");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Layout Viewer", "Panels");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Cuelist sheet", "Panels");

    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Outliner", "");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Dashboard", "");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Help", "");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("Parrots", "Organic Tools");
    ShapeShifterManager::getInstance()->isInViewSubMenu.set("The Detective", "Organic Tools");

    std::sort(ShapeShifterFactory::getInstance()->defs.begin(), ShapeShifterFactory::getInstance()->defs.end(),
        [](ShapeShifterDefinition* a, ShapeShifterDefinition* b) { return a->contentName < b->contentName; });

    OrganicMainContentComponent::init();

    //getLookAndFeel().setColour(juce::TextButton::textColourOffId, Colour(127,127,127));
    getLookAndFeel().setColour(juce::TextButton::buttonColourId, Colour(64,64,64));

}

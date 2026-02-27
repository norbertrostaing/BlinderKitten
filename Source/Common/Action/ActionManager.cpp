/*
  ==============================================================================

    ActionManager.cpp
    Created: 12 Oct 2020 10:55:26am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(ActionFactory);

#include "Common/CommonIncludes.h"

#include "Definitions/Actions/CuelistAction.h"
#include "Definitions/Actions/EffectAction.h"
#include "Definitions/Actions/CarouselAction.h"
#include "Definitions/Actions/MapperAction.h"
#include "Definitions/Actions/TrackerAction.h"
#include "Definitions/Actions/SelectionMasterAction.h"
#include "Definitions/Actions/VirtualButtonAction.h"
#include "Definitions/Actions/VirtualFaderAction.h"
#include "Definitions/Actions/InputPanelAction.h"
#include "Definitions/Actions/EncoderAction.h"
#include "Definitions/Actions/MultiplicatorAction.h"
#include "Definitions/Actions/TapTempoMultiple.h"
#include "Definitions/Actions/GenericAction.h"
#include "Definitions/Actions/BundleAction.h"
#include "Definitions/Actions/InterfaceAction.h"
#include "Definitions/Actions/PresetAction.h"
#include "Definitions/Actions/SoloPoolAction.h"
#include "Definitions/Actions/StampAction.h"
#include "ActionManager.h"

ActionFactory::ActionFactory()
{
    // defs.add(Factory<Action>::Definition::createDef("Scene", "Load Scene", &SceneAction::create)->addParam("actionType", SceneAction::LOAD_SCENE));
    //defs.add(Factory<Action>::Definition::createDef("Scene", "Load Next Scene", &SceneAction::create)->addParam("actionType", SceneAction::LOAD_NEXT_SCENE));
    //defs.add(Factory<Action>::Definition::createDef("Scene", "Load Previous Scene", &SceneAction::create)->addParam("actionType", SceneAction::LOAD_PREVIOUS_SCENE));

    //defs.add(Factory<Action>::Definition::createDef("Effect", "Enable Effect", &EffectAction::create)->addParam("actionType", EffectAction::ENABLE_EFFECT));
    //defs.add(Factory<Action>::Definition::createDef("Effect", "Disable Effect", &EffectAction::create)->addParam("actionType", EffectAction::DISABLE_EFFECT));
    //defs.add(Factory<Action>::Definition::createDef("Effect", "Set Effect Weight", &EffectAction::create)->addParam("actionType", EffectAction::SET_EFFECT_WEIGHT));

    //defs.add(Factory<Action>::Definition::createDef("Sequence", "Play Sequence", &SequenceAction::create)->addParam("actionType", SequenceAction::PLAY_SEQUENCE));
    //defs.add(Factory<Action>::Definition::createDef("Sequence", "Stop Sequence", &SequenceAction::create)->addParam("actionType", SequenceAction::STOP_SEQUENCE));

    //defs.add(Factory<Action>::Definition::createDef("Object", "Enable Object", &ObjectAction::create)->addParam("actionType", ObjectAction::ENABLE_OBJECT));
    //defs.add(Factory<Action>::Definition::createDef("Object", "Disable Object", &ObjectAction::create)->addParam("actionType", ObjectAction::DISABLE_OBJECT));
    //defs.add(Factory<Action>::Definition::createDef("Object", "Set Component Parameter", &ObjectAction::create)->addParam("actionType", ObjectAction::SET_COMPONENT_PARAMETER_VALUE));

    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Go All Loaded", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_GOALLLOADED));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Go", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_GO));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Go back", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_GOBACK));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Go instant", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_GOINSTANT));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Go back instant", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_GOBACKINSTANT));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Go random", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_GORANDOM));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Flash", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_FLASH));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Swop", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_SWOP));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Timed Flash", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_FLASHTIMED));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Timed Swop", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_SWOPTIMED));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Off", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_OFF));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Off Instant", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_OFF_INSTANT));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Toggle", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_TOGGLE));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Load", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_LOAD));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Load and Go", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_LOADANDGO));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "HTP Level", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_HTPLEVEL));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Flash Level", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_FLASHLEVEL));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "LTP Level", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_LTPLEVEL));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Chaser Speed", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_CHASERSPEED));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Chaser Tap tempo", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_CHASERTAPTEMPO));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Cross fade", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_CROSSFADE));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Up fade", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_UPFADE));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Down fade", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_DOWNFADE));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Load content", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_LOADCONTENT));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Insert before", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_INSERTBEFORE));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Insert after", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_INSERTAFTER));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Take selection", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_TAKESELECTION));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Go and add timecode", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_GO_ADD_TIMECODE));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Go and replace timecode", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_GO_REPLACE_TIMECODE));

    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Start", &EffectAction::create)->addParam("actionType", EffectAction::FX_START));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Stop", &EffectAction::create)->addParam("actionType", EffectAction::FX_STOP));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Stop Instant", &EffectAction::create)->addParam("actionType", EffectAction::FX_STOP_INSTANT));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Toggle", &EffectAction::create)->addParam("actionType", EffectAction::FX_TOGGLE));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Flash", &EffectAction::create)->addParam("actionType", EffectAction::FX_FLASH));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Swop", &EffectAction::create)->addParam("actionType", EffectAction::FX_SWOP));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Size", &EffectAction::create)->addParam("actionType", EffectAction::FX_SIZE));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Speed", &EffectAction::create)->addParam("actionType", EffectAction::FX_SPEED));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Double Speed", &EffectAction::create)->addParam("actionType", EffectAction::FX_DOUBLESPEED));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Half Speed", &EffectAction::create)->addParam("actionType", EffectAction::FX_HALFSPEED));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Buddy Block Wing", &EffectAction::create)->addParam("actionType", EffectAction::FX_BBW));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Set Seek", &EffectAction::create)->addParam("actionType", EffectAction::FX_SET_SEEK));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Add Seek", &EffectAction::create)->addParam("actionType", EffectAction::FX_ADD_SEEK));

    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Start", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_START));
    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Stop", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_STOP));
    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Stop Instant", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_STOP_INSTANT));
    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Toggle", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_TOGGLE));
    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Flash", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_FLASH));
    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Swop", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_SWOP));
    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Size", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_SIZE));
    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Speed", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_SPEED));
    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Double Speed", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_DOUBLESPEED));
    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Half Speed", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_HALFSPEED));
    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Buddy Block Wing", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_BBW));
    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Set Seek", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_SET_SEEK));
    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Add Seek", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_ADD_SEEK));

    defs.add(Factory<Action>::Definition::createDef("Mapper", "Mapper Start", &MapperAction::create)->addParam("actionType", MapperAction::TRK_START));
    defs.add(Factory<Action>::Definition::createDef("Mapper", "Mapper Stop", &MapperAction::create)->addParam("actionType", MapperAction::TRK_STOP));
    defs.add(Factory<Action>::Definition::createDef("Mapper", "Mapper Toggle", &MapperAction::create)->addParam("actionType", MapperAction::TRK_TOGGLE));
    defs.add(Factory<Action>::Definition::createDef("Mapper", "Mapper Size", &MapperAction::create)->addParam("actionType", MapperAction::TRK_SIZE));

    defs.add(Factory<Action>::Definition::createDef("Tracker", "Tracker Start", &TrackerAction::create)->addParam("actionType", TrackerAction::TRK_START));
    defs.add(Factory<Action>::Definition::createDef("Tracker", "Tracker Stop", &TrackerAction::create)->addParam("actionType", TrackerAction::TRK_STOP));
    defs.add(Factory<Action>::Definition::createDef("Tracker", "Tracker Toggle", &TrackerAction::create)->addParam("actionType", TrackerAction::TRK_TOGGLE));
    defs.add(Factory<Action>::Definition::createDef("Tracker", "Tracker Size", &TrackerAction::create)->addParam("actionType", TrackerAction::TRK_SIZE));
    defs.add(Factory<Action>::Definition::createDef("Tracker", "Tracker Axis", &TrackerAction::create)->addParam("actionType", TrackerAction::TRK_AXIS));

    defs.add(Factory<Action>::Definition::createDef("Stamp", "Stamp Start", &StampAction::create)->addParam("actionType", StampAction::ST_START));
    defs.add(Factory<Action>::Definition::createDef("Stamp", "Stamp Stop", &StampAction::create)->addParam("actionType", StampAction::ST_STOP));
    defs.add(Factory<Action>::Definition::createDef("Stamp", "Stamp Stop Instant", &StampAction::create)->addParam("actionType", StampAction::ST_STOP_INSTANT));
    defs.add(Factory<Action>::Definition::createDef("Stamp", "Stamp Toggle", &StampAction::create)->addParam("actionType", StampAction::ST_TOGGLE));
    defs.add(Factory<Action>::Definition::createDef("Stamp", "Stamp Flash", &StampAction::create)->addParam("actionType", StampAction::ST_FLASH));
    defs.add(Factory<Action>::Definition::createDef("Stamp", "Stamp Swop", &StampAction::create)->addParam("actionType", StampAction::ST_SWOP));
    defs.add(Factory<Action>::Definition::createDef("Stamp", "Stamp Size", &StampAction::create)->addParam("actionType", StampAction::ST_SIZE));


    defs.add(Factory<Action>::Definition::createDef("Selection Master", "Selection Master Start", &SelectionMasterAction::create)->addParam("actionType", SelectionMasterAction::SELM_START));
    defs.add(Factory<Action>::Definition::createDef("Selection Master", "Selection Master Stop", &SelectionMasterAction::create)->addParam("actionType", SelectionMasterAction::SELM_STOP));
    defs.add(Factory<Action>::Definition::createDef("Selection Master", "Selection Master Toggle", &SelectionMasterAction::create)->addParam("actionType", SelectionMasterAction::SELM_TOGGLE));
    defs.add(Factory<Action>::Definition::createDef("Selection Master", "Selection Master Size", &SelectionMasterAction::create)->addParam("actionType", SelectionMasterAction::SELM_SIZE));
    defs.add(Factory<Action>::Definition::createDef("Selection Master", "Selection Master Fixed Size", &SelectionMasterAction::create)->addParam("actionType", SelectionMasterAction::SELM_STATIC_SIZE));

    defs.add(Factory<Action>::Definition::createDef("Bundle", "Bundle Start", &BundleAction::create)->addParam("actionType", BundleAction::BUN_START));
    defs.add(Factory<Action>::Definition::createDef("Bundle", "Bundle Stop", &BundleAction::create)->addParam("actionType", BundleAction::BUN_STOP));
    defs.add(Factory<Action>::Definition::createDef("Bundle", "Bundle Stop Instant", &BundleAction::create)->addParam("actionType", BundleAction::BUN_STOP_INSTANT));
    defs.add(Factory<Action>::Definition::createDef("Bundle", "Bundle Size", &BundleAction::create)->addParam("actionType", BundleAction::BUN_SIZE));
    defs.add(Factory<Action>::Definition::createDef("Bundle", "Bundle Set Size", &BundleAction::create)->addParam("actionType", BundleAction::BUN_SET_SIZE));
    defs.add(Factory<Action>::Definition::createDef("Bundle", "Bundle Tap Tempo", &BundleAction::create)->addParam("actionType", BundleAction::BUN_TAPTEMPO));
    defs.add(Factory<Action>::Definition::createDef("Bundle", "Bundle Speed", &BundleAction::create)->addParam("actionType", BundleAction::BUN_SPEED));
    defs.add(Factory<Action>::Definition::createDef("Bundle", "Bundle Set Speed", &BundleAction::create)->addParam("actionType", BundleAction::BUN_SET_SPEED));
    defs.add(Factory<Action>::Definition::createDef("Bundle", "Bundle Double Speed", &BundleAction::create)->addParam("actionType", BundleAction::BUN_DOUBLESPEED));
    defs.add(Factory<Action>::Definition::createDef("Bundle", "Bundle Half Speed", &BundleAction::create)->addParam("actionType", BundleAction::BUN_HALFSPEED));
    defs.add(Factory<Action>::Definition::createDef("Bundle", "Bundle Flash", &BundleAction::create)->addParam("actionType", BundleAction::BUN_FLASH));
    defs.add(Factory<Action>::Definition::createDef("Bundle", "Bundle Swop", &BundleAction::create)->addParam("actionType", BundleAction::BUN_SWOP));
    defs.add(Factory<Action>::Definition::createDef("Bundle", "Bundle Timed Flash", &BundleAction::create)->addParam("actionType", BundleAction::BUN_TIMED_FLASH));
    defs.add(Factory<Action>::Definition::createDef("Bundle", "Bundle Timed Swop", &BundleAction::create)->addParam("actionType", BundleAction::BUN_TIMED_SWOP));

    defs.add(Factory<Action>::Definition::createDef("SoloPool", "SoloPool Stop", &SoloPoolAction::create)->addParam("actionType", SoloPoolAction::SOLP_STOP));
    defs.add(Factory<Action>::Definition::createDef("SoloPool", "SoloPool Random", &SoloPoolAction::create)->addParam("actionType", SoloPoolAction::SOLP_RANDOM));

    defs.add(Factory<Action>::Definition::createDef("Multiplicator", "Set Mult Value", &MultiplicatorAction::create)->addParam("actionType", MultiplicatorAction::MULT_SET));
    defs.add(Factory<Action>::Definition::createDef("Multiplicator", "Set Mult Static Value", &MultiplicatorAction::create)->addParam("actionType", MultiplicatorAction::MULT_SETAT));

    defs.add(Factory<Action>::Definition::createDef("VirtualButton", "Virtual Button Press", &VirtualButtonAction::create)->addParam("actionType", VirtualButtonAction::VB_PRESS));
    defs.add(Factory<Action>::Definition::createDef("VirtualButton", "Virtual Buttons Page Up", &VirtualButtonAction::create)->addParam("actionType", VirtualButtonAction::VB_PAGEUP));
    defs.add(Factory<Action>::Definition::createDef("VirtualButton", "Virtual Buttons Page Down", &VirtualButtonAction::create)->addParam("actionType", VirtualButtonAction::VB_PAGEDOWN));
    defs.add(Factory<Action>::Definition::createDef("VirtualButton", "Virtual Buttons Go To Page", &VirtualButtonAction::create)->addParam("actionType", VirtualButtonAction::VB_GOTOPAGE));

    defs.add(Factory<Action>::Definition::createDef("VirtualFader", "Encoder Move", &VirtualFaderAction::create)->addParam("actionType", VirtualFaderAction::VF_ROTARY));
    defs.add(Factory<Action>::Definition::createDef("VirtualFader", "Above Button Press", &VirtualFaderAction::create)->addParam("actionType", VirtualFaderAction::VF_ABOVEBUTTON));
    defs.add(Factory<Action>::Definition::createDef("VirtualFader", "Fader Move", &VirtualFaderAction::create)->addParam("actionType", VirtualFaderAction::VF_FADER));
    defs.add(Factory<Action>::Definition::createDef("VirtualFader", "Below Button Press", &VirtualFaderAction::create)->addParam("actionType", VirtualFaderAction::VF_BELOWBUTTON));
    defs.add(Factory<Action>::Definition::createDef("VirtualFader", "Virtual Faders Page Up", &VirtualFaderAction::create)->addParam("actionType", VirtualFaderAction::VF_PAGEUP));
    defs.add(Factory<Action>::Definition::createDef("VirtualFader", "Virtual Faders Page Down", &VirtualFaderAction::create)->addParam("actionType", VirtualFaderAction::VF_PAGEDOWN));
    defs.add(Factory<Action>::Definition::createDef("VirtualFader", "Virtual Faders Go To Page", &VirtualFaderAction::create)->addParam("actionType", VirtualFaderAction::VF_GOTOPAGE));

    defs.add(Factory<Action>::Definition::createDef("Input Panel", "Button Press", &InputPanelAction::create)->addParam("actionType", InputPanelAction::IP_PRESS));
    defs.add(Factory<Action>::Definition::createDef("Input Panel", "Grand Master", &InputPanelAction::create)->addParam("actionType", InputPanelAction::IP_GM));
    defs.add(Factory<Action>::Definition::createDef("Input Panel", "Black out temp", &InputPanelAction::create)->addParam("actionType", InputPanelAction::IP_BLACKOUTTEMP));
    defs.add(Factory<Action>::Definition::createDef("Input Panel", "Black out toggle", &InputPanelAction::create)->addParam("actionType", InputPanelAction::IP_BLACKOUTTOGGLE));
    defs.add(Factory<Action>::Definition::createDef("Input Panel", "Off All cuelists", &InputPanelAction::create)->addParam("actionType", InputPanelAction::IP_OFFCL));
    defs.add(Factory<Action>::Definition::createDef("Input Panel", "Kill All Cuelists", &InputPanelAction::create)->addParam("actionType", InputPanelAction::IP_KILLCL));
    defs.add(Factory<Action>::Definition::createDef("Input Panel", "Stop All Effects", &InputPanelAction::create)->addParam("actionType", InputPanelAction::IP_STOPFX));
    defs.add(Factory<Action>::Definition::createDef("Input Panel", "Stop All Carousels", &InputPanelAction::create)->addParam("actionType", InputPanelAction::IP_STOPCAR));
    defs.add(Factory<Action>::Definition::createDef("Input Panel", "Reset random Seed", &InputPanelAction::create)->addParam("actionType", InputPanelAction::IP_RANDOMSEED));
    defs.add(Factory<Action>::Definition::createDef("Input Panel", "Load all cuelists in programmer", &InputPanelAction::create)->addParam("actionType", InputPanelAction::IP_LOADALLCUELISTS));
    defs.add(Factory<Action>::Definition::createDef("Input Panel", "Select Window", &InputPanelAction::create)->addParam("actionType", InputPanelAction::IP_SELECTWINDOW));
    defs.add(Factory<Action>::Definition::createDef("Input Panel", "Save", &InputPanelAction::create)->addParam("actionType", InputPanelAction::IP_SAVE));
    defs.add(Factory<Action>::Definition::createDef("Input Panel", "Use another", &InputPanelAction::create)->addParam("actionType", InputPanelAction::IP_USEANOTHER));
    defs.add(Factory<Action>::Definition::createDef("Input Panel", "Window layout change", &InputPanelAction::create)->addParam("actionType", InputPanelAction::IP_ORGANICLAYOUT));

    defs.add(Factory<Action>::Definition::createDef("Encoder", "Encoder Value", &EncoderAction::create)->addParam("actionType", EncoderAction::ENC_VALUE));
    defs.add(Factory<Action>::Definition::createDef("Encoder", "Encoder Value Parameter", &EncoderAction::create)->addParam("actionType", EncoderAction::ENC_TYPE));
    defs.add(Factory<Action>::Definition::createDef("Encoder", "Encoder Reset", &EncoderAction::create)->addParam("actionType", EncoderAction::ENC_RESET));
    defs.add(Factory<Action>::Definition::createDef("Encoder", "Encoder Select", &EncoderAction::create)->addParam("actionType", EncoderAction::ENC_SELECT));
    defs.add(Factory<Action>::Definition::createDef("Encoder", "Encoder select next command", &EncoderAction::create)->addParam("actionType", EncoderAction::ENC_NEXTCOMMAND));
    defs.add(Factory<Action>::Definition::createDef("Encoder", "Encoder select previous command", &EncoderAction::create)->addParam("actionType", EncoderAction::ENC_PREVCOMMAND));
    defs.add(Factory<Action>::Definition::createDef("Encoder", "Encoder toggle filter nth", &EncoderAction::create)->addParam("actionType", EncoderAction::ENC_TOGGLEFILTERNUM));
    defs.add(Factory<Action>::Definition::createDef("Encoder", "Encoder toggle filter family", &EncoderAction::create)->addParam("actionType", EncoderAction::ENC_TOGGLEFILTERFAMILY));
    defs.add(Factory<Action>::Definition::createDef("Encoder", "Encoder clear filters", &EncoderAction::create)->addParam("actionType", EncoderAction::ENC_CLEARFILTERS));
    defs.add(Factory<Action>::Definition::createDef("Encoder", "Encoder HighLight", &EncoderAction::create)->addParam("actionType", EncoderAction::ENC_HL));
    defs.add(Factory<Action>::Definition::createDef("Encoder", "Encoder Blind", &EncoderAction::create)->addParam("actionType", EncoderAction::ENC_BLIND));
    defs.add(Factory<Action>::Definition::createDef("Encoder", "Encoder Range", &EncoderAction::create)->addParam("actionType", EncoderAction::ENC_RANGE));
    defs.add(Factory<Action>::Definition::createDef("Encoder", "Encoder Thru", &EncoderAction::create)->addParam("actionType", EncoderAction::ENC_THRU));
    defs.add(Factory<Action>::Definition::createDef("Encoder", "Encoder Explode", &EncoderAction::create)->addParam("actionType", EncoderAction::ENC_EXPLODE));

    defs.add(Factory<Action>::Definition::createDef("Presets", "Timing Preset set", &PresetAction::create)->addParam("actionType", PresetAction::TIMING_PRESET_SET));
    defs.add(Factory<Action>::Definition::createDef("Presets", "Timing Preset fixed", &PresetAction::create)->addParam("actionType", PresetAction::TIMING_PRESET_FIXED));

    defs.add(Factory<Action>::Definition::createDef("Tap tempo multiple", "Cuelist", &TapTempoMultiple::create)->addParam("actionType", TapTempoMultiple::TAPTEMPO_CUELIST));
    defs.add(Factory<Action>::Definition::createDef("Tap tempo multiple", "FX", &TapTempoMultiple::create)->addParam("actionType", TapTempoMultiple::TAPTEMPO_FX));
    defs.add(Factory<Action>::Definition::createDef("Tap tempo multiple", "Carousel", &TapTempoMultiple::create)->addParam("actionType", TapTempoMultiple::TAPTEMPO_CAR));

    defs.add(Factory<Action>::Definition::createDef("Interface", "Send MIDI", &InterfaceAction::create)->addParam("actionType", InterfaceAction::MIDI_SEND));
    defs.add(Factory<Action>::Definition::createDef("Interface", "Send OSC", &InterfaceAction::create)->addParam("actionType", InterfaceAction::OSC_SEND));

    defs.add(Factory<Action>::Definition::createDef("Generic", "Set Parameter Value", &GenericAction::create)->addParam("actionType", GenericAction::SET_VALUE));
    defs.add(Factory<Action>::Definition::createDef("Generic", "Trigger a control", &GenericAction::create)->addParam("actionType", GenericAction::TRIGGER));




}

ActionManager::ActionManager(const String &name) :
    BaseManager(name)
{
    managerFactory = ActionFactory::getInstance();
}

ActionManager::~ActionManager()
{
}

void ActionManager::triggerAll()
{
    for (auto& i : items) i->trigger();
}

void ActionManager::setValueAll(var value)
{
    for (auto& i : items) i->setValue(value);
}

void ActionManager::setValueAll(var value, String origin, int incrementIndex, bool isRelative)
{
    for (auto& i : items) i->setValue(value, origin, incrementIndex, isRelative);
}

var ActionManager::getValue()
{
    if (items.size() > 0) {
        return items[0]->getValue();
    }

    return var();
}

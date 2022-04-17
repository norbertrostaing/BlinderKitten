/*
  ==============================================================================

    ActionManager.cpp
    Created: 12 Oct 2020 10:55:26am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(ActionFactory);

#include "Definitions/Actions/CuelistAction.h"
#include "Definitions/Actions/EffectAction.h"
#include "Definitions/Actions/CarouselAction.h"
#include "Definitions/Actions/MapperAction.h"
#include "Definitions/Actions/VirtualButtonAction.h"
#include "Definitions/Actions/VirtualFaderAction.h"
#include "Definitions/Actions/InputPanelAction.h"
#include "Definitions/Actions/MultiplicatorAction.h"

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

    // defs.add(Factory<Action>::Definition::createDef("Generic", "Set Parameter Value", &GenericAction::create)->addParam("actionType", GenericAction::SET_VALUE));
    // defs.add(Factory<Action>::Definition::createDef("Generic", "Trigger a control", &GenericAction::create)->addParam("actionType", GenericAction::TRIGGER));

    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Go", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_GO));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Go random", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_GORANDOM));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Flash", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_FLASH));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Swop", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_SWOP));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Off", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_OFF));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Load", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_LOAD));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Load and Go", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_LOADANDGO));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "HTP Level", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_HTPLEVEL));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Flash Level", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_FLASHLEVEL));

    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Start", &EffectAction::create)->addParam("actionType", EffectAction::FX_START));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Stop", &EffectAction::create)->addParam("actionType", EffectAction::FX_STOP));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Size", &EffectAction::create)->addParam("actionType", EffectAction::FX_SIZE));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Effect Speed", &EffectAction::create)->addParam("actionType", EffectAction::FX_SPEED));

    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Start", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_START));
    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Stop", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_STOP));
    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Size", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_SIZE));
    defs.add(Factory<Action>::Definition::createDef("Carousel", "Carousel Speed", &CarouselAction::create)->addParam("actionType", CarouselAction::CAR_SPEED));

    defs.add(Factory<Action>::Definition::createDef("Mapper", "Mapper Start", &MapperAction::create)->addParam("actionType", MapperAction::TRK_START));
    defs.add(Factory<Action>::Definition::createDef("Mapper", "Mapper Stop", &MapperAction::create)->addParam("actionType", MapperAction::TRK_STOP));
    defs.add(Factory<Action>::Definition::createDef("Mapper", "Mapper Size", &MapperAction::create)->addParam("actionType", MapperAction::TRK_SIZE));

    defs.add(Factory<Action>::Definition::createDef("Multiplicator", "Set Value", &MultiplicatorAction::create)->addParam("actionType", MultiplicatorAction::MULT_SET));
    defs.add(Factory<Action>::Definition::createDef("Multiplicator", "Set static Value", &MultiplicatorAction::create)->addParam("actionType", MultiplicatorAction::MULT_SETAT));

    defs.add(Factory<Action>::Definition::createDef("VirtualButton", "Virtual Button Press", &VirtualButtonAction::create)->addParam("actionType", VirtualButtonAction::VB_PRESS));

    defs.add(Factory<Action>::Definition::createDef("VirtualFader", "Encoder Move", &VirtualFaderAction::create)->addParam("actionType", VirtualFaderAction::VF_ENCODER));
    defs.add(Factory<Action>::Definition::createDef("VirtualFader", "Above Button Press", &VirtualFaderAction::create)->addParam("actionType", VirtualFaderAction::VF_ABOVEBUTTON));
    defs.add(Factory<Action>::Definition::createDef("VirtualFader", "Fader Move", &VirtualFaderAction::create)->addParam("actionType", VirtualFaderAction::VF_FADER));
    defs.add(Factory<Action>::Definition::createDef("VirtualFader", "Below Button Press", &VirtualFaderAction::create)->addParam("actionType", VirtualFaderAction::VF_BELOWBUTTON));

    defs.add(Factory<Action>::Definition::createDef("Input Panel", "Button Press", &InputPanelAction::create)->addParam("actionType", InputPanelAction::IP_PRESS));

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

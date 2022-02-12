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
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "HTP Level", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_HTPLEVEL));
    defs.add(Factory<Action>::Definition::createDef("Cuelist", "Flash Level", &CuelistAction::create)->addParam("actionType", CuelistAction::CL_FLASHLEVEL));

    defs.add(Factory<Action>::Definition::createDef("Effect", "Start", &EffectAction::create)->addParam("actionType", EffectAction::FX_START));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Stop", &EffectAction::create)->addParam("actionType", EffectAction::FX_STOP));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Size", &EffectAction::create)->addParam("actionType", EffectAction::FX_SIZE));
    defs.add(Factory<Action>::Definition::createDef("Effect", "Speed", &EffectAction::create)->addParam("actionType", EffectAction::FX_SPEED));

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

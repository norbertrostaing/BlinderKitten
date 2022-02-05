#include "Command.h"
#include "CommandManager.h"

juce_ImplementSingleton(CommandManager);


CommandManager::CommandManager() :
    BaseManager("Command")
    {
    itemDataType = "Command";
    selectItemWhenCreated = true;
       
}

CommandManager::~CommandManager()
{
    // stopThread(1000);
}


void CommandManager::addItemInternal(Command* o, var data)
{
    // o->addFixtureParamTypeListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void CommandManager::removeItemInternal(Command* o)
{
    // o->removeObjectListener(this);
}


void CommandManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}


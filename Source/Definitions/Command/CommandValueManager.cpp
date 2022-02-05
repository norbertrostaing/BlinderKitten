#include "CommandValueManager.h"
#include "../Fixture/Fixture.h"
#include "../../Brain.h"

CommandValueManager::CommandValueManager() :
    BaseManager("CommandValue")
{
    itemDataType = "CommandValue";
    selectItemWhenCreated = false;       

}

CommandValueManager::~CommandValueManager()
{
}

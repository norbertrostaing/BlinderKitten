#include "CommandValueManager.h"
#include "../SubFixture/SubFixture.h"
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

#include "CommandValueManager.h"
#include "../SubFixture/SubFixture.h"
#include "../../Brain.h"

CommandValueManager::CommandValueManager() :
    BaseManager("Values")
{
    itemDataType = "CommandValue";
    selectItemWhenCreated = false;

}

CommandValueManager::~CommandValueManager()
{
}

void CommandValueManager::showStepSize(bool should)
{
    shouldShowStepSize = should;
    for (CommandValue* cmdVal : items) {
        cmdVal->shouldShowStepSize = should;
        cmdVal->updateDisplay();
    }
}

void CommandValueManager::addItemInternal(CommandValue* cv, juce::var data)
{
    cv->shouldShowStepSize = shouldShowStepSize;
    cv->updateDisplay();
}


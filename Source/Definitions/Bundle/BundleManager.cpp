#include "Bundle.h"
#include "BundleManager.h"

juce_ImplementSingleton(BundleManager);

int compare(Bundle* A, Bundle* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}

BundleManager::BundleManager() :
    BaseManager("Bundle")
    {
    itemDataType = "Bundle";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;
    autoReorderOnAdd = true;

}

BundleManager::~BundleManager()
{
    // stopThread(1000);
}


void BundleManager::addItemInternal(Bundle* o, var data)
{
    // o->addChannelFamilyListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void BundleManager::removeItemInternal(Bundle* o)
{
    // o->removeObjectListener(this);
}


void BundleManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}


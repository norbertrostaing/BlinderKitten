#include "BundleSelectionManager.h"
#include "../SubFixture/SubFixture.h"
#include "../../Brain.h"
#include "../../UserInputManager.h"
#include "Bundle.h"
#include "Definitions/Layout/Layout.h"

BundleSelectionManager::BundleSelectionManager() :
    BaseManager("Selections")
{
    itemDataType = "BundleSelection";
    selectItemWhenCreated = false; 
}

BundleSelectionManager::~BundleSelectionManager()
{
	computing.enter();
	computing.exit();
}

void BundleSelectionManager::computeSelection() {
	Array<int> histo;
	computeSelection(histo);
}

void BundleSelectionManager::computeSelection(Array<int> groupHistory) {
	ScopedLock lock(computing);

	//int n = subFixtureToPosition.size();
	//auto t = this;
	//LOG("coucou");
}


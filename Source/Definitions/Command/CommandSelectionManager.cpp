#include "CommandSelectionManager.h"
#include "../SubFixture/SubFixture.h"
#include "../../Brain.h"

CommandSelectionManager::CommandSelectionManager() :
    BaseManager("Selections")
{
    itemDataType = "CommandSelection";
    selectItemWhenCreated = false;       

}

CommandSelectionManager::~CommandSelectionManager()
{
}

void CommandSelectionManager::computeSelection() {
	Array<int> histo;
	computeSelection(histo);
}

void CommandSelectionManager::computeSelection(Array<int> groupHistory) {
	computedSelectedSubFixtures.clear();
	Brain* b = Brain::getInstance();
	Array<CommandSelection*> selections = getItemsWithType<CommandSelection>();
	for (int selId = 0; selId < selections.size(); selId++) {
		Array<SubFixture*> tempSelection;
		int idFrom = selections[selId]->valueFrom->getValue();
		int idThru = selections[selId]->thru->getValue() ? selections[selId]->valueTo->getValue() : idFrom;
		int mod = idFrom <= idThru ? 1 : -1;

		if (selections[selId]->targetType->getValue() == "Fixture") {
			for (int id = idFrom; id != idThru + mod; id = id + mod) {
				if (b->Fixtures.contains(id)) {
					Fixture* fixt = b->Fixtures.getReference(id);
					if (selections[selId]->subSel->getValue()) {
						int subFrom = selections[selId]->subFrom->getValue();
						int subThru = selections[selId]->subThru->getValue() ? selections[selId]->subTo->getValue() : subFrom;
						int subMod = subFrom <= subThru ? 1 : -1;
						for (int subId = subFrom; subId != subThru + subMod; subId = subId + subMod) {
							SubFixture* s = fixt->getSubFixture(subId);
							if (s != nullptr) {
								tempSelection.add(s);
							}
						}
					}
					else {
						tempSelection.addArray(fixt->getAllSubFixtures());
					}
				}
			}
		}
		else if (selections[selId]->targetType->getValue() == "group") {
			for (int id = idFrom; id != idThru + mod; id = id + mod) {
				if (b->groups.contains(id)) {
					Group* g = b->groups.getReference(id);
					if (groupHistory.indexOf(g->id->getValue()) == -1) {
						groupHistory.add(id);
						g->selection.computeSelection(groupHistory);
						tempSelection.addArray(g->selection.computedSelectedSubFixtures);
					}
				}
			}
		}
		else {}
			
		// filter here with divide and pattern...
		String pattern = selections[selId]->pattern->getValue();
		bool sym = selections[selId]->symmetry->getValue();
		int patternLength = pattern.length();
		int tempSelectionSize = tempSelection.size();
		if (selections[selId]->filter->getValue() == "pattern" && patternLength > 0) {
			Array<SubFixture*> filteredSelection;
			for (int i = 0; i < tempSelection.size(); i++) {
				int patternIndex = i % patternLength; 
				if (sym && i >= tempSelectionSize/2) {
					patternIndex = tempSelectionSize - 1 - i;
					patternIndex = patternIndex % patternLength;
				}
				char c = pattern[patternIndex];
				if (c == '1') {
					filteredSelection.add(tempSelection[i]);
				}
			}
			tempSelection = filteredSelection;
		}
		else if(selections[selId]->filter->getValue() == "divide") {
			if (sym) {
				for (int i = patternLength -1; i >= 0; i--) {
					pattern += pattern[i];
				}
				patternLength *= 2;
			}
			Array<SubFixture*> filteredSelection;
			for (int i = 0; i < tempSelection.size(); i++) {

				float patternIndex = 0; 
				if (i > tempSelectionSize/2) {
					patternIndex = (i+1) * patternLength / float(tempSelectionSize);
				}
				else {
					patternIndex = i * patternLength / float(tempSelectionSize);
				}
				patternIndex = floor(patternIndex);
				if (sym && i >= tempSelectionSize / 2) {
					patternIndex = tempSelectionSize - 1 - i;
					patternIndex = patternIndex * patternLength / tempSelectionSize;
				}

				patternIndex = jmin(int(patternIndex), patternLength-1);
				char c = pattern[patternIndex];
				if (c == '1') {
					filteredSelection.add(tempSelection[i]);
				}
			}
			tempSelection = filteredSelection;

		}
			//apply patterns here

		if (selections[selId]->plusOrMinus->getValue() == "add") {
			computedSelectedSubFixtures.addArray(tempSelection);
		}
		else if (selections[selId]->plusOrMinus->getValue() == "remove") {
			computedSelectedSubFixtures.removeValuesIn(tempSelection);
		}
		else {}

	}


}
#include "CommandSelectionManager.h"
#include "../Fixture/Fixture.h"
#include "../../Brain.h"

CommandSelectionManager::CommandSelectionManager() :
    BaseManager("CommandSelection")
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
	computedSelectedFixtures.clear();
	Brain* b = Brain::getInstance();
	Array<CommandSelection*> selections = getItemsWithType<CommandSelection>();
	for (int selId = 0; selId < selections.size(); selId++) {
		Array<Fixture*> tempSelection;
		int idFrom = selections[selId]->valueFrom->getValue();
		int idThru = selections[selId]->thru->getValue() ? selections[selId]->valueTo->getValue() : idFrom;
		int mod = idFrom <= idThru ? 1 : -1;

		if (selections[selId]->targetType->getValue() == "fixture") {
			for (int id = idFrom; id != idThru + mod; id = id + mod) {
				if (b->fixtures.contains(id)) {
					tempSelection.add(b->fixtures.getReference(id));
				}
			}
		}
		else if (selections[selId]->targetType->getValue() == "device") {
			for (int id = idFrom; id != idThru + mod; id = id + mod) {
				if (b->devices.contains(id)) {
					Device* d = b->devices.getReference(id);
					tempSelection.addArray(d->fixtures->getItemsWithType<Fixture>());
				}
			}
		}
		else if (selections[selId]->targetType->getValue() == "group") {
			for (int id = idFrom; id != idThru + mod; id = id + mod) {
				if (b->groups.contains(id)) {
					Group* g = b->groups.getReference(id);
					if (groupHistory.indexOf(g->id->getValue()) == -1) {
						groupHistory.add(id);
						g->selection->computeSelection(groupHistory);
						tempSelection.addArray(g->selection->computedSelectedFixtures);
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
			Array<Fixture*> filteredSelection;
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
			Array<Fixture*> filteredSelection;
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
			computedSelectedFixtures.addArray(tempSelection);
		}
		else if (selections[selId]->plusOrMinus->getValue() == "remove") {
			computedSelectedFixtures.removeValuesIn(tempSelection);
		}
		else {}

	}


}
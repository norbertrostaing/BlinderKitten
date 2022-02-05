/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Cue.h"
#include "../Cuelist/Cuelist.h"

Cue::Cue(var params) :
	BaseItem(params.getProperty("name", "Cue")),
	objectType(params.getProperty("type", "Cue").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = false;
	itemDataType = "Cue";
	
	id = addFloatParameter("ID", "Id of this cue", 0, 0);
	autoGoTime = addFloatParameter("Auto go", "automatic go after X seconds, 0 means disabled",0,0);
	goBtn = addTrigger("GO", "trigger this cue");

	BaseManager<Command>* m = new BaseManager<Command>("Commands");
	m->selectItemWhenCreated = false;
	commands.reset(m);
	addChildControllableContainer(commands.get());


}

Cue::~Cue()
{
	LOG("delete cue");
}


void Cue::triggerTriggered(Trigger* t) {
	if (t == goBtn) {
		Cuelist* parentCuelist = dynamic_cast<Cuelist*>(this->parentContainer->parentContainer.get());
		Cue* temp = this;
		parentCuelist->go(temp);
	}
}

void Cue::parameterValueChanged(Parameter* p) {
	BaseItem::parameterValueChanged(p);
}

void Cue::computeValues() {
	maxTiming = 0;
	computedValues.clear();
	Array<Command*> cs = commands->getItemsWithType<Command>();
	for (int i = 0; i < cs.size(); i++) {
		cs[i]->computeValues();
		maxTiming = std::max(maxTiming, cs[i]->maxTiming);
		for (auto it = cs[i]->computedValues.begin(); it != cs[i]->computedValues.end(); it.next()) {
			FixtureChannel* fc = it.getKey();
			computedValues.set(fc, it.getValue());
		}
	}
}



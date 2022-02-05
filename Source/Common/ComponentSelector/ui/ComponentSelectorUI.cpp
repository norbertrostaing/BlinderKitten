/*
  ==============================================================================

    ComponentSelectorUI.cpp
    Created: 3 Oct 2020 2:09:02pm
    Author:  bkupe

  ==============================================================================
*/

ComponentSelectorUI::ComponentSelectorUI(ComponentSelector* s) :
    TextButton("Components", "Select components to be affected by this element"),
    selector(s),
    model(s)
{
}

ComponentSelectorUI::~ComponentSelectorUI()
{
}

void ComponentSelectorUI::clicked()
{
    model.removeModelListener(this);
  
    std::unique_ptr<ListBox> box(new ListBox());
    listBox = box.get();
   
    int numTypes = selector->allowedComponents.size();
    box->setModel(&model);
    box->setRowHeight(16);
    box->setBounds(0, 0, 200, 16 * numTypes);
    box->setMultipleSelectionEnabled(true);
    box->setClickingTogglesRowSelection(true);

    SparseSet<int> rows;
    for (int i = 0; i < numTypes; i++) {
        if (selector->selectedComponents[selector->allowedComponents[i]]) rows.addRange(Range<int>(i, i+1));
    }
    box->setSelectedRows(rows);
    
    CallOutBox::launchAsynchronously(std::move(box), localAreaToGlobal(getLocalBounds()), nullptr);

    model.addModelListener(this);

}

void ComponentSelectorUI::selectionChanged()
{
    for (int i = 0; i <selector->allowedComponents.size(); i++)
    {
        selector->selectedComponents.set(selector->allowedComponents[i], listBox->isRowSelected(i));
    }
}


// MODEL

ComponentListModel::ComponentListModel(ComponentSelector * s) :
    selector(s)
{
}

ComponentListModel::~ComponentListModel()
{
}

void ComponentListModel::selectedRowsChanged(int lastRowSelected)
{
    modelListeners.call(&ModelListener::selectionChanged);
}

int ComponentListModel::getNumRows()
{
    return selector->allowedComponents.size();
}

void ComponentListModel::paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
{
    g.setColour(BG_COLOR.brighter((rowNumber % 2 == 0) ? .2 : .3f));
    g.fillRect(0, 0,width, height);

    g.setColour(rowIsSelected?GREEN_COLOR:TEXT_COLOR.darker(.3f));
    g.drawText(componentTypeNames[selector->allowedComponents[rowNumber]], Rectangle<float>(0,0,width, height).reduced(2), Justification::centred);
}
/*
  ==============================================================================

	ComponentSelectorUI.h
	Created: 3 Oct 2020 2:09:02pm
	Author:  bkupe

  ==============================================================================
*/

#pragma once

class ComponentListModel :
	public ListBoxModel
{
public:
	ComponentListModel(ComponentSelector *s);
	~ComponentListModel();

	ComponentSelector* selector;

	void selectedRowsChanged(int lastRowSelected) override;
	
	virtual int getNumRows() override;
	virtual void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;

	class  ModelListener
	{
	public:
		/** Destructor. */
		virtual ~ModelListener() {}
		virtual void selectionChanged() {}
	};

	ListenerList<ModelListener> modelListeners;
	void addModelListener(ModelListener* newListener) { modelListeners.add(newListener); }
	void removeModelListener(ModelListener* listener) { modelListeners.remove(listener); }
};


class ComponentSelectorUI :
	public TextButton,
	public ComponentListModel::ModelListener
{
public:
	ComponentSelectorUI(ComponentSelector* s);
	~ComponentSelectorUI();

	ComponentSelector* selector;
	ListBox * listBox;
	ComponentListModel model;

	void clicked() override;

	void selectionChanged() override;
};
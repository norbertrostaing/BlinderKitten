/*
  ==============================================================================

	LayoutViewer.cpp
	Created: 8 Oct 2020 6:59:51pm
	Author:  bkupe

  ==============================================================================
*/

#include "LayoutViewer.h"
#include "Brain.h"
#include "UserInputManager.h"

LayoutViewer::LayoutViewer() :
	ShapeShifterContentComponent("Layout Viewer")
{
	layoutsList.setTextWhenNoChoicesAvailable("No Layout created");
	layoutsList.setTextWhenNothingSelected("Select a Layout");
	layoutsList.addListener(this);
	addAndMakeVisible(&layoutsList);

	rebuildLayoutsList();
	LayoutManager::getInstance()->addAsyncManagerListener(this);

}

LayoutViewer::~LayoutViewer()
{
	if (selectedLayout != nullptr) {
		selectedLayout->removeChangeListener(this);
	}
	if (LayoutManager::getInstanceWithoutCreating()) LayoutManager::getInstance()->removeAsyncManagerListener(this);
}


void LayoutViewer::rebuildLayoutsList()
{
	int idToSelect = 0;

	layoutsList.clear(dontSendNotification);
	layoutsList.addItem("No Layout selected", -1);
	for (auto& l : LayoutManager::getInstance()->items)
	{
		if (l != nullptr) {
			int id = l->id->intValue();
			if (l == selectedLayout) idToSelect = id;
			layoutsList.addItem(l->niceName, id);
		}
	}

	layoutsList.setSelectedId(idToSelect, dontSendNotification);
}

void LayoutViewer::newMessage(const LayoutManager::ManagerEvent& e)
{
	if (e.type == LayoutManager::ManagerEvent::ITEM_ADDED || e.type == LayoutManager::ManagerEvent::ITEM_REMOVED || e.type == LayoutManager::ManagerEvent::ITEMS_REORDERED)
	{
		if (e.type == LayoutManager::ManagerEvent::ITEM_REMOVED && e.getItem() == selectedLayout) {
			selectedLayout = nullptr;
		}
		rebuildLayoutsList();
	}
}

void LayoutViewer::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	int id = comboBoxThatHasChanged->getSelectedId();
	selectLayout(id);
}

void LayoutViewer::selectLayout(int id)
{
	if (selectedLayout != nullptr) {
		selectedLayout->removeChangeListener(this);
	}
	Layout* l = Brain::getInstance()->getLayoutById(id);
	if (l != nullptr) {
		selectedLayout = l;
		selectedLayout->addChangeListener(this);
		selectedLayout->computeData();
	}
	repaint();
}

void LayoutViewer::resized()
{
	Rectangle<int> r = getLocalBounds();
	Rectangle<int> hr = r.removeFromTop(20);

	layoutsList.setBounds(hr.removeFromLeft(150).reduced(2));
}


void LayoutViewer::changeListenerCallback(ChangeBroadcaster* source)
{
	repaint();
}

void LayoutViewer::paint(Graphics& g)
{
	Rectangle<int> r = getLocalBounds().reduced(2);
	g.fillAll(Colour(0, 0, 0));

	// ajouter liste déroulante pour choisir le layout
	if (selectedLayout == nullptr) {
		g.setColour(Colours::white.withAlpha(.8f));
		g.setFont(jlimit<float>(12, 20, getHeight() - 30));
		g.drawText("No layout selected", getLocalBounds().toFloat(), Justification::centred, false);
		return;
	}

	var dimensionX = selectedLayout->dimensionsX->getValue();
	var dimensionY = selectedLayout->dimensionsY->getValue();
	if (dimensionX[1] < dimensionX[0]) { var temp = dimensionX[0]; dimensionX[0] = dimensionX[1]; dimensionX[1] = temp; }
	if (dimensionY[1] < dimensionY[0]) { var temp = dimensionY[0]; dimensionY[0] = dimensionY[1]; dimensionY[1] = temp; }

	float layoutWidth = (float)dimensionX[1] - (float)dimensionX[0];
	float layoutHeight = (float)dimensionY[1] - (float)dimensionY[0];
	if (layoutWidth == 0 || layoutHeight == 0) {
		return;
	}

	Rectangle<float> layoutZone;
	float layoutRatio = abs(layoutWidth / layoutHeight);
	float windowRatio = (float)getWidth() / (float)getHeight();
	float originX = 0;
	float originY = 0;
	float width = 0;
	float height = 0;
	float uiScale = 1;

	if (layoutRatio > windowRatio) {
		// crop top and bottom;
		originX = 0;
		width = getWidth();
		height = width / layoutRatio;
		originY = getHeight() - height;
		originY /= 2;
	}
	else {
		// crop left and right
		originY = 0;
		height = getHeight();
		width = height * layoutRatio;
		originX = getWidth()-width;
		originX /= 2;
	}

	uiScale = width/layoutWidth;

	//g.setColour(Colour(64,64,64));
	//g.fillRect(originX, originY, width, height);
	g.reduceClipRegion(originX, originY, width, height);
	g.setOrigin(originX, originY);
	g.fillAll(Colour(32, 32, 32));

	float fixtWidth = 10;
	float halfFixtWidth = fixtWidth/2;

	for (int i = 0; i < selectedLayout->paths.items.size(); i++) {
		BKPath* p = selectedLayout->paths.items[i];
		BKPath::PathType type = p->pathType->getValueDataAsEnum<BKPath::PathType>(); //::PATH_LINE) 
		if (type == BKPath::PATH_LINE) {
			if (true) { // wanna draw lines ?
				float fromX = jmap((float)p->position->getValue()[0], (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
				float fromY = jmap((float)p->position->getValue()[1], (float)dimensionY[0], (float)dimensionY[1], (float)0, height);
				float toX = jmap((float)p->lineEndPosition->getValue()[0], (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
				float toY = jmap((float)p->lineEndPosition->getValue()[1], (float)dimensionY[0], (float)dimensionY[1], (float)0, height);

				g.setColour(juce::Colours::orange);
				Line<float> line(Point<float>(fromX, fromY), Point<float>(toX, toY));
				g.drawLine(line, 2.0f);

			}
			if (p->spreadSubFixtures->boolValue()) {
				p->isComputing.enter();
				for (auto it = p->subFixtToPos.begin(); it != p->subFixtToPos.end(); it.next()) {
					float X = jmap((float)it.getValue()->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
					float Y = jmap((float)it.getValue()->y, (float)dimensionY[0], (float)dimensionY[1], (float)0, height);
					g.drawRect(X - halfFixtWidth, Y - halfFixtWidth, fixtWidth, fixtWidth, (float)1);
				}
				p->isComputing.exit();
			}
			if (!p->spreadSubFixtures->boolValue()) {
				p->isComputing.enter();
				for (auto it = p->fixtToPos.begin(); it != p->fixtToPos.end(); it.next()) {
					float X = jmap((float)it.getValue()->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
					float Y = jmap((float)it.getValue()->y, (float)dimensionY[0], (float)dimensionY[1], (float)0, height);
					g.drawRect(X - halfFixtWidth, Y - halfFixtWidth, fixtWidth, fixtWidth, (float)1);
				}
				p->isComputing.exit();
			}
		}
		if (type == BKPath::PATH_GRID) {
			if (true) {
				for (int iGrid = 0; iGrid < p->gridPath.size() - 1; iGrid++) {
					float fromX = jmap(p->gridPath[iGrid]->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
					float fromY = jmap(p->gridPath[iGrid]->y, (float)dimensionY[0], (float)dimensionY[1], (float)0, height);
					float toX = jmap(p->gridPath[iGrid + 1]->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
					float toY = jmap(p->gridPath[iGrid + 1]->y, (float)dimensionY[0], (float)dimensionY[1], (float)0, height);
					g.setColour(juce::Colours::orange);
					Line<float> line(Point<float>(fromX, fromY), Point<float>(toX, toY));
					g.drawLine(line, 0.5f);
				}
			}


			if (p->spreadSubFixtures->boolValue()) {
				p->isComputing.enter();
				for (auto it = p->subFixtToPos.begin(); it != p->subFixtToPos.end(); it.next()) {
					float X = jmap((float)it.getValue()->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
					float Y = jmap((float)it.getValue()->y, (float)dimensionY[0], (float)dimensionY[1], (float)0, height);
					g.drawRect(X - halfFixtWidth, Y - halfFixtWidth, fixtWidth, fixtWidth, (float)1);
				}
				p->isComputing.exit();
			}
			if (!p->spreadSubFixtures->boolValue()) {
				p->isComputing.enter();
				for (auto it = p->fixtToPos.begin(); it != p->fixtToPos.end(); it.next()) {
					float X = jmap((float)it.getValue()->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
					float Y = jmap((float)it.getValue()->y, (float)dimensionY[0], (float)dimensionY[1], (float)0, height);
					g.drawRect(X - halfFixtWidth, Y - halfFixtWidth, fixtWidth, fixtWidth, (float)1);
				}
				p->isComputing.exit();
			}
		}
		if (type == BKPath::PATH_CIRCLE) {
			if (true) {
				float from = -(p->circleFrom->floatValue() / 360. ) * 2 * MathConstants<float>::pi;
				float to = -(p->circleTo->floatValue() / 360. ) * 2 * MathConstants<float>::pi;
				from += MathConstants<float>::pi / 2;
				to += MathConstants<float>::pi / 2;
				float X = jmap(p->position->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
				float Y = jmap(p->position->y, (float)dimensionY[0], (float)dimensionY[1], (float)0, height);
				float W = p->circleRadius->floatValue() * uiScale;
				Path path;
				path.addArc(X-(W), Y-(W), W*2, W*2, from, to, true);
				g.strokePath(path, PathStrokeType(0.5));
			}


			if (p->spreadSubFixtures->boolValue()) {
				p->isComputing.enter();
				for (auto it = p->subFixtToPos.begin(); it != p->subFixtToPos.end(); it.next()) {
					float X = jmap((float)it.getValue()->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
					float Y = jmap((float)it.getValue()->y, (float)dimensionY[0], (float)dimensionY[1], (float)0, height);
					g.drawRect(X - halfFixtWidth, Y - halfFixtWidth, fixtWidth, fixtWidth, (float)1);
				}
				p->isComputing.exit();
			}
			if (!p->spreadSubFixtures->boolValue()) {
				p->isComputing.enter();
				for (auto it = p->fixtToPos.begin(); it != p->fixtToPos.end(); it.next()) {
					float X = jmap((float)it.getValue()->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
					float Y = jmap((float)it.getValue()->y, (float)dimensionY[0], (float)dimensionY[1], (float)0, height);
					g.drawRect(X - halfFixtWidth, Y - halfFixtWidth, fixtWidth, fixtWidth, (float)1);
				}
				p->isComputing.exit();
			}
		}
	}

}


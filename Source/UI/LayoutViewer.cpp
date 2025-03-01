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
#include "BKEngine.h"

LayoutViewer::LayoutViewer() :
	ShapeShifterContentComponent("Layout Viewer")
{
	layoutsList.setTextWhenNoChoicesAvailable("No Layout created");
	layoutsList.setTextWhenNothingSelected("Select a Layout");
	layoutsList.addListener(this);
	addAndMakeVisible(&layoutsList);

	editMode.setButtonText("Edit mode");
	editMode.setWantsKeyboardFocus(false);
	editMode.addListener(this);
	addAndMakeVisible(&editMode);

	viewPaths.setButtonText("Paths lines");
	viewPaths.setWantsKeyboardFocus(false);
	viewPaths.addListener(this);
	addAndMakeVisible(&viewPaths);

	exportBtn.setButtonText("Export");
	exportBtn.setWantsKeyboardFocus(false);
	addAndMakeVisible(exportBtn);
	exportBtn.onClick = [this](){ exportToPNG();};

	rebuildLayoutsList();
	LayoutManager::getInstance()->addAsyncManagerListener(this);

	engine = dynamic_cast<BKEngine*>(BKEngine::mainEngine);
	engine->layoutViewerLayout->addParameterListener(this);
	repaint();
	startTimerHz(30);
}

LayoutViewer::~LayoutViewer()
{
	stopTimer();
	engine->layoutViewerLayout->removeParameterListener(this);
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
	if (e.type == LayoutManager::ManagerEvent::ITEM_REMOVED || e.type == LayoutManager::ManagerEvent::ITEMS_REMOVED) {
		selectedLayout = nullptr;
		engine->layoutViewerLayout->resetValue();
	}
	rebuildLayoutsList();
}

Colour LayoutViewer::getClickColour(BKPath* path, ClicAction action)
{
	currentClicColour+= 5;
	uint32 temp = currentClicColour;
	int r = temp % 256;
	temp /= 256;
	int g = temp % 256;
	temp /= 256;
	int b = temp % 256;

	colourToPath.set(currentClicColour, path);
	colourToAction.set(currentClicColour, action);
	return Colour(r,g,b);
}

Colour LayoutViewer::getClickColour(SubFixture* sf)
{
	currentClicColour += 5;
	uint32 temp = currentClicColour;
	int r = temp % 256;
	temp /= 256;
	int g = temp % 256;
	temp /= 256;
	int b = temp % 256;

	colourToSubFixture.set(currentClicColour, sf);
	colourToAction.set(currentClicColour, CLIC_SELECT);
	return Colour(r, g, b);
}

Colour LayoutViewer::getClickColour(Fixture* f)
{
	currentClicColour += 5;
	uint32 temp = currentClicColour;
	int r = temp % 256;
	temp /= 256;
	int g = temp % 256;
	temp /= 256;
	int b = temp % 256;

	colourToFixture.set(currentClicColour, f);
	colourToAction.set(currentClicColour, CLIC_SELECT);
	return Colour(r, g, b);
}

void LayoutViewer::resetClickColour()
{
	currentClicColour = 0;
	colourToAction.clear();
	colourToAction.clear();
}

void LayoutViewer::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	int id = comboBoxThatHasChanged->getSelectedId();
	Layout* l = Brain::getInstance()->getLayoutById(id);
	if (l == nullptr) {
		engine->layoutViewerLayout->resetValue(l);
	} else {
		engine->layoutViewerLayout->setValueFromTarget(l);
	}
}

void LayoutViewer::parameterValueChanged(Parameter* p)
{
	if (p == engine->layoutViewerLayout) {
		Layout* l = engine->layoutViewerLayout->getTargetContainerAs<Layout>();
		selectLayout(l);
		rebuildLayoutsList();
	}
}

void LayoutViewer::buttonStateChanged(Button* b)
{
}

void LayoutViewer::buttonClicked(Button* b)
{
	repaint();
	if (b == &editMode && selectedLayout != nullptr && editMode.getToggleState()) {
		selectedLayout->selectThis();
	}
}

void LayoutViewer::selectLayout(Layout* l)
{
	if (selectedLayout != nullptr) {
		selectedLayout->removeChangeListener(this);
	}
	if (l != nullptr) {
		selectedLayout = l;
		selectedLayout->addChangeListener(this);
		selectedLayout->computeData();
		if (editMode.getToggleState()) {
			selectedLayout->selectThis();
		}
	}
	else {
		selectedLayout = nullptr;
	}
	repaint();
}

void LayoutViewer::resized()
{
	Rectangle<int> r = getLocalBounds();
	Rectangle<int> hr = r.removeFromTop(20);

	currentWidth = getWidth();
	currentHeight = getHeight();

	layoutsList.setBounds(hr.removeFromLeft(80).reduced(2));
	viewPaths.setBounds(hr.removeFromLeft(80).reduced(2));
	editMode.setBounds(hr.removeFromLeft(80).reduced(2));
	exportBtn.setBounds(hr.removeFromLeft(80).reduced(2));

	if (selectedLayout != nullptr) {
		selectedLayout->sizeChanged();
	}
}


void LayoutViewer::mouseExit(const MouseEvent& e)
{
	currentMouseAction = CLIC_NOACTION;
	currentMousePath = nullptr;
	hoveredPath = nullptr;
}

void LayoutViewer::mouseDown(const MouseEvent& e)
{
	if (selectedLayout == nullptr) {return;}

	int x = e.getMouseDownX();
	int y = e.getMouseDownY();

	int r = clicZones.getPixelAt(x, y).getRed();
	int g = clicZones.getPixelAt(x, y).getGreen();
	int b = clicZones.getPixelAt(x, y).getBlue();
	
	int c = (((b*256) + g )*256) + r;

	if (colourToAction.contains(c)) {
		ClicAction a = colourToAction.getReference(c);

		if (a == CLIC_SELECT) {
			if (colourToFixture.contains(c)) {
				Fixture* f = colourToFixture.getReference(c);
				UserInputManager::getInstance()->gridViewCellPressed("fixture",f->id->intValue());
				//UserInputManager::getInstance()->processInput("Fixture");
				//UserInputManager::getInstance()->processInput(f->id->stringValue());
			}
			else if(colourToSubFixture.contains(c)) {
				SubFixture* sf = colourToSubFixture.getReference(c);
				UserInputManager::getInstance()->gridViewCellPressed("fixture", sf->parentFixture->id->intValue());
				if (sf->parentFixture->subFixtures.size() > 1) {
					UserInputManager::getInstance()->processInput("subfixture");
					UserInputManager::getInstance()->processInput(String(sf->subId));
				}
			}
			else if (colourToPath.contains(c)) {
				colourToPath.getReference(c)->clicked();
			}
 		}
		if (colourToPath.contains(c)) {

			float layoutX = jmap(float(e.getDistanceFromDragStartX() + e.getMouseDownX()), topLeftX, bottomRightX, (float)selectedLayout->dimensionsX->getValue()[0], (float)selectedLayout->dimensionsX->getValue()[1]);
			float layoutY = jmap(float(e.getDistanceFromDragStartY() + e.getMouseDownY()), topLeftY, bottomRightY, (float)selectedLayout->dimensionsY->getValue()[1], (float)selectedLayout->dimensionsY->getValue()[0]);

			currentMouseAction = colourToAction.getReference(c);
			BKPath* p = colourToPath.getReference(c);
			currentMousePath = p;
			deltaMouseX = (float)p->position->getValue()[0] - layoutX;
			deltaMouseY = (float)p->position->getValue()[1] - layoutY;
			if (currentMouseAction == CLIC_TL) { fixedMouseX = p->gridBR.x; fixedMouseY = p->gridBR.y; }
			if (currentMouseAction == CLIC_TR) { fixedMouseX = p->gridBL.x; fixedMouseY = p->gridBL.y; }
			if (currentMouseAction == CLIC_BL) { fixedMouseX = p->gridTR.x; fixedMouseY = p->gridTR.y; }
			if (currentMouseAction == CLIC_BR) { fixedMouseX = p->gridTL.x; fixedMouseY = p->gridTL.y; }

			if (e.getNumberOfClicks() == 2 && editMode.getToggleState()) {
				currentMousePath->selectThis();
			}
		}
	}
	else {
		clickTracker(e);
	}
}

void LayoutViewer::mouseDrag(const MouseEvent& e)
{
	if (selectedLayout == nullptr) {return;}
	if (currentMousePath != nullptr) {
		float layoutX = jmap(float(e.getDistanceFromDragStartX() + e.getMouseDownX()), topLeftX, bottomRightX, (float)selectedLayout->dimensionsX->getValue()[0], (float)selectedLayout->dimensionsX->getValue()[1]);
		float layoutY = jmap(float(e.getDistanceFromDragStartY() + e.getMouseDownY()), topLeftY, bottomRightY, (float)selectedLayout->dimensionsY->getValue()[1], (float)selectedLayout->dimensionsY->getValue()[0]);

		if (selectedLayout->gridSnapSize->floatValue() > 0) {
			deltaMouseX = 0;
			deltaMouseY = 0;
			float m = selectedLayout->gridSnapSize->floatValue();
			float layoutXDown = layoutX - fmod(layoutX, m);
			float layoutXUp = layoutX < 0 ? layoutXDown - m : layoutXDown + m;
			layoutX = abs(layoutXDown - layoutX) < abs(layoutXUp - layoutX) ? layoutXDown : layoutXUp;
			float layoutYDown = layoutY - fmod(layoutY, m);
			float layoutYUp = layoutY < 0 ? layoutYDown - m : layoutYDown + m;
			layoutY = abs(layoutYDown - layoutY) < abs(layoutYUp - layoutY) ? layoutYDown : layoutYUp;
		}

		BKPath::PathType type = currentMousePath->pathType->getValueDataAsEnum<BKPath::PathType>();
		if (type == BKPath::PATH_POINT) {
			if (currentMouseAction == CLIC_DRAG) {
				var v;
				v.append(layoutX + deltaMouseX);
				v.append(layoutY + deltaMouseY);
				currentMousePath->position->setValue(v);
			}
		}
		else if (type == BKPath::PATH_LINE) {
			if (currentMouseAction == CLIC_DRAG) {
				float lineX = (float)currentMousePath->lineEndPosition->getValue()[0] - (float)currentMousePath->position->getValue()[0];
				float lineY = (float)currentMousePath->lineEndPosition->getValue()[1] - (float)currentMousePath->position->getValue()[1];

				var v;
				v.append(layoutX + deltaMouseX);
				v.append(layoutY + deltaMouseY);
				currentMousePath->position->setValue(v);
				v[0] = (float)v[0] + lineX;
				v[1] = (float)v[1] + lineY;
				currentMousePath->lineEndPosition->setValue(v);

			}
			else if (currentMouseAction == CLIC_ORIGIN) {
				var v;
				v.append(layoutX);
				v.append(layoutY);
				currentMousePath->position->setValue(v);
			}
			else if (currentMouseAction == CLIC_END) {
				var v;
				v.append(layoutX);
				v.append(layoutY);
				currentMousePath->lineEndPosition->setValue(v);
			}

		}
		else if (type == BKPath::PATH_ROD) {
			if (currentMouseAction == CLIC_DRAG) {
				//float lineX = (float)currentMousePath->lineEndPosition->getValue()[0] - (float)currentMousePath->position->getValue()[0];
				//float lineY = (float)currentMousePath->lineEndPosition->getValue()[1] - (float)currentMousePath->position->getValue()[1];

				var v;
				v.append(layoutX + deltaMouseX);
				v.append(layoutY + deltaMouseY);
				currentMousePath->position->setValue(v);

			}
			else if (currentMouseAction == CLIC_END) {
				Point<float> delta(layoutX, layoutY);
				delta.x -= (float)currentMousePath->position->getValue()[0];
				delta.y -= (float)currentMousePath->position->getValue()[1];

				currentMousePath->rodSize->setValue( delta.getDistanceFromOrigin() *2);
				currentMousePath->rodAngle->setValue(radiansToDegrees( BKPath::getVectAngle(&delta)));
			}

		}
		else if(type == BKPath::PATH_GRID) {
			if (currentMouseAction == CLIC_DRAG) {
				var v;
				v.append(layoutX + deltaMouseX);
				v.append(layoutY + deltaMouseY);
				currentMousePath->position->setValue(v);
			}
			else if (currentMouseAction == CLIC_TL || currentMouseAction == CLIC_TR || currentMouseAction == CLIC_BL || currentMouseAction == CLIC_BR) {
				float originX = fixedMouseX + ((layoutX - fixedMouseX) / 2);
				float originY = fixedMouseY + ((layoutY - fixedMouseY) / 2);

				Point<float> diag(layoutX-fixedMouseX, layoutY-fixedMouseY);
				Point<float> xAxis(1, 0);
				Point<float> yAxis(0, 1);
				BKPath::rotateVect(&xAxis, currentMousePath->gridAngle->floatValue());
				BKPath::rotateVect(&yAxis, currentMousePath->gridAngle->floatValue());
				float width = abs(diag.getDotProduct(xAxis));
				float height = abs(diag.getDotProduct(yAxis));

				var v;
				v.append(width);
				v.append(height);
				currentMousePath->gridSize->setValue(v);
				v[0] = originX;
				v[1] = originY;
				currentMousePath->position->setValue(v);
			}
			else if (currentMouseAction == CLIC_ROTATE) {
				Point<float> origin((float)currentMousePath->position->getValue()[0], (float)currentMousePath->position->getValue()[1]);
				Point<float> mouse(layoutX, layoutY);
				mouse -= origin;
				float angle = BKPath::getVectAngle(&mouse);
				currentMousePath->gridAngle->setValue(radiansToDegrees(angle));
			}
		}
		if (type == BKPath::PATH_CIRCLE) {
			if (currentMouseAction == CLIC_DRAG) {
				var v;
				v.append(layoutX + deltaMouseX);
				v.append(layoutY + deltaMouseY);
				currentMousePath->position->setValue(v);
			}
		}

	}
	else {
		clickTracker(e);
	}
}

void LayoutViewer::mouseUp(const MouseEvent& e)
{

	currentMouseAction = CLIC_NOACTION;
	currentMousePath = nullptr;
}

void LayoutViewer::mouseMove(const MouseEvent& e)
{
	if (selectedLayout != nullptr && currentMousePath == nullptr) {
		BKPath* newHovered = nullptr;
		int x = e.getMouseDownX();
		int y = e.getMouseDownY();

		int r = clicZones.getPixelAt(x, y).getRed();
		int g = clicZones.getPixelAt(x, y).getGreen();
		int b = clicZones.getPixelAt(x, y).getBlue();

		int c = (((b * 256) + g) * 256) + r;

		if (colourToPath.contains(c) && editMode.getToggleState()) {
			newHovered = colourToPath.getReference(c);
		}
		else {
			newHovered = nullptr;
		}

		if (newHovered != hoveredPath) {
			hoveredPath = newHovered;
			repaint();
		}

	}
}

void LayoutViewer::clickTracker(const MouseEvent& e)
{
	if (selectedLayout == nullptr) return;
	if (!selectedLayout->controlTracker->boolValue()) return;
	Tracker* t = Brain::getInstance()->getTrackerById(selectedLayout->trackerId->intValue());
	if (t == nullptr) return;

	float layoutX = jmap(float(e.position.getX()), topLeftX, bottomRightX, (float)selectedLayout->dimensionsX->getValue()[0], (float)selectedLayout->dimensionsX->getValue()[1]);
	float layoutY = jmap(float(e.position.getY()), topLeftY, bottomRightY, (float)selectedLayout->dimensionsY->getValue()[1], (float)selectedLayout->dimensionsY->getValue()[0]);
	float third = selectedLayout->trackerThirdValue->floatValue();
	String plane = selectedLayout->trackerPlane->getValue();
	if (plane == "XY") {
		t->targetPosition->setVector(layoutX, layoutY, third);
	}
	else if (plane == "XZ") {
		t->targetPosition->setVector(layoutX, third, layoutY);
	}
	else if (plane == "YZ") {
		t->targetPosition->setVector(third, layoutX, layoutY);
	}
	
}

void LayoutViewer::changeListenerCallback(ChangeBroadcaster* source)
{
	repaint();
}

void LayoutViewer::drawMidArrow(Graphics& g, Point<float>& from, Point<float>& to)
{
	Point<float> endArrow;
	Line<float> line;
	endArrow = ((to - from)/2.0f);
	float arrSize = endArrow.getDistanceFromOrigin();
	float ratio = (arrSize + 4) / arrSize;
	endArrow *= ratio;
	endArrow += from;
	line.setStart(from);
	line.setEnd(endArrow);
	g.drawArrow(line, 0, 6, 8);
}

void LayoutViewer::drawMidArrow(Graphics& g, float fromX, float fromY, float toX, float toY)
{
	Point<float>from;
	from.setXY(fromX, fromY);
	Point<float>to;
	to.setXY(toX, toY);
	drawMidArrow(g, from, to);
}

void LayoutViewer::paint(Graphics& g)
{
	Brain::getInstance()->layoutViewerNeedRepaint = false;
	if (Brain::getInstance()->layoutViewerNeedRefresh == true && selectedLayout!=nullptr) {
		Brain::getInstance()->layoutViewerNeedRefresh = false;
		selectedLayout->computeData();
	}
	clicZones = Image(Image::ARGB, getWidth(), getHeight(), true);
	Graphics clicg = Graphics(clicZones);
	g.fillAll(Colour(0,0,0));
	clicg.fillAll(Colour(0.f, 0.f, 0.f, 0.f));
	resetClickColour();

	if (selectedLayout == nullptr) {
		g.setColour(Colours::white.withAlpha(.8f));
		g.setFont(jlimit<float>(12, 20, getHeight() - 30));
		g.drawText("No layout selected", getLocalBounds().toFloat(), Justification::centred, false);
		return;
	}

	float scaleTileX = selectedLayout->tilesScale->getValue()[0];
	float scaleTileY = selectedLayout->tilesScale->getValue()[1];
	float scaleText = selectedLayout->textScale->floatValue();

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
	float windowRatio = (float)getWidth() / ((float)getHeight()-20);
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
		originY = 20;
		height = getHeight()-20;
		width = height * layoutRatio;
		originX = getWidth()-width;
		originX /= 2;
	}

	topLeftX = originX;
	topLeftY = originY;
	bottomRightX = originX + width;
	bottomRightY = originY + height;
	uiScale = width/layoutWidth;

	g.reduceClipRegion(originX, originY, width, height);
	g.setOrigin(originX, originY);
	g.fillAll(Colour(32, 32, 32));

	clicg.reduceClipRegion(originX, originY, width, height);
	clicg.setOrigin(originX, originY);

	// ici
	if (selectedLayout->backgroundImage.isValid()) {
		g.saveState();
		g.setOpacity(selectedLayout->backgroundAlpha->floatValue());
		g.drawImageWithin(selectedLayout->backgroundImage, 0,0,width, height,RectanglePlacement());
		g.restoreState();
	}

	if (selectedLayout->viewGrid->boolValue()) {
		float currentX = 0;
		float currentY = 0;
		g.setColour(selectedLayout->gridColor->getColor());
		float fromX = (float)selectedLayout->dimensionsX->getValue()[0];
		float toX = (float)selectedLayout->dimensionsX->getValue()[1];
		float fromY = (float)selectedLayout->dimensionsY->getValue()[0];
		float toY = (float)selectedLayout->dimensionsY->getValue()[1];
		float gridX = (float)selectedLayout->gridSize->getValue()[0];
		float gridY = (float)selectedLayout->gridSize->getValue()[1];
		if (gridX > 0.1) {
			while (currentX < jmax(abs(toX), abs(fromX))) {
				float x = jmap(currentX, fromX, toX, 0.0f, width);
				int strokeSize = currentX == 0 ? 3 : 1;
				g.drawLine(x, 0, x, height, strokeSize);
				if (currentX != 0) {
					x = jmap(-currentX, fromX, toX, 0.0f, width);
					g.drawLine(x, 0, x, height, 1);
				}
				currentX += gridX;
			}
		}
		if (gridY > 0.1) {
			while (currentY < jmax(abs(toY), abs(fromY))) {
				float y = jmap(currentY, toY, fromY, 0.0f, height);
				int strokeSize = currentY == 0 ? 3 : 1;
				g.drawLine(0, y, width, y, strokeSize);
				if (currentY != 0) {
					y = jmap(-currentY, toY, fromY, 0.0f, height);
					g.drawLine(0, y, width, y, 1);
				}
				currentY += gridY;
			}
		}
	}

	float handleWidth = 10;
	float halfHandleWidth = handleWidth/2;

	//int currentClickColor = 1;
	colourToPath.clear();
	colourToSubFixture.clear();
	colourToFixture.clear();
	colourToAction.clear();

	for (int iPath = 0; iPath < selectedLayout->paths.items.size(); iPath++) {
		BKPath* p = selectedLayout->paths.items[iPath];
		if (!p->enabled->boolValue()) continue;
		BKPath::PathType type = p->pathType->getValueDataAsEnum<BKPath::PathType>(); //::PATH_LINE) 
		BKPath::LabelPosition labelPos = p->labelPosition->getValueDataAsEnum<BKPath::LabelPosition>();
		Colour hoverColour((uint8)255, (uint8)255, (uint8)255, (uint8)63);
		Colour handleColour((uint8)255, (uint8)255, (uint8)255);

		float tileWidth = p->tilesSize->getValue()[0];
		float tileHeight = p->tilesSize->getValue()[1];

		tileWidth *= scaleTileX;
		tileHeight *= scaleTileY;
		float halfTileWidth = tileWidth/2;
		float halfTileHeight = tileHeight/2;

		g.setFont(p->textSize->floatValue()*scaleText);

		bool overrideColor = p->overrideStrokeColor->boolValue();
		Colour overridenColor = juce::Colours::black;
		if (p->overrideStrokeColor->boolValue()) {
			overridenColor = p->strokeColor->getColor();
		}

		bool drawPaths = viewPaths.getToggleState();
		bool edit = editMode.getToggleState();

		if (type == BKPath::PATH_POINT) {
			float fromX = jmap((float)p->position->getValue()[0], (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
			float fromY = jmap((float)p->position->getValue()[1], (float)dimensionY[1], (float)dimensionY[0], (float)0, height);

			if (edit) {
				clicg.setColour(getClickColour(p, CLIC_DRAG));
				clicg.fillEllipse(fromX - halfHandleWidth, fromY - halfHandleWidth, handleWidth, handleWidth);
			}
			else {
				if (p->selection.computedSelectedSubFixtures.size() == 1 && p->spreadSubFixtures->boolValue()) {
					clicg.setColour(getClickColour(p->selection.computedSelectedSubFixtures[0]));
				}
				else if (p->selection.computedSelectedFixtures.size() == 1 && !p->spreadSubFixtures->boolValue()) {
					clicg.setColour(getClickColour(p->selection.computedSelectedSubFixtures[0]->parentFixture));
				}
				else {
					clicg.setColour(getClickColour(p, CLIC_SELECT));
				}
				clicg.fillRect(fromX - halfTileWidth, fromY - halfTileHeight, tileWidth, tileHeight);
			}
			Colour drawColor = juce::Colours::white;
			if (overrideColor) {drawColor = overridenColor; }
			else if (p->selection.computedSelectedSubFixtures.size() > 0) {
				drawColor = p->selection.computedSelectedSubFixtures[0]->parentFixture->getLayoutStrokeColor();
			}

			bool drawed = false;

			if (p->spreadSubFixtures->boolValue() && p->selection.computedSelectedSubFixtures.size() == 1 ) {
				drawed = true;
				drawSubFixture(g, p->selection.computedSelectedSubFixtures[0], fromX - halfTileWidth, fromY - halfTileHeight, tileWidth, tileHeight, drawColor, labelPos);
			}
			if (!drawed && !p->spreadSubFixtures->boolValue() && p->selection.computedSelectedFixtures.size() == 1) {
				drawed = true;
				float angle = p->fixturesAngleFrom->floatValue();
				drawFixture(g, p->selection.computedSelectedSubFixtures[0]->parentFixture,p , fromX - halfTileWidth, fromY - halfTileHeight, tileWidth, tileHeight, angle, drawColor, labelPos);
			}
			if (!drawed) {
				g.setColour(drawColor);
				g.drawRect(fromX - halfTileWidth, fromY - halfTileHeight, tileWidth, tileHeight, (float)1);
				String name = "";
				if (p->customText->stringValue() != "") {
					name = p->customText->stringValue().trim();
				}
				g.drawText(name, fromX - halfTileWidth, fromY - halfTileHeight, tileWidth, tileHeight, juce::Justification::centred);

			}
			if (p == hoveredPath) {
				g.setColour(handleColour);
				g.fillEllipse(fromX - halfHandleWidth, fromY - halfHandleWidth, handleWidth, handleWidth);
			}

		}
		if (type == BKPath::PATH_LINE) {
			float fromX = jmap((float)p->position->getValue()[0], (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
			float fromY = jmap((float)p->position->getValue()[1], (float)dimensionY[1], (float)dimensionY[0], (float)0, height);
			float toX = jmap((float)p->lineEndPosition->getValue()[0], (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
			float toY = jmap((float)p->lineEndPosition->getValue()[1], (float)dimensionY[1], (float)dimensionY[0], (float)0, height);
			Line<float> line(Point<float>(fromX, fromY), Point<float>(toX, toY));

			if (drawPaths) { // wanna draw lines ?
				g.setColour(juce::Colours::lightgrey);
				g.drawLine(line, 0.5f);
			}

			if (edit) {
				clicg.setColour(getClickColour(p, CLIC_DRAG));
				clicg.drawLine(line, handleWidth);
				clicg.setColour(getClickColour(p, CLIC_ORIGIN));
				clicg.fillEllipse(fromX - halfHandleWidth, fromY - halfHandleWidth, handleWidth, handleWidth);
				clicg.setColour(getClickColour(p, CLIC_END));
				clicg.fillEllipse(toX - halfHandleWidth, toY - halfHandleWidth, handleWidth, handleWidth);
			}
			float currentArrowX = -1;
			float currentArrowY = -1;
			if (p->spreadSubFixtures->boolValue()) {
				p->isComputing.enter();
				for (int iFixt = 0; iFixt < p->selection.computedSelectedSubFixtures.size(); iFixt++) {
					SubFixture* sf = p->selection.computedSelectedSubFixtures[iFixt];
					if (p->subFixtToPos.contains(sf)) {
						std::shared_ptr<Point<float>> t = p->subFixtToPos.getReference(sf);
						float X = jmap((float)t->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
						float Y = jmap((float)t->y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height);
						if (drawPaths && (currentArrowX != -1 || currentArrowY != -1)) {
							g.setColour(juce::Colours::lightgrey);
							drawMidArrow(g, currentArrowX, currentArrowY, X, Y);
						}
						currentArrowX = X;
						currentArrowY = Y;
						Colour drawColor = juce::Colours::white;
						if (overrideColor) { drawColor = overridenColor; }
						else {
							drawColor = sf->parentFixture->getLayoutStrokeColor();
						}
						drawSubFixture(g, sf, X - halfTileWidth, Y - halfTileHeight, tileWidth, tileHeight, drawColor, labelPos);
						if (!edit) {
							clicg.setColour(getClickColour(sf));
							clicg.fillRect(X - halfTileWidth, Y - halfTileHeight, tileWidth, tileHeight);
						}
					}
				}
				p->isComputing.exit();
			}
			if (!p->spreadSubFixtures->boolValue()) {
				p->isComputing.enter();
				Array<Fixture*> drawedFixtures;
				for (int iFixt = 0; iFixt < p->selection.computedSelectedSubFixtures.size(); iFixt++) {
					SubFixture* sf = p->selection.computedSelectedSubFixtures[iFixt];
					if (p->fixtToPos.contains(sf->parentFixture) && !drawedFixtures.contains(sf->parentFixture)) {
						std::shared_ptr<Point<float>> t = p->fixtToPos.getReference(sf->parentFixture);
						float X = jmap((float)t->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
						float Y = jmap((float)t->y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height);
						if (drawPaths && (currentArrowX != -1 || currentArrowY != -1)) {
							g.setColour(juce::Colours::lightgrey);
							drawMidArrow(g, currentArrowX, currentArrowY, X, Y);
						}
						currentArrowX = X;
						currentArrowY = Y;
						Colour drawColor = juce::Colours::white;
						if (overrideColor) { drawColor = overridenColor; }
						else {
							drawColor = sf->parentFixture->getLayoutStrokeColor();
						}
						float angle = jmap((float)iFixt, (float)0, (float)p->selection.computedSelectedSubFixtures.size(), p->fixturesAngleFrom->floatValue(), p->fixturesAngleTo->floatValue());
						drawFixture(g, sf->parentFixture,p , X - halfTileWidth, Y - halfTileHeight, tileWidth, tileHeight, angle, drawColor, labelPos);
						if (!edit) {
							clicg.setColour(getClickColour(sf->parentFixture));
							clicg.fillRect(X - halfTileWidth, Y - halfTileHeight, tileWidth, tileHeight);
						}
					}
				}
				p->isComputing.exit();
			}

			if (p == hoveredPath) {
				g.setColour(hoverColour);
				g.drawLine(line, halfHandleWidth);
				g.setColour(handleColour);
				g.fillEllipse(fromX - halfHandleWidth, fromY - halfHandleWidth, handleWidth, handleWidth);
				g.fillEllipse(toX - halfHandleWidth, toY - halfHandleWidth, handleWidth, handleWidth);
			}

		}
		if (type == BKPath::PATH_ROD) {
			Point tl(jmap(p->gridTL.x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width), jmap(p->gridTL.y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height));
			Point tr(jmap(p->gridTR.x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width), jmap(p->gridTR.y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height));
			Line<float> line(tl, tr);

			if (drawPaths) { // wanna draw lines ?
				g.setColour(juce::Colours::lightgrey);
				g.drawLine(line, 0.5f);
			}

			clicg.setColour(getClickColour(p, CLIC_DRAG));
			clicg.drawLine(line, handleWidth);
			//clicg.setColour(getClickColour(p, CLIC_ORIGIN));
			//clicg.fillEllipse(tl.x - halfHandleWidth, tl.y - halfHandleWidth, handleWidth, handleWidth);
			clicg.setColour(getClickColour(p, CLIC_END));
			clicg.fillEllipse(tr.x - halfHandleWidth, tr.y - halfHandleWidth, handleWidth, handleWidth);
			float currentArrowX = -1;
			float currentArrowY = -1;
			if (p->spreadSubFixtures->boolValue()) {
				p->isComputing.enter();
				for (int iFixt = 0; iFixt < p->selection.computedSelectedSubFixtures.size(); iFixt++) {
					SubFixture* sf = p->selection.computedSelectedSubFixtures[iFixt];
					if (p->subFixtToPos.contains(sf)) {
						std::shared_ptr<Point<float>> t = p->subFixtToPos.getReference(sf);
						float X = jmap((float)t->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
						float Y = jmap((float)t->y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height);
						if (drawPaths && (currentArrowX != -1 || currentArrowY != -1)) {
							g.setColour(juce::Colours::lightgrey);
							drawMidArrow(g, currentArrowX, currentArrowY, X, Y);
						}
						currentArrowX = X;
						currentArrowY = Y;
						Colour drawColor = juce::Colours::white;
						if (overrideColor) { drawColor = overridenColor; }
						else {
							drawColor = sf->parentFixture->getLayoutStrokeColor();
						}
						drawSubFixture(g, sf, X - halfTileWidth, Y - halfTileHeight, tileWidth, tileHeight, drawColor, labelPos);
						if (!edit) {
							clicg.setColour(getClickColour(sf));
							clicg.fillRect(X - halfTileWidth, Y - halfTileHeight, tileWidth, tileHeight);
						}
					}
				}
				p->isComputing.exit();
			}
			if (!p->spreadSubFixtures->boolValue()) {
				p->isComputing.enter();
				Array<Fixture*> drawedFixtures;
				for (int iFixt = 0; iFixt < p->selection.computedSelectedSubFixtures.size(); iFixt++) {
					SubFixture* sf = p->selection.computedSelectedSubFixtures[iFixt];
					if (p->fixtToPos.contains(sf->parentFixture) && !drawedFixtures.contains(sf->parentFixture)) {
						std::shared_ptr<Point<float>> t = p->fixtToPos.getReference(sf->parentFixture);
						float X = jmap((float)t->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
						float Y = jmap((float)t->y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height);
						if (drawPaths && (currentArrowX != -1 || currentArrowY != -1)) {
							g.setColour(juce::Colours::lightgrey);
							drawMidArrow(g, currentArrowX, currentArrowY, X, Y);
						}
						currentArrowX = X;
						currentArrowY = Y;
						Colour drawColor = juce::Colours::white;
						if (overrideColor) { drawColor = overridenColor; }
						else {
							drawColor = sf->parentFixture->getLayoutStrokeColor();
						}
						float angle = jmap((float)iFixt, (float)0, (float)p->selection.computedSelectedSubFixtures.size(), p->fixturesAngleFrom->floatValue(), p->fixturesAngleTo->floatValue());
						drawFixture(g, sf->parentFixture,p , X - halfTileWidth, Y - halfTileWidth, tileWidth, tileHeight, angle, drawColor, labelPos);
						if (!edit) {
							clicg.setColour(getClickColour(sf->parentFixture));
							clicg.fillRect(X - halfTileWidth, Y - halfTileHeight, tileWidth, tileHeight);
						}
					}
				}
				p->isComputing.exit();
			}

			if (p == hoveredPath) {
				g.setColour(hoverColour);
				g.drawLine(line, halfHandleWidth);
				g.setColour(handleColour);
				//g.fillEllipse(tl.x - halfHandleWidth, tl.y - halfHandleWidth, handleWidth, handleWidth);
				g.fillEllipse(tr.x - halfHandleWidth, tr.y - halfHandleWidth, handleWidth, handleWidth);
			}

		}
		if (type == BKPath::PATH_GRID) {
			if (drawPaths) {
				g.setColour(juce::Colours::lightgrey);
				for (int iGrid = 0; iGrid < p->gridPath.size() - 1; iGrid++) {
					float fromX = jmap(p->gridPath[iGrid]->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
					float fromY = jmap(p->gridPath[iGrid]->y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height);
					float toX = jmap(p->gridPath[iGrid + 1]->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
					float toY = jmap(p->gridPath[iGrid + 1]->y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height);
					Line<float> line(Point<float>(fromX, fromY), Point<float>(toX, toY));
					g.drawLine(line, 0.5f);
					drawMidArrow(g, fromX, fromY, toX, toY);
				}
			}

			Point tl (jmap(p->gridTL.x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width), jmap(p->gridTL.y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height));
			Point tr (jmap(p->gridTR.x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width), jmap(p->gridTR.y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height));
			Point bl (jmap(p->gridBL.x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width), jmap(p->gridBL.y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height));
			Point br (jmap(p->gridBR.x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width), jmap(p->gridBR.y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height));
			Point rotate(tr);
			rotate -= br;
			rotate /= 2;
			rotate += br;

			//g.setColour(juce::Colours::orange);
			if (p->spreadSubFixtures->boolValue()) {
				p->isComputing.enter();
				for (auto it = p->subFixtToPos.begin(); it != p->subFixtToPos.end(); it.next()) {
					float X = jmap((float)it.getValue()->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
					float Y = jmap((float)it.getValue()->y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height);
					Colour drawColor = juce::Colours::white;
					if (overrideColor) { drawColor = overridenColor; }
					else {
						drawColor = it.getKey()->parentFixture->getLayoutStrokeColor();
					}
					drawSubFixture(g, it.getKey(), X - halfTileWidth, Y - halfTileWidth, tileWidth, tileHeight, drawColor, labelPos);
					if (!edit) {
						clicg.setColour(getClickColour(it.getKey()));
						clicg.fillRect(X - halfTileWidth, Y - halfTileHeight, tileWidth, tileHeight);
					}
				}
				p->isComputing.exit();
			}
			if (!p->spreadSubFixtures->boolValue()) {
				p->isComputing.enter();
				int iFixt = 0;
				for (auto it = p->fixtToPos.begin(); it != p->fixtToPos.end(); it.next()) {
					float X = jmap((float)it.getValue()->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
					float Y = jmap((float)it.getValue()->y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height);
					Colour drawColor = juce::Colours::white;
					if (overrideColor) { drawColor = overridenColor; }
					else {
						drawColor = it.getKey()->getLayoutStrokeColor();
					}
					iFixt++;
					float angle = jmap((float)iFixt, (float)0, (float)p->selection.computedSelectedSubFixtures.size(), p->fixturesAngleFrom->floatValue(), p->fixturesAngleTo->floatValue());
					drawFixture(g, it.getKey(),p , X - halfTileWidth, Y - halfTileWidth, tileWidth, tileHeight, angle, drawColor, labelPos);
					if (!edit) {
						clicg.setColour(getClickColour(it.getKey()));
						clicg.fillRect(X - halfTileWidth, Y - halfTileHeight, tileWidth, tileHeight);
					}
				}
				p->isComputing.exit();
			}

			if (edit) {
				Path path;
				path.startNewSubPath(tl);
				path.lineTo(tr);
				path.lineTo(br);
				path.lineTo(bl);
				path.closeSubPath();
				clicg.setColour(getClickColour(p, CLIC_DRAG));
				clicg.fillPath(path);

				clicg.setColour(getClickColour(p, CLIC_TL));
				clicg.fillEllipse(tl.x - halfHandleWidth, tl.y - halfHandleWidth, handleWidth, handleWidth);
				clicg.setColour(getClickColour(p, CLIC_TR));
				clicg.fillEllipse(tr.x - halfHandleWidth, tr.y - halfHandleWidth, handleWidth, handleWidth);
				clicg.setColour(getClickColour(p, CLIC_BL));
				clicg.fillEllipse(bl.x - halfHandleWidth, bl.y - halfHandleWidth, handleWidth, handleWidth);
				clicg.setColour(getClickColour(p, CLIC_BR));
				clicg.fillEllipse(br.x - halfHandleWidth, br.y - halfHandleWidth, handleWidth, handleWidth);
				clicg.setColour(getClickColour(p, CLIC_ROTATE));
				clicg.fillEllipse(rotate.x - halfHandleWidth, rotate.y - halfHandleWidth, handleWidth, handleWidth);

				if (p == hoveredPath) {
					g.setColour(hoverColour);
					g.fillPath(path);
					g.setColour(handleColour);
					g.fillEllipse(tl.x - halfHandleWidth, tl.y - halfHandleWidth, handleWidth, handleWidth);
					g.fillEllipse(tr.x - halfHandleWidth, tr.y - halfHandleWidth, handleWidth, handleWidth);
					g.fillEllipse(bl.x - halfHandleWidth, bl.y - halfHandleWidth, handleWidth, handleWidth);
					g.fillEllipse(br.x - halfHandleWidth, br.y - halfHandleWidth, handleWidth, handleWidth);
					g.fillEllipse(rotate.x - halfHandleWidth, rotate.y - halfHandleWidth, handleWidth, handleWidth);
				}

			}

		}
		if (type == BKPath::PATH_CIRCLE) {
			float from = (p->circleFrom->floatValue() / 360.) * 2 * MathConstants<float>::pi;
			float to = (p->circleTo->floatValue() / 360.) * 2 * MathConstants<float>::pi;
			from += MathConstants<float>::pi / 2;
			to += MathConstants<float>::pi / 2;
			float X = jmap(p->position->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
			float Y = jmap(p->position->y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height);
			float W = p->circleRadius->floatValue() * uiScale;
			Path path;
			path.addArc(X - (W), Y - (W), W * 2, W * 2, from, to, true);
			if (drawPaths) {
				g.setColour(juce::Colours::lightgrey);
				g.strokePath(path, PathStrokeType(0.5));
			}
			if (edit) {
				clicg.setColour(getClickColour(p, CLIC_DRAG));
				clicg.strokePath(path, PathStrokeType(handleWidth));
			}
			if (p == hoveredPath) {
				g.setColour(hoverColour);
				g.strokePath(path, PathStrokeType(handleWidth));
			}

			//g.setColour(juce::Colours::orange);
			if (p->spreadSubFixtures->boolValue()) {
				p->isComputing.enter();
				for (auto it = p->subFixtToPos.begin(); it != p->subFixtToPos.end(); it.next()) {
					float XFixt = jmap((float)it.getValue()->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
					float YFixt = jmap((float)it.getValue()->y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height);
					Colour drawColor = juce::Colours::white;
					if (overrideColor) { drawColor = overridenColor; }
					else {
						drawColor = it.getKey()->parentFixture->getLayoutStrokeColor();
					}
					drawSubFixture(g, it.getKey(), XFixt - halfTileWidth, YFixt - halfTileWidth, tileWidth, tileHeight, drawColor, labelPos);
					if (!edit) {
						clicg.setColour(getClickColour(it.getKey()));
						clicg.fillRect(XFixt - halfTileWidth, YFixt - halfTileHeight, tileWidth, tileHeight);
					}
				}
				p->isComputing.exit();
			}
			if (!p->spreadSubFixtures->boolValue()) {
				p->isComputing.enter();
				int iFixt = 0;
				for (auto it = p->fixtToPos.begin(); it != p->fixtToPos.end(); it.next()) {
					float XFixt = jmap((float)it.getValue()->x, (float)dimensionX[0], (float)dimensionX[1], (float)0, width);
					float YFixt = jmap((float)it.getValue()->y, (float)dimensionY[1], (float)dimensionY[0], (float)0, height);
					Colour drawColor = juce::Colours::white;
					if (overrideColor) { drawColor = overridenColor; }
					else {
						drawColor = it.getKey()->getLayoutStrokeColor();
					}
					int num = p->selection.computedSelectedFixtures.indexOf(it.getKey());
					float angle = jmap((float)num, (float)0, (float)p->selection.computedSelectedFixtures.size(), p->fixturesAngleFrom->floatValue(), p->fixturesAngleTo->floatValue());
					drawFixture(g, it.getKey(),p , XFixt - halfTileWidth, YFixt - halfTileWidth, tileWidth, tileHeight, angle, drawColor, labelPos);
					if (!edit) {
						clicg.setColour(getClickColour(it.getKey()));
						clicg.fillRect(XFixt - halfTileWidth, YFixt - halfTileWidth, tileWidth, tileHeight);
					}
					iFixt++;
				}
				p->isComputing.exit();
			}
		}
	}
}

void LayoutViewer::stopAndCheckTimer()
{
}

void LayoutViewer::drawFixture(Graphics& g, Fixture* f, BKPath* path, float x, float y, float w, float h, float angle, Colour c, BKPath::LabelPosition pos)
{
	if (selectedLayout == nullptr) return;
	bool fillBox = selectedLayout->viewOutput->boolValue();
	Colour forcedFillColor = path->fillColor->getColor();

	bool fixtureIsSelected = false;
	Programmer* prog = UserInputManager::getInstance()->getProgrammer(false);
	if (prog != nullptr && prog->currentUserCommand != nullptr) {
		if (prog->currentUserCommand->selection.computedSelectedFixtures.contains(f)) {
			fixtureIsSelected = true;
		}
	}



	FixtureType* ft = dynamic_cast<FixtureType*>(f->devTypeParam->targetContainer.get());
	if (ft != nullptr && ft->useLayoutIcon) {
		g.setColour(c);

		if (!path->fixtImageContent.contains(f)) {
			int size = sqrt(pow(ft->layoutContentImage.getWidth(), 2) + pow(ft->layoutContentImage.getHeight(), 2));
			Image i(Image::PixelFormat::ARGB, size, size, true);
			Graphics tempG(i);
			AffineTransform t;
			t = t.translated(-ft->layoutContentImage.getWidth() / 2, -ft->layoutContentImage.getHeight() / 2);
			t = t.rotated(degreesToRadians(angle));
			//t = t.translated(ft->layoutContentImage.getWidth() / 2, ft->layoutContentImage.getHeight() / 2);
			t = t.translated(size / 2, size / 2);
			tempG.setColour(c);
			tempG.drawImageTransformed(ft->layoutContentImage, t, false);
			path->fixtImageContent.set(f, i);
		}

		if (!path->fixtImageBorder.contains(f)) {
			int size = sqrt(pow(ft->layoutBorderImage.getWidth(), 2) + pow(ft->layoutBorderImage.getHeight(), 2));
			Image i(Image::PixelFormat::ARGB, size, size, true);
			Graphics tempG(i);
			AffineTransform t;
			t = t.translated(-ft->layoutBorderImage.getWidth() / 2, -ft->layoutBorderImage.getHeight() / 2);
			t = t.rotated(degreesToRadians(angle));
			//t = t.translated(ft->layoutBorderImage.getWidth() / 2, ft->layoutBorderImage.getHeight() / 2);
			t = t.translated(size / 2, size / 2);
			tempG.setColour(c);
			tempG.drawImageTransformed(ft->layoutBorderImage, t, true);
			path->fixtImageBorder.set(f, i);
		}

		if (!path->fixtImageSelected.contains(f)) {
			int size = sqrt(pow(ft->layoutBorderImage.getWidth(), 2) + pow(ft->layoutBorderImage.getHeight(), 2));
			Image i(Image::PixelFormat::ARGB, size, size, true);
			Graphics tempG(i);
			AffineTransform t;
			t = t.translated(-ft->layoutBorderImage.getWidth() / 2, -ft->layoutBorderImage.getHeight() / 2);
			t = t.rotated(degreesToRadians(angle));
			//t = t.translated(ft->layoutBorderImage.getWidth() / 2, ft->layoutBorderImage.getHeight() / 2);
			t = t.translated(size / 2, size / 2);
			tempG.setColour(Colours::white.withAlpha(0.3f));
			tempG.drawImageTransformed(ft->layoutBorderImage, t.translated(-2, -2), true);
			tempG.drawImageTransformed(ft->layoutBorderImage, t.translated(-2, 2), true);
			tempG.drawImageTransformed(ft->layoutBorderImage, t.translated(2, -2), true);
			tempG.drawImageTransformed(ft->layoutBorderImage, t.translated(2, 2), true);
			//tempG.drawImageTransformed(ft->layoutContentImage, t.translated(-2, -2), true);
			//tempG.drawImageTransformed(ft->layoutContentImage, t.translated(-2, 2), true);
			//tempG.drawImageTransformed(ft->layoutContentImage, t.translated(2, -2), true);
			//tempG.drawImageTransformed(ft->layoutContentImage, t.translated(2, 2), true);
			path->fixtImageSelected.set(f, i);
		}

		if (!path->fixtTransform.contains(f)) {
			Rectangle<float> r(x, y, w, h);
			RectanglePlacement placement(0);
			AffineTransform t;
			Image i = path->fixtImageBorder.getReference(f);
			t = t.translated(-i.getWidth() / 2, -i.getHeight() / 2);
			//t = t.rotated(degreesToRadians(angle));
			t = t.translated(i.getWidth() / 2, i.getHeight() / 2);
			t = t.followedBy(placement.getTransformToFit(i.getBounds().toFloat(), r));
			path->fixtTransform.set(f, t);
		}

		if (fixtureIsSelected) {
			g.drawImageTransformed(path->fixtImageSelected.getReference(f), path->fixtTransform.getReference(f), false);
		}

		if (fillBox) {
			//g.saveState();


			float sfCount = f->subFixtures.size();
			if (sfCount > 0) {
				Image tempImage = path->fixtImageContent.getReference(f).createCopy();
				tempImage.clear(tempImage.getBounds(), Colour::fromRGBA(0, 0, 0, 0));
				Graphics tempGraphics(tempImage);
				tempGraphics.reduceClipRegion(path->fixtImageContent.getReference(f), AffineTransform());
				int i = 0;
				float wSub = tempImage.getWidth() / sfCount;
				float imgH = tempImage.getHeight();
				//g.reduceClipRegion(path->fixtImageContent.getReference(f), path->fixtTransform.getReference(f));
				for (auto it = f->subFixtures.begin(); it != f->subFixtures.end(); it.next()) {
					Colour fill = it.getValue()->getOutputColor();
					Colour mult = f->getLayoutFillColor();
					if (path->overrideFillColor->boolValue()) mult = forcedFillColor;
					float red = fill.getFloatRed() * mult.getFloatRed();
					float green = fill.getFloatGreen() * mult.getFloatGreen();
					float blue = fill.getFloatBlue() * mult.getFloatBlue();
					fill = Colour(red,green,blue, 1.0f,1.0f);
					tempGraphics.setColour(fill);
					tempGraphics.fillRect(0.0 + (i * wSub), 0.0, wSub, imgH);
					i++;
				}
				g.drawImageTransformed(tempImage, path->fixtTransform.getReference(f), false);
			}
			//g.restoreState();
		}

		g.drawImageTransformed(path->fixtImageBorder.getReference(f), path->fixtTransform.getReference(f), false);
		//g.drawImageWithin(ft->layoutBorderImage, x, y, w, h, RectanglePlacement::fillDestination, true);
	}
	else {
		if (fixtureIsSelected) {
			g.setColour(Colours::white.withAlpha(0.3f));
			g.drawRect(x-2, y-2, w+4, h+4, (float)5);
		}
		if (fillBox) {
			float sfCount = f->subFixtures.size();
			if (sfCount == 0) return;
			int i = 0;
			float wSub = w / sfCount;
			for (auto it = f->subFixtures.begin(); it != f->subFixtures.end(); it.next()) {
				Colour fill = it.getValue()->getOutputColor();
				Colour mult = f->getLayoutFillColor();
				if (path->overrideFillColor->boolValue()) mult = forcedFillColor;
				float red = fill.getFloatRed() * mult.getFloatRed();
				float green = fill.getFloatGreen() * mult.getFloatGreen();
				float blue = fill.getFloatBlue() * mult.getFloatBlue();
				fill = Colour(red, green, blue, 1.0f, 1.0f);
				g.setColour(fill);
				g.fillRect(x + (i * wSub), y, wSub, h);
				i++;

			}
		}
		g.setColour(c);
		g.drawRect(x, y, w, h, (float)1);
	}

	String name = f->id->stringValue();
	drawName(g, name, x, y, w, h, c, pos);

}

void LayoutViewer::drawSubFixture(Graphics& g, SubFixture* sf, float x, float y, float w, float h, Colour c, BKPath::LabelPosition pos)
{
	if (selectedLayout == nullptr) return;
	bool fillBox = selectedLayout->viewOutput->boolValue();

	bool subFixtureIsSelected = false;
	Programmer* prog = UserInputManager::getInstance()->getProgrammer(false);
	if (prog != nullptr && prog->currentUserCommand != nullptr) {
		if (prog->currentUserCommand->selection.computedSelectedSubFixtures.contains(sf)) {
			subFixtureIsSelected = true;
		}
	}

	if (subFixtureIsSelected) {
		g.setColour(Colours::white.withAlpha(0.3f));
		g.drawRect(x-2, y-2, w+4, h+4, (float)5);

	}
	if (fillBox) {
		Colour fill = sf->getOutputColor();
		g.setColour(fill);
		g.fillRect(x, y, w, h);
	}
	g.setColour(c);
	g.drawRect(x, y, w, h, (float)1);
	String name = sf->displayName;
	drawName(g, name, x, y, w, h, c, pos);
}

void LayoutViewer::drawName(Graphics& g, String& name, float x, float y, float w, float h, Colour c, BKPath::LabelPosition pos)
{
	if (pos == BKPath::LabelPosition::HIDDEN) return;
	juce::Justification j = juce::Justification::centred;
	switch (pos) {
	case BKPath::LabelPosition::LEFT:
		j = juce::Justification::centredRight;
		x -= w;
		break;
	case BKPath::LabelPosition::RIGHT:
		j = juce::Justification::centredLeft;
		x += w;
		break;
	case BKPath::LabelPosition::TOP:
		j = juce::Justification::centredBottom;
		y -= h;
		break;
	case BKPath::LabelPosition::BOTTOM:
		j = juce::Justification::centredTop;
		y += h;
		break;
	}

	g.setColour(Colour(0, 0, 0));
	g.drawText(name, x - 1, y - 1, w, h, j);
	g.drawText(name, x - 1, y + 1, w, h, j);
	g.drawText(name, x + 1, y - 1, w, h, j);
	g.drawText(name, x + 1, y + 1, w, h, j);
	g.setColour(c);
	g.drawText(name, x, y, w, h, j);
}

void LayoutViewer::exportToPNG()
{
	FileChooser fc("Save Layout as PNG", File::getCurrentWorkingDirectory(), "*.png");
	if (!fc.browseForFileToSave(true)) return;

	File file = fc.getResult();
	if (file.exists()) file.deleteFile();

	Image myImage(Image::RGB, currentWidth, currentHeight, true);
	Graphics g(myImage);

	paint(g);

	FileOutputStream stream(file);
	PNGImageFormat pngWriter;
	pngWriter.writeImageToStream(myImage, stream);
}

bool LayoutViewer::isInterestedInDragSource(const SourceDetails& source)
{
	if (source.description.getProperty("type", "") == "GridViewButton") {
		String targetType = source.description.getProperty("targetType", "");
		if (targetType == "Fixture") return true;
		if (targetType == "Group") return true;
	}
	return false;
}

void LayoutViewer::itemDropped(const SourceDetails& source)
{
	if (selectedLayout == nullptr) return;
	if (source.description.getProperty("type", "") == "GridViewButton") {
		String targetType = source.description.getProperty("targetType", "");
		var id = source.description.getProperty("id", 0);
		BKPath* path = selectedLayout->paths.addItem();
		CommandSelection* sel = path->selection.addItem();
		path->setNiceName(targetType+" "+id.toString());
		sel->valueFrom->setValue(id);
		if (targetType == "Group") sel->targetType->setValueWithKey("Group");

		Point<int> mousePos = getMouseXYRelative();
		float layoutX = jmap(float(mousePos.x), topLeftX, bottomRightX, (float)selectedLayout->dimensionsX->getValue()[0], (float)selectedLayout->dimensionsX->getValue()[1]);
		float layoutY = jmap(float(mousePos.y), topLeftY, bottomRightY, (float)selectedLayout->dimensionsY->getValue()[1], (float)selectedLayout->dimensionsY->getValue()[0]);
		var pos = layoutX;
		pos.append(layoutY);
		path->position->setValue(pos);
		path->selectThis();

	}
}

void LayoutViewer::timerCallback()
{
	if (selectedLayout != nullptr && selectedLayout->viewOutput->boolValue()) {
		repaint();
	}
	else if (Brain::getInstance()->layoutViewerNeedRefresh || Brain::getInstance()->layoutViewerNeedRepaint){
		repaint();
	}
}


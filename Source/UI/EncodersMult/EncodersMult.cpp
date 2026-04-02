/*
  ==============================================================================

	EncodersMult.cpp
	Created: 8 Oct 2020 6:59:51pm
	Author:  bkupe

  ==============================================================================
*/

#include "EncodersMult.h"
#include "UserInputManager.h"
#include "Definitions/Programmer/Programmer.h"
#include "EncodersMultCmd.h"

EncodersMult::EncodersMult() 
	//testingUI(nullptr),
{
    addAndMakeVisible(btnSelectAll); 
    btnSelectAll.setWantsKeyboardFocus(false);
    btnSelectAll.setButtonText("Select all");
    btnSelectAll.onClick=[this](){selectAll();};
    addAndMakeVisible(btnSelectNone);
    btnSelectNone.setWantsKeyboardFocus(false);
    btnSelectNone.setButtonText("Select none");
    btnSelectNone.onClick = [this]() {selectNone();};
    addAndMakeVisible(btnAllEqual);
    btnAllEqual.setWantsKeyboardFocus(false);
    btnAllEqual.setButtonText("=");
    btnAllEqual.onClick = [this]() {selectAlign(Align::EQUAL); };
    addAndMakeVisible(btnMoveOnlyLeft);
    btnMoveOnlyLeft.setWantsKeyboardFocus(false);
    btnMoveOnlyLeft.setButtonText(">");
    btnMoveOnlyLeft.onClick = [this]() {selectAlign(Align::ONLYLEFT); };
    addAndMakeVisible(btnMoveOnlyRight);
    btnMoveOnlyRight.setWantsKeyboardFocus(false);
    btnMoveOnlyRight.setButtonText("<");
    btnMoveOnlyRight.onClick = [this]() {selectAlign(Align::ONLYRIGHT); };
    addAndMakeVisible(btnMoveOnlyIn);
    btnMoveOnlyIn.setWantsKeyboardFocus(false);
    btnMoveOnlyIn.setButtonText("<>");
    btnMoveOnlyIn.onClick = [this]() {selectAlign(Align::ONLYIN); };
    addAndMakeVisible(btnMoveOnlyOut);
    btnMoveOnlyOut.setWantsKeyboardFocus(false);
    btnMoveOnlyOut.setButtonText("><");
    btnMoveOnlyOut.onClick = [this]() {selectAlign(Align::ONLYOUT); };
    addAndMakeVisible(btnMoveBalanced);
    btnMoveBalanced.setWantsKeyboardFocus(false);
    btnMoveBalanced.setButtonText("\\");
    btnMoveBalanced.onClick = [this]() {selectAlign(Align::BALANCED); };
    addAndMakeVisible(btnMoveBalancedSym);
    btnMoveBalancedSym.setWantsKeyboardFocus(false);
    btnMoveBalancedSym.setButtonText("\\/");
    btnMoveBalancedSym.onClick = [this]() {selectAlign(Align::BALANCEDSYM); };


	addAndMakeVisible(viewport);
	viewport.setViewedComponent(&cmdContainer);
    viewport.setBounds(0,20,100,100);
    cmdContainer.setSize(200,200);

    addAndMakeVisible(sensitivity);
    sensitivity.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    sensitivity.setRange(0, 1);
    sensitivity.setValue(1, juce::dontSendNotification);
    sensitivity.setColour(Slider::rotarySliderFillColourId, Colour(0,127,0));
    sensitivity.setNumDecimalPlacesToDisplay(5);
    sensitivity.addListener(this);
    sensitivity.setWantsKeyboardFocus(false);
    sensitivity.addMouseListener(this, false);

    addAndMakeVisible(sensitivityLabel);
    sensitivityLabel.setText("Sensit.", juce::dontSendNotification);
    sensitivityLabel.attachToComponent(&sensitivity, false);
    sensitivityLabel.setWantsKeyboardFocus(false);

    updateAlignButtons();


}

juce_ImplementSingleton(EncodersMult);

EncodersMult::~EncodersMult()
{
    if (targetCommandManager != nullptr) {
        targetCommandManager->removeAsyncManagerListener(this);
    }
    //commandItems.clear();
}

void EncodersMult::newMessage(const CommandManager::ManagerEvent& e)
{
    reconstructSubComponents();
}

float EncodersMult::setSensitivity(float v)
{
    sensitivity.setValue(v, juce::dontSendNotification);
    currentSensitivity = v;
    for (int i = 0; i < commandItems.size(); i++) {
        for (int j = 0; j < commandItems[i]->encoders.size(); j++) {
            commandItems[i]->encoders[j]->customScaleFactor = v;
        }
    }
    return 0.0f;
}

void EncodersMult::resized()
{
	Rectangle<int> r = getLocalBounds();

    int maxWidth = r.getWidth();
    int btnWidth = maxWidth / 12;
    btnSelectAll.setBounds(0*btnWidth,0,2*btnWidth, 20);
    btnSelectNone.setBounds(2 * btnWidth, 0, 2 * btnWidth, 20);
    btnAllEqual.setBounds(5 * btnWidth, 0, 1 * btnWidth, 20);
    btnMoveOnlyLeft.setBounds(6 * btnWidth, 0, 1 * btnWidth, 20);
    btnMoveOnlyRight.setBounds(7 * btnWidth, 0, 1 * btnWidth, 20);
    btnMoveOnlyIn.setBounds(8 * btnWidth, 0, 1 * btnWidth, 20);
    btnMoveOnlyOut.setBounds(9 * btnWidth, 0, 1 * btnWidth, 20);
    btnMoveBalanced.setBounds(10 * btnWidth, 0, 1 * btnWidth, 20);
    btnMoveBalancedSym.setBounds(11 * btnWidth, 0, 1 * btnWidth, 20);

    r.removeFromTop(20);
    
    float w = 57;
    float h = 57;
    sensitivity.setBounds(0, 40, w, h);
    sensitivity.setTextBoxStyle(Slider::TextBoxBelow, false, 44, 20);

    int currentX = w;
    int currentY = 20;
    for (int i = 0; i < encoders.size(); i++) {
        if (currentX + w > maxWidth) {
            currentY += 80; 
            currentX = 0;
        }
        encoders[i]->setBounds(currentX, currentY + 20, w, h);
        encoders[i]->setTextBoxStyle(Slider::TextBoxBelow, false, 44, 20);
        labels[i]->setBounds(currentX, currentY, w, 20);
        labels[i]->setJustificationType(36);
        currentX += w;
    }

    viewport.setBounds(0,currentY+80, r.getWidth(), r.getHeight()-currentY);

    currentX = 0;
    currentY = 20;
    int highestY = 0;
    for (int i = 0; i < commandItems.size(); i++) {
        commandItems[i]->calcSize();
        if (currentX + commandItems[i]->calculatedWidth > getWidth()) {
            currentY += highestY;
            highestY = 0;
            currentX = 0;
        }
        commandItems[i]->setBounds(currentX, currentY, commandItems[i]->calculatedWidth, commandItems[i]->calculatedHeight);
        highestY = jmax(highestY, commandItems[i]->calculatedHeight);
        currentX += commandItems[i]->calculatedWidth+10;
    }
    cmdContainer.setBounds(0,0,r.getWidth(), currentY+highestY+80);
}

void EncodersMult::targetChanged()
{
    if (targetCommandManager != nullptr) {
        //targetCommandManager->removeAsyncManagerListener(this);
    }
    targetProgrammer = UserInputManager::getInstance()->getProgrammer(true);
    targetCommandManager = &targetProgrammer->commands;
    targetCommandManager->addAsyncManagerListener(this);
    reconstructSubComponents();
}

void EncodersMult::reconstructSubComponents()
{
    clear();
    if (targetCommandManager != nullptr) { // bug ici !!!!!
        for (int i = 0; i < targetCommandManager->items.size(); i++) {
            auto elmt = targetCommandManager->items[i];
            EncodersMultCmd* test = commandItems.add(new EncodersMultCmd(this, elmt));
            test->setBounds(10, i*50, 100, 50);
            cmdContainer.addAndMakeVisible(test);
            for (int n = 0; n < test->channels.size(); n++) {
                channels.addIfNotAlreadyThere(test->channels[n]);
            }
        }
    }
    for (int i = 0; i < channels.size(); i++) {
        Slider* s = new Slider();
        addAndMakeVisible(s);
        s->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        s->setRange(-1, 1);
        s->setValue(0, juce::dontSendNotification);
        s->setColour(Slider::rotarySliderFillColourId, Colour(127, 0, 0));
        s->setNumDecimalPlacesToDisplay(5);
        s->addListener(this);
        s->setWantsKeyboardFocus(false);
        s->addMouseListener(this, false);
        encoders.add(s);

        Label* l = new Label();
        addAndMakeVisible(l);
        l->setText(channels[i]->niceName, juce::dontSendNotification);
        l->attachToComponent(s, false);
        labels.add(l);
        l->setWantsKeyboardFocus(false);

        lastValues.add(0);

    }
    resized();
    setSensitivity(sensitivity.getValue());
}

void EncodersMult::reconstructNames()
{
    for (int i = 0; i < commandItems.size(); i++) 
    {
    commandItems[i]->commandValueChanged();
    }
}

void EncodersMult::mouseDown(const MouseEvent& event)
{
}

void EncodersMult::clear()
{
    encoders.clear();
    labels.clear();
    channels.clear();
    lastValues.clear();
    commandItems.clear();
    cmdContainer.removeAllChildren();
}

void EncodersMult::sliderValueChanged(Slider* slider)
{
    if (slider == &sensitivity) {
        setSensitivity(sensitivity.getValue());
    }
    int index = encoders.indexOf(slider);
    if (index != -1) {
        float val = slider->getValue();
        double delta = val- lastValues[index];
        delta *= currentSensitivity;
        lastValues.set(index, val);
        ChannelType* ct = channels[index];
        if (targetProgrammer!= nullptr) targetProgrammer->autoSelectCommand = false;

        Array<CommandValue*> toChange;
        Array<Command*> canBeChanged;

        for (EncodersMultCmd* cmd : commandItems) {
            if (cmd->commandLineBtn.getToggleState()) {
                canBeChanged.add(cmd->targetCommand);
            }
        }

        for (int i = 0; i < targetCommandManager->items.size(); i++) {
            Command* cmd = targetCommandManager->items[i];
            if (canBeChanged.contains(cmd)) {
                for (int v = 0; v < targetCommandManager->items[i]->values.items.size(); v++) {
                    CommandValue* cv = targetCommandManager->items[i]->values.items[v];
                    if (cv->presetOrValue->stringValue() == "value") {
                        ChannelType* localCt = dynamic_cast<ChannelType*>(cv->channelType->targetContainer.get());
                        if (localCt == ct) {
                            toChange.add(cv);
                        }
                    }
                }
            }
        }

        int nMax = toChange.size()-1;
        double nMaxDouble = (double)nMax;
        for (int i = 0; i < toChange.size(); i++) {
            CommandValue* cv = toChange[i];
            double ratio = i / nMaxDouble;
            double localDelta = delta;
            if (nMax > 0) {

                if (currentAlign == ONLYIN || currentAlign == ONLYOUT || currentAlign == BALANCEDSYM) {
                    int index = i > nMaxDouble/2. ? nMax - i : i;
                    ratio = nMax % 2 == 0 ? (double)index / (nMaxDouble/2) : (double)index / ((nMaxDouble-1)/2);
                }

                switch (currentAlign) {
                    case EQUAL: break;
                    case ONLYLEFT: localDelta = jmap(ratio,0.,1.,delta, 0.); break;
                    case ONLYRIGHT: localDelta = jmap(ratio, 0., 1., 0., delta); break;
                    case ONLYIN: localDelta = jmap(ratio, 0., 1., 0., delta); break;
                    case ONLYOUT: localDelta = jmap(ratio, 0., 1., delta, 0.); break;
                    case BALANCED: localDelta = jmap(ratio, 0., 1., delta, -delta); break;
                    case BALANCEDSYM: localDelta = jmap(ratio, 0., 1., delta, -delta); break;

                }
            }

            cv->valueFrom->setValue(cv->valueFrom->floatValue() + localDelta);
            if (cv->thru->boolValue()) {
                cv->valueTo->setValue(cv->valueTo->floatValue() + localDelta);
            }
        }

        if (targetProgrammer != nullptr) targetProgrammer->autoSelectCommand = true;
    }
}

void EncodersMult::sliderDragEnded(Slider* slider)
{
    int index = encoders.indexOf(slider);
    if (index != -1) {
        lastValues.set(index, 0);
        slider->setValue(0, juce::dontSendNotification);
    }
}

void EncodersMult::selectAlign(Align a)
{
    currentAlign = a;
    updateAlignButtons();
}

void EncodersMult::updateAlignButtons()
{
    btnAllEqual.removeColour(TextButton::buttonColourId); 
    btnMoveOnlyLeft.removeColour(TextButton::buttonColourId);
    btnMoveOnlyRight.removeColour(TextButton::buttonColourId);
    btnMoveOnlyIn.removeColour(TextButton::buttonColourId);
    btnMoveOnlyOut.removeColour(TextButton::buttonColourId);
    btnMoveBalanced.removeColour(TextButton::buttonColourId);
    btnMoveBalancedSym.removeColour(TextButton::buttonColourId);

    switch (currentAlign) {
        case EQUAL: btnAllEqual.setColour(TextButton::buttonColourId, Colour(127,64,0)); break;
        case ONLYLEFT: btnMoveOnlyLeft.setColour(TextButton::buttonColourId, Colour(127,64,0)); break;
        case ONLYRIGHT: btnMoveOnlyRight.setColour(TextButton::buttonColourId, Colour(127,64,0)); break;
        case ONLYIN: btnMoveOnlyIn.setColour(TextButton::buttonColourId, Colour(127,64,0)); break;
        case ONLYOUT: btnMoveOnlyOut.setColour(TextButton::buttonColourId, Colour(127,64,0)); break;
        case BALANCED: btnMoveBalanced.setColour(TextButton::buttonColourId, Colour(127,64,0)); break;
        case BALANCEDSYM: btnMoveBalancedSym.setColour(TextButton::buttonColourId, Colour(127,64,0)); break;
    }
}

void EncodersMult::selectAll()
{
    for (EncodersMultCmd* cmd : commandItems) {
        cmd->commandLineBtn.setToggleState(true, juce::dontSendNotification);
    }
}

void EncodersMult::selectNone()
{
    for (EncodersMultCmd* cmd : commandItems) {
        cmd->commandLineBtn.setToggleState(false, juce::dontSendNotification);
    }
}



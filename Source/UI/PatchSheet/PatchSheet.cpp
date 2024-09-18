#include <JuceHeader.h>
#include "PatchSheet.h"
#include "Brain.h"
#include "UI/LabelAndTime.h"
#include "Fixture/FixtureManager.h"
#include "Definitions/Interface/InterfaceIncludes.h"
#include "Definitions/Assistant/Assistant.h"

//==============================================================================
PatchSheetUI::PatchSheetUI(const String& contentName):
    ShapeShifterContent(PatchSheet::getInstance(), contentName)
{
    
}

PatchSheetUI::~PatchSheetUI()
{
}

juce_ImplementSingleton(PatchSheet);

PatchSheet::PatchSheet()
{
    csvExportBtn.setButtonText("Export to CSV");
    csvExportBtn.setWantsKeyboardFocus(false);
    addOneBtn.setButtonText("Add one");
    addOneBtn.setWantsKeyboardFocus(false);
    addMultBtn.setButtonText("Add multiple");
    addMultBtn.setWantsKeyboardFocus(false);
    addAndMakeVisible(csvExportBtn);
    addAndMakeVisible(addOneBtn);
    addAndMakeVisible(addMultBtn);
    //addAndMakeVisible(inspectCuelistBtn);
    
    addAndMakeVisible(idLabel); idLabel.setText("ID", juce::dontSendNotification);
    addAndMakeVisible(nameLabel); nameLabel.setText("Name", juce::dontSendNotification);
    addAndMakeVisible(typeLabel); typeLabel.setText("Type", juce::dontSendNotification);
    addAndMakeVisible(universeLabel); universeLabel.setText("Universe", juce::dontSendNotification);
    addAndMakeVisible(adressLabel); adressLabel.setText("Adress", juce::dontSendNotification);

    idLabel.setJustificationType(Justification::centred);
    nameLabel.setJustificationType(Justification::centred);
    typeLabel.setJustificationType(Justification::centred);
    universeLabel.setJustificationType(Justification::centred);
    adressLabel.setJustificationType(Justification::centred);

    //addAndMakeVisible(isSelected);
    /*
    isSelected.onClick = [this]() {
        BKEngine* e = dynamic_cast<BKEngine*>(BKEngine::mainEngine);
        if (isSelected.getToggleState()) {
            e->selectAllCuesOfCuelist(targetCuelist);
        }
        else {
            e->clearCueSelection();
        }
    };
    isSelected.setWantsKeyboardFocus(false);
    */

    csvExportBtn.onClick = [this]() { exportToCSV(); };
    addOneBtn.onClick = [this]() { FixtureManager::getInstance()->addItem(); };
    addMultBtn.onClick = [this]() { Assistant::getInstance()->patcherCC.selectThis(); };
    //inspectCuelistBtn.onClick = [this]() {inspectCuelist(); };

    //labelAndTimeBtn.setButtonText("Edit Label and time");
    //inspectCuelistBtn.setButtonText("Inspect Cuelist");

    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&linesContainer);
}

PatchSheet::~PatchSheet()
{
    //if (targetCuelist != nullptr) targetCuelist->removeChangeListener(this);
}

void PatchSheet::paint (juce::Graphics& g)
{
}

void PatchSheet::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    BKEngine* engine = dynamic_cast<BKEngine*>(BKEngine::mainEngine);

    float textSize = engine->conductorTextSize->floatValue();
    float titleSize = engine->conductorTitleSize->floatValue();

    int width = getLocalBounds().getWidth();
    int height = getLocalBounds().getHeight();

    csvExportBtn.setBounds(width * 0 / 5, 0, width / 5, 20);
    addOneBtn.setBounds(width * 3 / 5, 0, width / 5, 20);
    addMultBtn.setBounds(width * 4 / 5, 0, width / 5, 20);
    //inspectCuelistBtn.setBounds(width * 4 / 5, 0, width / 5, 20);

    int w = width / 10;
    //w = jmin(45, w);
    int h = 30;

    //isSelected.setBounds(0, 20, 0.5 * w, h);

    idLabel.setBounds(0 * w, 20, w, h);
    nameLabel.setBounds(1 * w, 20, 2*w, h);
    typeLabel.setBounds(3 * w, 20, 2*w, h);
    universeLabel.setBounds(5 * w, 20, 2*w, h);
    adressLabel.setBounds(7 * w, 20, 2*w, h);

    viewport.setBounds(0,50,width, height-50);

    int currHeight = 0;
    for (int i = 0; i < lines.size(); i++) {
        lines[i]->resized();
        lines[i]->setBounds(0, currHeight, getWidth(), lines[i]->finalHeight);
        currHeight += lines[i]->finalHeight;
    }
    linesContainer.setBounds(0,0,getWidth() - 10, currHeight);
    

}

void PatchSheet::updateStyle()
{
}


void PatchSheet::updateContent()
{
    BKEngine* engine = dynamic_cast<BKEngine*>(BKEngine::mainEngine);
    resized();
    bool changed = false;
    Cuelist* newTarget = Brain::getInstance()->getCuelistById(engine->conductorCuelistId->intValue());

}

void PatchSheet::rebuildLines()
{
    lines.clear();
    BKEngine* e = dynamic_cast<BKEngine*>(BKEngine::mainEngine);
    if (e->isClearing) {return;}
    for (int iFixt = 0; iFixt < FixtureManager::getInstance()->items.size(); iFixt++) {
        Fixture* f = FixtureManager::getInstance()->items[iFixt];
        for (int iPatch = 0; iPatch < f->patchs.items.size(); iPatch++) {
            FixturePatch* fp = f->patchs.items[iPatch];
            PatchSheetLine* l = new PatchSheetLine(f, fp, this);
            lines.add(l);
            linesContainer.addAndMakeVisible(l);
            l->updateContent();
        }
        if (f->patchs.items.size() == 0) {
            PatchSheetLine* l = new PatchSheetLine(f, nullptr, this);
            lines.add(l);
            linesContainer.addAndMakeVisible(l);
            l->updateContent();
        }
    }
    resized();
    updateSelection();
}

void PatchSheet::inspect(int delta)
{
    BKEngine* engine = dynamic_cast<BKEngine*>(BKEngine::mainEngine);
    int targetCueId = engine->conductorCuelistId->intValue();
    Cuelist* target = Brain::getInstance()->getCuelistById(targetCueId);
    if (target == nullptr) { return; }
    Cue* currentCue = target->cueA;
    int index = -1;
    if (currentCue != nullptr) {
        index = target->cues.items.indexOf(currentCue);
    }
    index += delta;
    if (index >= 0 && index < target->cues.items.size()) {
        target->cues.items[index]->selectThis();
    }
}

void PatchSheet::inspectCuelist()
{
}

void PatchSheet::updateSelection()
{
    BKEngine* e = dynamic_cast<BKEngine*>(BKEngine::mainEngine);
    for (int i = 0; i < lines.size(); i++) {
        //lines[i]->isSelected.setToggleState(e->selectedCues.contains(lines[i]->targetCue), juce::dontSendNotification);
    }
}

void PatchSheet::changeListenerCallback(ChangeBroadcaster* source)
{
    rebuildLines();
}

void PatchSheet::exportToCSV()
{
    FileChooser fc("Save patch as CSV", File::getCurrentWorkingDirectory(), "*.csv");
    if (!fc.browseForFileToSave(true)) return;

    String output = "";
    output += "ID,";
    output += "Name,";
    output += "Fixture type,";
    output += "Universe,";
    output += "Address,";
    output += "Long Address\n";

    Array<DMXInterface*> universes = InterfaceManager::getInstance()->getInterfacesOfType<DMXInterface>();

    for (int iFixt = 0; iFixt < FixtureManager::getInstance()->items.size(); iFixt++) {
        Fixture* f = FixtureManager::getInstance()->items[iFixt];
        FixtureType* ft = dynamic_cast<FixtureType*>(f->devTypeParam->targetContainer.get());
        String ftName = ft != nullptr ? ft->niceName : "";
        for (int iPatch = 0; iPatch < f->patchs.items.size(); iPatch++) {
            FixturePatch* fp = f->patchs.items[iPatch];
            DMXInterface* inter = dynamic_cast<DMXInterface*>(fp->targetInterface->targetContainer.get());
            int interfaceIndex = universes.indexOf(inter);
            output += f->id->stringValue() + ",";
            output += f->userName->stringValue() + ",";
            output += ftName + ",";
            if (inter != nullptr) {
                output += inter->niceName;
            }
            output += ",";
            output += fp->address->stringValue() + ",";
            if (interfaceIndex >= 0) {
                output += String(interfaceIndex+1)+"."+ fp->address->stringValue();
            }
            output += "\n";
        }
        if (f->patchs.items.size() == 0) {
            output += f->id->stringValue() + ",";
            output += f->userName->stringValue() + ",";
            output += ftName+",";
            output += ",,\n";
        }
    }

    File file = fc.getResult();
    if (file.exists()) file.deleteFile();
    file.create();	
    std::unique_ptr<OutputStream> os(file.createOutputStream());
    if (os == nullptr)
    {
        LOGERROR("Error saving document, please try again");
        return;
    }

    os->writeString(output);
    os->flush();
}


/*
  ==============================================================================

    OSCInputEditor.cpp
    Created: 13 Oct 2020 9:44:13am
    Author:  bkupe

  ==============================================================================
*/

OSCInputEditor::OSCInputEditor(EnablingControllableContainer* cc, bool isRoot) :
    EnablingControllableContainerEditor(cc, isRoot)
{
    StringArray ips = NetworkHelpers::getLocalIPs();
    ipLabel.setText("IPs : " + ips.joinIntoString(","), dontSendNotification);
    ipLabel.setColour(ipLabel.textColourId, TEXTNAME_COLOR);
    ipLabel.setFont(headerHeight - 6);
    addAndMakeVisible(&ipLabel);
}

OSCInputEditor::~OSCInputEditor()
{
}

void OSCInputEditor::resizedInternalHeader(Rectangle<int>& r)
{
    ipLabel.setBounds(r.removeFromRight(jmin(ipLabel.getFont().getStringWidth(ipLabel.getText()), getWidth() - 100)));
    r.removeFromRight(2);
    EnablingControllableContainerEditor::resizedInternalHeader(r);
}

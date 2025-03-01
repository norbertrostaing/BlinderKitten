/*
 ==============================================================================

 MainComponentMenuBar.cpp
 Created: 25 Mar 2016 6:02:02pm
 Author:  Martin Hermant

 ==============================================================================
 */
 #include "MainComponent.h"
 #include "UserInputManager.h"
 #include "Definitions/Assistant/Assistant.h"

namespace OrganicCommandId
{
	// we override these with additional code
	static const int editProjectSettings = 0x50001;
	static const int editGlobalSettings = 0x50002;
}

namespace BlinderKittenCommandId
{
	static const int showAbout = 0x60000;
	static const int gotoWebsite = 0x60001;
	static const int gotoDiscord = 0x60002;
	static const int gotoDocs = 0x60003;
	static const int postGithubIssue = 0x60004;
	static const int donate = 0x60005;
	static const int sponsor = 0x60055;
	static const int showWelcome = 0x60006;
	static const int gotoChangelog = 0x60007;

	static const int guideStart = 0x300; //up to 0x300 +100
	static const int exitGuide = 0x399; 
	static const int goToCommunityModules = 0x500;
	static const int reloadCustomModules = 0x501;
	static const int exportSelection = 0x800;
	static const int importSelection = 0x801;

	//static const int keyBackspace = 0x60100;
	static const int keyClear = 0x60101;
	static const int keyEnter = 0x60102;
	static const int keyThru = 0x60103;
	static const int keyBackspace = 0x60104;
	static const int keyPlus = 0x60105;
	static const int keyMinus = 0x60106;

	static const int keyRecord = 0x60201;
	static const int keyEdit = 0x60202;
	static const int keyCopy = 0x60203;
	static const int keyDelete = 0x60204;
	static const int keyLabel = 0x60205;

	static const int keyFixture = 0x60301;
	static const int keyGroup = 0x60302;
	static const int keyPreset = 0x60303;
	static const int keyCuelist = 0x60304;
	static const int keyCue = 0x60305;
	static const int keyEffect = 0x60306;
	static const int keyCarousel = 0x60307;
	static const int keyMapper = 0x60308;
	static const int keyAssistant = 0x60309;
	static const int keySubFixture = 0x60310;

	static const int keyHighlight = 0x60320;
	static const int keyBlind = 0x60321;
	static const int keyGo = 0x60322;

	static const int key1 = 0x60401;
	static const int key2 = 0x60402;
	static const int key3 = 0x60403;
	static const int key4 = 0x60404;
	static const int key5 = 0x60405;
	static const int key6 = 0x60406;
	static const int key7 = 0x60407;
	static const int key8 = 0x60408;
	static const int key9 = 0x60409;
	static const int key0 = 0x60410;



}

void MainContentComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) 
{
	if (commandID >= BlinderKittenCommandId::guideStart && commandID < BlinderKittenCommandId::guideStart + 99)
	{
		// result.setInfo(Guider::getInstance()->getGuideName(commandID - BlinderKittenCommandId::guideStart), "", "Guides", result.readOnlyInKeyEditor);
		return;
	}

	switch (commandID)
	{
	case BlinderKittenCommandId::showAbout:
		result.setInfo("About...", "", "General", result.readOnlyInKeyEditor);
		break;

	case BlinderKittenCommandId::showWelcome:
		result.setInfo("Show Welcome Screen...", "", "General", result.readOnlyInKeyEditor);
		break;

	case BlinderKittenCommandId::donate:
		result.setInfo("Be cool and donate", "", "General", result.readOnlyInKeyEditor);
		break;

	case BlinderKittenCommandId::sponsor:
		result.setInfo("Be even cooler and sponsor !", "", "General", result.readOnlyInKeyEditor);
		break;

	case BlinderKittenCommandId::gotoWebsite:
		result.setInfo("Go to website", "", "Help", result.readOnlyInKeyEditor);
		break;

	case BlinderKittenCommandId::gotoDiscord:
		result.setInfo("Go to Discord", "", "Help", result.readOnlyInKeyEditor);
		break;

	case BlinderKittenCommandId::gotoDocs:
		result.setInfo("Go to the Amazing Documentation", "", "Help", result.readOnlyInKeyEditor);
		break;

	case BlinderKittenCommandId::gotoChangelog:
		result.setInfo("See the changelog", "", "Help", result.readOnlyInKeyEditor);
		break;


	case BlinderKittenCommandId::postGithubIssue:
		result.setInfo("Post an issue on github", "", "Help", result.readOnlyInKeyEditor);
		break;

	case BlinderKittenCommandId::goToCommunityModules:
		result.setInfo("Community Modules Manager", "", "General", result.readOnlyInKeyEditor);
		break;

	case BlinderKittenCommandId::reloadCustomModules:
		result.setInfo("Reload Custom Modules", "", "General", result.readOnlyInKeyEditor);
		break;

	case BlinderKittenCommandId::exitGuide:
		result.setInfo("Exit current guide", "", "Guides", result.readOnlyInKeyEditor);
		result.addDefaultKeypress(KeyPress::escapeKey, ModifierKeys::noModifiers);
		// result.setActive(Guider::getInstance()->guide != nullptr);
		break;

	case BlinderKittenCommandId::exportSelection:
		result.setInfo("Export Selection", "This will export the current selection as *.mochi file that can be later imported", "File", result.readOnlyInKeyEditor);
		result.addDefaultKeypress(KeyPress::createFromDescription("s").getKeyCode(), ModifierKeys::altModifier);
		result.setActive(InspectableSelectionManager::mainSelectionManager->currentInspectables.size() > 0);
		break;

	case BlinderKittenCommandId::importSelection:
		result.setInfo("Import...", "This will import a *.mochi file and add it to the current project", "File", result.readOnlyInKeyEditor);
		result.addDefaultKeypress(KeyPress::createFromDescription("o").getKeyCode(), ModifierKeys::altModifier);
		break;




	case BlinderKittenCommandId::keyBackspace:
		result.setInfo("backspace", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("backspace").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keyClear:
		result.setInfo("Clear", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("backspace").getKeyCode(), ModifierKeys::shiftModifier);
		break;

	case BlinderKittenCommandId::keyEnter:
		result.setInfo("Enter", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("return").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keyThru:
		result.setInfo("Thru", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("t").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keyPlus:
		result.setInfo("Plus +", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("+").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keyMinus:
		result.setInfo("Minus -", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("-").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keyRecord:
		result.setInfo("Record", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("r").getKeyCode(), ModifierKeys::noModifiers);
		result.addDefaultKeypress(KeyPress::createFromDescription("r").getKeyCode(), ModifierKeys::altModifier);
		break;

	case BlinderKittenCommandId::keyEdit:
		result.setInfo("Edit", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("e").getKeyCode(), ModifierKeys::altModifier);
		break;

	case BlinderKittenCommandId::keyCopy:
		result.setInfo("Copy", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("c").getKeyCode(), ModifierKeys::altModifier);
		break;

	case BlinderKittenCommandId::keyDelete:
		result.setInfo("Delete", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("d").getKeyCode(), ModifierKeys::altModifier);
		break;

	case BlinderKittenCommandId::keyLabel:
		result.setInfo("Label", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("l").getKeyCode(), ModifierKeys::noModifiers);
		result.addDefaultKeypress(KeyPress::createFromDescription("l").getKeyCode(), ModifierKeys::altModifier);
		break;




	case BlinderKittenCommandId::keyFixture:
		result.setInfo("Fixture", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("f").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keyGroup:
		result.setInfo("Group", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("g").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keyPreset:
		result.setInfo("Preset", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("p").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keyCuelist:
		result.setInfo("Cuelist", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("l").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keyCue:
		result.setInfo("Cue", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("q").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keyEffect:
		result.setInfo("Effect", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("e").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keyCarousel:
		result.setInfo("Carousel", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("c").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keyMapper:
		result.setInfo("Mapper", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("m").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keyAssistant:
		result.setInfo("Assistant", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("a").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keySubFixture:
		result.setInfo("Subfixture", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("s").getKeyCode(), ModifierKeys::noModifiers);
		break;


	case BlinderKittenCommandId::keyHighlight:
		result.setInfo("Hightlight", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("h").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keyBlind:
		result.setInfo("Blind", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("b").getKeyCode(), ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::keyGo:
		result.setInfo("Go main cuelist", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription(" ").getKeyCode(), ModifierKeys::noModifiers);
		break;





	case BlinderKittenCommandId::key1:
		result.setInfo("Key 1", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("1").getKeyCode(), ModifierKeys::noModifiers);
		result.addDefaultKeypress(KeyPress::numberPad1, ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::key2:
		result.setInfo("Key 2", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("2").getKeyCode(), ModifierKeys::noModifiers);
		result.addDefaultKeypress(KeyPress::numberPad2, ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::key3:
		result.setInfo("Key 3", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("3").getKeyCode(), ModifierKeys::noModifiers);
		result.addDefaultKeypress(KeyPress::numberPad3, ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::key4:
		result.setInfo("Key 4", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("4").getKeyCode(), ModifierKeys::noModifiers);
		result.addDefaultKeypress(KeyPress::numberPad4, ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::key5:
		result.setInfo("Key 5", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("5").getKeyCode(), ModifierKeys::noModifiers);
		result.addDefaultKeypress(KeyPress::numberPad5, ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::key6:
		result.setInfo("Key 6", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("6").getKeyCode(), ModifierKeys::noModifiers);
		result.addDefaultKeypress(KeyPress::numberPad6, ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::key7:
		result.setInfo("Key 7", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("7").getKeyCode(), ModifierKeys::noModifiers);
		result.addDefaultKeypress(KeyPress::numberPad7, ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::key8:
		result.setInfo("Key 8", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("8").getKeyCode(), ModifierKeys::noModifiers);
		result.addDefaultKeypress(KeyPress::numberPad8, ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::key9:
		result.setInfo("Key 9", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("9").getKeyCode(), ModifierKeys::noModifiers);
		result.addDefaultKeypress(KeyPress::numberPad9, ModifierKeys::noModifiers);
		break;

	case BlinderKittenCommandId::key0:
		result.setInfo("Key 0", "", "Direct Commands", 0);
		result.addDefaultKeypress(KeyPress::createFromDescription("0").getKeyCode(), ModifierKeys::noModifiers);
		result.addDefaultKeypress(KeyPress::numberPad0, ModifierKeys::noModifiers);
		break;

	case 0x40004: // bypass deleteing item with backspace 
		OrganicMainContentComponent::getCommandInfo(commandID, result);
		//result.defaultKeypresses.remove(1);
		// result.defaultKeypresses.remove(0);
		break;

	default:
		OrganicMainContentComponent::getCommandInfo(commandID, result);
		break;
	}
}



void MainContentComponent::getAllCommands(Array<CommandID>& commands) {

	OrganicMainContentComponent::getAllCommands(commands);
	const CommandID ids[] = {

		// BlinderKittenCommandId::showAbout,
		//BlinderKittenCommandId::showWelcome,
		//BlinderKittenCommandId::donate,
		//BlinderKittenCommandId::sponsor,
		//BlinderKittenCommandId::gotoWebsite,
		//BlinderKittenCommandId::gotoDiscord,
		//BlinderKittenCommandId::gotoDocs,
		//BlinderKittenCommandId::gotoChangelog,
		//BlinderKittenCommandId::postGithubIssue,
		BlinderKittenCommandId::importSelection,
		BlinderKittenCommandId::exportSelection,
		//BlinderKittenCommandId::goToCommunityModules,
		//BlinderKittenCommandId::reloadCustomModules,
		//BlinderKittenCommandId::exitGuide,

		BlinderKittenCommandId::keyEnter,
		BlinderKittenCommandId::keyBackspace,
		BlinderKittenCommandId::keyClear,
		BlinderKittenCommandId::keyThru,
		BlinderKittenCommandId::keyPlus,
		BlinderKittenCommandId::keyMinus,

		BlinderKittenCommandId::keyRecord,
		BlinderKittenCommandId::keyEdit,
		BlinderKittenCommandId::keyCopy,
		BlinderKittenCommandId::keyDelete,
		BlinderKittenCommandId::keyLabel,

		BlinderKittenCommandId::keyAssistant,
		BlinderKittenCommandId::keyFixture,
		BlinderKittenCommandId::keySubFixture,
		BlinderKittenCommandId::keyGroup,
		BlinderKittenCommandId::keyPreset,
		BlinderKittenCommandId::keyCuelist,
		BlinderKittenCommandId::keyCue,
		BlinderKittenCommandId::keyEffect,
		BlinderKittenCommandId::keyCarousel,
		BlinderKittenCommandId::keyMapper,

		BlinderKittenCommandId::keyHighlight,
		BlinderKittenCommandId::keyBlind,
		BlinderKittenCommandId::keyGo,

		BlinderKittenCommandId::key1,
		BlinderKittenCommandId::key2,
		BlinderKittenCommandId::key3,
		BlinderKittenCommandId::key4,
		BlinderKittenCommandId::key5,
		BlinderKittenCommandId::key6,
		BlinderKittenCommandId::key7,
		BlinderKittenCommandId::key8,
		BlinderKittenCommandId::key9,
		BlinderKittenCommandId::key0,

	};

	commands.addArray(ids, numElementsInArray(ids));
	//for (int i = 0; i < Guider::getInstance()->factory.defs.size(); ++i) commands.add(BlinderKittenCommandId::guideStart + i);
}


PopupMenu MainContentComponent::getMenuForIndex(int topLevelMenuIndex, const String& menuName) 
{
	PopupMenu menu = OrganicMainContentComponent::getMenuForIndex(topLevelMenuIndex, menuName);

	if (menuName == "Help")
	{
		menu.addCommandItem(&getCommandManager(), BlinderKittenCommandId::showAbout);
		menu.addCommandItem(&getCommandManager(), BlinderKittenCommandId::showWelcome);
		menu.addCommandItem(&getCommandManager(), BlinderKittenCommandId::donate);
		menu.addCommandItem(&getCommandManager(), BlinderKittenCommandId::sponsor);
		menu.addSeparator();
		menu.addCommandItem(&getCommandManager(), BlinderKittenCommandId::gotoWebsite);
		menu.addCommandItem(&getCommandManager(), BlinderKittenCommandId::gotoDiscord);
		menu.addCommandItem(&getCommandManager(), BlinderKittenCommandId::gotoDocs);
		menu.addCommandItem(&getCommandManager(), BlinderKittenCommandId::gotoChangelog);
		menu.addCommandItem(&getCommandManager(), BlinderKittenCommandId::postGithubIssue);

	}else if (menuName == "Guides")
	{
	/*
		for (int i = 0; i < Guider::getInstance()->factory.defs.size(); ++i)
		{
			menu.addCommandItem(&getCommandManager(), BlinderKittenCommandId::guideStart + i);
		}

		menu.addSeparator();
		menu.addCommandItem(&getCommandManager(), BlinderKittenCommandId::exitGuide);
	*/
	}

	return menu;
}

void MainContentComponent::fillFileMenuInternal(PopupMenu & menu)
{
	menu.addCommandItem(&getCommandManager(), BlinderKittenCommandId::importSelection);
	menu.addCommandItem(&getCommandManager(), BlinderKittenCommandId::exportSelection);
	menu.addSeparator();
	menu.addCommandItem(&getCommandManager(), BlinderKittenCommandId::goToCommunityModules);
	menu.addCommandItem(&getCommandManager(), BlinderKittenCommandId::reloadCustomModules);
}

bool MainContentComponent::perform(const InvocationInfo& info)
{

	if (info.commandID >= BlinderKittenCommandId::guideStart && info.commandID < BlinderKittenCommandId::guideStart + 100)
	{
		// Guider::getInstance()->launchGuideAtIndex(info.commandID - BlinderKittenCommandId::guideStart);
		return true;
	}

	switch (info.commandID)
	{


	case BlinderKittenCommandId::showAbout:
	{
		// AboutWindow w;
		// DialogWindow::showModalDialog("About", &w, getTopLevelComponent(), Colours::transparentBlack, true);
	}
	break;

	case BlinderKittenCommandId::showWelcome:
	{
		// WelcomeScreen w;
		// DialogWindow::showModalDialog("Welcome", &w, getTopLevelComponent(), Colours::black, true);
	}
	break;

	case BlinderKittenCommandId::donate:
		URL("http://blinderkitten.lighting/").launchInDefaultBrowser();
		break;

	case BlinderKittenCommandId::sponsor:
		URL("http://blinderkitten.lighting/").launchInDefaultBrowser();
		break;

	case BlinderKittenCommandId::gotoWebsite:
		URL("http://blinderkitten.lighting/").launchInDefaultBrowser();
		break;

	case BlinderKittenCommandId::gotoDiscord:
		URL("https://discord.gg/wYNB3rK").launchInDefaultBrowser();
		break;

	case BlinderKittenCommandId::gotoDocs:
		URL("https://benjamin.kuperberg.fr/chataigne/docs").launchInDefaultBrowser();
		break;

	case BlinderKittenCommandId::gotoChangelog:
		URL("https://benjamin.kuperberg.fr/chataigne/releases/changelog.html").launchInDefaultBrowser();
		break;

	case BlinderKittenCommandId::postGithubIssue:
		URL("http://github.com/benkuper/Chataigne/issues").launchInDefaultBrowser();
		break;


	case BlinderKittenCommandId::goToCommunityModules:
		// CommunityModuleManager::getInstance()->selectThis();
		break;

	case BlinderKittenCommandId::reloadCustomModules:
		// ModuleManager::getInstance()->factory->updateCustomModules();
		break;

	case BlinderKittenCommandId::exitGuide:
		// Guider::getInstance()->setCurrentGuide(nullptr);
		break;

	case BlinderKittenCommandId::exportSelection:
	{
		((BKEngine*)Engine::mainEngine)->exportSelection();
	}
	break;

	case BlinderKittenCommandId::importSelection:
	{
		((BKEngine*)Engine::mainEngine)->importSelection();
	}
	break;

	case BlinderKittenCommandId::keyAssistant: {Assistant::getInstance()->selectThis(); }break;

	case BlinderKittenCommandId::keyFixture: { UserInputManager::getInstance()->processInput("Fixture"); }break;
	case BlinderKittenCommandId::keySubFixture: { UserInputManager::getInstance()->processInput("Subfixture"); }break;
	case BlinderKittenCommandId::keyGroup: {	UserInputManager::getInstance()->processInput("Group"); }break;
	case BlinderKittenCommandId::keyPreset: {	UserInputManager::getInstance()->processInput("Preset"); }break;
	case BlinderKittenCommandId::keyCuelist: {	UserInputManager::getInstance()->processInput("Cuelist"); }break;
	case BlinderKittenCommandId::keyCue: {		UserInputManager::getInstance()->processInput("Cue"); }break;
	case BlinderKittenCommandId::keyEffect: {	UserInputManager::getInstance()->processInput("Effect"); }break;
	case BlinderKittenCommandId::keyCarousel: {	UserInputManager::getInstance()->processInput("Carousel"); }break;
	case BlinderKittenCommandId::keyMapper: {	UserInputManager::getInstance()->processInput("Mapper"); }break;

	case BlinderKittenCommandId::keyHighlight: {UserInputManager::getInstance()->toggleHightlight(); }break;
	case BlinderKittenCommandId::keyBlind: { UserInputManager::getInstance()->toggleBlind(); }break;
	case BlinderKittenCommandId::keyGo: { UserInputManager::getInstance()->goMainCuelist(); }break;

	case BlinderKittenCommandId::keyClear: {	UserInputManager::getInstance()->processInput("Clear"); }break;
	case BlinderKittenCommandId::keyBackspace: {UserInputManager::getInstance()->processInput("Backspace"); }break;
	case BlinderKittenCommandId::keyEnter: {	UserInputManager::getInstance()->processInput("Enter"); }break;
	case BlinderKittenCommandId::keyThru: {		UserInputManager::getInstance()->processInput("Thru"); }break;
	case BlinderKittenCommandId::keyPlus: {		UserInputManager::getInstance()->processInput("+"); }break;
	case BlinderKittenCommandId::keyMinus: {	UserInputManager::getInstance()->processInput("-"); }break;

	case BlinderKittenCommandId::keyRecord: {	UserInputManager::getInstance()->processInput("record"); }break;
	case BlinderKittenCommandId::keyCopy: {		UserInputManager::getInstance()->processInput("copy"); }break;
	case BlinderKittenCommandId::keyEdit: {		UserInputManager::getInstance()->processInput("edit"); }break;
	case BlinderKittenCommandId::keyDelete: {	UserInputManager::getInstance()->processInput("delete"); }break;
	case BlinderKittenCommandId::keyLabel: {	UserInputManager::getInstance()->processInput("label"); }break;

	case BlinderKittenCommandId::key1: {		UserInputManager::getInstance()->processInput("1"); }break;
	case BlinderKittenCommandId::key2: {		UserInputManager::getInstance()->processInput("2"); }break;
	case BlinderKittenCommandId::key3: {		UserInputManager::getInstance()->processInput("3"); }break;
	case BlinderKittenCommandId::key4: {		UserInputManager::getInstance()->processInput("4"); }break;
	case BlinderKittenCommandId::key5: {		UserInputManager::getInstance()->processInput("5"); }break;
	case BlinderKittenCommandId::key6: {		UserInputManager::getInstance()->processInput("6"); }break;
	case BlinderKittenCommandId::key7: {		UserInputManager::getInstance()->processInput("7"); }break;
	case BlinderKittenCommandId::key8: {		UserInputManager::getInstance()->processInput("8"); }break;
	case BlinderKittenCommandId::key9: {		UserInputManager::getInstance()->processInput("9"); }break;
	case BlinderKittenCommandId::key0: {		UserInputManager::getInstance()->processInput("0"); }break;

	case OrganicCommandId::editProjectSettings:
	case OrganicCommandId::editGlobalSettings:
	{
		bool handled = OrganicMainContentComponent::perform(info);
		Brain::getInstance()->showWindow("Inspector");
		return handled;
        }

	default:
		return OrganicMainContentComponent::perform(info);
	}

	return true;
}

StringArray MainContentComponent::getMenuBarNames()
{
	StringArray names = OrganicMainContentComponent::getMenuBarNames();
	// names.add("Help");
	// names.add("Yataaa");
	return names;
}

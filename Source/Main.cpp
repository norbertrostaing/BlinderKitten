#include "Main.h"
#include "JuceHeader.h"
#include "BKEngine.h"
#include "MainComponent.h"

//==============================================================================

BlinderKittenApplication::BlinderKittenApplication() :
	OrganicApplication("BlinderKitten", true)
{
}


void BlinderKittenApplication::initialiseInternal(const String &)
{
	engine.reset(new BKEngine());
	if(useWindow) mainComponent.reset(new MainContentComponent());

	//Call after engine init
	// AppUpdater::getInstance()->setURLs("http://benjamin.kuperberg.fr/chataigne/releases/update.json", "http://benjamin.kuperberg.fr/chataigne/user/data/", "Chataigne");
	// HelpBox::getInstance()->helpURL = URL("http://benjamin.kuperberg.fr/chataigne/help/");

	// CrashDumpUploader::getInstance()->init("http://benjamin.kuperberg.fr/chataigne/support/crash_report.php",ImageCache::getFromMemory(BinaryData::crash_png, BinaryData::crash_pngSize));

	// DashboardManager::getInstance()->setupDownloadURL("http://benjamin.kuperberg.fr/download/dashboard/dashboard.php?folder=dashboard");

	// ShapeShifterManager::getInstance()->setDefaultFileData(BinaryData::default_chalayout);
	// ShapeShifterManager::getInstance()->setLayoutInformations("chalayout", "Chataigne/layouts");
}


void BlinderKittenApplication::afterInit()
{
	//ANALYTICS
	if (mainWindow != nullptr)
	{
		
		// DBG("coucou");
		// MainContentComponent* comp = (MainContentComponent*)mainComponent.get();
		// BKEngine* eng = (BKEngine*)engine.get();
		//BKMenuBarComponent* menu = new BKMenuBarComponent(comp, eng);
		// mainWindow->setMenuBarComponent(menu);
	}
	

}

void BlinderKittenApplication::shutdown()
{   
	/*
	for (auto& m : ModuleManager::getInstance()->getItemsWithType<OSModule>())
	{
		m->terminateTrigger->trigger();
	}
	*/
	OrganicApplication::shutdown();
	// AppUpdater::deleteInstance();
}

void BlinderKittenApplication::handleCrashed()
{
	/*
	for (auto& m : ModuleManager::getInstance()->getItemsWithType<OSModule>())
	{
		m->crashedTrigger->trigger();
	}

	if (enableSendAnalytics->boolValue())
	{
		MatomoAnalytics::getInstance()->log(MatomoAnalytics::CRASH);
		while (MatomoAnalytics::getInstance()->isThreadRunning())
		{
			//wait until thread is done
		}
	}
	*/
	OrganicApplication::handleCrashed();
}

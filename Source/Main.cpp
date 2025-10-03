#include "Main.h"
#include "JuceHeader.h"
#include "BKEngine.h"
#include "MainComponent.h"

//==============================================================================

BlinderKittenApplication::BlinderKittenApplication() :
	OrganicApplication("BlinderKitten", true, ImageCache::getFromMemory(BinaryData::icon_png, BinaryData::icon_pngSize))
{
	//_crtBreakAlloc = 136317;
}


void BlinderKittenApplication::initialiseInternal(const String &)
{
	engine.reset(new BKEngine());

	//Call after engine init
	AppUpdater::getInstance()->setURLs("https://blinderkitten.lighting/update.json", "https://blinderkitten.lighting/installs/", "BlinderKitten");
	// HelpBox::getInstance()->helpURL = URL("http://benjamin.kuperberg.fr/chataigne/help/");

	CrashDumpUploader::getInstance()->init("https://hazlab.fr/BlinderKitten/crash_report.php",ImageCache::getFromMemory(BinaryData::crash_png, BinaryData::crash_pngSize));

	DashboardManager::getInstance()->setupDownloadURL("http://benjamin.kuperberg.fr/download/dashboard/dashboard.php?folder=dashboard");
	
	ShapeShifterManager::getInstance()->setDefaultFileData(BinaryData::default_blinderlayout);
	ShapeShifterManager::getInstance()->setLayoutInformations("blinderlayout", "BlinderKitten/layouts");
    if(useWindow) mainComponent.reset(new MainContentComponent());
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
	AppUpdater::deleteInstance();
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

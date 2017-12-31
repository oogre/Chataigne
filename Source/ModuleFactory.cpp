/*
  ==============================================================================

    ModuleFactory.cpp
    Created: 8 Dec 2016 2:36:06pm
    Author:  Ben

  ==============================================================================
*/

#include "ModuleFactory.h"

#include "CustomOSCModule.h"
#include "ResolumeModule.h"
#include "MilluminModule.h"
#include "MIDIModule.h"
#include "HIDModule.h"
#include "GamepadModule.h"
#include "WiimoteModule.h"
#include "ReaperModule.h"
#include "AudioModule.h"
#include "KinectV2Module.h"
#include "SerialModule.h"
#include "DMXModule.h"
#include "LiveOSCModule.h"
#include "MyoModule.h"
#include "LaunchpadModule.h"
#include "DLightModule.h"
#include "TCPModule.h"
#include "VLCModule.h"
#include "MetronomeModule.h"
#include "SignalModule.h"

juce_ImplementSingleton(ModuleFactory)

ModuleFactory::ModuleFactory() {
	moduleDefs.add(new ModuleDefinition("Protocol", "OSC", &CustomOSCModule::create));
	moduleDefs.add(new ModuleDefinition("Protocol", "MIDI", &MIDIModule::create));
	moduleDefs.add(new ModuleDefinition("Protocol", "DMX", &DMXModule::create));
	moduleDefs.add(new ModuleDefinition("Protocol", "Serial", &SerialModule::create));
	moduleDefs.add(new ModuleDefinition("Protocol", "TCP", &TCPModule::create));
	/*
	moduleDefs.add(new ModuleDefinition("Protocol", "HID", &HIDModule::create));
	moduleDefs.add(new ModuleDefinition("Protocol", "Gamepad", &GamepadModule::create));
	*/

	moduleDefs.add(new ModuleDefinition("Hardware", "Sound Card", &AudioModule::create));
#if JUCE_WINDOWS
	//moduleDefs.add(new ModuleDefinition("Controller", "Myo", &MyoModule::create));
	moduleDefs.add(new ModuleDefinition("Hardware", "KinectV2", &KinectV2Module::create));
#endif
	moduleDefs.add(new ModuleDefinition("Hardware", "Wiimote", &WiimoteModule::create));
	moduleDefs.add(new ModuleDefinition("Hardware", "Launchpad", &LaunchpadModule::create));

	moduleDefs.add(new ModuleDefinition("Software", "LiveOSC", &LiveOSCModule::create));
	moduleDefs.add(new ModuleDefinition("Software", "Reaper", &ReaperModule::create));
	
	moduleDefs.add(new ModuleDefinition("Software", "DLight", &DLightModule::create));

	moduleDefs.add(new ModuleDefinition("Software", "Resolume", &ResolumeModule::create));
	moduleDefs.add(new ModuleDefinition("Software", "Millumin", &MilluminModule::create));
	moduleDefs.add(new ModuleDefinition("Software", "VLC", &VLCModule::create));

	moduleDefs.add(new ModuleDefinition("Generator", "Metronome", &MetronomeModule::create));
	moduleDefs.add(new ModuleDefinition("Generator", "Signal", &SignalModule::create));
	
	addCustomModules();
	buildPopupMenu();
}

void ModuleFactory::addCustomModules()
{
	File modulesFolder = File::getSpecialLocation(File::SpecialLocationType::userDocumentsDirectory).getChildFile("Chataigne/modules");
	if (!modulesFolder.exists()) return;
	if (!modulesFolder.isDirectory()) return;

	Array<File> modulesList;
	modulesFolder.findChildFiles(modulesList, File::findDirectories, false);
	for (auto &m : modulesList)
	{
		File moduleFile = m.getChildFile("module.json");
		var moduleData = JSON::parse(moduleFile);
		moduleData.getDynamicObject()->setProperty("modulePath", m.getFullPathName());
		
		String moduleName = moduleData.getProperty("name", "");
		String moduleType = moduleData.getProperty("type", "");
		String moduleMenuPath = moduleData.getProperty("path", "");

		if (moduleName.isNotEmpty() && moduleType.isNotEmpty())
		{
			std::function<Module*()> createFunc;
			if (moduleType == "Serial") createFunc = &SerialModule::create;
			else if (moduleType == "OSC") createFunc = &CustomOSCModule::create;
			else if (moduleType == "MIDI") createFunc = &MIDIModule::create;
			else continue;

			LOG("Found custom module : " << moduleMenuPath << ":" << moduleName);
			ModuleDefinition * def = moduleDefs.add(new ModuleDefinition(moduleMenuPath,moduleName, createFunc));
			def->jsonData = moduleData;

		}
		
	}
}

void ModuleFactory::buildPopupMenu()
{
	OwnedArray<PopupMenu> subMenus;
	Array<String> subMenuNames;

	for (auto &d : moduleDefs)
	{
		int itemID = moduleDefs.indexOf(d) + 1;//start at 1 for menu

		if (d->menuPath.isEmpty())
		{
			menu.addItem(itemID, d->moduleType);
			continue;
		}

		int subMenuIndex = -1;

		for (int i = 0; i < subMenus.size(); i++)
		{
			if (subMenuNames[i] == d->menuPath)
			{
				subMenuIndex = i;
				break;
			}
		}
		if (subMenuIndex == -1)
		{
			subMenuNames.add(d->menuPath);
			subMenus.add(new PopupMenu());
			subMenuIndex = subMenus.size() - 1;
		}

		subMenus[subMenuIndex]->addItem(itemID, d->moduleType);
	}

	for (int i = 0; i < subMenus.size(); i++) menu.addSubMenu(subMenuNames[i], *subMenus[i]);
}

Module * ModuleFactory::showCreateMenu()
{
	int result = getInstance()->menu.show();
	if (result == 0) return nullptr;

	ModuleDefinition * d = getInstance()->moduleDefs[result - 1];//result 0 is no result
	Module * m = d->createFunc();
	if (!d->jsonData.isVoid())
	{
		m->setupModuleFromJSONData(d->jsonData);
	}

	return m;
}

Module * ModuleFactory::createModule(const String & moduleType)
{
	for (auto &d : getInstance()->moduleDefs)
	{
		if (d->moduleType == moduleType)
		{
			Module * m = d->createFunc();
			if (!d->jsonData.isVoid()) m->setupModuleFromJSONData(d->jsonData);
			else DBG("No Json Data for module definition");
			return m;
		}
	}
	return nullptr;
}

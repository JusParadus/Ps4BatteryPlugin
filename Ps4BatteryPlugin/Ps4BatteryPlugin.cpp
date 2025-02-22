#include "pch.h"
#include "Ps4BatteryPlugin.h"


BAKKESMOD_PLUGIN(Ps4BatteryPlugin, "PS4 Battery Plugin", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void Ps4BatteryPlugin::onLoad() {
	_globalCvarManager = cvarManager;
	LOG("Plugin loaded!");
	//!! Enable debug logging by setting DEBUG_LOG = true in logging.h !!
	DEBUGLOG("Ps4BatteryPlugin debug mode enabled");

    // Initialize hidapi
    if (hid_init() == 0) {
        cvarManager->log("hidapi initialized.");
    }
    else {
        const wchar_t* error = hid_error(NULL);
        cvarManager->log(L"hidapi initialization failed: " + std::wstring(error));
        return;
    }

    // Open the PS4 controller
    controllerHandle = hid_open(0x054c, 0x09cc, NULL);
    if (controllerHandle) {
        cvarManager->log("PS4 controller opened.");
    }
    else {
        cvarManager->log("Failed to open PS4 controller.");
    }

    // Set up a timer to poll the battery status periodically
    gameWrapper->SetTimeout([this](GameWrapper*) { pollBatteryStatus(); }, 1.0f);
}

void Ps4BatteryPlugin::onUnload() {
	LOG("Plugin unloaded!");

    // Close the controller handle
    if (controllerHandle) {
        hid_close(controllerHandle);
        controllerHandle = nullptr;
    }

    // Finalize hidapi
    hid_exit();
}

void Ps4BatteryPlugin::pollBatteryStatus() {
    if (controllerHandle) {
        unsigned char buf[256];
        int res = hid_read(controllerHandle, buf, sizeof(buf));
        if (res > 0) {
            // Parse the battery status from buf
            // This is a placeholder for actual parsing logic
            int batteryLevel = buf[30]; // Example: battery level might be at index 30
            cvarManager->log("Battery Level: " + std::to_string(batteryLevel) + "%");

            // Display the battery percentage in Rocket League
            // You can use BakkesMod's drawing functions to display this on the screen
        }
    }
}
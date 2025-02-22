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
        unsigned char buffer[64];
        int res = hid_read(controllerHandle, buffer, sizeof(buffer));
        if (res > 0) {
            // Parse the battery status from buf
            // This is a placeholder for actual parsing logic
            int batteryLevel = buffer[12] & 0x0F;  // Battery level is in the lower 4 bits
			int batteryLevelPercentage = (batteryLevel * 100) / 8;

            switch (batteryLevel) {
				case 0: // Empty
					cvarManager->log("Battery is empty!");
					break;
				case 1: // Very Low
					cvarManager->log("Battery is very low!");
					break;
				case 2: // Low
					cvarManager->log("Battery is low!");
					break;
				case 3: // Medium-Low
					cvarManager->log("Battery is medium-low!");
					break;
				case 4: // Medium
					cvarManager->log("Battery is medium!");
					break;
				case 5: // Medium-High
					cvarManager->log("Battery is medium-high!");
					break;
				case 6: // High
					cvarManager->log("Battery is high!");
					break;
				case 7: // Full
					cvarManager->log("Battery is full!");
					break;
				case 8: // Charging / Fully Charged
					cvarManager->log("Battery is charging or fully charged!");
					break;
				default: // Unknown
					cvarManager->log("Battery Level is not in known range!");
                    break;
            }

            cvarManager->log("Battery Level: " + std::to_string(batteryLevelPercentage) + "%");

            // Display the battery percentage in Rocket League
            // You can use BakkesMod's drawing functions to display this on the screen
        }
    }

    // Reschedule the pollBatteryStatus function to be called again after 1 second
	gameWrapper->SetTimeout([this](GameWrapper*) { pollBatteryStatus(); }, 10.0f);  // 60.of = 60 seconds (1 minute)
}
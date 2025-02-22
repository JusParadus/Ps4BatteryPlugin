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
    if (!controllerHandle) return;

    unsigned char buffer[78];  // Use a 78-byte buffer for feature reports
    int res = hid_read(controllerHandle, buffer, sizeof(buffer));

    if (res > 12) {
        // USB Mode: Parse battery level from byte 12
        int batteryLevel = buffer[12] & 0x0F;  // Lower 4 bits = battery level
		int chargingStatus = buffer[12] & 0xF0;  // Upper 4 bits = charging status

        // Calculate battery percentage
        int batteryLevelPercentage = (batteryLevel * 100) / 11;

        // Log charging status
        if (chargingStatus == 0x10 || chargingStatus == 0x20) {
            cvarManager->log("Battery is charging.");
        }
        else {
            cvarManager->log("Battery is not charging.");
        }

        // Log battery level
        std::string batteryMessage;
        switch (batteryLevel) {
            case 0x00: batteryMessage = "Battery is empty!"; break;
            case 0x01: batteryMessage = "Battery is very low!"; break;
            case 0x02: batteryMessage = "Battery is low!"; break;
            case 0x03: batteryMessage = "Battery is medium-low!"; break;
            case 0x04: batteryMessage = "Battery is medium!"; break;
            case 0x05: batteryMessage = "Battery is medium-high!"; break;
            case 0x06: batteryMessage = "Battery is high!"; break;
            case 0x07: batteryMessage = "Battery is very high!"; break;
            case 0x08: batteryMessage = "Battery is almost full!"; break;
            case 0x09: batteryMessage = "Battery is full!"; break;
            case 0x0A: batteryMessage = "Battery is fully charged!"; break;
            case 0x0B: batteryMessage = "Battery is fully charged!"; break;
            default: batteryMessage = "Battery level is unknown!"; break;
        }

        cvarManager->log(batteryMessage);
        cvarManager->log("Battery Level: " + std::to_string(batteryLevelPercentage) + "%");
    }
    else {
        // Bluetooth Mode: Send a feature report to request battery data
        unsigned char request[78] = { 0x05, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        if (hid_send_feature_report(controllerHandle, request, sizeof(request)) < 0) {
            cvarManager->log("Failed to send feature report.");
            return;
        }

        // Read the response
        res = hid_read(controllerHandle, buffer, sizeof(buffer));
        if (res > 0) {
            // Parse battery level from byte 1
            int batteryLevel = buffer[1] & 0x0F;
            int batteryLevelPercentage = (batteryLevel * 100) / 11;

            cvarManager->log("Battery Level (Bluetooth): " + std::to_string(batteryLevelPercentage) + "%");
        }
        else {
            cvarManager->log("No battery data received.");
        }
    }

    // Reschedule the pollBatteryStatus function
    gameWrapper->SetTimeout([this](GameWrapper*) { pollBatteryStatus(); }, 10.0f);
}
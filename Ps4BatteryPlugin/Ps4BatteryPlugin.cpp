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

    // Enable input reports (only needed for Bluetooth)
    unsigned char report[] = { 0x05, 0xFF }; // Request full input report
    hid_write(controllerHandle, report, sizeof(report));

    unsigned char buffer[64];
    int res = hid_read(controllerHandle, buffer, sizeof(buffer));

    unsigned char featureBuffer[64] = { 0 };
    int resfeature = hid_get_feature_report(controllerHandle, featureBuffer, sizeof(featureBuffer));

    if (res > 12) {
        int batteryLevel = buffer[12] & 0x0F;
        int batteryLevelPercentage = (batteryLevel * 100) / 8;

        std::string batteryMessage;
        switch (batteryLevel) {
            case 0: batteryMessage = "Battery is empty!"; break;
            case 1: batteryMessage = "Battery is very low!"; break;
            case 2: batteryMessage = "Battery is low!"; break;
            case 3: batteryMessage = "Battery is medium-low!"; break;
            case 4: batteryMessage = "Battery is medium!"; break;
            case 5: batteryMessage = "Battery is medium-high!"; break;
            case 6: batteryMessage = "Battery is high!"; break;
            case 7: batteryMessage = "Battery is full!"; break;
            case 8: batteryMessage = "Battery is charging or fully charged!"; break;
            default: batteryMessage = "Battery Level is not in known range!"; break;
        }

        cvarManager->log(batteryMessage);
        cvarManager->log("Battery Level: " + std::to_string(batteryLevelPercentage) + "%");
    }
    else {
        cvarManager->log("No battery data received.");
    }

    // Poll again after x seconds
	gameWrapper->SetTimeout([this](GameWrapper*) { pollBatteryStatus(); }, 10.0f); // 10.0f = 10 seconds
}
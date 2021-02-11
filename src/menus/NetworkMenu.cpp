#include "../../include/UIMenu.h"
#include "../../include/UserInterface.h"
#include "../../include/WiFiHelper.h"
#include "../../include/BluetoothServer.h"

#include <WiFi.h>

static void onDisableWiFi(UIMenu *menu) {
  UI.toastNow("Disconnecting...", 0, false);
  Config.wifi_on = false;
  WiFiHelper::disconnect();
  saveConfigToSd(0);
  UI.toast("Disconnected.", 3000);
  menu->initialize();
  menu->render();
}

static void onEnableWiFi(UIMenu *menu) {
  if (Config.wifi_ssid[0] == '\0' || Config.wifi_key[0] == '\0') {
    UI.toastNow("No WiFi Config\nEdit config.json\non SD card.", 0);
    return;
  }

  UI.toastNow("Connecting...", 0, false);
  Config.wifi_on = true;
  if (WiFiHelper::begin()) {
    UI.toastNow("WiFi Connected!", 3000);
    saveConfigToSd(0);
    menu->initialize();
  } else {
    UI.toastNow("Failed to connect.", 3000);
    Config.wifi_on = false;
  }

  menu->render();
}

static void onViewStatus(UIMenu*) {
  String status = "";

  if (Config.bt_on) {
    status += "Bluetooth On\n";
    status += String(Config.bt_display_name);
  } else if (WiFiHelper::connected()) {
    status += "Connected\n";
    status += WiFiHelper::ip();
    status += "\nSignal: " + WiFiHelper::signalStrengthStr();
  } else {
    status += "Disconnected";
  }

  UI.toast(status.c_str(), 0);
}

static void onEnableBluetooth(UIMenu* menu) {
  UI.toastNow("Enabling...", 0, false);
  Config.bt_on = true;
  Config.wifi_on = false;

  if (WiFiHelper::connected()) {
    WiFiHelper::disconnect();
  }

  saveConfigToSd(0);
  BT.begin();

  menu->initialize();
  menu->render();
  UI.toastNow("Bluetooth On", 3000);
}

static void onDisableBluetooth(UIMenu* menu) {
  UI.toastNow("Disconnecting...", 0, false);
  Config.bt_on = false;
  saveConfigToSd(0);
  BT.disconnect();
  menu->initialize();
  menu->render();
  UI.toastNow("Disconnected.", 3000);
}

static void buildMenu(UIMenu *menu) {
  if (Config.bt_on) {
    menu->addItem("Disable Bluetooth", &onDisableBluetooth);
    menu->addItem(&BluetoothScanMenu);
  } else if (WiFiHelper::connected()) {
    menu->addItem("Disable WiFi", &onDisableWiFi);
  } else {
    menu->addItem("Enable WiFi", &onEnableWiFi);
    menu->addItem("Enable Bluetooth", &onEnableBluetooth);
  }

  menu->addItem("Connection Status", &onViewStatus);
  menu->addItem(&AccessoryPortMenu);
}

UIMenu NetworkMenu("Network Settings", &buildMenu);
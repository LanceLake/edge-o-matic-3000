#include <WiFi.h>

#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>

#include <time.h>

#include "config.h"
#include "VERSION.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include <FastLED.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneButton.h>
#include <ESP32Encoder.h>

#include "include/Hardware.h"
#include "include/UserInterface.h"
#include "include/BluetoothServer.h"
#include "include/Page.h"
#include "include/RunningAverage.h"
#include "include/Console.h"
#include "include/OrgasmControl.h"
#include "include/WiFiHelper.h"
#include "include/UpdateHelper.h"
#include "include/WebSocketHelper.h"

uint8_t LED_Brightness = 13;

// Declare LCD
#ifdef NG_PLUS
  Adafruit_SSD1306 display(128, 64);
#else
  Adafruit_SSD1306 display(128, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS);
#endif

UserInterface UI(&display);
BluetoothServer BT;

// Globals

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void resetSD() {
  // SD
  if(!SD.begin()) {
    UI.drawSdIcon(0);
    Serial.println("Card Mount Failed");
    return;
  }

  uint8_t cardType = SD.cardType();

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
      Serial.println("MMC");
      UI.drawSdIcon(1);
  } else if(cardType == CARD_SD){
      Serial.println("SDSC");
      UI.drawSdIcon(1);
  } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
      UI.drawSdIcon(1);
  } else {
      Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  loadConfigFromSd();
}

void setupHardware() {
  pinMode(BUTT_PIN, INPUT);
  pinMode(MOT_PWM_PIN, OUTPUT);

  if(!Hardware::initialize()) {
    Serial.println("Hardware initialization failed!");
    for(;;){}
  }

  if(!UI.begin()) {
    Serial.println("SSD1306 allocation failed");
    for(;;){}
  }
}

TaskHandle_t BackgroundLoopTask;

void backgroundLoop(void*) {
  for (;;) {
    WebSocketHelper::tick();
    delay(1);
  }
}

void setup() {
  // Start Serial port
  Serial.begin(115200);

#ifdef NG_PLUS
  Serial.println("Maus-Tec presents: NoGasm Plus");
#else
  Serial.println("Maus-Tec presents: Edge-o-Matic 3000");
#endif
  Serial.println("Version: " VERSION);

  // Setup Hardware
  setupHardware();

  // Go to the splash page:
  Page::Go(&DebugPage, false);

  // Setup SD, which loads our config
  resetSD();

  UI.drawWifiIcon(1);
  UI.render();

  // Initialize WiFi
  if (Config.wifi_on) {
    WiFiHelper::begin();
    WebSocketHelper::begin();
  }

  // Initialize Bluetooth
  if (Config.bt_on) {
    Serial.println("Starting up Bluetooth...");
    BT.begin();
    Serial.println("Now Discoverable!");
    BT.advertise();
  }

  // Start background worker:
  xTaskCreatePinnedToCore(
      backgroundLoop, /* Task function. */
      "backgroundLoop",   /* name of task. */
      10000,     /* Stack size of task */
      NULL,      /* parameter of the task */
      1,         /* priority of the task */
      &BackgroundLoopTask,    /* Task handle to keep track of created task */
      0);        /* pin task to core 0 */

  // I'm always one for the dramatics:
  // Hold Key1 for fast boot, used in testing.
#ifdef KEY_1_PIN
  if (digitalRead(KEY_1_PIN) == HIGH) {
#else
  if (true) {
#endif
    delay(3000);
    UI.fadeTo();
  }

  Page::Go(&RunGraphPage);
  Serial.println("READY");
}

void loop() {
  Console::loop(); // <- TODO rename to tick
  Hardware::tick();
  OrgasmControl::tick();
  UI.tick();

  static long lastStatusTick = 0;
  static long lastTick = 0;
  static int led_i = 0;

  // Periodically send out WiFi status:
  if (millis() - lastStatusTick > 1000 * 10) {
    lastStatusTick = millis();
    WebSocketHelper::sendWxStatus();
  }
  
  if (millis() - lastTick > 1000/15) {
    lastTick = millis();

    // Update LEDs
#ifdef LED_PIN
    uint8_t bar = map(OrgasmControl::getArousal(), 0, Config.sensitivity_threshold, 0, LED_COUNT - 1);
    uint8_t dot = map(OrgasmControl::getAveragePressure(), 0, 4096, 0, LED_COUNT - 1);
    for (uint8_t i = 0; i < LED_COUNT; i++) {
      if (i < bar) {
        Hardware::setLedColor(i, CRGB(map(i, 0, LED_COUNT - 1, 0, LED_Brightness),
                                      map(i, 0, LED_COUNT - 1, LED_Brightness, 0), 0));
      } else {
        Hardware::setLedColor(i);
      }
    }

    Hardware::setLedColor(dot, CRGB(LED_Brightness, 0, LED_Brightness));
    Hardware::ledShow();
#endif

    // Update Icons
    WiFiHelper::drawSignalIcon();
    WebSocketHelper::sendReadings();
  }

  Page::DoLoop();

  // Tick and see if we need to save config:
  saveConfigToSd(-1);
}

// Entrypoints:

void app_main() {
  // esp32 entry
  setup();
  while(1) loop();
}

int main(void) {
  // cpp entry
  setup();
  while(1) loop();
}
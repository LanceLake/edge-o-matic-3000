#ifndef __p_RUN_GRAPH_h
#define __p_RUN_GRAPH_h

#include "../../include/Page.h"
#include "../../include/UserInterface.h"
#include "../../include/OrgasmControl.h"
#include "../../include/Hardware.h"
#include "../../include/assets.h"
#include "../../include/WebSocketHelper.h"

enum RGView {
  GraphView,
  StatsView
};

enum RGMode {
  Manual,
  Automatic
};

class pRunGraph : public Page {
  RGView view;
  RGMode mode;

  void Enter(bool reinitialize) override {
    if (reinitialize) {
      view = StatsView;
      mode = Manual;
      OrgasmControl::controlMotor(false);
    }

    updateButtons();
    UI.setButton(1, "STOP");
  }

  void updateButtons() {
    if (view == StatsView) {
      UI.setButton(0, "CHART");
    } else {
      UI.setButton(0, "STATS");
    }

    if (mode == Automatic) {
      UI.drawStatus("Automatic");
      UI.setButton(2, "MANUAL");
    } else {
      UI.drawStatus("Manual");
      UI.setButton(2, "AUTO");
    }
  }

  void renderChart() {
    // Update Counts
    char status[7] = "";
    byte motor = Hardware::getMotorSpeedPercent() * 100;
    byte stat_a = OrgasmControl::getArousalPercent() * 100;

    UI.display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    UI.display->setCursor(0, 10);
    sprintf(status, "M:%03d%%", motor);
    UI.display->print(status);

    UI.display->setCursor(SCREEN_WIDTH / 3 + 3, 10);
    sprintf(status, "P:%04d", OrgasmControl::getAveragePressure());
    UI.display->print(status);

    UI.display->setCursor(SCREEN_WIDTH / 3 * 2 + 7, 10);
    sprintf(status, "A:%03d%%", stat_a);
    UI.display->print(status);

    // Update Chart
    UI.drawChartAxes();
    UI.drawChart(Config.sensitivity_threshold);
  }

  void renderStats() {
    static long arousal_peak = 0;
    static long last_peak_ms = millis();
    long arousal = OrgasmControl::getArousal();

    if (arousal > arousal_peak) {
      last_peak_ms = millis();
      arousal_peak = arousal;
    }

    if (millis() - last_peak_ms > 3000) {
      // decay peak after 3 seconds stale data
      arousal_peak *= 0.995f; // Decay Peak Value
    }

    // Motor / Arousal bars
    UI.drawBar(10, 'M', Hardware::getMotorSpeed(), 255, mode == Automatic ? Config.motor_max_speed : 0);
    UI.drawBar(SCREEN_HEIGHT - 18, 'A', OrgasmControl::getArousal(), 1023, Config.sensitivity_threshold, arousal_peak);

    // Pressure Icon
    int pressure_icon = map(OrgasmControl::getAveragePressure(), 0, 4095, 0, 4);
    UI.display->drawBitmap(0, 19, PLUG_ICON[pressure_icon], 24, 24, SSD1306_WHITE);

    const int horiz_split_x = (SCREEN_WIDTH / 2) + 25;

    // Pressure Bar Drawing Stuff
    const int press_x = 24 + 2;  // icon_x + icon_width + 2
    const int press_y = 19 + 12; // icon_y + (icon_height / 2)
    UI.drawCompactBar(press_x, press_y, horiz_split_x - press_x - 9, OrgasmControl::getAveragePressure(), 4095,
                   Config.sensor_sensitivity, 255);

    // Draw a Border!
    UI.display->drawLine(horiz_split_x - 3, 19, horiz_split_x - 3, SCREEN_HEIGHT - 21, SSD1306_WHITE);

    // Orgasm Denial Counter
    UI.display->setCursor(horiz_split_x + 3, 19);
    UI.display->print("Denied");
    UI.display->setCursor(horiz_split_x + 3, 19 + 9);
    UI.display->setTextSize(2);
    UI.display->printf("%3d", OrgasmControl::getDenialCount() % 1000);
    UI.display->setTextSize(1);
  }

  void Render() override {
    if (view == StatsView) {
      renderStats();
    } else {
      renderChart();
    }

    UI.drawIcons();
    UI.drawStatus();
    UI.drawButtons();
  }

  void Loop() override {
    if (OrgasmControl::updated()) {
      if (view == GraphView) {
        // Update Chart
        UI.addChartReading(0, OrgasmControl::getAveragePressure());
        UI.addChartReading(1, OrgasmControl::getArousal());
      }

      Rerender();
    }
  }

  void onKeyPress(byte i) {
    Serial.println("Key Press: " + String(i));

    switch (i) {
      case 0:
        if (view == GraphView) {
          view = StatsView;
        } else {
          view = GraphView;
        }
        break;
      case 1:
        mode = Manual;
        Hardware::setMotorSpeed(0);
        OrgasmControl::controlMotor(false);
        break;
      case 2:
        if (mode == Automatic) {
          mode = Manual;
          OrgasmControl::controlMotor(false);
        } else {
          mode = Automatic;
          OrgasmControl::controlMotor(true);
        }
        break;
    }

    updateButtons();
    Rerender();
  }

  void onEncoderChange(int diff) override {
    const int step = 255 / 20;

    if (mode == Automatic) {
      // TODO this may go out of bounds. Also, change in steps?
      Config.sensitivity_threshold += (diff * step);
      saveConfigToSd(millis() + 300);
    } else {
      Hardware::changeMotorSpeed(diff * step);
    }

    Rerender();
  }

public:
  void setMode(const char* newMode) {
    if (! strcmp(newMode, "automatic")) {
      mode = Automatic;
      OrgasmControl::controlMotor(true);
    } else if (! strcmp(newMode, "manual")) {
      mode = Manual;
      OrgasmControl::controlMotor(false);
    }

    updateButtons();
  }
};

#endif
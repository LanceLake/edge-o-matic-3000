#ifndef __p_RUN_GRAPH_h
#define __p_RUN_GRAPH_h

#include "../../include/Page.h"
#include "../../include/OrgasmControl.h"
#include "../../include/Hardware.h"

enum RGView {
    GraphView,
    StatsView
};

enum RGMode {
  Manual,
  Automatic
};

class pRunGraph : public Page {
  RGView view = GraphView;
  RGMode mode = Automatic;

  void Enter() override {
    view = GraphView;
    mode = Manual;

    updateButtons();
    OrgasmControl::controlMotor(false);
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
    byte motor  = Hardware::getMotorSpeedPercent() * 100;
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

  void drawBar(int y, char label, int value, int maximum, int limit = 0) {
    int box_left = 8;
    int box_right = SCREEN_WIDTH - (6*3) - 3;
    int box_width = box_right - box_left;
    float pct = max(0.0f, min((float)value / (float)maximum, 1.0f));
    int pct_width = floor((float)(box_width - 2) * pct);

    // Calculate a Limit bar
    int limit_left, limit_width;

    if (limit > 0) {
      float limit_pct = max(0.0f, min((float) limit / (float) maximum, 1.0f));
      int limit_pct_width = floor((float) (box_width - 2) * (1.0f - limit_pct));
      limit_left = box_right - limit_pct_width - 1;
      limit_width = box_right - limit_left - 2;
    }

    // Render Stuff
    UI.display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    UI.display->drawRect(box_left, y, box_width, 7, SSD1306_WHITE);
    UI.display->fillRect(box_left + 1, y + 1, pct_width, 5, SSD1306_WHITE);

    // Draw Limit
    if (limit > 0) {
      for (int i = limit_left; i < limit_left + limit_width; i++) {
        for (int j = 0; j < 3; j++) {
          if ((i + j) % 2 != 0)
            continue;
          UI.display->drawPixel(i, y + 2 + j, SSD1306_WHITE);
        }
      }
    }

    // Print Labels
    UI.display->setCursor(0, y);
    UI.display->print(label);
    UI.display->setCursor(box_right + 3, y);

    if (value < maximum) {
      UI.display->printf("%02d%%", (int)floor(pct * 100.0f));
    } else {
      UI.display->print("MAX");
    }
  }

  void renderStats() {
    // Important Stats:
    // 1. Arousal
    // 2. Motor Speed
    // 3. Pressure
    // 4. Arousal Sensitivity
    // 5. Pressure Sensitivity
    // 6. Max Motor Speed
    // 7. Vibrate pattern???
    drawBar(10, 'M', Hardware::getMotorSpeed(), 255, mode == Automatic ? Config.motor_max_speed : 0);
    drawBar(20-1, 'P', OrgasmControl::getLastPressure(), 4096);
    drawBar(30-2, 'A', OrgasmControl::getArousal(), Config.sensitivity_threshold);
//    drawBar(40-3, 'P', 0, 100);
//    drawBar(50-4, 'U', 100, 100);
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
    UI.render();
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

    switch(i) {
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
    const int step = 255/20;
    Serial.println("Encoder change: " + String(diff));

    if (mode == Automatic) {
      // TODO this may go out of bounds. Also, change in steps?
      Config.sensitivity_threshold += (diff * step);
    } else {
      Hardware::changeMotorSpeed(diff * step);
    }

    Rerender();
  }
};

#endif
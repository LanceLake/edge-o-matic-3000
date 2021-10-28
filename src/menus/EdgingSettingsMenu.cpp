#include "UIMenu.h"
#include "UIInput.h"
#include "Hardware.h"
#include "OrgasmControl.h"

UIInput MotorMaxSpeedInput("Motor Max Speed", [](UIMenu *ip) {
  UIInput *input = (UIInput*) ip;
  input->setMax(255);
  input->setValue(Config.motor_max_speed);
  input->setStep(16);

  input->onOpen([](UIMenu*) {
    OrgasmControl::pauseControl();
  });

  input->onClose([](UIMenu*) {
    OrgasmControl::resumeControl();
  });

  input->onChange([](int value) {
    Config.motor_max_speed = value;
    Hardware::setMotorSpeed(value);
  });

  input->onConfirm([](int value) {
    Config.motor_max_speed = value;
    saveConfigToSd(0);
  });
});

UIInput MotorStartSpeedInput("Motor Start Speed", [](UIMenu *ip) {
  UIInput *input = (UIInput*) ip;
  input->setMax(150);
  input->setStep(1);
  input->setValue(Config.motor_start_speed);
  input->onChange([](int value) {
    Config.motor_start_speed = value;
  });
  input->onConfirm([](int) {
    saveConfigToSd(0);
  });
});

UIInput MotorRampTimeInput("Motor Ramp Time", [](UIMenu *ip) {
  UIInput *input = (UIInput*) ip;
  input->setMax(120);
  input->setStep(5);
  input->setValue(Config.motor_ramp_time_s);
  input->onChange([](int value) {
    Config.motor_ramp_time_s = value;
  });
  input->onConfirm([](int) {
    saveConfigToSd(0);
  });
});

UIInput EdgeDelayInput("Edge Delay", [](UIMenu *ip) {
  UIInput *input = (UIInput*) ip;
  input->setMax(60);
  input->setStep(1);
  input->setValue(Config.edge_delay / 1000);
  input->onChange([](int value) {
    Config.edge_delay = value * 1000;
  });
  input->onConfirm([](int) {
    saveConfigToSd(0);
  });
});

UIInput MaxAdditionalDelayInput("Maximum Additional Delay", [](UIMenu *ip) {
  UIInput *input = (UIInput*) ip;
  input->setMax(60);
  input->setStep(1);
  input->setValue(Config.max_additional_delay / 1000);
  input->onChange([](int value) {
    Config.max_additional_delay = value * 1000;
  });
  input->onConfirm([](int) {
    saveConfigToSd(0);
  });
});

UIInput MinimumOnTimeInput("Minimum On Time", [](UIMenu *ip) {
  UIInput *input = (UIInput*) ip;
  input->setMax(5000);
  input->setStep(100);
  input->setValue(Config.minimum_on_time);
  input->onChange([](int value) {
    Config.minimum_on_time = value;
  });
  input->onConfirm([](int) {
    saveConfigToSd(0);
  });
});

UIInput ArousalLimitInput("Arousal Limit", [](UIMenu *ip) {
  UIInput *input = (UIInput*) ip;
  input->setMax(1023);
  input->setStep(16);
  input->setValue(Config.sensitivity_threshold);
  input->onChange([](int value) {
    Config.sensitivity_threshold = value;
  });
  input->onConfirm([](int) {
    saveConfigToSd(0);
  });
});

UIInput SensorSensitivityInput("Sensor Sensitivity", [](UIMenu *ip) {
  UIInput *input = (UIInput*) ip;
  input->setMax(255);
  input->setStep(1);
  input->setValue(Config.sensor_sensitivity);
  input->setPollPeriod(1000 / 30);
  input->getSecondaryValue([](int value) {
    return Hardware::getPressure();
  });
  input->onChange([](int value) {
    Config.sensor_sensitivity = value;
    Hardware::setPressureSensitivity(value);
  });
  input->onConfirm([](int) {
    saveConfigToSd(0);
  });
});

<<<<<<< HEAD
UIInput ClenchPressureSensitivity("Clench Pressure Sensitivity", [](UIMenu *ip) {
  UIInput *input = (UIInput*) ip;
  input->setMax(300);
  input->setStep(1);
  input->setValue(Config.clench_pressure_sensitivity);
  input->onChange([](int value) {
    Config.clench_pressure_sensitivity = value;
  });
  input->onConfirm([](int) {
    saveConfigToSd(0);
  });
});

UIInput ClenchTimeThreshold("Clench Time Threshold", [](UIMenu *ip) {
  UIInput *input = (UIInput*) ip;
  input->setMax(300);
  input->setStep(1);
  input->setValue(Config.clench_duration_threshold);
  input->onChange([](int value) {
    Config.clench_duration_threshold = value;
  });
  input->onConfirm([](int) {
    saveConfigToSd(0);
  });
});

UIInput EdgingDuration("Edge Duration Minutes", [](UIMenu *ip) {
  UIInput *input = (UIInput*) ip;
  input->setMax(300);
  input->setStep(1);
  input->setValue(Config.auto_edging_duration_minutes);
  input->onChange([](int value) {
    Config.auto_edging_duration_minutes = value;
  });
  input->onConfirm([](int) {
    saveConfigToSd(0);
  });
});

UIInput PostOrgasmDuration("Post Orgasm Seconds", [](UIMenu *ip) {
  UIInput *input = (UIInput*) ip;
  input->setMax(300);
  input->setStep(1);
  input->setValue(Config.post_orgasm_duration_seconds);
  input->onChange([](int value) {
    Config.post_orgasm_duration_seconds = value;
  });
  input->onConfirm([](int) {
    saveConfigToSd(0);
  });
});

=======
>>>>>>> my-personnal-not-usable-by-edge-o-matic-3000
static void setVibrateMode(UIMenu *menu, int m) {
  VibrationMode mode = (VibrationMode) m;

  Serial.print("Setting mode to: ");
  switch(mode) {
    case VibrationMode::Depletion:
      Serial.println("Depletion");
      break;
    case VibrationMode::Enhancement:
      Serial.println("Enhancement");
      break;
    case VibrationMode::RampStop:
      Serial.println("RampStop");
      break;
  }

  Config.vibration_mode = mode;
  saveConfigToSd(0);
  menu->rerender();
}

static void add_vibration_item(UIMenu *menu, std::string label, VibrationMode mode) {
  std::string text = "";
  if (Config.vibration_mode == mode) {
    text += "X";
  } else {
    text += " ";
  }
  text += " " + label;

  menu->addItem(text, &setVibrateMode, (int) mode);
}

static void buildVibrationModeMenu(UIMenu *menu) {
  add_vibration_item(menu, "Depletion", VibrationMode::Depletion);
  add_vibration_item(menu, "Enhancement", VibrationMode::Enhancement);
  add_vibration_item(menu, "Ramp-Stop", VibrationMode::RampStop);
}

static void onEdgeMenuUnLock(UIMenu* menu) {
  UI.toastNow("Edge UnLock", 3000);
  Config.edge_menu_lock = false;
  saveConfigToSd(0);
  menu->initialize();
  menu->render();
}

static void onEdgeMenuLock(UIMenu* menu) {
  UI.toastNow("Edge Lock", 3000);
  Config.edge_menu_lock = true;
  saveConfigToSd(0);
  menu->initialize();
  menu->render();
}

static void onPostOrgasmMenuUnLock(UIMenu* menu) {
  UI.toastNow("Post orgasm UnLock", 3000);
  Config.post_orgasm_menu_lock = false;
  saveConfigToSd(0);
  menu->initialize();
  menu->render();
}

static void onPostOrgasmMenuLock(UIMenu* menu) {
  UI.toastNow("Post orgasm Lock", 3000);
  Config.post_orgasm_menu_lock = true;
  saveConfigToSd(0);
  menu->initialize();
  menu->render();
}


UIMenu VibrationModeMenu("Vibration Mode", &buildVibrationModeMenu);

static void buildMenu(UIMenu *menu) {
  menu->addItem(&VibrationModeMenu);
  menu->addItem(&MotorMaxSpeedInput);
  menu->addItem(&MotorStartSpeedInput);
  menu->addItem(&MotorRampTimeInput);
  menu->addItem(&EdgeDelayInput);
  menu->addItem(&MaxAdditionalDelayInput);
  menu->addItem(&MinimumOnTimeInput);
  menu->addItem(&ArousalLimitInput);
  menu->addItem(&SensorSensitivityInput);
<<<<<<< HEAD
  menu->addItem(&ClenchPressureSensitivity); //  Clench Menu
  menu->addItem(&ClenchTimeThreshold);       //  Clench Menu
  menu->addItem(&EdgingDuration);            //   Auto Edging
  menu->addItem(&PostOrgasmDuration);        //   Post Orgasm
  if (Config.edge_menu_lock) {
    menu->addItem("Edge+Orgasm UnLock", &onEdgeMenuUnLock);
  } else {
    menu->addItem("Edge+Orgasm Lock", &onEdgeMenuLock);
  }
  if (Config.post_orgasm_menu_lock) {
    menu->addItem("Post Orgasm UnLock", &onPostOrgasmMenuUnLock);
  } else {
    menu->addItem("Post Orgasm Lock", &onPostOrgasmMenuLock);
  }
=======
>>>>>>> my-personnal-not-usable-by-edge-o-matic-3000
}

UIMenu EdgingSettingsMenu("Edging Settings", &buildMenu);

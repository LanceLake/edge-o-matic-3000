#include "config.h"
#include "ui/menu.h"

void on_config_save(int value, int final, UI_MENU_ARG_TYPE* arg);

static const ui_input_select_option_t vibration_modes[] = {
    {
        .label = "Depletion",
        .ival = Depletion,
        .value = NULL,
    },
    {
        .label = "Enhancement",
        .ival = Enhancement,
        .value = NULL,
    },
    {
        .label = "Ramp / Stop",
        .ival = RampStop,
        .value = NULL,
    },
};

// static const ui_input_select_t VIBRATION_MODE_INPUT = SelectInput(
//     "Vibration Mode",
//     &Config.vibration_mode,
//     &vibration_modes,
//     sizeof(vibration_modes) / sizeof(vibration_modes[0])
// );

static ui_input_numeric_t MOTOR_MAX_SPEED_INPUT = {
    ByteInputValues("Motor Max Speed", &Config.motor_max_speed, "%", on_config_save),
};

static ui_input_numeric_t MOTOR_START_SPEED_INPUT = {
    ByteInputValues("Motor Start Speed", &Config.motor_start_speed, "%", on_config_save),
};

static ui_input_numeric_t MOTOR_RAMP_TIME_INPUT = {
    ByteInputValues("Motor Ramp Time", &Config.motor_ramp_time_s, "Seconds", on_config_save),
    .max = 120,
    .step = 5,
};

static ui_input_numeric_t EDGE_DELAY_INPUT = {
    UnsignedInputValues("Edge Delay", &Config.edge_delay, "Milliseconds", on_config_save),
    .step = 100,
};

static ui_input_numeric_t MAX_ADDITIONAL_DELAY_INPUT = {
    UnsignedInputValues(
        "Max Additional Delay", &Config.max_additional_delay, "Milliseconds", on_config_save
    ),
    .max = 6000,
    .step = 100,
};

static ui_input_numeric_t MINIMUM_ON_TIME_INPUT = {
    UnsignedInputValues("Minimum On Time", &Config.minimum_on_time, "Milliseconds", on_config_save),
    .max = 5000,
    .step = 50,
};

static ui_input_numeric_t AROUSAL_LIMIT_INPUT = {
    UnsignedInputValues("Arousal Limit", &Config.sensitivity_threshold, "", on_config_save),
    .max = 1023,
    .input.help = "The cutoff point for arousal. This is adjusted on the main screen."
};

// TODO: This no longer shows a secondary chart, but it could.
static ui_input_numeric_t SENSOR_SENSITIVITY_INPUT = {
    ByteInputValues("Sensor Sensitivity", &Config.sensor_sensitivity, "%", on_config_save),
    .input.help = "Sensitivity value of the pressure sensor.",
};

static void on_open(const ui_menu_t* m, const void* arg) {
    // ui_menu_add_input(m, &VIBRATION_MODE_INPUT);
    ui_menu_add_input(m, &MOTOR_MAX_SPEED_INPUT);
    ui_menu_add_input(m, &MOTOR_START_SPEED_INPUT);
    ui_menu_add_input(m, &MOTOR_RAMP_TIME_INPUT);
    ui_menu_add_input(m, &EDGE_DELAY_INPUT);
    ui_menu_add_input(m, &MAX_ADDITIONAL_DELAY_INPUT);
    ui_menu_add_input(m, &MINIMUM_ON_TIME_INPUT);
    ui_menu_add_input(m, &AROUSAL_LIMIT_INPUT);
    ui_menu_add_input(m, &SENSOR_SENSITIVITY_INPUT);
}

DYNAMIC_MENU(EDGING_SETTINGS_MENU, "Edging Settings", on_open);
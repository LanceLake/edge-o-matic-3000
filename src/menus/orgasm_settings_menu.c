#include "assets/config_help.h"
#include "config.h"
#include "ui/input.h"
#include "ui/menu.h"
#include "util/unit_str.h"

void on_config_save(int value, int final, UI_INPUT_ARG_TYPE arg);

static const ui_input_numeric_t EDGING_DURATION_INPUT = {
    UnsignedInputValues(
        "Edge Duration Minutes", &Config.auto_edging_duration_minutes, UNIT_MINUTES, on_config_save
    ),
    .max = 300,
    .input.help = AUTO_EDGING_DURATION_MINUTES_HELP
};

// Todo: Refactor this to ms, not ticks. Ticks is kinda deprecated a whole lot.
static const ui_input_numeric_t CLENCH_ORGASM_TIME_THRESHOLD_INPUT = {
    UnsignedInputValues(
        "Clench Time to Orgasm", &Config.clench_threshold_2_orgasm, "Ticks*", on_config_save
    ),
    .max = 300, // this is actually Config.max_clench_duration but you know, c'est la vie
    .input.help = CLENCH_THRESHOLD_2_ORGASM_HELP
};

static const ui_input_numeric_t POST_ORGASM_DURATION_SECONDS_INPUT = {
    UnsignedInputValues(
        "Post Orgasm Seconds", &Config.post_orgasm_duration_seconds, UNIT_SECONDS, on_config_save
    ),
    .max = 300,
    .input.help = POST_ORGASM_DURATION_SECONDS_HELP
};

static const ui_input_toggle_t EDGE_MENU_LOCK_INPUT = {
    ToggleInputValues("Menu Lock - Edge", &Config.edge_menu_lock, on_config_save),
    .input.help = EDGE_MENU_LOCK_HELP
};

static const ui_input_toggle_t POST_ORGASM_MENU_LOCK_INPUT = {
    ToggleInputValues("Menu Lock - Post", &Config.post_orgasm_menu_lock, on_config_save),
    .input.help = POST_ORGASM_MENU_LOCK_HELP
};

static const ui_input_toggle_t CLENCH_DETECTOR_IN_EDGING_INPUT = {
    ToggleInputValues("Use Clench in Edging", &Config.clench_detector_in_edging, on_config_save),
    .input.help = CLENCH_DETECTOR_IN_EDGING_HELP
};

static const ui_input_numeric_t CLENCH_PRESSURE_SENSITIVITY_INPUT = {
    UnsignedInputValues(
        "Clench Pressure Sensitivity", &Config.clench_pressure_sensitivity, "", on_config_save
    ),
    .max = 300,
    .input.help = CLENCH_PRESSURE_SENSITIVITY_HELP
};

static void on_open(const ui_menu_t* m, UI_MENU_ARG_TYPE arg) {
    ui_menu_add_input(m, (ui_input_t*)&EDGING_DURATION_INPUT);
    ui_menu_add_input(m, (ui_input_t*)&CLENCH_ORGASM_TIME_THRESHOLD_INPUT);
    ui_menu_add_input(m, (ui_input_t*)&POST_ORGASM_DURATION_SECONDS_INPUT);
    ui_menu_add_input(m, (ui_input_t*)&EDGE_MENU_LOCK_INPUT);
    ui_menu_add_input(m, (ui_input_t*)&POST_ORGASM_MENU_LOCK_INPUT);
    ui_menu_add_input(m, (ui_input_t*)&CLENCH_DETECTOR_IN_EDGING_INPUT);
    ui_menu_add_input(m, (ui_input_t*)&CLENCH_PRESSURE_SENSITIVITY_INPUT);
}

DYNAMIC_MENU(ORGASM_SETTINGS_MENU, "Orgasm Settings", on_open);
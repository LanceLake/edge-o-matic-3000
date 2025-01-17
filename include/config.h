#ifndef __config_h
#define __config_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// SD card files:
#define CONFIG_PATH_MAX 64

// This is just a default, others can be loaded after boot.
static const char* CONFIG_FILENAME = "/config.json";

// String Lengths
#define WIFI_SSID_MAX_LEN 64
#define WIFI_KEY_MAX_LEN 64

// Some Experiments
#define EOM_BETA 1
#define I18N_USE_CJSON_DICT 1

// System Defaults
static const char* REMOTE_UPDATE_URL =
    "http://us-central1-maustec-io.cloudfunctions.net/gh-release-embedded-bridge";

// Vibration Modes
// See vibration_mode_controller.h for more.

enum vibration_mode { RampStop = 1, Depletion = 2, Enhancement = 3, Pattern = 4, GlobalSync = 0 };

typedef enum vibration_mode vibration_mode_t;

/**
 * Main Configuration Struct!
 *
 * Place all presistent runtime config variables in here, and be sure to add the appropriate def
 * in config.c!
 */

struct config {
    // Private Things, do not erase!
    char _filename[CONFIG_PATH_MAX + 1];

    //= Networking

    // Your WiFi SSID
    char wifi_ssid[WIFI_SSID_MAX_LEN + 1];
    // Your WiFi Password.
    char wifi_key[WIFI_KEY_MAX_LEN + 1];
    // True to enable WiFi / Websocket server.
    bool wifi_on;

    // AzureFang* device name, you might wanna change this.
    char bt_display_name[64];
    // True to enable the AzureFang connection.
    bool bt_on;
    // True to force AzureFang and WiFi at the same time**.
    bool force_bt_coex;

    //= UI And Stuff

    // LED Ring max brightness, only for NoGasm+.
    uint8_t led_brightness;
    // Time, in seconds, before the screen dims. 0 to disable.
    int screen_dim_seconds;
    // Time, in seconds, before the screen turns off. 0 to disable.
    int screen_timeout_seconds;
    bool enable_screensaver;
    char language_file_name[CONFIG_PATH_MAX + 1];

    //= Console

    bool store_command_history;
    bool console_basic_mode;

    //= Server

    // Port to listen for incoming Websocket connections.
    int websocket_port;
    // Output continuous stream of arousal data over serial for backwards compatibility with other
    // software.
    bool classic_serial;
    // Enable SSL server, which will eat all your RAM!
    bool use_ssl;
    // Local hostname for your device.
    char hostname[64];

    //= Orgasms and Stuff

    // Maximum speed for the motor in auto-ramp mode.
    uint8_t motor_max_speed;
    // The minimum speed the motor will start at in automatic mode.
    uint8_t motor_start_speed;
    // Minimum time (ms) after edge detection before resuming stimulation.
    int edge_delay;
    // Maximum time (ms) that can be added to the edge delay before resuming stimulation. A random
    // number will be picked between 0 and this setting each cycle. 0 to disable.
    int max_additional_delay;
    // Time (ms) after stimulation starts before edge detection is resumed.
    int minimum_on_time;
    // Number of samples to take an average of. Higher results in lag and lower resolution!
    uint8_t pressure_smoothing;
    // The arousal threshold for orgasm detection. Lower = sooner cutoff.
    int sensitivity_threshold;
    // The time it takes for the motor to reach `motor_max_speed` in auto ramp mode.
    int motor_ramp_time_s;
    // Update frequency for pressure readings and arousal steps. Higher = crash your serial monitor.
    int update_frequency_hz;
    // Analog pressure prescaling. Adjust this until the pressure is ~60-70%
    uint8_t sensor_sensitivity;
    // Use average values when calculating arousal. This smooths noisy data.
    bool use_average_values;

    //= Vibration Output Mode

    // Vibration Mode for main vibrator control.
    int vibration_mode;

    //= Post orgasm torure stuff

    // Use post-orgasm torture mode and functionality.
    bool use_post_orgasm;
    // Threshold over arousal to detect a clench : Lower values increase sensitivity
    int clench_pressure_sensitivity;
    // Duration the clench detector can raise arousal if clench detector turned on in edging session
    int max_clench_duration;
    // Threshold variable that is tick counts of clench to detect orgasm
    int clench_threshold_2_orgasm;
    // Use the clench detector to adjust Arousal
    bool clench_detector_in_edging;
    // How long to edge before permiting an orgasm
    int auto_edging_duration_minutes;
    // How long to stimulate after orgasm detected
    int post_orgasm_duration_seconds;
    // Deny access to menu starting in the edging session
    bool edge_menu_lock;
    // Deny access to menu starting after orgasm detected
    bool post_orgasm_menu_lock;

    //= Internal System Configuration (Update only if you know what you're doing)

    // Remote update server URL. You may change this to OTA update other versions.
    char* remote_update_url;
};

typedef struct config config_t;

extern config_t Config;

// These operations work on the global Config struct. For more lower-level access, check out
// the ones presented on config_defs.h
void config_enqueue_save(long save_at_ms);
bool get_config_value(const char* option, char* buffer, size_t len);
bool set_config_value(const char* option, const char* value, bool* require_reboot);

// TODO: These should be calculated values based on HAL, which works everywhere *BUT*
//       on chart rendering, where we're statically initializing our data array. That
//       should be dynamic initialization, thank yoooou. (Can be classified?)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#ifdef __cplusplus
}
#endif

#endif

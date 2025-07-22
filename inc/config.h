#define USE_DYNAMIC_MEMORY

#define DIGITAL_OUTPUT_MAX_INSTANCE     8
#define DIGITAL_INPUT_MAX_INSTANCE      4

#define SHIELD_MAX_INSTANCE             1
#define DISPLAY_MAX_DIGITS              4
#define TIME_TO_HOLD_TO_CHANGE_STATE_MS 300

// Sistema Operativo

// Definiciones para los eventos
#define ACCEPT_EVENT                    BUTTONS_EVENT_BUTTON_0
#define CANCEL_EVENT                    BUTTONS_EVENT_BUTTON_1
#define DECREMENT_EVENT                 BUTTONS_EVENT_BUTTON_2
#define INCREMENT_EVENT                 BUTTONS_EVENT_BUTTON_3
#define SET_TIME_EVENT                  BUTTONS_EVENT_BUTTON_4
#define SET_ALARM_EVENT                 BUTTONS_EVENT_BUTTON_5

#define BUTTONS_EVENT                   ACCEPT_EVENT | CANCEL_EVENT | DECREMENT_EVENT | INCREMENT_EVENT | SET_TIME_EVENT | SET_ALARM_EVENT

// Definiciones para los estados
#define INVALID_TIME_STATE              0
#define VALID_TIME_STATE                1
#define ADJUST_TIME_MINUNTES_STATE      2
#define ADJUST_TIME_HOURS_STATE         3
#define ADJUST_ALARM_MINUNTES_STATE     4
#define ADJUST_ALARM_HOURS_STATE        5
#define USE_DYNAMIC_MEMORY

#define DIGITAL_OUTPUT_MAX_INSTANCE     8
#define DIGITAL_INPUT_MAX_INSTANCE      4

#define SHIELD_MAX_INSTANCE             1
#define DISPLAY_MAX_DIGITS              4
#define TIME_TO_HOLD_TO_CHANGE_STATE_MS 300

// Sistema Operativo

// Definiciones para los eventos
#define ACCEPT_EVENT                    (1 << 0)
#define HOLD_ACCEPT_EVENT               (1 << 1)
#define CANCEL_EVENT                    (1 << 2)
#define HOLD_CANCEL_EVENT               (1 << 3)
#define DECREMENT_EVENT                 (1 << 4)
#define HOLD_DECREMENT_EVENT            (1 << 5)
#define INCREMENT_EVENT                 (1 << 6)
#define HOLD_INCREMENT_EVENT            (1 << 7)
#define SET_TIME_EVENT                  (1 << 8)
#define HOLD_SET_TIME_EVENT             (1 << 9)
#define SET_ALARM_EVENT                 (1 << 10)
#define HOLD_SET_ALARM_EVENT            (1 << 11)
#define DIDNT_PRESS_EVENT               (1 << 12)

#define PUSH_BUTTONS_EVENT                                                                                             \
    ACCEPT_EVENT | CANCEL_EVENT | DECREMENT_EVENT | INCREMENT_EVENT | SET_TIME_EVENT | SET_ALARM_EVENT
#define HOLD_BUTTONS_EVENT                                                                                             \
    HOLD_ACCEPT_EVENT | HOLD_CANCEL_EVENT | HOLD_DECREMENT_EVENT | HOLD_INCREMENT_EVENT | HOLD_SET_TIME_EVENT |        \
        HOLD_SET_ALARM_EVENT
#define BUTTONS_EVENT                                                                                                  \
    ACCEPT_EVENT | HOLD_ACCEPT_EVENT | CANCEL_EVENT | HOLD_CANCEL_EVENT | DECREMENT_EVENT | HOLD_DECREMENT_EVENT |     \
        INCREMENT_EVENT | HOLD_INCREMENT_EVENT | SET_TIME_EVENT | HOLD_SET_TIME_EVENT | SET_ALARM_EVENT |              \
        HOLD_SET_ALARM_EVENT | DIDNT_PRESS_EVENT

#define SECOND_EVENT (1 << 0)
#define WRITE_FLAG   (1 << 1)
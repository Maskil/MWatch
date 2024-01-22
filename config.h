#ifndef CONFIG_H
#define CONFIG_H

//Change to your board version
#define ARDUINO_WATCHY_V20

/* #define MENU_BTN_PIN 26 */
/* #define BACK_BTN_PIN 25 */
/* #define DOWN_BTN_PIN 4 */
/* #define DISPLAY_CS 5 */
/* #define DISPLAY_RES 9 */
/* #define DISPLAY_DC 10 */
/* #define DISPLAY_BUSY 19 */
/* #define ACC_INT_1_PIN 14 */
/* #define ACC_INT_2_PIN 12 */
/* #define VIB_MOTOR_PIN 13 */
/* #define RTC_INT_PIN 27 */
/* #define UP_BTN_PIN 35 */
/* #define BATT_ADC_PIN 34 */
/* #define UP_BTN_MASK  GPIO_SEL_35 */
/* #define RTC_TYPE 2 //PCF8563 */
/* #define MENU_BTN_MASK GPIO_SEL_26 */
/* #define BACK_BTN_MASK GPIO_SEL_25 */
/* #define DOWN_BTN_MASK GPIO_SEL_4 */
/* #define ACC_INT_MASK  GPIO_SEL_14 */
/* #define BTN_PIN_MASK  MENU_BTN_MASK|BACK_BTN_MASK|UP_BTN_MASK|DOWN_BTN_MASK */

//display
#define DISPLAY_WIDTH 200
#define DISPLAY_HEIGHT 200
#define BIG_FONT_Y_OFFSET 40
#define SMALL_FONT_Y_OFFSET 15
// menu
#define WATCHFACE_STATE 0
#define MAIN_MENU_STATE 1
#define APP_STATE       2
#define MAIL_STATE      3
#define MENU_HEIGHT     25
#define MENU_LENGTH     5
#define WIFI_LENGTH     3
// set time
#define SET_HOUR   0
#define SET_MINUTE 1
#define SET_YEAR   2
#define SET_MONTH  3
#define SET_DAY    4
#define HOUR_12_24 24
// BLE OTA
#define SOFTWARE_VERSION_MAJOR 1
#define SOFTWARE_VERSION_MINOR 0
#define SOFTWARE_VERSION_PATCH 0
#define HARDWARE_VERSION_MAJOR 1
#define HARDWARE_VERSION_MINOR 0

#endif

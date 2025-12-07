#ifndef CONFIG_H
#define CONFIG_H

// LCD Pin Definitions
#define EXAMPLE_PIN_NUM_LCD_SCLK 39
#define EXAMPLE_PIN_NUM_LCD_MOSI 38
#define EXAMPLE_PIN_NUM_LCD_MISO 40
#define EXAMPLE_PIN_NUM_LCD_DC 42
#define EXAMPLE_PIN_NUM_LCD_RST -1
#define EXAMPLE_PIN_NUM_LCD_CS 45
#define EXAMPLE_PIN_NUM_LCD_BL 1

// Touch Pin Definitions
#define EXAMPLE_PIN_NUM_TP_SDA 48
#define EXAMPLE_PIN_NUM_TP_SCL 47

// LED PWM Configuration
#define LEDC_FREQ             5000
#define LEDC_TIMER_10_BIT     10

// Display Configuration
#define EXAMPLE_LCD_ROTATION 0
#define EXAMPLE_LCD_H_RES 240
#define EXAMPLE_LCD_V_RES 320

// Theme Colors
#define THEME_BG_COLOR lv_color_white()
#define THEME_PRIMARY_COLOR lv_color_make(33, 150, 243) 
#define THEME_TEXT_COLOR lv_color_make(33, 150, 243)

// Settings Default Values
#define DEFAULT_BRIGHTNESS 80
#define DEFAULT_SLEEP_TIMER 60000  // 1 minute in milliseconds (0 = disabled)

#endif // CONFIG_H
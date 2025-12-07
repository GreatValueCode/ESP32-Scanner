#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

// Settings structure
struct AppSettings {
  uint8_t brightness;      // 0-100
  uint32_t sleepTimer;     // milliseconds (0 = disabled)
};

// Global settings
extern AppSettings settings;
extern uint32_t lastActivityTime;

// Function declarations
void initSettings();
void applyBrightness(uint8_t brightness);
void resetSleepTimer();
void checkSleepTimer();
void sleepScreen();
void wakeScreen();

#endif // SETTINGS_H
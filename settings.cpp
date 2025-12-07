#include "settings.h"
#include "config.h"

AppSettings settings;
uint32_t lastActivityTime = 0;
bool screenAsleep = false;

void initSettings() {
  settings.brightness = DEFAULT_BRIGHTNESS;
  settings.sleepTimer = DEFAULT_SLEEP_TIMER;
  applyBrightness(settings.brightness);
  lastActivityTime = millis();
}

void applyBrightness(uint8_t brightness) {
  settings.brightness = brightness;
  uint32_t dutyCycle = ((1 << LEDC_TIMER_10_BIT) * brightness) / 100;
  ledcWrite(EXAMPLE_PIN_NUM_LCD_BL, dutyCycle);
}

void resetSleepTimer() {
  lastActivityTime = millis();
  if (screenAsleep) {
    wakeScreen();
  }
}

void checkSleepTimer() {
  if (settings.sleepTimer == 0) return;  // Sleep timer disabled
  
  uint32_t currentTime = millis();
  if (!screenAsleep && (currentTime - lastActivityTime) >= settings.sleepTimer) {
    sleepScreen();
  }
}

void sleepScreen() {
  if (screenAsleep) return;
  ledcWrite(EXAMPLE_PIN_NUM_LCD_BL, 0);
  screenAsleep = true;
  Serial.println("Screen sleeping...");
}

void wakeScreen() {
  if (!screenAsleep) return;
  applyBrightness(settings.brightness);
  screenAsleep = false;
  Serial.println("Screen waking...");
}
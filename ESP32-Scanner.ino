#include <lvgl.h>
#include <demos/lv_demos.h>

#include "config.h"
#include "display.h"
#include "wifi_scanner.h"
#include "settings.h"
#include "ui.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=========================");
  Serial.println("ESP32-S3 WiFi Scanner with LVGL");
  Serial.println("=========================\n");

  // Initialize display hardware
  initDisplay();
  
  // Initialize touch controller
  initTouch();
  
  // Initialize WiFi
  initWiFi();
  
  // Initialize settings
  initSettings();
  
  // Initialize LVGL
  initLVGL();
  
  // Create user interface
  create_ui();

  Serial.println("Setup complete - Ready to scan!");
  Serial.println("Tap SCAN button to start\n");
}

void loop() {
  renderDisplay();
}
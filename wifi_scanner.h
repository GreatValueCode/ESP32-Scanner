#ifndef WIFI_SCANNER_H
#define WIFI_SCANNER_H

#include <Arduino.h>
#include <WiFi.h>
#include <vector>

// WiFi data structure
struct WiFiNetwork {
  String ssid;
  int32_t rssi;
  String bssid;
  int channel;
  wifi_auth_mode_t encryption;
};

// WiFi scanner state
extern std::vector<WiFiNetwork> networks;
extern bool isScanning;
extern bool scanStarted;

// Function declarations
void initWiFi();
void scan_wifi_networks();
void scan_complete_callback(void *param);
int getRSSIQuality(int32_t rssi);

#endif // WIFI_SCANNER_H
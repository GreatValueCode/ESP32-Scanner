#include "wifi_scanner.h"
#include "ui.h"
#include <lvgl.h>
#include <algorithm>

std::vector<WiFiNetwork> networks;
bool isScanning = false;
bool scanStarted = false;
lv_timer_t *scan_timer = NULL;

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

int getRSSIQuality(int32_t rssi) {
  if (rssi >= -50) return 4;
  else if (rssi >= -60) return 3;
  else if (rssi >= -70) return 2;
  else if (rssi >= -80) return 1;
  else return 0;
}

void scan_complete_callback(void *param) {
  Serial.println("=== SCAN COMPLETE CALLBACK ===");
  Serial.println("Setting isScanning = false");
  isScanning = false;
  Serial.println("Setting scanStarted = false");
  scanStarted = false;
  
  Serial.println("Sorting networks...");
  std::sort(networks.begin(), networks.end(), [](const WiFiNetwork &a, const WiFiNetwork &b) {
    return a.rssi > b.rssi;
  });
  
  Serial.println("Updating network list UI...");
  update_network_list();
  
  Serial.println("Loading results screen with animation...");
  lv_scr_load_anim(get_results_screen(), LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 300, 0, false);
  
  Serial.println("Scan complete!");
  Serial.printf("Found %d networks\n", networks.size());
  Serial.println("=== DONE ===");
}

void scan_wifi_networks() {
  Serial.println("=== START FAKE SCAN ===");

  if (scan_timer) lv_timer_del(scan_timer);

  scan_timer = lv_timer_create([](lv_timer_t *t){
    lv_timer_del(t);
    scan_timer = NULL;

    networks.clear();

    WiFiNetwork net1 = {"Sirloin 222", -45, "A4:5E:60:E2:3F:11", 36, WIFI_AUTH_WPA2_PSK};
    networks.push_back(net1);
    WiFiNetwork net2 = {"Patty 678", -62, "B8:27:EB:4A:12:C3", 6, WIFI_AUTH_OPEN};
    networks.push_back(net2);
    WiFiNetwork net3 = {"Wagu 102", -71, "C0:56:27:9D:8E:F4", 11, WIFI_AUTH_WPA2_PSK};
    networks.push_back(net3);
    WiFiNetwork net4 = {"Chuck 677", -58, "D4:6E:0E:1B:7A:22", 1, WIFI_AUTH_WPA2_WPA3_PSK};
    networks.push_back(net4);
    WiFiNetwork net5 = {"Daisy 112", -78, "E8:94:F6:3C:5D:88", 48, WIFI_AUTH_WPA2_PSK};
    networks.push_back(net5);
    WiFiNetwork net6 = {"Bob 288", -85, "F0:9F:C2:1A:4E:67", 149, WIFI_AUTH_WPA3_PSK};
    networks.push_back(net6);

    scan_complete_callback(NULL);
  }, 3000, NULL);
}
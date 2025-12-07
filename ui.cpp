#include "ui.h"
#include "config.h"
#include "display.h"
#include "wifi_scanner.h"
#include "settings.h"

// UI objects
lv_obj_t *main_screen;
lv_obj_t *results_screen;
lv_obj_t *settings_screen;
lv_obj_t *scan_btn;
lv_obj_t *scan_btn_label;
lv_obj_t *loading_spinner;
lv_obj_t *network_list;
lv_obj_t *back_btn;
lv_obj_t *title_label;
lv_obj_t *settings_btn;

lv_obj_t* get_results_screen() {
  return results_screen;
}

lv_obj_t* get_main_screen() {
  return main_screen;
}

void create_network_list_item(WiFiNetwork &network) {
  lv_obj_t *list_btn = lv_list_add_btn(network_list, LV_SYMBOL_WIFI, "");
  lv_obj_set_style_bg_color(list_btn, THEME_BG_COLOR, 0);
  lv_obj_set_style_text_color(list_btn, THEME_TEXT_COLOR, 0);
  lv_obj_set_height(list_btn, 80);
  
  // Container for all text (to keep it aligned)
  lv_obj_t *text_container = lv_obj_create(list_btn);
  lv_obj_set_size(text_container, 180, 75);
  lv_obj_align(text_container, LV_ALIGN_TOP_LEFT, 35, 2);
  lv_obj_set_style_bg_opa(text_container, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(text_container, 0, 0);
  lv_obj_set_style_pad_all(text_container, 0, 0);
  
  // SSID Title (Blue)
  lv_obj_t *ssid_label = lv_label_create(text_container);
  lv_label_set_text(ssid_label, network.ssid.c_str());
  lv_obj_set_style_text_color(ssid_label, THEME_TEXT_COLOR, 0);
  lv_obj_set_style_text_font(ssid_label, &lv_font_montserrat_14, 0);
  lv_obj_align(ssid_label, LV_ALIGN_TOP_LEFT, 0, 0);
  
  // Signal strength bars
  String bars = "";
  int quality = getRSSIQuality(network.rssi);
  for (int i = 0; i < 4; i++) {
    if (i < quality) {
      bars += "|";
    } else {
      bars += ".";
    }
  }
  
  int min_temp = 60;
  int max_temp = 100;
  int temp = min_temp + (std::rand() % (max_temp - min_temp + 1));

  // Convert temperature to String
  String temp_str = String(temp);

  // Create the LVGL label
  lv_obj_t *info_label = lv_label_create(text_container);

  // Build the info text
  String info = "rssi: " + String(network.rssi) + "\n" +
                "MAC: " + String(network.bssid) + "\n" +
                temp_str + "F°";

  // Set the text for the label
  lv_label_set_text(info_label, info.c_str());
  lv_obj_set_style_text_color(info_label, lv_color_black(), 0);
  lv_obj_set_style_text_font(info_label, &lv_font_montserrat_12, 0);
  lv_obj_set_width(info_label, 180);
  lv_obj_align(info_label, LV_ALIGN_TOP_LEFT, 0, 20);
}

void update_network_list() {
  if (network_list) {
    lv_obj_clean(network_list);
  }
  
  for (auto &network : networks) {
    create_network_list_item(network);
  }
}

void scan_btn_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    if (!isScanning) {
      Serial.println("=== BUTTON CLICKED ===");
      isScanning = true;

      lv_obj_add_flag(scan_btn, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(loading_spinner, LV_OBJ_FLAG_HIDDEN);

      scan_wifi_networks();  
    }
  }
}

void settings_btn_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    lv_scr_load_anim(settings_screen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
  }
}

void back_btn_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    // Reset scan button state
    lv_obj_clear_flag(scan_btn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(loading_spinner, LV_OBJ_FLAG_HIDDEN);
    scanStarted = false;
    
    // Go back to main screen
    lv_scr_load(main_screen);
  }
}

void settings_back_btn_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    lv_scr_load_anim(main_screen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
  }
}

void brightness_slider_event_cb(lv_event_t *e) {
  lv_obj_t *slider = lv_event_get_target(e);
  int32_t value = lv_slider_get_value(slider);
  applyBrightness((uint8_t)value);
  
  // Update label
  lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e);
  lv_label_set_text_fmt(label, "Brightness: %d%%", value);
}

void sleep_timer_dropdown_event_cb(lv_event_t *e) {
  lv_obj_t *dropdown = lv_event_get_target(e);
  uint16_t selected = lv_dropdown_get_selected(dropdown);
  
  // Map dropdown index to sleep timer value
  switch(selected) {
    case 0: settings.sleepTimer = 0; break;           // Off
    case 1: settings.sleepTimer = 30000; break;       // 30 seconds
    case 2: settings.sleepTimer = 60000; break;       // 1 minute
    case 3: settings.sleepTimer = 180000; break;      // 3 minutes
    case 4: settings.sleepTimer = 300000; break;      // 5 minutes
    case 5: settings.sleepTimer = 600000; break;      // 10 minutes
  }
  
  Serial.printf("Sleep timer set to: %lu ms\n", settings.sleepTimer);
}

void create_ui() {
  // ========== MAIN SCAN SCREEN ==========
  main_screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(main_screen, THEME_BG_COLOR, 0);
  
  // Settings button in top right corner
  settings_btn = lv_btn_create(main_screen);
  lv_obj_set_size(settings_btn, 60, 60);
  lv_obj_align(settings_btn, LV_ALIGN_TOP_RIGHT, -10, 10);
  lv_obj_set_style_bg_opa(settings_btn, LV_OPA_TRANSP, 0);  // Make background transparent
  lv_obj_set_style_shadow_width(settings_btn, 0, 0);  // Remove shadow
  lv_obj_set_style_border_width(settings_btn, 0, 0);  // Remove border
  lv_obj_add_event_cb(settings_btn, settings_btn_event_cb, LV_EVENT_ALL, NULL);
  
  lv_obj_t *settings_icon = lv_label_create(settings_btn);
  lv_label_set_text(settings_icon, LV_SYMBOL_SETTINGS);
  lv_obj_set_style_text_color(settings_icon, THEME_PRIMARY_COLOR, 0);  // Blue icon
  lv_obj_set_style_text_font(settings_icon, &lv_font_montserrat_32, 0);
  lv_obj_center(settings_icon);
  
  // Loading spinner (behind button, hidden by default)
  loading_spinner = lv_spinner_create(main_screen, 1000, 60);
  lv_obj_set_size(loading_spinner, 100, 100);
  lv_obj_align(loading_spinner, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_arc_color(loading_spinner, THEME_PRIMARY_COLOR, LV_PART_INDICATOR);
  lv_obj_set_style_arc_width(loading_spinner, 8, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(loading_spinner, lv_color_make(200, 220, 255), LV_PART_MAIN);
  lv_obj_set_style_arc_width(loading_spinner, 8, LV_PART_MAIN);
  lv_obj_add_flag(loading_spinner, LV_OBJ_FLAG_HIDDEN);
  
  // Circular scan button in the center (on top of spinner)
  scan_btn = lv_btn_create(main_screen);
  lv_obj_set_size(scan_btn, 120, 120);
  lv_obj_align(scan_btn, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_bg_color(scan_btn, THEME_PRIMARY_COLOR, 0);
  lv_obj_set_style_radius(scan_btn, LV_RADIUS_CIRCLE, 0);
  lv_obj_add_event_cb(scan_btn, scan_btn_event_cb, LV_EVENT_ALL, NULL);
  
  // Button label
  scan_btn_label = lv_label_create(scan_btn);
  lv_label_set_text(scan_btn_label, "SCAN");
  lv_obj_set_style_text_color(scan_btn_label, lv_color_white(), 0);
  lv_obj_set_style_text_font(scan_btn_label, &lv_font_montserrat_16, 0);
  lv_obj_center(scan_btn_label);
  
  // ========== RESULTS SCREEN ==========
  results_screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(results_screen, THEME_BG_COLOR, 0);
  
  // Title bar
  lv_obj_t *title_bar = lv_obj_create(results_screen);
  lv_obj_set_size(title_bar, screenWidth, 50);
  lv_obj_align(title_bar, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(title_bar, THEME_PRIMARY_COLOR, 0);
  lv_obj_set_style_radius(title_bar, 0, 0);
  lv_obj_set_style_border_width(title_bar, 0, 0);
  lv_obj_set_style_pad_all(title_bar, 0, 0);
  
  // Title
  title_label = lv_label_create(title_bar);
  lv_label_set_text(title_label, "AP List");
  lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
  lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, 0);
  lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);
  
  // Back button
  back_btn = lv_btn_create(title_bar);
  lv_obj_set_size(back_btn, 70, 40);
  lv_obj_align(back_btn, LV_ALIGN_LEFT_MID, 5, 0);
  lv_obj_set_style_bg_color(back_btn, lv_color_make(50, 130, 220), 0);
  lv_obj_add_event_cb(back_btn, back_btn_event_cb, LV_EVENT_ALL, NULL);
  
  lv_obj_t *back_label = lv_label_create(back_btn);
  lv_label_set_text(back_label, LV_SYMBOL_LEFT " Back");
  lv_obj_set_style_text_color(back_label, lv_color_white(), 0);
  lv_obj_center(back_label);
  
  // Network list (full screen minus title bar)
  network_list = lv_list_create(results_screen);
  lv_obj_set_size(network_list, screenWidth, screenHeight - 50);
  lv_obj_align(network_list, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(network_list, THEME_BG_COLOR, 0);
  lv_obj_set_style_border_width(network_list, 0, 0);
  lv_obj_set_style_radius(network_list, 0, 0);
  
  // ========== SETTINGS SCREEN ==========
  settings_screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(settings_screen, THEME_BG_COLOR, 0);
  
  // Title bar
  lv_obj_t *settings_title_bar = lv_obj_create(settings_screen);
  lv_obj_set_size(settings_title_bar, screenWidth, 50);
  lv_obj_align(settings_title_bar, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(settings_title_bar, THEME_PRIMARY_COLOR, 0);
  lv_obj_set_style_radius(settings_title_bar, 0, 0);
  lv_obj_set_style_border_width(settings_title_bar, 0, 0);
  lv_obj_set_style_pad_all(settings_title_bar, 0, 0);
  
  // Title
  lv_obj_t *settings_title_label = lv_label_create(settings_title_bar);
  lv_label_set_text(settings_title_label, "Settings");
  lv_obj_set_style_text_color(settings_title_label, lv_color_white(), 0);
  lv_obj_set_style_text_font(settings_title_label, &lv_font_montserrat_14, 0);
  lv_obj_align(settings_title_label, LV_ALIGN_CENTER, 0, 0);
  
  // Back button
  lv_obj_t *settings_back_btn = lv_btn_create(settings_title_bar);
  lv_obj_set_size(settings_back_btn, 70, 40);
  lv_obj_align(settings_back_btn, LV_ALIGN_LEFT_MID, 5, 0);
  lv_obj_set_style_bg_color(settings_back_btn, lv_color_make(50, 130, 220), 0);
  lv_obj_add_event_cb(settings_back_btn, settings_back_btn_event_cb, LV_EVENT_ALL, NULL);
  
  lv_obj_t *settings_back_label = lv_label_create(settings_back_btn);
  lv_label_set_text(settings_back_label, LV_SYMBOL_LEFT " Back");
  lv_obj_set_style_text_color(settings_back_label, lv_color_white(), 0);
  lv_obj_center(settings_back_label);
  
  // Settings content container
  lv_obj_t *settings_content = lv_obj_create(settings_screen);
  lv_obj_set_size(settings_content, screenWidth, screenHeight - 50);
  lv_obj_align(settings_content, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(settings_content, THEME_BG_COLOR, 0);
  lv_obj_set_style_border_width(settings_content, 0, 0);
  lv_obj_set_style_radius(settings_content, 0, 0);
  lv_obj_set_style_pad_all(settings_content, 20, 0);
  lv_obj_set_flex_flow(settings_content, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(settings_content, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  
  // ===== BRIGHTNESS SETTING =====
  lv_obj_t *brightness_label = lv_label_create(settings_content);
  lv_label_set_text_fmt(brightness_label, "Brightness: %d%%", settings.brightness);
  lv_obj_set_style_text_color(brightness_label, THEME_TEXT_COLOR, 0);
  lv_obj_set_style_text_font(brightness_label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_pad_bottom(brightness_label, 10, 0);
  
  lv_obj_t *brightness_slider = lv_slider_create(settings_content);
  lv_slider_set_range(brightness_slider, 10, 100);
  lv_slider_set_value(brightness_slider, settings.brightness, LV_ANIM_OFF);
  lv_obj_set_width(brightness_slider, 200);
  lv_obj_set_style_pad_bottom(brightness_slider, 30, 0);
  lv_obj_add_event_cb(brightness_slider, brightness_slider_event_cb, LV_EVENT_VALUE_CHANGED, brightness_label);
  
  // ===== SLEEP TIMER SETTING =====
  lv_obj_t *sleep_timer_label = lv_label_create(settings_content);
  lv_label_set_text(sleep_timer_label, "Sleep Timer");
  lv_obj_set_style_text_color(sleep_timer_label, THEME_TEXT_COLOR, 0);
  lv_obj_set_style_text_font(sleep_timer_label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_pad_bottom(sleep_timer_label, 10, 0);
  
  lv_obj_t *sleep_timer_dropdown = lv_dropdown_create(settings_content);
  lv_dropdown_set_options(sleep_timer_dropdown, "Off\n30 Seconds\n1 Minute\n3 Minutes\n5 Minutes\n10 Minutes");
  
  // Set default selection based on current sleep timer
  uint16_t default_selection = 4; // Default to 5 minutes
  if (settings.sleepTimer == 0) default_selection = 0;
  else if (settings.sleepTimer == 30000) default_selection = 1;
  else if (settings.sleepTimer == 60000) default_selection = 2;
  else if (settings.sleepTimer == 180000) default_selection = 3;
  else if (settings.sleepTimer == 300000) default_selection = 4;
  else if (settings.sleepTimer == 600000) default_selection = 5;
  
  lv_dropdown_set_selected(sleep_timer_dropdown, default_selection);
  lv_obj_set_width(sleep_timer_dropdown, 200);
  lv_obj_add_event_cb(sleep_timer_dropdown, sleep_timer_dropdown_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
  
  // Load main screen initially
  lv_scr_load(main_screen);
}
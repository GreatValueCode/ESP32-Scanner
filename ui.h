#ifndef UI_H
#define UI_H

#include <lvgl.h>
#include "wifi_scanner.h"

// UI Screen objects
extern lv_obj_t *main_screen;
extern lv_obj_t *results_screen;
extern lv_obj_t *settings_screen;
extern lv_obj_t *scan_btn;
extern lv_obj_t *scan_btn_label;
extern lv_obj_t *loading_spinner;
extern lv_obj_t *network_list;
extern lv_obj_t *back_btn;
extern lv_obj_t *title_label;
extern lv_obj_t *settings_btn;

// Function declarations
void create_ui();
void create_network_list_item(WiFiNetwork &network);
void update_network_list();
lv_obj_t* get_results_screen();
lv_obj_t* get_main_screen();

// Event callbacks
void scan_btn_event_cb(lv_event_t *e);
void back_btn_event_cb(lv_event_t *e);
void settings_btn_event_cb(lv_event_t *e);
void settings_back_btn_event_cb(lv_event_t *e);
void brightness_slider_event_cb(lv_event_t *e);
void sleep_timer_dropdown_event_cb(lv_event_t *e);

#endif // UI_H
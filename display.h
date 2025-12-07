#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino_GFX_Library.h>
#include <lvgl.h>

// Display objects
extern Arduino_DataBus *bus;
extern Arduino_GFX *gfx;
extern uint32_t screenWidth;
extern uint32_t screenHeight;
extern uint32_t bufSize;
extern lv_disp_draw_buf_t draw_buf;
extern lv_color_t *disp_draw_buf;
extern lv_disp_drv_t disp_drv;

// Function declarations
void initDisplay();
void initLVGL();
void initTouch();
void renderDisplay();

// LVGL callbacks
void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);

#if LV_USE_LOG != 0
void my_print(const char *buf);
#endif

#endif // DISPLAY_H
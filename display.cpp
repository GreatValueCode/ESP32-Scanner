#include "display.h"
#include "config.h"
#include "bsp_cst816.h"
#include "settings.h"

Arduino_DataBus *bus = new Arduino_ESP32SPI(
  EXAMPLE_PIN_NUM_LCD_DC, EXAMPLE_PIN_NUM_LCD_CS,
  EXAMPLE_PIN_NUM_LCD_SCLK, EXAMPLE_PIN_NUM_LCD_MOSI, EXAMPLE_PIN_NUM_LCD_MISO);

Arduino_GFX *gfx = new Arduino_ST7789(
  bus, EXAMPLE_PIN_NUM_LCD_RST, EXAMPLE_LCD_ROTATION, true,
  EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES);

uint32_t screenWidth;
uint32_t screenHeight;
uint32_t bufSize;
lv_disp_draw_buf_t draw_buf;
lv_color_t *disp_draw_buf;
lv_disp_drv_t disp_drv;

#if LV_USE_LOG != 0
void my_print(const char *buf) {
  Serial.printf(buf);
  Serial.flush();
}
#endif

void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
  lv_disp_flush_ready(disp_drv);
}

void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
  uint16_t touchpad_x;
  uint16_t touchpad_y;
  bsp_touch_read();
  if (bsp_touch_get_coordinates(&touchpad_x, &touchpad_y)) {
    data->point.x = touchpad_x;
    data->point.y = touchpad_y;
    data->state = LV_INDEV_STATE_PRESSED;
    resetSleepTimer();  // Reset sleep timer on touch
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void initDisplay() {
  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);

  ledcAttach(EXAMPLE_PIN_NUM_LCD_BL, LEDC_FREQ, LEDC_TIMER_10_BIT);
  ledcWrite(EXAMPLE_PIN_NUM_LCD_BL, (1 << LEDC_TIMER_10_BIT) / 100 * 80);
}

void initTouch() {
  Wire.begin(EXAMPLE_PIN_NUM_TP_SDA, EXAMPLE_PIN_NUM_TP_SCL);
  bsp_touch_init(&Wire, gfx->getRotation(), gfx->width(), gfx->height());
}

void initLVGL() {
  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print);
#endif

  screenWidth = gfx->width();
  screenHeight = gfx->height();
  bufSize = screenWidth * screenHeight;

  disp_draw_buf = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  if (!disp_draw_buf) {
    disp_draw_buf = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_8BIT);
  }

  if (!disp_draw_buf) {
    Serial.println("LVGL disp_draw_buf allocate failed!");
  } else {
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, bufSize);

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.direct_mode = true;

    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
  }
}

void renderDisplay() {
  lv_timer_handler();
  checkSleepTimer();  // Check if screen should sleep
#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(0, 0, (uint16_t *)disp_draw_buf, screenWidth, screenHeight);
#else
  gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)disp_draw_buf, screenWidth, screenHeight);
#endif
}
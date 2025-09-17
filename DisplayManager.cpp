#include "DisplayManager.h"
#include <M5Unified.h>

// Use LVGL v8+ types (if your LVGL is v8+)
void DisplayManager::begin() {
    lv_init();
    M5.Lcd.init();
    static lv_color_t buf1[LV_HOR_RES_MAX * 10];
    static lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, LV_HOR_RES_MAX * 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = [](lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p){
        M5.Lcd.startWrite();
        M5.Lcd.pushColors((uint16_t*)color_p, (area->x2-area->x1+1)*(area->y2-area->y1+1));
        M5.Lcd.endWrite();
        lv_disp_flush_ready(drv);
    };
    disp_drv.draw_buf = &draw_buf;
    disp_drv.hor_res = M5.Lcd.width();
    disp_drv.ver_res = M5.Lcd.height();
    lv_disp_drv_register(&disp_drv);

    createPage();
}

void DisplayManager::createPage() {
    labelRPM = lv_label_create(lv_scr_act());
    lv_label_set_text(labelRPM, "RPM: 0");
    lv_obj_align(labelRPM, LV_ALIGN_TOP_LEFT, 5, 5);

    labelDAC = lv_label_create(lv_scr_act());
    lv_label_set_text(labelDAC, "DAC: 0 mV");
    lv_obj_align(labelDAC, LV_ALIGN_TOP_LEFT, 5, 30);

    labelAT = lv_label_create(lv_scr_act());
    lv_label_set_text(labelAT, "AutoTune: Idle");
    lv_obj_align(labelAT, LV_ALIGN_TOP_LEFT, 5, 55);
}

void DisplayManager::update(float rpm, float dacOutput, bool autotuneActive) {
    static char buf[32];

    snprintf(buf, sizeof(buf), "RPM: %.0f", rpm);
    lv_label_set_text(labelRPM, buf);

    snprintf(buf, sizeof(buf), "DAC: %.0f mV", dacOutput);
    lv_label_set_text(labelDAC, buf);

    snprintf(buf, sizeof(buf), "AutoTune: %s", autotuneActive ? "Running" : "Idle");
    lv_label_set_text(labelAT, buf);

    lv_timer_handler();  // Update LVGL
}
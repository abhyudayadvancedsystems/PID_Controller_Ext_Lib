#pragma once
#include <lvgl.h>

class DisplayManager {
public:
    void begin();
    void update(float rpm, float dacOutput, bool autotuneActive);
private:
    lv_obj_t* labelRPM;
    lv_obj_t* labelDAC;
    lv_obj_t* labelAT;
    void createPage();
};

#ifndef LV_H
#define LV_H

#include "lvgl.h"

#define DRAW_REF_LINE 0

extern lv_obj_t *chart;
extern lv_chart_series_t *ser1;
#if DRAW_REF_LINE
extern lv_chart_series_t *ser2;
#endif
extern lv_obj_t *label_hr;
extern lv_obj_t *heart_img;

LV_IMG_DECLARE(COR1);
LV_IMG_DECLARE(COR2);

void lv_setup();
void lv_loop();

#endif //LV_H
#include "lvgl.h"

extern lv_obj_t *chart;
extern lv_chart_series_t *ser1;
extern lv_chart_series_t *ser2;
extern lv_obj_t *label_hr;
extern lv_obj_t * img1;

LV_IMG_DECLARE(COR1);
LV_IMG_DECLARE(COR2);

void lv_setup();
void lv_loop();
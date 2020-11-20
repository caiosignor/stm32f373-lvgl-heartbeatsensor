#include "lvgl.h"
#include "lv.h"
#include "ILI9341_Touchscreen.h"
#include "ILI9341_STM32_Driver.h"
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "adc.h"
#include "math.h"
#include "tim.h"

#define start_conversion()                          \
    {                                               \
        HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_2); \
        HAL_ADC_Start_IT(&hadc1);                   \
    }

#define BASE_TEMPO 5 //ms
#define CHART_POINTS 128

uint32_t tempo;
lv_obj_t *chart;
lv_chart_series_t *ser1;
lv_obj_t *label_hr;
lv_style_t style;
lv_obj_t *heart_img;

#if DRAW_REF_LINE
lv_chart_series_t *ser2;
#endif

void ili9341_flush(lv_disp_drv_t *drv, const lv_area_t *area, const lv_color_t *color_p)
{
    if (area->x2 < 0 || area->y2 < 0 || area->x1 > (LV_HOR_RES_MAX - 1) || area->y1 > (LV_VER_RES_MAX - 1))
    {
        lv_disp_flush_ready(drv);
        return;
    }

    /* Truncate the area to the screen */
    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2 > LV_HOR_RES_MAX - 1 ? LV_HOR_RES_MAX - 1 : area->x2;
    int32_t act_y2 = area->y2 > LV_VER_RES_MAX - 1 ? LV_VER_RES_MAX - 1 : area->y2;

    int32_t y;
    uint8_t data[4];
    int32_t len = len = (act_x2 - act_x1 + 1) * 2;
    lv_coord_t w = (area->x2 - area->x1) + 1;

    /* window horizontal */
    ILI9341_Write_Command(ILI9341_CASET);
    data[0] = act_x1 >> 8;
    data[1] = act_x1;
    data[2] = act_x2 >> 8;
    data[3] = act_x2;
    ILI9341_Write_Array(data, 4);

    /* window vertical */
    ILI9341_Write_Command(ILI9341_PASET);
    data[0] = act_y1 >> 8;
    data[1] = act_y1;
    data[2] = act_y2 >> 8;
    data[3] = act_y2;
    ILI9341_Write_Array(data, 4);

    ILI9341_Write_Command(ILI9341_RAMWR);
    for (y = act_y1; y <= act_y2; y++)
    {
        ILI9341_Write_Array((uint8_t *)color_p, len);
        color_p += w;
    }

    lv_disp_flush_ready(drv);
}

void create_container_chart()
{
    /** Container chart **/
    lv_obj_t *cont_chart = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_size(cont_chart, 240, 150);
    lv_obj_align(cont_chart, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);

    /*Create a chart*/
    chart = lv_chart_create(cont_chart, NULL);
    lv_obj_set_size(chart, 240, 150);
    lv_obj_align(chart, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
    lv_chart_set_point_count(chart, CHART_POINTS);
    lv_chart_set_range(chart, 0, 50);

    lv_obj_set_style_local_line_width(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 2);
    lv_obj_set_style_local_size(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0); /*radius of points*/

    /*Add two data series*/
    ser1 = lv_chart_add_series(chart, LV_COLOR_RED); // pulse wave form
#if DRAW_REF_LINE
    ser2 = lv_chart_add_series(chart, LV_COLOR_WHITE); //line reference to consider a pulse.
#endif
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_CIRCULAR);
    lv_chart_refresh(chart); /*Required after direct set*/
}

void create_container_labels()
{
    /** container bpm **/
    lv_obj_t *cont_bpm = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_size(cont_bpm, 75, 150);
    lv_obj_align(cont_bpm, NULL, LV_ALIGN_IN_RIGHT_MID, 0, 0);
    lv_cont_set_layout(cont_bpm, LV_LAYOUT_CENTER);

    /** Images **/
    heart_img = lv_img_create(cont_bpm, NULL);
    lv_img_set_src(heart_img, &COR1);
    lv_obj_align(heart_img, NULL, LV_ALIGN_CENTER, 0, 0);

    /* create label */
    label_hr = lv_label_create(cont_bpm, NULL);
    lv_style_init(&style);
    lv_style_set_text_font(&style, LV_STATE_DEFAULT, &lv_font_montserrat_40);

    lv_style_set_text_color(&style, LV_STATE_DEFAULT, LV_COLOR_RED);

    lv_obj_add_style(label_hr, LV_OBJ_PART_MAIN, &style);
    lv_obj_set_size(label_hr, 100, 70);

    lv_obj_t *label = lv_label_create(cont_bpm, NULL);
    lv_label_set_text(label, "BPM");
}

void create_main_scr(void)
{
    create_container_chart();
    create_container_labels();
}

void lv_setup()
{
    ILI9341_Init(); //initial driver setup to drive ili9341
    ILI9341_Set_Rotation(1);
    char txt[32];
    size_t len = sprintf(txt, "oi\n");

    HAL_UART_Transmit(&huart1, (uint8_t *)txt, len, 1000);
    lv_init();

    //buffer interno para do display
    static lv_disp_buf_t disp_buff;

    //inicializa os buffers internos.
    static lv_color_t buf_1[LV_HOR_RES_MAX * 10];
    lv_disp_buf_init(&disp_buff, buf_1, NULL, LV_HOR_RES_MAX * 10);

    // //informações do driver do display; contém as callbacks necessárias para interagir com o display;
    lv_disp_drv_t disp_drv;
    //inicializacao basica
    lv_disp_drv_init(&disp_drv);

    //configura o driver do display;
    // disp_drv.rotated = 1;
    disp_drv.buffer = &disp_buff;
    disp_drv.flush_cb = ili9341_flush;

    //registra o driver do display no objeto do display.
    lv_disp_drv_register(&disp_drv);

    create_main_scr();

    tempo = HAL_GetTick();
    start_conversion();
}

void lv_loop()
{
    if (tempo < HAL_GetTick())
    {
        lv_tick_inc(BASE_TEMPO);
        tempo = HAL_GetTick() + BASE_TEMPO;
        lv_task_handler();
    }
}
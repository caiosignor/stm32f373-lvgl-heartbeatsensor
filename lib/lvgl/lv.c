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

#define TAMANHO_AMOSTRAS_DMA 32

uint32_t amostras[TAMANHO_AMOSTRAS_DMA];
uint32_t amostras_dma[TAMANHO_AMOSTRAS_DMA];

uint32_t tempo;

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

lv_obj_t *chart;
lv_chart_series_t *ser1;
lv_chart_series_t *ser2;
lv_obj_t *label_hr;

lv_style_t style;
lv_obj_t *img1;

void lv_ex_btn_1(void)
{


    /** Images **/
    img1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img1, &COR1);
    lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 75, -60);    

    // LV_IMG_DECLARE(logo2_bmp);
    // LV_IMG_DECLARE(logo3_bmp);

    /* create label */
    label_hr = lv_label_create(lv_scr_act(), NULL);
    lv_style_init(&style);
    lv_style_set_text_font(&style, LV_STATE_DEFAULT, &lv_font_montserrat_40);
    // lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
    // lv_style_set_(&style, LV_STATE_DEFAULT, LV_COLOR_YELLOW);;
    lv_style_set_text_color(&style, LV_STATE_DEFAULT, LV_COLOR_RED);
    // lv_style_set_text_color(&style, LV_STATE_DEFAULT, LV_COLOR_BLUE);

    lv_obj_add_style(label_hr, LV_OBJ_PART_MAIN, &style);
    lv_obj_align(label_hr, NULL, LV_ALIGN_IN_RIGHT_MID, 15, -85);
    static lv_style_t style1; /*Declare a new style. Should be static`*/
    lv_obj_set_size(label_hr, 100, 70);

    lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(label, NULL, LV_ALIGN_IN_RIGHT_MID, -65, -90);
    lv_label_set_text(label, "HR");
    lv_obj_set_height(label, 100);

    // lv_label_set_align(label_hr, LV_LABEL_ALIGN_LEFT); /*Center aligned lines*/

    /*Create a chart*/
    chart = lv_chart_create(lv_scr_act(), NULL);
    lv_obj_set_size(chart, 220, 100);
    lv_obj_align(chart, NULL, LV_ALIGN_IN_LEFT_MID, 0, -50);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
    lv_chart_set_point_count(chart, TAMANHO_AMOSTRAS_DMA * 4);
    lv_chart_set_range(chart, 0, 50);



    // lv_obj_set_style_local_border_opa(chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    // lv_obj_set_style_local_pad_all(chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, 0);
    // lv_obj_set_style_local_bg_opa(chart, LV_CHART_PART_SERIES_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    // lv_obj_set_style_local_bg_color(chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, LV_COLOR_CYAN);
    lv_obj_set_style_local_line_width(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 2);
    lv_obj_set_style_local_size(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0); /*radius of points*/

    /*Add two data series*/
    ser1 = lv_chart_add_series(chart, LV_COLOR_RED);
    ser2 = lv_chart_add_series(chart, LV_COLOR_WHITE);
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_CIRCULAR);
    lv_chart_refresh(chart); /*Required after direct set*/
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

    lv_ex_btn_1(); //inicializa os botões

    tempo = HAL_GetTick();
    start_conversion();
}

uint32_t tempo2;

void lv_loop()
{

    if (tempo < HAL_GetTick())
    {
        lv_tick_inc(BASE_TEMPO);
        tempo = HAL_GetTick() + BASE_TEMPO;
        lv_task_handler();
    }

    // if (tempo2 < HAL_GetTick())
    // {
    //     static uint32_t pos = 0;
    //     HAL_ADC_Start(&hadc1);
    //     HAL_ADC_PollForConversion(&hadc1, 1000);
    //     uint32_t valor = HAL_ADC_GetValue(&hadc1);
    //     // uint32_t valor = 2048 + 2048 * sin((3.1415 / TAMANHO_AMOSTRAS_DMA) * pos++);
    //     lv_chart_set_next(chart, ser1, valor);
    //     if (pos > TAMANHO_AMOSTRAS_DMA)
    //         pos = 0;
    //     tempo2 = HAL_GetTick() + 5;
    //     lv_chart_refresh(chart); /*Required after direct set*/
    // }
}
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "ui.h"

// Touchscreen pins
#define XPT2046_IRQ 36  // T_IRQ
#define XPT2046_MOSI 32 // T_DIN
#define XPT2046_MISO 39 // T_OUT
#define XPT2046_CLK 25  // T_CLK
#define XPT2046_CS 33   // T_CS

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

// Touchscreen coordinates: (x, y) and pressure (z)
int x, y, z;

#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

void touchscreen_read(lv_indev_t *indev, lv_indev_data_t *data);

void setup()
{
    String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
    Serial.begin(115200);
    Serial.println(LVGL_Arduino);

    lv_init();

    touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touchscreen.begin(touchscreenSPI);
    touchscreen.setRotation(4);

    // Create a display object
    lv_display_t *disp;
    // Initialize the TFT display using the TFT_eSPI library
    disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));

    // Initialize an LVGL input device object (Touchscreen)
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    // Set the callback function to read Touchscreen input
    lv_indev_set_read_cb(indev, touchscreen_read);

    ui_init();
}

void loop()
{
    lv_task_handler();
    lv_tick_inc(5);
    delay(5);
}

// Get the Touchscreen data
void touchscreen_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    // Checks if Touchscreen was touched, and prints X, Y and Pressure (Z)
    if (touchscreen.tirqTouched() && touchscreen.touched())
    {
        // Get Touchscreen points
        TS_Point p = touchscreen.getPoint();
        // Calibrate Touchscreen points with map function to the correct width and height
        x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
        y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
        z = p.z;

        data->state = LV_INDEV_STATE_PRESSED;

        // Set the coordinates
        data->point.x = x;
        data->point.y = y;

        // Print Touchscreen info about X, Y and Pressure (Z) on the Serial Monitor
        /* Serial.print("X = ");
        Serial.print(x);
        Serial.print(" | Y = ");
        Serial.print(y);
        Serial.print(" | Pressure = ");
        Serial.print(z);
        Serial.println();*/
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

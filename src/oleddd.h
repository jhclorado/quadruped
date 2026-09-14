#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <pgmspace.h>
#include "../drawings/all_face_bitmaps.h"


// Define ESP32 pins connected to display
#define OLED_CLK   18   // Hardware VSPI SCK
#define OLED_MOSI  23   // Hardware VSPI MOSI
#define OLED_CS    5    // Connected to CS1
#define OLED_DC    2    // Connected to DC
#define OLED_RESET -1   // Set to -1 if your board lacks a physical RST pin

/* 
 * CONSTRUCTOR SELECTION:
 * Below is a standard 4-wire HW SPI constructor for an SSD1306 128x64 display.
 * If your screen stays blank, swap this line out for your specific driver 
 * (e.g., U8G2_SSD1309_128X64_F_4W_HW_SPI or U8G2_SH1106_128X64_F_4W_HW_SPI)
 */
U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs= */ OLED_CS, /* dc= */ OLED_DC, /* reset= */ OLED_RESET);

// Draw a 1-bit bitmap stored row-major, MSB-first per byte.
// Format: each row is padded to a whole number of bytes: row_bytes = (width+7)/8.
// Bit ordering in each byte: bit7 = leftmost pixel of the byte, bit0 = rightmost.
// The bitmap may be stored in flash (PROGMEM); this function reads via pgm_read_byte_near.
// Example usage:
//   drawBitmapPixels(myBitmap, 128, 64, 0, 0);
void drawBitmapPixels(const uint8_t *bmp, uint16_t width, uint16_t height, int x0, int y0) {
    uint16_t row_bytes = (width + 7) / 8;
    for (uint16_t y = 0; y < height; ++y) {
        for (uint16_t x = 0; x < width; ++x) {
            uint32_t idx = (uint32_t)y * row_bytes + (x >> 3);
            uint8_t byte = pgm_read_byte_near(bmp + idx);
            uint8_t bit = 7 - (x & 7); // MSB-first
            if (byte & (1 << bit)) {
                u8g2.drawPixel(x0 + x, y0 + y);
            }
        }
    }
}

static void showFaceBitmap(const uint8_t *bitmap) {
    if (bitmap == nullptr) {
        return;
    }

    u8g2.clearBuffer();
    drawBitmapPixels(bitmap, 128, 64, 0, 0);
    u8g2.sendBuffer();
}

static void showIdleFace() {
    showFaceBitmap(epd_bitmap_idle);
}

static void showSleepFace() {
    showFaceBitmap(epd_bitmap_sleepy);
}

static void showHappyFace() {
    showFaceBitmap(epd_bitmap_happy);
}

static void showSadFace() {
    showFaceBitmap(epd_bitmap_sad);
}

static void showAngryFace() {
    showFaceBitmap(epd_bitmap_angry);
}

static void showTalkHappyFace() {
    showFaceBitmap(epd_bitmap_talk_happy);
}

static void showTalkAngryFace() {
    showFaceBitmap(epd_bitmap_talk_angry);
}

static void showTalkSadFace() {
    showFaceBitmap(epd_bitmap_talk_sad);
}

static void showTalkSleepyFace() {
    showFaceBitmap(epd_bitmap_talk_sleepy);
}

static void showIdleAnimation(uint16_t delay_ms = 120, uint8_t repeats = 1) {
    (void)delay_ms;
    (void)repeats;
    showIdleFace();
}

static void showSleepAnimation(uint16_t delay_ms = 120, uint8_t repeats = 1) {
    (void)delay_ms;
    (void)repeats;
    showSleepFace();
}

static void showHappyAnimation(uint16_t delay_ms = 120, uint8_t repeats = 1) {
    (void)delay_ms;
    (void)repeats;
    showHappyFace();
}

static void showSadAnimation(uint16_t delay_ms = 120, uint8_t repeats = 1) {
    (void)delay_ms;
    (void)repeats;
    showSadFace();
}

static void showAngryAnimation(uint16_t delay_ms = 120, uint8_t repeats = 1) {
    (void)delay_ms;
    (void)repeats;
    showAngryFace();
}

static void oled_updateAnimation() {
    // The bitmap faces are rendered directly, so there is no frame-by-frame loop to advance here.
}
void oled_init() {
    Serial.begin(115200);
    
    // Fire up the display
    u8g2.begin();
}

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <pgmspace.h>
#include "../drawings/smiley.h"
#include "../drawings/happy_frame_01.h"
#include "../drawings/happy_frame_02.h"
#include "../drawings/happy_frame_03.h"
#include "../drawings/happy_frame_04.h"
#include "../drawings/happy_frame_05.h"
#include "../drawings/happy_frame_06.h"
#include "../drawings/happy_frame_07.h"
#include "../drawings/happy_frame_08.h"

#include "../drawings/sad/sad_1.h"
#include "../drawings/sad/sad_2.h"
#include "../drawings/sad/sad_3.h"
#include "../drawings/sad/sad_4.h"
#include "../drawings/sad/sad_5.h"
#include "../drawings/sad/sad_6.h"
#include "../drawings/sad/sad_7.h"
#include "../drawings/sad/sad_8.h"

#include "../drawings/idle/idle_1.h"
#include "../drawings/idle/idle_2.h"
#include "../drawings/idle/idle_3.h"
#include "../drawings/idle/idle_4.h"
#include "../drawings/idle/idle_5.h"
#include "../drawings/idle/idle_6.h"
#include "../drawings/idle/idle_7.h"
#include "../drawings/idle/idle_8.h"


#include "../drawings/angry/angry_1.h"
#include "../drawings/angry/angry_2.h"
#include "../drawings/angry/angry_3.h"
#include "../drawings/angry/angry_4.h"
#include "../drawings/angry/angry_5.h"
#include "../drawings/angry/angry_6.h"
#include "../drawings/angry/angry_7.h"
#include "../drawings/angry/angry_8.h"

#include "../drawings/sleep/sleep_1.h"
#include "../drawings/sleep/sleep_2.h"
#include "../drawings/sleep/sleep_3.h"
#include "../drawings/sleep/sleep_4.h"
#include "../drawings/sleep/sleep_5.h"
#include "../drawings/sleep/sleep_6.h"
#include "../drawings/sleep/sleep_7.h"
#include "../drawings/sleep/sleep_8.h"


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

struct OledAnimation {
    const uint8_t * const *frames = nullptr;
    size_t frame_count = 0;
    uint16_t delay_ms = 120;
    uint8_t repeats = 1;
    uint8_t repeat_index = 0;
    size_t frame_index = 0;
    uint32_t next_update_ms = 0;
    bool active = false;
};

static OledAnimation oled_animation;

static const uint8_t * const happy_frames[] PROGMEM = {
    happy_frame_01,
    happy_frame_02,
    happy_frame_03,
    happy_frame_04,
    happy_frame_05,
    happy_frame_06,
    happy_frame_07,
    happy_frame_08,
};

static const uint8_t * const sleep_frames[] PROGMEM = {
    sleep_1,
    sleep_2,
    sleep_3,
    sleep_4,
    sleep_5,
    sleep_6,
    sleep_7,
    sleep_8,
};

static const uint8_t * const idle_frames[] PROGMEM = {
    idle_1,
    idle_2,
    idle_3,
    idle_4,
    idle_5,
    idle_6,
    idle_7,
    idle_8,
};

static const uint8_t * const sad_frames[] PROGMEM = {
    sad_1,
    sad_2,
    sad_3,
    sad_4,
    sad_5,
    sad_6,
    sad_7,
    sad_8,
};

static const uint8_t * const angry_frames[] PROGMEM = {
    angry_1,
    angry_2,
    angry_3,
    angry_4,
    angry_5,
    angry_6,
    angry_7,
    angry_8,
};

static void renderAnimationFrame(const uint8_t *frame_ptr) {
    u8g2.clearBuffer();
    drawBitmapPixels(frame_ptr, 128, 64, 0, 0);
    u8g2.sendBuffer();
}

static void renderAnimationFrameIndex(size_t index) {
    if (oled_animation.frames == nullptr || oled_animation.frame_count == 0 || index >= oled_animation.frame_count) {
        return;
    }

    const uint8_t *frame_ptr = (const uint8_t *)pgm_read_ptr(&(oled_animation.frames[index]));
    renderAnimationFrame(frame_ptr);
}

static void playAnimation(const uint8_t * const frames[] PROGMEM, size_t frame_count, uint16_t delay_ms=120, uint8_t repeats=1) {
    if (frame_count == 0) {
        oled_animation.active = false;
        return;
    }

    oled_animation.frames = frames;
    oled_animation.frame_count = frame_count;
    oled_animation.delay_ms = delay_ms;
    oled_animation.repeats = repeats;
    oled_animation.repeat_index = 0;
    oled_animation.frame_index = 0;
    oled_animation.next_update_ms = millis();
    oled_animation.active = true;

    renderAnimationFrameIndex(0);
    oled_animation.next_update_ms = millis() + oled_animation.delay_ms;
}

static void oled_updateAnimation() {
    if (!oled_animation.active || oled_animation.frames == nullptr || oled_animation.frame_count == 0) {
        return;
    }

    if ((int32_t)(millis() - oled_animation.next_update_ms) < 0) {
        return;
    }

    size_t next_index = oled_animation.frame_index + 1;

    if (next_index >= oled_animation.frame_count) {
        next_index = 0;
        if (oled_animation.repeats != 0) {
            if (++oled_animation.repeat_index >= oled_animation.repeats) {
                oled_animation.active = false;
                return;
            }
        }
    }

    oled_animation.frame_index = next_index;
    renderAnimationFrameIndex(oled_animation.frame_index);
    oled_animation.next_update_ms = millis() + oled_animation.delay_ms;
}

// Show the happy animation. Call this from anywhere after `setup()`.
static void showHappyAnimation(uint16_t delay_ms = 120, uint8_t repeats = 1) {
    const size_t happy_count = sizeof(happy_frames) / sizeof(happy_frames[0]);
    playAnimation(happy_frames, happy_count, delay_ms, repeats);
}

static void showSleepAnimation(uint16_t delay_ms = 120, uint8_t repeats = 1) {
    const size_t sleep_count = sizeof(sleep_frames) / sizeof(sleep_frames[0]);
    playAnimation(sleep_frames, sleep_count, delay_ms, repeats);
}

static void showIdleAnimation(uint16_t delay_ms = 120, uint8_t repeats = 1) {
    const size_t idle_count = sizeof(idle_frames) / sizeof(idle_frames[0]);
    playAnimation(idle_frames, idle_count, delay_ms, repeats);
}

// Show the sad animation. Call this from anywhere after `setup()`.
static void showSadAnimation(uint16_t delay_ms = 120, uint8_t repeats = 1) {
    const size_t sad_count = sizeof(sad_frames) / sizeof(sad_frames[0]);
    playAnimation(sad_frames, sad_count, delay_ms, repeats);
}

static void showAngryAnimation(uint16_t delay_ms = 120, uint8_t repeats = 1) {
    const size_t angry_count = sizeof(angry_frames) / sizeof(angry_frames[0]);
    playAnimation(angry_frames, angry_count, delay_ms, repeats);
}

void oled_init() {
    Serial.begin(115200);
    
    // Fire up the display
    u8g2.begin();
}

#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"
#include "SD_MMC.h"
#include "head.h"

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM 1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

#define WINDOW_TOP 120
#define WINDOW_LEFT 80

#define WINDOW_HEIGHT 60
#define WINDOW_WIDTH 80

#define SAMPLING_SPACING 1

void initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB565; 
  config.frame_size = FRAMESIZE_240X240;
  config.jpeg_quality = 10;
  config.fb_count = 2;
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void doCapturing(struct HsvColor *hsv) {
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();  
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  int sumR = 0;
  int sumG = 0;
  int sumB = 0;
  int sampleCount = 0;
  for (int i = 0; i < WINDOW_WIDTH; i += SAMPLING_SPACING) {
    for (int j = 0; j < WINDOW_HEIGHT; j += SAMPLING_SPACING) {
      uint8_t rgb565 = fb->buf[((j + WINDOW_TOP) * 240 + i + WINDOW_LEFT) * 2];
      sampleCount++;
      sumR += rgb565 >> 11;
      sumG += (rgb565 & 0x7E0) >> 5;
      sumB += rgb565 & 0x1F;
    }
  }

  esp_camera_fb_return(fb);
  float r = (sumR) / (32.0F * sampleCount);
  float g = (sumG) / (32.0F * sampleCount);
  float b = (sumB) / (32.0F * sampleCount);
  float max = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
  float min = (r < g) ? ((r < b) ? r : b) : ((g < b) ? g : b);
  float v = max;
  float diff = max - min;
  float s = (max == 0.0F) ? 0 : (diff / max);
  float h;
  if (s == 0.0F) {
    h = 0;
  } else if (max == r) {
    h = fmod(60 * ((g - b) / diff) + 360, 360) / 360;
  } else if (max == g) {
    h = fmod(60 * ((b - r) / diff) + 120, 360) / 360;
  } else if (max == b) {
    h = fmod(60 * ((r - g) / diff) + 240, 360) / 360;
  }

  hsv->h = h;
  hsv->s = s;
  hsv->v = v;
  Serial.printf("h = %f, s = %f, v = %f\n", h, s, v); // Debug
}
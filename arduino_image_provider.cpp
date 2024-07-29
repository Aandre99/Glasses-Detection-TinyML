#include "image_provider.h"

#ifndef ARDUINO_EXCLUDE_CODE

#include "Arduino.h"
#include <TinyMLShield.h>

constexpr int kFrameWidth = 176;
constexpr int kFrameHeight = 144;
constexpr int kCropSize = 64;

// Get an image from the camera module
TfLiteStatus GetImage(tflite::ErrorReporter* error_reporter, int image_width,
                      int image_height, int channels, int8_t* image_data) {

  byte data[kFrameWidth * kFrameHeight]; // Receiving QCIF grayscale from camera

  static bool g_is_camera_initialized = false;

  // Initialize camera if necessary
  if (!g_is_camera_initialized) {
    if (!Camera.begin(QCIF, GRAYSCALE, 5, OV7675)) {
      TF_LITE_REPORT_ERROR(error_reporter, "Failed to initializesss camera!");
      return kTfLiteError;
    }
    g_is_camera_initialized = true;
  }

  // Read camera data
  Camera.readFrame(data);

  int min_x = (kFrameWidth - kCropSize) / 2;
  int min_y = (kFrameHeight - kCropSize) / 2;
  int index = 0;

  // Crop 96x96 image. This lowers FOV, ideally we would downsample but this is simpler.
  for (int y = min_y; y < min_y + kCropSize; y++) {
    for (int x = min_x; x < min_x + kCropSize; x++) {
      image_data[index++] = static_cast<int8_t>(data[(y * kFrameWidth) + x - 128]); // convert TF input image to signed 8-bit
    }
  }

  return kTfLiteOk;
}

#endif  // ARDUINO_EXCLUDE_CODE

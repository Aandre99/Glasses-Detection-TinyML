/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at
  http://www.apache.org/licenses/LICENSE-2.0
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
  ==============================================================================*/

#include "image_provider.h"

#ifndef ARDUINO_EXCLUDE_CODE

#include "Arduino.h"
#include "model/model_settings.h"
#include <TinyMLShield.h>

int8_t bilinear_interpolate(const uint8_t* data, float x, float y, int width) {
  int x1 = static_cast<int>(x);
  int y1 = static_cast<int>(y);
  int x2 = x1 + 1;
  int y2 = y1 + 1;

  float a = x - x1;
  float b = y - y1;

  int8_t value = static_cast<int8_t>(
    (1 - a) * (1 - b) * data[y1 * width + x1] +
    a * (1 - b) * data[y1 * width + x2] +
    (1 - a) * b * data[y2 * width + x1] +
    a * b * data[y2 * width + x2]
  );

  return value - 128; // convert TF input image to signed 8-bit
}

// Get an image from the camera module
TfLiteStatus GetImage(tflite::ErrorReporter* error_reporter, int image_width,
                      int image_height, int channels, int8_t* image_data) {

  byte data[176 * 144]; // Receiving QCIF grayscale from camera = 176 * 144 * 1

  static bool g_is_camera_initialized = false;
  // static bool serial_is_initialized = false;

  // Initialize camera if necessary
  if (!g_is_camera_initialized) {
    if (!Camera.begin(QCIF, GRAYSCALE, 5, OV7675)) {
      TF_LITE_REPORT_ERROR(error_reporter, "Failed to initialize camera!");
      return kTfLiteError;
    }
    g_is_camera_initialized = true;
  }

  // Read camera data
  Camera.readFrame(data);

  /*int min_x = (176 - kNumRows) / 2;
  int min_y = (144 - kNumCols) / 2;
  int index = 0;*/

  /*// Crop 64x64 image. This lowers FOV, ideally we would downsample but this is simpler. 
  for (int y = min_y; y < min_y + kNumCols; y++) {
    for (int x = min_x; x < min_x + kNumRows; x++) {
      image_data[index++] = static_cast<int8_t>(data[(y * 176) + x] - 128); // convert TF input image to signed 8-bit
    }
  }*/

  int index = 0;
  float scale_x = 176.0 / kNumRows;
  float scale_y = 144.0 / kNumCols;

  // Resize 64x64 image using bilinear interpolation
  for (int y = 0; y < kNumCols; y++) {
    for (int x = 0; x < kNumRows; x++) {
      float src_x = x * scale_x;
      float src_y = y * scale_y;
      image_data[index++] = bilinear_interpolate(data, src_x, src_y, 176);
    }
  }

  return kTfLiteOk;
}

#endif  // ARDUINO_EXCLUDE_CODE
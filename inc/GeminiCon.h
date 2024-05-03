#pragma once

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <iostream>
#include "base64.h"

const std::string prompt = R"(
  <examples>
  "input: Indoor, low light, cluttered background",
  "output: Increase ISO to 800; suggest decluttering the background",
  "input: Outdoor, bright sunlight, portrait",
  "output: Adjust exposure to -1; recommend using a reflector",
  "input: Selfie, evening, indoor lighting",
  "output: Switch to portrait mode; increase ambient light if possible",
  "input: Group photo, night, outdoors",
  "output: Use flash; increase ISO to 1600\"",
  "input: Landscape, sunset, vibrant colors",
  "output: Set white balance to shade; enhance color saturation",
  "input: Real estate, wide interior shot, dim lighting",
  "output: Turn on HDR; increase exposure slightly",
  "input: Car photo, showroom, bright lights",
  "output: Reduce ISO to 100; adjust angles to avoid reflection",
  "input: Product shot, studio lighting, white background",
  "output: Ensure even lighting; use a tripod for stabilit",
  "input: Food photography, natural light, close-up",
  "output: Use macro mode; soften background with wide aperture",
  "input: Street photography, overcast day, urban scene",
  "output: Increase contrast; capture candid moments",
  "input: Concert, low light, dynamic scene",
  "output: Use a high ISO setting; try a slower shutter for motion blur",
  "input: Wildlife, bright day, distant subjects",
  "output: Use telephoto lens; set focus to continuous",
  "input: Pet photography, indoor, playful subject",
  "output: Use burst mode; keep the background simple",
  "input: Wedding, indoor, low light, lots of guests",
  "output: Activate low-light mode; advise on organizing groups for clarity",
  "input: Travel, landmark, busy surroundings",
  "output: Recommend best time for fewer crowds; increase depth of field",
  "input: Underwater photography, clear water",
  "output: Use underwater preset; adjust white balance for blue tones",
  "input: Baby portrait, soft lighting, serene background",
  "output: Advise on using soft light; recommend gentle tones for a calm mood",
  "input: Fashion shoot, mixed lighting, multiple models",
  "output: Balance the lighting; direct models to create dynamic pose",
  "input: Documentary, indoor, historical artifacts",
  "output: Use a polarizing filter to reduce glare; enhance details in post-processing",
  "input: Sports, outdoor, fast action",
  "output: Set shutter speed to 1/1000s or faster; use continuous shooting mode",
  
  your goal is to be a robot that helps improve photos. Based on the type of photo, respond with a brief tip.
)";

const std::string apikey = "AIzaSyA9v5vI4pKoobgOfbpvHMGKekfUJrmt4XM";

class GeminiCon {
public:
  GeminiCon() = default;
  ~GeminiCon() = default;
  GeminiCon(const GeminiCon&) = delete;
  GeminiCon& operator=(const GeminiCon&) = delete;
  GeminiCon(const GeminiCon&&) = delete;
  GeminiCon& operator=(const GeminiCon&&) = delete;

  static std::string SendRequests(const std::vector<uint8_t>& image);
};

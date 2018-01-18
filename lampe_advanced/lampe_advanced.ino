#include <Basecamp.hpp>
Basecamp iot;

#include <NeoPixelBus.h>

#define RGBW_PIN        16
#define RGBW_PIXELS     270

#define WWA_PIN         17
#define WWA_PIXELS      315

#define PIXEL_PER_ROW   45

NeoPixelBus<NeoGrbwFeature, NeoWs2813Method> rgbwstrip(RGBW_PIXELS, RGBW_PIN);
NeoPixelBus<NeoRgbFeature, NeoWs2813Method> wwastrip(WWA_PIXELS, WWA_PIN);

String RGBWTopic;
String WWATopic;

void setup() {
  iot.begin();

  RGBWTopic = "cmnd/" + iot.hostname + "/rgbw";
  WWATopic = "cmnd/" + iot.hostname + "/wwa";

  iot.mqtt.onMessage(onMqttMessage);
  iot.mqtt.onConnect(onMqttConnect);

  rgbwstrip.Begin();
  rgbwstrip.Show();
  wwastrip.Begin();
  wwastrip.Show();

}

void onMqttConnect(bool sessionPresent) {
  iot.mqtt.subscribe(RGBWTopic.c_str(), 2);
  iot.mqtt.subscribe(WWATopic.c_str(), 2);
}

void onMqttMessage(char* topic, char* payload,
                   AsyncMqttClientMessageProperties properties,
                   size_t len, size_t index, size_t total) {

  Serial.println(topic);
  if (strcmp(topic, RGBWTopic.c_str()) == 0) {
    Serial.println("rgb");
    int R = getIntFromHex(payload, 0);
    int G = getIntFromHex(payload, 1);
    int B = getIntFromHex(payload, 2);
    int W = getIntFromHex(payload, 3);
    setRGBW(R, G, B, W);

  } else if (strcmp(topic, WWATopic.c_str()) == 0) {
    Serial.println("wwa");
    int WW = getIntFromHex(payload, 0);
    int CW = getIntFromHex(payload, 1);
    int A = getIntFromHex(payload, 2);
    setWWA(WW, CW, A);
  }
}

void setRGBW(int R, int G, int B, int W) {
  for (int i = 0; i < RGBW_PIXELS; i++) {
    rgbwstrip.SetPixelColor(i, RgbwColor(R, G, B, W));
  }
  delay(10);
  portDISABLE_INTERRUPTS();
  rgbwstrip.Show();
  portENABLE_INTERRUPTS();
}

void setWWA(int WW, int CW, int A) {
  for (int i = 0; i < WWA_PIXELS; i++) {
    wwastrip.SetPixelColor(i, RgbColor(WW, CW, A));
  }
  delay(10);
  portDISABLE_INTERRUPTS();
  wwastrip.Show();
  portENABLE_INTERRUPTS();
}

int getIntFromHex(char* hexC, int index) {
  String hexvalue = hexC;
  hexvalue.toLowerCase();

  if (hexvalue.charAt(0) == '#') {
    hexvalue.remove(0, 1);
  }

  int substring_start = index * 2;
  int substring_end = substring_start + 2;
  int hexlength = hexvalue.length();

  if (hexlength % 2 != 0 || hexlength < substring_end ) {

    return 0;
  }


  String singular_hexvalue = hexvalue.substring(substring_start, substring_end);
  for (int i = 0; i < singular_hexvalue.length(); i++) {
    if (isxdigit(singular_hexvalue.charAt(i)) == 0) {
      return 0;
    }
  }

  int decvalue = (int) strtol(singular_hexvalue.c_str(), 0, 16);

  return decvalue;
}


void loop() {}

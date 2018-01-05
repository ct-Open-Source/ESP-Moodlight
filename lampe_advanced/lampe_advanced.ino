#include <Basecamp.hpp>
#include <NeoPixelBus.h>

#define RGBW_PIN            16
#define RGBW_PIXELS      270
#define WWA_PIN            17
#define WWA_PIXELS      315
NeoPixelBus<NeoGrbwFeature, NeoEsp32BitBangWs2813Method> rgbwstrip(RGBW_PIXELS, RGBW_PIN);
NeoPixelBus<NeoRgbFeature, NeoEsp32BitBangWs2813Method> wwastrip(WWA_PIXELS, WWA_PIN);
Basecamp iot;
String RGBWTopic;
String WWATopic;

void setup() {
  iot.begin();
  iot.mqtt.onMessage(onMqttMessage);
  iot.mqtt.onConnect(onMqttConnect);
  RGBWTopic = "stat/" + iot.hostname + "/rgbw";
  WWATopic = "stat/" + iot.hostname + "/wwa";
  rgbwstrip.Begin();
  rgbwstrip.Show();
  wwastrip.Begin();
  wwastrip.Show();

}

void onMqttConnect(bool sessionPresent) {
  iot.mqtt.subscribe(RGBWTopic.c_str(), 0);
  iot.mqtt.subscribe(WWATopic.c_str(), 0);
}

void onMqttMessage(char* topic, char* payload,
                   AsyncMqttClientMessageProperties properties,
                   size_t len, size_t index, size_t total) {
  if (topic == RGBWTopic.c_str()) {
    int R = getCsvIntAtIndex(payload, 0);
    int G = getCsvIntAtIndex(payload, 1);
    int B = getCsvIntAtIndex(payload, 2);
    int W = getCsvIntAtIndex(payload, 3);
    setRGBW(R, G, B, W);

  } else if (topic == WWATopic.c_str()) {

    int WW = getCsvIntAtIndex(payload, 0);
    int CW = getCsvIntAtIndex(payload, 1);
    int A = getCsvIntAtIndex(payload, 2);
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
    rgbwstrip.SetPixelColor(i, RgbwColor(WW, CW, A));
  }
  delay(10);
  portDISABLE_INTERRUPTS();
  wwastrip.Show();
  portENABLE_INTERRUPTS();
}

int getCsvIntAtIndex(String csv, int index) {
  int startChar = 0, valueNo = 0;
  for (int i = 0; i <= csv.length() - 1; i++) {
    if (csv[i] == ',')
      if (valueNo == index) {
        return csv.substring(startChar, i).toInt();
      } else {
        startChar = i + 1;
        valueNo++;
      }
  }
  if (startChar < csv.length() - 1)
    return csv.substring(startChar, csv.length()).toInt();
  return 0;
}

void loop() {}

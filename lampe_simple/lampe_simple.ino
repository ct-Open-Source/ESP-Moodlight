#include <Basecamp.hpp>
#include <NeoPixelBus.h>

#define RGBW_PIN            16
#define RGBW_PIXELS      270
NeoPixelBus<NeoGrbwFeature, NeoEsp32BitBangWs2813Method>
rgbwstrip(RGBW_PIXELS, RGBW_PIN);
Basecamp iot;

void setup() {
  iot.begin();
  iot.mqtt.onMessage(onMqttMessage);
  iot.mqtt.onConnect(onMqttConnect);

  rgbwstrip.Begin();
  rgbwstrip.Show();

}

void onMqttConnect(bool sessionPresent) {
  iot.mqtt.subscribe("cmnd/moodlight/setRGBW", 0);

}

void onMqttMessage(char* topic, char* payload,
                   AsyncMqttClientMessageProperties properties,
                   size_t len, size_t index, size_t total) {

  int R = getCsvIntAtIndex(payload, 0);
  int G = getCsvIntAtIndex(payload, 1);
  int B = getCsvIntAtIndex(payload, 2);
  int W = getCsvIntAtIndex(payload, 3);
  setRGBW(R, G, B, W);
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

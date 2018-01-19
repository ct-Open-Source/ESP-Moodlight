#include <Basecamp.hpp>
// Init the Basecamp instance
Basecamp iot;

#include <NeoPixelBus.h>

//Define the properties of both LED strips
#define RGBW_PIN        16
#define RGBW_PIXELS     270

#define WWA_PIN         17
#define WWA_PIXELS      315

// Create instances of NeoPixelBus objects. This uses the BitBang workaround for the ESP32
NeoPixelBus<NeoGrbwFeature, NeoEsp32BitBangWs2813Method> rgbwstrip(RGBW_PIXELS, RGBW_PIN);
NeoPixelBus<NeoRgbFeature, NeoEsp32BitBangWs2813Method> wwastrip(WWA_PIXELS, WWA_PIN);

// Create strings for the MQTT-topics
String RGBWTopic;
String WWATopic;

void setup() {
  // Start the Basecamp instance
  iot.begin();

  // Get the defined hostname and create the topic names from it.
  RGBWTopic = "cmnd/" + iot.hostname + "/rgbw";
  WWATopic = "cmnd/" + iot.hostname + "/wwa";

  // Define callbacks for MQTT events
  iot.mqtt.onMessage(onMqttMessage);
  iot.mqtt.onConnect(onMqttConnect);

  // Zero out both strips to get a clear (off) state
  rgbwstrip.Begin();
  rgbwstrip.Show();
  wwastrip.Begin();
  wwastrip.Show();

}

// If the MQTT-client gets a connection it subscribes to it's topics
void onMqttConnect(bool sessionPresent) {
  iot.mqtt.subscribe(RGBWTopic.c_str(), 2);
  iot.mqtt.subscribe(WWATopic.c_str(), 2);
}

// If an MQTT-message is received process it
void onMqttMessage(char* topic, char* payload,
                   AsyncMqttClientMessageProperties properties,
                   size_t len, size_t index, size_t total) {

  // If the message is for the RGBW strip convert the payload from a hex-string to decimal values
  if (strcmp(topic, RGBWTopic.c_str()) == 0) {
  
    int R = getIntFromHex(payload, 0);
    int G = getIntFromHex(payload, 1);
    int B = getIntFromHex(payload, 2);
    int W = getIntFromHex(payload, 3);
    // set the colors accordingly
    setRGBW(R, G, B, W);
    
  // If the message is for the WWA strip convert the payload from a hex-string to decimal values
  } else if (strcmp(topic, WWATopic.c_str()) == 0) {
  
    int WW = getIntFromHex(payload, 0);
    int CW = getIntFromHex(payload, 1);
    int A = getIntFromHex(payload, 2);
    // set the colors accordingly
    setWWA(WW, CW, A);
  }
}

// Sets the colors of the RGBW strip
void setRGBW(int R, int G, int B, int W) {
  // Walk over all LEDs and set the color
  for (int i = 0; i < RGBW_PIXELS; i++) {
    rgbwstrip.SetPixelColor(i, RgbwColor(R, G, B, W));
  }
  // The following delays and interrupt commands are workarounds for the missing ESP32 hardware bitbang support
  // Delay the execution of the command
  delay(10);
  // Disable interrupts
  portDISABLE_INTERRUPTS();
  // Transfer the color values to the strip and show them
  rgbwstrip.Show();
  // Enable interrupts
  portENABLE_INTERRUPTS();
}

// Sets the colors of the WWA strip
void setWWA(int WW, int CW, int A) {
  // Walk over all LEDs and set the color
  for (int i = 0; i < WWA_PIXELS; i++) {
    wwastrip.SetPixelColor(i, RgbColor(WW, CW, A));
  }
  // The following delays and interrupt commands are workarounds for the missing ESP32 hardware bitbang support
  // Delay the execution of the command
  delay(10);
  // Disable interrupts
  portDISABLE_INTERRUPTS();
  // Transfer the color values to the strip and show them
  wwastrip.Show();
  // Enable interrupts
  portENABLE_INTERRUPTS();
}

// Convert a given (html)hex color code to integer values. Takes a string for the hexcode and an int for the index 
// to get each element of the hex color code.
int getIntFromHex(char* hexC, int index) {
  // Hacky fix for MQTT char* issues
  String hexvalue = hexC;
  
  hexvalue.toLowerCase();

  // Remove leading # if present
  if (hexvalue.charAt(0) == '#') {
    hexvalue.remove(0, 1);
  }

  // calculate the start position of the two bytes that correspond to the given index
  int substring_start = index * 2;
  // The length is always two bytes
  int substring_end = substring_start + 2;
  // Get the length of the given hex code
  int hexlength = hexvalue.length();

  // If the hex code is not divisble by two or is shorter than the end of the requested substring
  // return 0 because it's to short.
  if (hexlength % 2 != 0 || hexlength < substring_end ) {
    return 0;
  }

  // Get the two selected bytes for the color
  String singular_hexvalue = hexvalue.substring(substring_start, substring_end);

  // Check if both values are hex digits. If one is not return 0
  for (int i = 0; i < singular_hexvalue.length(); i++) {
    if (isxdigit(singular_hexvalue.charAt(i)) == 0) {
      return 0;
    }
  }
  // Convert the hex value to an integer
  int decvalue = (int) strtol(singular_hexvalue.c_str(), 0, 16);
  // return the integer
  return decvalue;  
}


void loop() {}

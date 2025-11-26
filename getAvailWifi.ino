#include "WiFi.h"
#include <string>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

typedef void (*ButtonPressHandler) (void);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
std::string currentOutput = "Started...";

void setup() {
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
  }

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Ensure clean scan
  delay(100);

  
}

void loop() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(currentOutput.c_str());
  
  display.display();
}

void scanWifi(){
  Serial.println("Starting WiFi scan...");
  int n = WiFi.scanNetworks();
  Serial.println("Scan complete.");
  if (n == 0) {
    Serial.println("No networks found.");
  } else {
    Serial.printf("%d networks found:\n", n);
    for (int i = 0; i < n; ++i) {
      Serial.printf("SSID: %s\n", WiFi.SSID(i).c_str());
      Serial.printf("  RSSI: %d dBm\n", WiFi.RSSI(i));
      Serial.printf("  Channel: %d\n", WiFi.channel(i));
      Serial.printf("  Encryption: %s\n\n", getEncryptionType(WiFi.encryptionType(i)));
    }
  }
}

const char* getEncryptionType(wifi_auth_mode_t type) {
  switch (type) {
    case WIFI_AUTH_OPEN: return "Open";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA";
    case WIFI_AUTH_WPA2_PSK: return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-Enterprise";
    case WIFI_AUTH_WPA3_PSK: return "WPA3";
    default: return "Unknown";
  }
}

void checkButton(const int BUTTON,  bool &last, bool &current, ButtonPressHandler handler ){
  
  current = debounce(last, BUTTON);              // Read debounced state
  
  if (last == LOW && current == HIGH)    // If it was pressed…
  {
    // This runs the code that we want to happen 
    // when the user presses the button.  The function ptr 
    // allows us to do different types of work
     handler();
  }
  last = current;                        // Reset button value
}

boolean debounce(boolean last, int button)
{
 boolean current = digitalRead(button);    // Read the button state
 if (last != current)                      // If it's different…
 {
  delay(5);                                // Wait 5ms -- thought delay mattered but it didn't (see INPUT_PULLDOWN note)
  current = digitalRead(button);           // Read it again
 }
 return current;                           // Return the current value
}

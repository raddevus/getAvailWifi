#include "WiFi.h"
#include <string>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

typedef void (*ButtonPressHandler) (void);
const int MAIN_BTN = 5;
bool changeMainBtnCurrent = false;
bool mainBtnPrev = LOW;
bool mainBtnCurrent = LOW;
const int DATA_LED = 7;
bool flashIsOn = false;
int wifiIdx = 0;
int availWifiCount = 0;

struct wifiInfo{
  std::string ssid;
  int32_t rssi;
  int32_t channel;
  const char* encType;
};

wifiInfo* allWifi;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
std::string currentOutput = "Started...";
std::string encType;
std::string channel;
std::string rssi;

void setup() {
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
  }
  pinMode(MAIN_BTN, INPUT_PULLDOWN);
  pinMode(DATA_LED, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Ensure clean scan
  scanWifi();
  delay(100);

  
}

void loop() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(currentOutput.c_str());
  display.println(rssi.c_str());
  display.println(channel.c_str());
  display.println(encType.c_str());
  
  display.display();
  checkButton(MAIN_BTN,mainBtnPrev,mainBtnCurrent,checkChangeMainButton);
}

void scanWifi(){
  Serial.println("Starting WiFi scan...");
  availWifiCount = WiFi.scanNetworks();
  allWifi = new wifiInfo[availWifiCount];
  Serial.println("Scan complete.");
  if (availWifiCount == 0) {
    Serial.println("No networks found.");
  } else {
    Serial.printf("%d networks found:\n", availWifiCount);
    for (int i = 0; i < availWifiCount; ++i) {
      Serial.printf("SSID: %s\n", WiFi.SSID(i).c_str());
      allWifi[i].ssid = WiFi.SSID(i).c_str();
      Serial.printf("  RSSI: %d dBm\n", WiFi.RSSI(i));
      allWifi[i].rssi = WiFi.RSSI(i);
      Serial.printf("  Channel: %d\n", WiFi.channel(i));
      allWifi[i].channel = WiFi.channel(i);
      Serial.printf("  Encryption: %s\n\n", getEncryptionType(WiFi.encryptionType(i)));
      allWifi[i].encType = getEncryptionType(WiFi.encryptionType(i));
      Serial.printf("#####  Encryption: %s\n\n", allWifi[i].encType);
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
int wifiInfoCount = 0;
void checkChangeMainButton(void){
  if (mainBtnPrev == LOW && mainBtnCurrent == HIGH){
    flashIsOn = !flashIsOn;
  }
  currentOutput = allWifi[wifiIdx].ssid;
  char buffer[50];
  sprintf(buffer, "RSSI: %d dBm\n", allWifi[wifiIdx].rssi);
  rssi = std::string(buffer);
  sprintf(buffer, "Channel: %d\n", allWifi[wifiIdx].channel);
  channel = std::string(buffer);
  sprintf(buffer, "Encrypt: %s\n\n", allWifi[wifiIdx].encType);
  encType = std::string(buffer);

  wifiIdx++;
  
  if (wifiIdx > availWifiCount-1){wifiIdx = 0;}
  mainBtnPrev = mainBtnCurrent;
  if (flashIsOn){
    analogWrite(DATA_LED, 255); 
    Serial.printf("availWifiCount: %d\n", availWifiCount);
  }
  else{
    analogWrite(DATA_LED, 0);
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

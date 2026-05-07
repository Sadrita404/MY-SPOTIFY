#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <OpenWeatherMapCurrent.h>
#include <spotify.h>

#define TFT_CS    1
#define TFT_RST   2 
#define TFT_DC    3
#define BTN_PP    7 
#define BTN_FWD   6 

const char* SSID = "your_wifi_ssid";
const char* PASSWORD = "your_wifi_password";
const char* CLIENT_ID = "your_spotify_id";
const char* CLIENT_SECRET = "your_spotify_secret";
const char* OW_KEY = "your_openweathermap_api_key";
const char* OW_LOC = "your_city_id";

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
spotify sp(CLIENT_ID, CLIENT_SECRET);
OpenWeatherMapCurrentData weatherData;
OpenWeatherMapCurrent weatherClient;

bool showWeather = false;
unsigned long lastWeatherUpdate = 0;
String lastTrack = "";
String lastArtist = "";

void updateWeather() {
  weatherClient.updateCurrentById(&weatherData, OW_KEY, OW_LOC);
  lastWeatherUpdate = millis();
}

void displayWeather() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.print("Weather");
  
  tft.setCursor(10, 40);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.print(String(weatherData.temp, 1));
  tft.print("C");

  tft.setCursor(10, 80);
  tft.setTextSize(1);
  tft.print(weatherData.description);
}

void displaySpotify() {
  String currentArtist = sp.current_artist_names();
  String currentTrack = sp.current_track_name();

  if (lastTrack != currentTrack && currentTrack != "null") {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 10);
    tft.setTextColor(ST77XX_GREEN);
    tft.setTextSize(1);
    tft.print("Spotify:");
    tft.setCursor(10, 35);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.print(currentTrack);
    tft.setCursor(10, 70);
    tft.setTextColor(ST77XX_CYAN);
    tft.setTextSize(1);
    tft.print(currentArtist);
    lastTrack = currentTrack;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BTN_PP, INPUT_PULLUP);
  pinMode(BTN_FWD, INPUT_PULLUP);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  
  sp.begin();
  while(!sp.is_auth()) { sp.handle_client(); }
  
  updateWeather();
}

void loop() {
  if (digitalRead(BTN_PP) == LOW && digitalRead(BTN_FWD) == LOW) {
    showWeather = !showWeather;
    tft.fillScreen(ST77XX_BLACK);
    lastTrack = ""; 
    delay(500);
  }

  if (showWeather) {
    if (millis() - lastWeatherUpdate > 600000) {
      updateWeather();
    }
    displayWeather();
  } else {
    displaySpotify();
    
    if (digitalRead(BTN_FWD) == LOW) {
      // Add sp.next() here if library supports
      delay(300);
    }
    if (digitalRead(BTN_PP) == LOW) {
      // Add sp.pause() here if library supports
      delay(300);
    }
  }
  
  delay(100);
}

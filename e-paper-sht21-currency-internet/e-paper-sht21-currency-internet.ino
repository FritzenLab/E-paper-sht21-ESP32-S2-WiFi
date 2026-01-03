// Youtube code adapted from here: https://github.com/BloxyLabs/ESP32_YouTubeCounter/blob/main/ESP32_YT_Counter_V2/ESP32_YT_Counter_V2.ino
// Edit 01/03/2026: got help from Google Gemini to tune the code
/*
Franzininho WiFi (ESP32-S2), WeAct 2.9" e-paper display
D/C 0
CS 1
Busy 2
Res 3
SDA 35
SCL 36
*/

#include "Adafruit_EPD.h"
#include <Adafruit_GFX.h> // Core graphics library

#include <Wire.h>
#include <SHT21.h>  // include SHT21 library

SHT21 sht; 

// Inclusão da(s) biblioteca(s)
#include <WiFi.h>       // Biblioteca nativa do ESP32
#include <WiFiClientSecure.h>
#include <HTTPClient.h> // Biblioteca nativa do ESP32
#include <ArduinoJson.h>

#include <YoutubeApi.h>

//double subs = 0;
long subs;

// Configurações da rede WiFi à se conectar
const char* ssid = "";
const char* password = "";
const char/ freeCurrencyAPIHere = "";
String payload;
String payload2;

HTTPClient http; // o objeto da classe HTTPClient
HTTPClient http2; // o objeto da classe HTTPClient

#ifdef ARDUINO_ADAFRUIT_FEATHER_RP2040_THINKINK // detects if compiling for
                                                // Feather RP2040 ThinkInk
#define EPD_DC PIN_EPD_DC       // ThinkInk 24-pin connector DC
#define EPD_CS PIN_EPD_CS       // ThinkInk 24-pin connector CS
#define EPD_BUSY PIN_EPD_BUSY   // ThinkInk 24-pin connector Busy
#define SRAM_CS -1              // use onboard RAM
#define EPD_RESET PIN_EPD_RESET // ThinkInk 24-pin connector Reset
#define EPD_SPI &SPI1           // secondary SPI for ThinkInk
#else
#define EPD_DC 0
#define EPD_CS 1
#define EPD_BUSY 2 // can set to -1 to not use a pin (will wait a fixed delay)
#define SRAM_CS -1
#define EPD_RESET 3  // can set to -1 and share with microcontroller Reset!
#define EPD_SPI &SPI // primary SPI
#endif

const char* apiKey   = "";
const char* channelIdNet = ""; // channel ID for the first channel
long subscribers= 0;
long repeatTime= 0;
bool startup= true;

/* Uncomment the following line if you are using 1.54" tricolor EPD */
// Adafruit_IL0373 display(152, 152, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

/* Uncomment the following line if you are using 1.54" monochrome EPD */
// Adafruit_SSD1608 display(200, 200, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

/* Uncomment the following line if you are using 2.13" tricolor EPD */
//Adafruit_IL0373 display(212, 104, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY,
//                        EPD_SPI);
//#define FLEXIBLE_213

/* Uncomment the following line if you are using 2.13" monochrome 250*122 EPD */
// Adafruit_SSD1675 display(250, 122, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

/* Uncomment the following line if you are using 2.7" tricolor or grayscale EPD
 */
// Adafruit_IL91874 display(264, 176, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

/* Uncomment the following line if you are using 2.9" EPD */
// Adafruit_IL0373 display(296, 128, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI); #define FLEXIBLE_290

/* Uncomment the following line if you are using 4.2" tricolor EPD */
// Adafruit_IL0398 display(300, 400, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

// Uncomment the following line if you are using 2.9" EPD with SSD1680
// THIS IS MY DISPLAY MODEL, YOURS MAY VARY
Adafruit_SSD1680 display(296, 128, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
EPD_BUSY, EPD_SPI);

float temp; 	// variable to store temperature
float humidity; // variable to store hemidity

void setup() {
  // put your setup code here, to run once:
  display.begin();
  display.clearBuffer();
  Wire.begin();		// begin Wire(I2C)
  Serial.begin(115200);

  WiFi.disconnect(); // Desconecta do WiFI se já houver alguma conexão
  WiFi.mode(WIFI_STA); // Configura o ESP32 para o modo de conexão WiFi Estação
  Serial.println("[SETUP] Tentando conexão com o WiFi...");
  WiFi.begin(ssid, password); // Conecta-se à rede
  if (WiFi.waitForConnectResult() == WL_CONNECTED) // aguarda até que o módulo se
    //                                                  conecte ao ponto de acesso
  {
    Serial.println("[SETUP] WiFi iniciado com sucesso!");
  } else
  {
    Serial.println("[SETUP] Houve falha na inicialização do WiFi. Reiniciando ESP.");
    ESP.restart();
  }
   
}

void loop() {
  double conversion = 0;
  double conversion2 = 0;

  if((millis() - repeatTime > 360000) || startup == true){ // six minutes per loop entrance
    repeatTime= millis();
    startup= false;
    // --- START CURRENCY BLOCK ---
    // The curly braces {} ensure all memory used here is freed before YouTube starts
    {
      HTTPClient http;
      Serial.println("[HTTP] Iniciando requisição de moedas...");
      
      // Request 1: BRL
      http.begin("https://api.freecurrencyapi.com/v1/latest?apikey=freeCurrencyAPIHere");
      int httpCode = http.GET();

      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
          conversion = doc["data"]["BRL"].as<double>();
          Serial.print("BRL: "); Serial.println(conversion, 4);
        }
      }
      http.end();

    } 
    // --- END CURRENCY BLOCK (Memory is now clean) ---

    // Get Sensors
    temp = sht.getTemperature();
    humidity = sht.getHumidity();
    Serial.print("Sensor: "); Serial.print(temp); Serial.print("C | "); Serial.println(humidity);

    // Get YouTube (This now has much more RAM available)
    getSubscribersNet();

    Serial.print("IP Local: "); Serial.println(WiFi.localIP());

    // --- UPDATE DISPLAY ---
    display.clearBuffer();
    display.setCursor(5, 5);
    display.setTextColor(EPD_BLACK);
    display.setTextSize(2);
    display.println("Thermometer/Currency/Youtube");
    
    display.setCursor(5, 40);
    display.setTextSize(2); // Small text for data to ensure it fits
    display.print("Temperature:    "); display.print(temp); display.println(" C"); 
    display.print("Humidity:       "); display.print(humidity); display.println(" %");
    display.print("BRL to USD:     "); display.println(conversion, 2);
    display.print("YT Subscribers: "); display.println(subscribers);
    
    //Serial.println("Updating E-Paper...");
    display.display();

    // Wait 1 minute before next cycle
  }
}
// This function is for a single youtube channel
void getSubscribersNet() {
  Serial.println("--- Starting YouTube Update ---");
  WiFiClientSecure *client = new WiFiClientSecure;
  
  if (client) {
    client->setInsecure(); 
    HTTPClient http;
    String url = "https://www.googleapis.com/youtube/v3/channels?part=statistics&id=" 
                 + String(channelIdNet) + "&key=" + String(apiKey);

    if (http.begin(*client, url)) {
      int httpCode = http.GET();
      Serial.print("YouTube HTTP Code: ");
      Serial.println(httpCode);

      if (httpCode == 200) {
        String payloadYT = http.getString();
        DynamicJsonDocument doc(2048); 
        DeserializationError error = deserializeJson(doc, payloadYT);

        if (!error) {
          if (doc["items"].size() > 0) {
            const char* countStr = doc["items"][0]["statistics"]["subscriberCount"];
            subscribers = atol(countStr); 
            Serial.print("YouTube Success! Subscribers: ");
            Serial.println(subscribers);
          } else {
            Serial.println("YouTube Error: items array is empty (Check Channel ID)");
          }
        } else {
          Serial.print("YouTube JSON Error: ");
          Serial.println(error.c_str());
        }
      } else {
        Serial.print("YouTube GET Failed, error: ");
        Serial.println(http.errorToString(httpCode).c_str());
      }
      http.end(); // Move this here so it always closes correctly
    } else {
      Serial.println("YouTube Connection Failed at .begin()");
    }
    delete client; 
  }
  Serial.println("--- End YouTube Update ---");
}
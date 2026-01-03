# ESP32-S2 E-Paper Dashboard (Temp/Humidity/Currency/YouTube)

This project turns a **Franzininho WiFi (ESP32-S2)** and a **WeAct 2.9" E-Paper Display** into a low-power information dashboard. It fetches live currency exchange rates and YouTube subscriber counts over WiFi while monitoring local environment conditions via an SHT21 sensor.

## 🚀 Features

* **Environmental Monitoring**: Real-time Temperature and Humidity via SHT21 (I2C).
* **Currency Tracking**: Live USD to BRL conversion using the FreeCurrencyAPI.
* **Social Stats**: Live YouTube subscriber count using the Google Data API v3.
* **Memory Optimized**: Uses scope-bracing and dynamic JSON parsing to handle multiple HTTPS requests on the ESP32-S2's memory constraints.
* **E-Paper Display**: Ultra-low power 2.9" SSD1680 monochrome display.

---

## 🛠️ Hardware Requirements

| Component | Pin (ESP32-S2) |
| --- | --- |
| **WeAct 2.9" E-Paper** | DC: 0, CS: 1, BUSY: 2, RES: 3 |
| **SHT21 Sensor** | SDA: 35, SCL: 36 |
| **Microcontroller** | Franzininho WiFi (ESP32-S2) |

---

## 💻 Software Dependencies

Ensure you have the following libraries installed in your Arduino IDE:

* `Adafruit_EPD` (For the e-paper display)
* `Adafruit_GFX` (Graphics core)
* `ArduinoJson` (v6 or higher)
* `SHT21` (By enjoyneering)
* `WiFiClientSecure` & `HTTPClient` (Native ESP32)

---

## Settings & API Keys

Before uploading, fill in your credentials in the `.ino` file:

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* freeCurrencyAPIHere = "YOUR_FREE_CURRENCY_API_KEY";
const char* apiKey = "YOUR_GOOGLE_YOUTUBE_API_KEY";
const char* channelIdNet = "YOUR_YOUTUBE_CHANNEL_ID";

```

### 🧠 Memory Management Note

The ESP32-S2 has a limited heap. This code uses a "Brace Block" strategy inside the `loop()` to ensure that large JSON strings and document objects are destroyed immediately after use, freeing up RAM for the `WiFiClientSecure` SSL handshake required by Google/YouTube.

---

## 🔄 Update Logic

* **Initial Boot**: Runs a full update immediately.
* **Interval**: Refreshes every **6 minutes** to respect API limits and preserve the e-paper lifespan.

---

## 🤝 Acknowledgments

* YouTube logic adapted from [BloxyLabs](https://www.google.com/search?q=https://github.com/BloxyLabs/ESP32_YouTubeCounter).
* Optimized for ESP32-S2 and tuned with assistance from **Google Gemini**.

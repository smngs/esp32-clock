#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <SPI.h>
#include <Wire.h>
// #include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// #include <Adafruit_BME280.h>
#include <Adafruit_BME680.h>

#include <secret.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Adafruit_BME680 bme;

struct tm timeInfo;
WiFiClient wifi;

int line=0;

void indicate(const char* msg) {
    if (line >= 7) {
        display.clearDisplay();
        display.setCursor(0, 0);
        line = 0;
    }
    Serial.println(msg);
    display.println(msg);
    display.display();
    line++;
}

void WiFisetup() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, WIFI_PASSWORD);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        indicate("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
}

void OTAsetup() {
    ArduinoOTA.setHostname("esp32-01");
    ArduinoOTA
        .onStart([]() {
                String type;
                if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
                else // U_SPIFFS
                type = "filesystem";

                // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                Serial.println("Start updating " + type);
                })
    .onEnd([]() {
            Serial.println("\nEnd");
            })
    .onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
            })
    .onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR) Serial.println("End Failed");
            });

    ArduinoOTA.begin();
}

void setup() {
    Serial.begin(115200);

    display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
    display.clearDisplay();
    display.setTextSize(0.5);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);

    indicate("Booting ESP32...");
    indicate("Connecting Network...");
    
    WiFisetup();
    OTAsetup();

    indicate("Network Connected!");

    pinMode(25, INPUT_PULLUP);
    pinMode(26, INPUT_PULLUP);
    pinMode(27, INPUT_PULLUP);

    configTime(9 * 3600L, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");

 
    bme.begin();
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    // bme.setGasHeater(320, 150); // 320*C for 150 ms
    bme.setGasHeater(0, 0); // disabled

}


void loop() {
    char date[15], time[15];
    char temp[20], hum[20], press[20], gass[20];

    ArduinoOTA.handle();

    bme.beginReading();
    sprintf(temp, "Temp: %.2f degC", bme.temperature);
    sprintf(hum, "Hum: %.2f %%rH", bme.humidity);
    sprintf(press, "Press: %.2f hPa", bme.pressure / 100.0);
    // sprintf(gass, "Gass: %.2f ohms", bme.gas_resistance / 1000.0);
    bme.endReading();

    getLocalTime(&timeInfo);
    sprintf(date, "%04d/%02d/%02d",
          timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday);
    sprintf(time, "%02d:%02d:%02d",
          timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);

    display.clearDisplay();
    display.setTextSize(2.0);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(date);
    display.setCursor(30, 16);
    display.println(time);
    
    display.setTextSize(1,0);
    display.setTextColor(WHITE);
    display.setCursor(0, 32);

    display.println(temp);
    display.println(hum);
    display.println(press);
    // display.println(gass);
    display.display();

    Serial.println(temp);
    Serial.println(hum);
    Serial.println(press);
    // Serial.println(gass);
}

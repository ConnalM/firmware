#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32httpUpdate.h>

#define FIRMWARE_VERSION 0.6
#define UPDATE_URL "https://connalm.github.io/firmware/firmware.json"

String wifiSSID = "Tootletshouse";
String wifiPassword = "H@zelm01";

void connectWifi()
{
  Serial.println("Connecting to WIFI");
  WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);

    Serial.println("Wifi connected");
    // Serial.println(WiFi.SSID());
    // Serial.println(WiFi.RSSI());
    // Serial.println(WiFi.macAddress());
    // Serial.println(WiFi.localIP());
    // Serial.println(WiFi.dnsIP());
  }
}


void setup() {
  Serial.begin(115200);
  connectWifi();

}


void loop() {
  checkUpdate();
  delay(5000);

}


void checkUpdate()
{
    Serial.println();
    Serial.println("Checking for online update");
    HTTPClient http;
    String response;
    String url = UPDATE_URL;
    http.begin(url);
    http.GET();
    response = http.getString();
    Serial.println(response);
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, response);
    JsonObject obj = doc.as<JsonObject>();
    String version = obj[String("version")];
    String url_update = obj[String("url")];
    Serial.print("current version : ");
    Serial.println(FIRMWARE_VERSION);
    Serial.print("online version : ");
    Serial.println(version);
    Serial.print("url of online version : ");
    Serial.println(url_update);
    if (version.toDouble() != FIRMWARE_VERSION)
    {
        Serial.println("Update Available");
        Serial.println(url_update);
        if (updateOverHttp(url_update) == HTTP_UPDATE_OK)
        {
            Serial.println("Update Success");
        }
        else
        {
            Serial.println("Update Failed");
        }
        Serial.println("Update Success");
    }
    else
    {
        Serial.println("No Update Available");
    }
}

t_httpUpdate_return updateOverHttp(String url_update)
{
    t_httpUpdate_return ret;

    if ((WiFi.status() == WL_CONNECTED))
    {

        ret = ESPhttpUpdate.update(url_update);

        switch (ret)
        {
        case HTTP_UPDATE_FAILED:
            Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            return ret;
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("HTTP_UPDATE_NO_UPDATES");
            return ret;
            break;

        case HTTP_UPDATE_OK:
            Serial.println("HTTP_UPDATE_OK");
            return ret;
            break;
        }
    }
}
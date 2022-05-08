/* -*- coding: utf-8 -*-
* vim: tabstop=2 shiftwidth=2 softtabstop=2
*
* ESP32_PWM_TRAIN - A basic PWM controller for DC trains
* Copyright (C) 2022 Daniele Viganò <daniele@vigano.me>
*
* ESP32_PWM_TRAIN is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ESP32_PWM_TRAIN is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* 
* Inspired by https://randomnerdtutorials.com/esp32-web-server-slider-pwm/
*/

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "settings.h"
#include "html.h"

int ENA_pin = 25;
int IN1 = 19;
int IN2 = 13;
const int pwm_channel = 0;

String pwm_value = "0";
String dir_value = "0";

unsigned int frequency = FREQUENCY;
unsigned int resolution = RESOLUTION;

const char* pwm_parameter = "pwm";
const char* dir_parameter = "direction";
const char* frequency_parameter = "frequency";

AsyncWebServer server(80);

// HTML is from html.h

String processor(const String& var){
  if (var == "PWMVALUE"){
    return pwm_value;
  }
  if (var == "DIRVALUE"){
    return dir_value;
  }
  if (var == "FREQUENCY"){
    return String(frequency);
  }
  return String();
}

void setup(){
  Serial.begin(115200);
  delay(1000);
  pinMode(ENA_pin, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  Serial.print("Starting PWM with ");
  Serial.print(resolution);
  Serial.print(" bit resolution and ");
  Serial.print(frequency);
  Serial.println(" hz frequency...");
  ledcSetup(pwm_channel, frequency, resolution);
  ledcAttachPin(ENA_pin, pwm_channel);
  ledcWrite(pwm_channel, pwm_value.toInt());
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  const char* _ssid = SECRET_SSID;
  const char* _password = SECRET_PASSWORD;

  Serial.print("Starting connecting WiFi to ");
  Serial.print(_ssid);
  delay(10);
  WiFi.begin(_ssid, _password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nWiFi connected. IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/settings", HTTP_GET, [] (AsyncWebServerRequest *request) {
    if (request->hasParam(frequency_parameter)) {
      frequency = request->getParam(frequency_parameter)->value().toInt();
      ledcSetup(pwm_channel, frequency, resolution);
      request->send(200, "text/plain", "OK");
    }
    request->send(400, "text/plan", "Bad request");
  });

  server.on("/command", HTTP_GET, [] (AsyncWebServerRequest *request) {
    if (request->hasParam(pwm_parameter) && request->hasParam(dir_parameter)) {
      pwm_value = request->getParam(pwm_parameter)->value();
      dir_value = request->getParam(dir_parameter)->value();

      if (dir_value == "0") {
        ledcWrite(pwm_channel, 0);
      } else {
        if (dir_value == "-1") {
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, LOW);
        }
        if (dir_value == "1") {
          digitalWrite(IN1, HIGH);
          digitalWrite(IN2, LOW);
        }
        ledcWrite(pwm_channel, pwm_value.toInt());
      }
      Serial.println("Direction: "+dir_value);
      Serial.println("Speed: "+pwm_value);
      request->send(200, "text/plain", "OK");
    }
    request->send(400, "text/plan", "Bad request");
  });

  server.begin();
}

void loop() {

}

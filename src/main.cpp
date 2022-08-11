/* -------------------------------------------------------------------
 * AdminESP - ElectronicIOT 2022
 * Sitio WEB: https://electroniciot.com
 * Correo: admin@electroniciot.com
 * Cel_WSP: +591 71243395
 * Plataforma: ESP32
 * Framework:  Arduino
 * Proyecto: Panel Administrativo para el ESP32 con Vue.js
 * Nombre: VUE32 Admin Tool
 * Autor: Ing. Yamir Hidalgo Peña
 * -------------------------------------------------------------------
 */

// -------------------------------------------------------------------
// Librerías
// -------------------------------------------------------------------
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <EEPROM.h>
#include <TimeLib.h>

#include "DHT.h"
// -------------------------------------------------------------------
// Archivos *.hpp - Fragmentar el Código
// -------------------------------------------------------------------
#include "header.hpp"
#include "functions.hpp"
#include "settings.hpp"
#include "wifi.hpp"
#include "mqtt.hpp"
#include "server.hpp"
#include "websockets.hpp"
#include "device.hpp"


#define DHTPIN 21    
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
long lastUpdate = 0;   

  float h ;
  float t ;
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f ;


// -------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  setCpuFrequencyMhz(240);
  // Memoria EEPROM init
  EEPROM.begin(256);
  // Leer el valor de la memoria
  EEPROM.get(Restart_Address, device_restart);
  device_restart++;
  // Guardar el valor a la memoria
  EEPROM.put(Restart_Address, device_restart);
  EEPROM.commit();
  EEPROM.end();
  log("\n[ INFO ] Iniciando Setup");
  log("[ INFO ] Reinicios " + String(device_restart));
  log("[ INFO ] Setup corriendo en el Core " + String(xPortGetCoreID()));
  // Iniciar el SPIFFS
  if (!SPIFFS.begin(true))
  {
    log("[ ERROR ] Falló la inicialización del SPIFFS");
    while (true)
      ;
  }
  // Leer el Archivo settings.json
  if (!settingsRead())
  {
    settingsSave();
  }
  // Inicializo dispositivo
  InitDevice();
       dht.begin(); 
  // Fin del Setup
  log("[ INFO ] Device Iniciado");
  // Setup WIFI
  wifi_setup();
  // Inicializacion del Servidor WEB
  InitServer();
  // Inicializamos el WS
  InitWebSockets();
  // Fin del Setup
  log("[ INFO ] Setup completado");
}
// -------------------------------------------------------------------
// Loop Principal
// -------------------------------------------------------------------
void loop()
{
  // -----------------------------------------------------------------
  // WIFI
  // -----------------------------------------------------------------
  if (wifi_mode == WIFI_STA)
  {
    wifiLoop();
  }
  else if (wifi_mode == WIFI_AP)
  {
    wifiAPLoop();
  }
  // -----------------------------------------------------------------
  // MQTT
  // -----------------------------------------------------------------
  if ((WiFi.status() == WL_CONNECTED) && (wifi_mode == WIFI_STA))
  {
    if (mqtt_server != 0)
    {
      // Función para el Loop principla de MQTT
      mqttLoop();
      if (mqttClient.connected() && mqtt_time_send)
      {
        // Funcion para enviar JSON por MQTT
        if (millis() - lastMsg > mqtt_time_interval)
        {
          lastMsg = millis();
          mqtt_publish();
        }
      }
    }
  }
  // -----------------------------------------------------------------
  // DEVICE
  // -----------------------------------------------------------------
if(millis()- lastUpdate > 5000){
  lastUpdate = millis();

  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  log("[ DATA ] Humidity: " + String(h) + " % Temperature: "+ String(t) +"°C ");

}
  // -------------------------------------------------------------------
  // Enviar JSON por WS cada 1s
  // -------------------------------------------------------------------
  if (millis() - lastWsSend > 1000)
  {
    lastWsSend = millis();
    WsMessage(getJsonIndex(), "", "");
    WsMessage(getJsonDashboard(), "", "");
  }
}
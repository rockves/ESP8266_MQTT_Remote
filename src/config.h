#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define DEBUG true

#define IR_LED_GPIO 4

#define MQTT_CLIENT_NAME "ESP"
#define MQTT_CONFIG_TOPIC "homeassistant/sensor/espTV/config"
#define MQTT_CONFIG_MESSAGE "{ \"name\": \"TV\", \"availability_topic\": \"homeassistant/sensor/espTV/status\", \"payload_available\": \"on\", \"payload_not_available\": \"off\" }"
#define MQTT_STATUS_TOPIC "homeassistant/sensor/espTV/status"
#define MQTT_STATUS_MESSAGE_ON "on"
#define MQTT_STATUS_MESSAGE_OFF "off"
#define MQTT_WILL_TOPIC MQTT_STATUS_TOPIC
#define MQTT_WILL_MESSAGE MQTT_STATUS_MESSAGE_OFF
#define MQTT_SERVER_TRY_CONNECT_DELAY 2000

#define AP_IP_OCTET_1 192     //Pierwszy oktet adresu IP
#define AP_IP_OCTET_2 168     //Drugi oktet adresu IP
#define AP_IP_OCTET_3 0       //Trzeci oktet adresu IP
#define AP_IP_OCTET_4 1       //Czwarty oktet adresu IP
#define AP_GATEWAY_OCTET_1 0  //Pierwszy oktet adresu bramy
#define AP_GATEWAY_OCTET_2 0  //Drugi oktet adresu bramy
#define AP_GATEWAY_OCTET_3 0  //Trzeci oktet adresu bramy
#define AP_GATEWAY_OCTET_4 0  //Czwarty oktet adresu bramy
#define AP_SUBNET_OCTET_1 255 //Pierwszy oktet adresu podsieci
#define AP_SUBNET_OCTET_2 255 //Drugi oktet adresu podsieci
#define AP_SUBNET_OCTET_3 255 //Trzeci oktet adresu podsieci
#define AP_SUBNET_OCTET_4 0   //Czwarty oktet adresu podsieci
#define AP_ENABLE_GPIO 4      //GPIO z przyciskiem do włączenia AP
#define AP_PORTAL_TIMEOUT 600 //Czas trwania AP (w sekundach)
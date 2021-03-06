#include "config.h"
#include "secrets.h"

WiFiClient espClient;
PubSubClient client(espClient);
IRsend irsend(IR_LED_GPIO);

void wifi_connect(){
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(AP_PORTAL_TIMEOUT); //Czas trwania AP
  wifiManager.setAPStaticIPConfig(IPAddress(AP_IP_OCTET_1,AP_IP_OCTET_2,AP_IP_OCTET_3,AP_IP_OCTET_4), IPAddress(AP_GATEWAY_OCTET_1,AP_GATEWAY_OCTET_2,AP_GATEWAY_OCTET_3,AP_GATEWAY_OCTET_4), IPAddress(AP_SUBNET_OCTET_1,AP_SUBNET_OCTET_2,AP_SUBNET_OCTET_3,AP_SUBNET_OCTET_4));
  wifiManager.autoConnect();
}

void mqtt_connect(){
  while (!client.connected()) {
    #if DEBUG
    Serial.println("Connecting to MQTT...");
    #endif
    //Łączenie z serwerem MQTT
    if (client.connect(MQTT_CLIENT_NAME, MQTT_USERNAME, MQTT_PASSWORD, MQTT_WILL_TOPIC, 0, true, MQTT_WILL_MESSAGE)) {
      client.publish(MQTT_CONFIG_TOPIC, MQTT_CONFIG_MESSAGE); //Publikacja wiadomości konfiguracyjnej (Discover)
      client.publish(MQTT_STATUS_TOPIC, MQTT_STATUS_MESSAGE_ON); //Publikacja stanu włączonego
      #if DEBUG
      Serial.println("connected"); 
      #endif
    } else {
      #if DEBUG
      Serial.print("failed with state ");
      Serial.print(client.state());
      #endif
    }
  }
  client.subscribe("homeassistant/sensor/esp/pioneerTV");
  client.subscribe("homeassistant/sensor/esp/LgTV");
}

void callback(char* topic, byte* payload, unsigned int length) {
//Odbiór wiadmości w tej funkcji
  String message = "";
  for(unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  #if DEBUG
    Serial.println(String(topic));
    Serial.println(message);
  #endif
  //irsend.sendNEC(0x20DF10EF, 32, 1);
  if(String(topic) == "homeassistant/sensor/esp/pioneerTV"){
    irsend.sendPioneer(strtoull(message.c_str(), NULL, 16));
  }else if(String(topic) == "homeassistant/sensor/esp/LgTV"){
    irsend.sendNEC(strtoul(message.c_str(), NULL, 10));
  }
}

void setup() {
  #if DEBUG
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  #endif
  pinMode(AP_ENABLE_GPIO, INPUT);
  irsend.begin();
  wifi_connect(); //Łączenie z wifi
  #if DEBUG
  Serial.println(WiFi.localIP());
  #endif
  client.setServer(IPAddress(MQTT_IP_OCTET_1, MQTT_IP_OCTET_2, MQTT_IP_OCTET_3, MQTT_IP_OCTET_4), MQTT_PORT); //Ustawienie danych serwera MQTT
  client.setCallback(callback); //Ustawienie funkcji dla odbierania wiadomości
  while(client.connected() != true){
    mqtt_connect();
    delay(MQTT_SERVER_TRY_CONNECT_DELAY);
  }
}

void loop() {
  //Sprawdzenie czy włączyć AP
  if(digitalRead(AP_ENABLE_GPIO) == LOW){
    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(300);
    wifiManager.setAPStaticIPConfig(IPAddress(AP_IP_OCTET_1,AP_IP_OCTET_2,AP_IP_OCTET_3,AP_IP_OCTET_4), IPAddress(AP_GATEWAY_OCTET_1,AP_GATEWAY_OCTET_2,AP_GATEWAY_OCTET_3,AP_GATEWAY_OCTET_4), IPAddress(AP_SUBNET_OCTET_1,AP_SUBNET_OCTET_2,AP_SUBNET_OCTET_3,AP_SUBNET_OCTET_4));
    wifiManager.startConfigPortal();
  }
  //Sprawdzanie połączenia
  while(WiFi.status() != WL_CONNECTED){
    wifi_connect();
  }
  while(client.connected() != true){
    if(WiFi.status() != WL_CONNECTED) return;
    mqtt_connect();
    delay(MQTT_SERVER_TRY_CONNECT_DELAY);
  }
  if(WiFi.status() != WL_CONNECTED || client.connected() != true) return;
  //Koniec sprawdzania połączenia

  client.loop(); //Musi być wywoływana ciągle aby utrzymać połączenie z serwerem MQTT (nie używać delay)
  //Tutaj wysyłanie wiadomości (jeśli w odstępach czasowych użyć millis zamiast delay aby nie zatrzymać programu)
}
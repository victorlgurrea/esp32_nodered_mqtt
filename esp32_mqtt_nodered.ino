#include <WiFi.h>
#include <PubSubClient.h>

/* CONFIG */
char* ssid = "RED_2G";
char* password = "PASS_RED_2G";

char* mqtt_server = "IPv4"; // la de ipconfig 
char* tempTopic = "ESP32/Temperatura/Salon";
char* ledTopic  = "ESP32/Led/Control";

#define ledPin  2

WiFiClient espClient;
PubSubClient client(espClient);

/* CALLBACK MQTT */
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  if (String(topic) == ledTopic)
    digitalWrite(ledPin, msg == "HIGH" ? HIGH : LOW);
}

void reconnect() {
  while (!client.connected()) {
    client.connect("ESP32");
    client.subscribe(ledTopic);
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  client.publish(tempTopic, String(random(0, 61)).c_str());
  delay(10000);
}

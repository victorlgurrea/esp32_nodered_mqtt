#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// --------- WiFi ----------
const char* ssid = "MIWIFI_2G_kZyd";
const char* password = "Gk9YZTj2";

// --------- MQTT ----------
const char* mqtt_server = "192.168.1.135";
const int mqtt_port = 1883;

const char* topic_temp = "ESP32/Temperatura/Salon";
const char* topic_hum  = "ESP32/Humedad/Salon";
const char* topic_led  = "ESP32/Control/Led";

// --------- DHT11 ----------
#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --------- LED ----------
const int ledPin = 21;

// --------- Objetos ----------
WiFiClient espClient;
PubSubClient client(espClient);

// --------- Callback MQTT ----------
void callback(char* topic, byte* payload, unsigned int length) {
  String mensaje = "";

  for (int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }

  mensaje.trim();
  mensaje.toUpperCase();

  Serial.print("Topic recibido: ");
  Serial.println(topic);
  Serial.print("Mensaje recibido: ");
  Serial.println(mensaje);

  // Control del LED
  if (String(topic) == topic_led) {
     Serial.println("MENSAJE:"+ mensaje);
    if (mensaje == "HIGH") {
      digitalWrite(ledPin, HIGH);
      Serial.println("LED -> ENCENDIDO");
    } else if (mensaje == "LOW") {
      digitalWrite(ledPin, LOW);
      Serial.println("LED -> APAGADO");
    } else {
      Serial.println("Payload no valido para LED (usa HIGH o LOW)");
    }
  }
}

// --------- WiFi ----------
void setup_wifi() {
  delay(10);
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// --------- Reconexión MQTT ----------
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");

    if (client.connect("ESP32Client")) {
      Serial.println(" conectado");

      // Suscribirse al control del LED
      client.subscribe(topic_led);

    } else {
      Serial.print(" error, rc=");
      Serial.print(client.state());
      Serial.println(" reintentando en 5s");
      delay(5000);
    }
  }
}

// --------- SETUP ----------
void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  dht.begin();

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// --------- LOOP ----------
void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  // Leer DHT11
  float humedad = dht.readHumidity();
  float temperatura = dht.readTemperature();

  if (isnan(humedad) || isnan(temperatura)) {
    Serial.println("Error al leer el DHT11");
    return;
  }

  Serial.print("Temp: ");
  Serial.print(temperatura);
  Serial.print(" °C | Humedad: ");
  Serial.println(humedad);

  // ---- Publicar temperatura ----
  char tempStr[8];
  dtostrf(temperatura, 1, 2, tempStr);
  client.publish(topic_temp, tempStr);

  // ---- Publicar humedad ----
  char humStr[8];
  dtostrf(humedad, 1, 2, humStr);
  client.publish(topic_hum, humStr);

  delay(2000);
}
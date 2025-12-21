#include <WiFi.h>
#include <PubSubClient.h>

// Credenciales WiFi RECORDAR SOLO REDES 2G
char* WIFI_SSID     = "MIWIFI_2G_kZyd";
char* WIFI_PASSWORD = "Gk9YZTj2";

// Configuración del broker MQTT
char* MQTT_SERVER = "192.168.1.135";
int   MQTT_PORT   = 1883;


char* TOPIC_LED         = "ESP32/Led/Control";

int LED_PIN = 2;

WiFiClient espClient;
PubSubClient mqttClient(espClient);


void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("]: ");

  // Mostrar el mensaje recibido
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Control del LED
  if (strcmp(topic, TOPIC_LED) == 0 && length > 0) {
    if (payload[0] == 'H') {          // H = HIGH
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ENCENDIDO");
    } else if (payload[0] == 'L') {   // L = LOW
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED APAGADO");
    }
  }
}

void connectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando a MQTT...");

    if (mqttClient.connect("ESP32_Client")) {
      Serial.println(" conectado");
      mqttClient.subscribe(TOPIC_LED);
    } else {
      Serial.print(" fallo (rc=");
      Serial.print(mqttClient.state());
      Serial.println(") reintentando en 2s");
      delay(2000);
    }
  }
}

void connectToWiFi() {
  Serial.print("Conectando a WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado");
  Serial.print("IP asignada: ");
  Serial.println(WiFi.localIP());
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  connectToWiFi();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
}


void loop() {
  // Verificar conexión MQTT
  if (!mqttClient.connected()) {
    connectToMQTT();
  }
  mqttClient.loop();
}
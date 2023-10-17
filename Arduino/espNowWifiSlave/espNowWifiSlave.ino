/* Sensor Remoto ESP-NOW - Transmisor */

// Incluir bibliotecas
#include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>

// Definir los parámetros del DHT11
#define DHTPin 5
#define DHTTYPE DHT11

unsigned long previousMillis_UpdateDHT11 = 0;
const long interval_UpdateDHT11 = 2000;

// Crear un objeto DHT
DHT dht(DHTPin, DHTTYPE);

// Variables para temperatura y humedad
float t;
float h;

// Identificación (único para cada transmisor)
int ident = 1;

// Dirección MAC del receptor (reemplazar con la dirección MAC del receptor)
uint8_t broadcastAddress[] = {0x08, 0x3A, 0xF2, 0xAB, 0xC2, 0xC0};

// Definir la estructura de datos
typedef struct struct_message {
  float a;
  float b;
  int c;
} struct_message;

// Crear un objeto de datos estructurados
struct_message myData;

// Registrar el receptor
esp_now_peer_info_t peerInfo;

// Función de devolución de llamada para datos enviados
void OnDataSent(const uint8_t *macAddr, esp_now_send_status_t status)
{
  Serial.print("Estado del último paquete enviado: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Éxito en la entrega" : "Falló en la entrega");
}

void setup() {

  Serial.begin(115200);
  delay(100);

  // Iniciar el sensor DHT11
  dht.begin();

  // Establecer temporalmente el modo WiFi del ESP32 como estación
  WiFi.mode(WIFI_STA);

  // Inicializar ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error al inicializar ESP-NOW");
    return;
  }

  // Definir la función de devolución de llamada
  esp_now_register_send_cb(OnDataSent);

  // Copiar la dirección MAC del receptor al objeto peerInfo
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Agregar el receptor
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error al agregar el receptor");
    return;
  }

}

void loop() {

unsigned long currentMillis_UpdateDHT11 = millis();
  
  if (currentMillis_UpdateDHT11 - previousMillis_UpdateDHT11 >= interval_UpdateDHT11) {
    previousMillis_UpdateDHT11 = currentMillis_UpdateDHT11;

    //Lee la temperatura y humedad del sensor
    h = dht.readHumidity();
    t = dht.readTemperature();

    //Verifica si las lecturas fallan y vuelve a intentarlo
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Error al leer el sensor DHT."));
      return;
    }
  }

  Serial.print("Temperatura: ");
  Serial.println(t);
  Serial.print("Humedad: ");
  Serial.println(h);

  // Agregar los datos al objeto de datos estructurados
  myData.a = t;
  myData.b = h;
  myData.c = ident;

  // Enviar los datos
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  // Retraso para el sensor DHT22
  delay(2000);
}

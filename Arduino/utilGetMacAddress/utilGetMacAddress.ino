/* Conocer dirección MAC de ESP32*/

// Incluir biblioteca WiFi
#include "WiFi.h"

void setup() {

  Serial.begin(115200);

  //Configurar ESP en Station mode
  WiFi.mode(WIFI_MODE_STA);

  // Imprimir la dirección MAC
  Serial.print("Dirección MAC: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
}

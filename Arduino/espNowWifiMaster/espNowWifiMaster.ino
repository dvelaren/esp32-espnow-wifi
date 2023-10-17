/* Sensor Remoto ESP-NOW - Receptor */

// Incluir las bibliotecas necesarias
#include <WiFi.h>
#include <esp_now.h>
#include <HTTPClient.h>

const char* host1 = "CHANGE_HTTP_SERVER";
const char* ssid = "CHANGE_SSID";  // Nombre red Wi-Fi
const char* password = "CHANGE_PASS";   // Contraseña red Wi-Fi


// Definir la estructura de datos
typedef struct struct_message {
    float a;
    float b;
    int c;
} struct_message;

// Crear un objeto de datos estructurados
struct_message myData;

// Variables FreeRTOS (MultiCore)
TaskHandle_t send_wifi_task_handle;

float sensorData[2][2]; // Matriz para almacenar los datos de los sensores

float t1;
float h1;
float t2;
float h2;

//______________________________________________________________________________________//

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  
  // Obtener los datos entrantes
  memcpy(&myData, incomingData, sizeof(myData));

  // Actualizar la matriz según el identificador
  if (myData.c == 1) {
    sensorData[0][0] = myData.a; // Temperatura del sensor 1
    t1 = sensorData[0][0];
    sensorData[0][1] = myData.b; // Humedad del sensor 1
    h1 = sensorData[0][1];
  } else if (myData.c == 2) {
    sensorData[1][0] = myData.a; // Temperatura del sensor 2
    t2 = sensorData[1][0];
    sensorData[1][1] = myData.b; // Humedad del sensor 2
    h2 = sensorData[1][1];
  }

  Serial.println("Datos de los sensores:");
  Serial.print("Sensor 1 - Temperatura: ");
  Serial.println(t1);
  Serial.print("Sensor 1 - Humedad: ");
  Serial.println(h1);
  Serial.print("Sensor 2 - Temperatura: ");
  Serial.println(t2);
  Serial.print("Sensor 2 - Humedad: ");
  Serial.println(h2);
  Serial.println("");
}

//_______________________________________________________________________________________//

void wifiInit() {
  //Inicia la conexión a internet
  WiFi.begin(ssid, password);
  delay(5000);
  Serial.println("Conectando a la red Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conexión Wi-Fi establecida.");
}

//_______________________________________________________________________________________//

void TaskSendDataToServer(void* pvParameters) {
  Serial.println("TaskSendDataToServer() running on core: " + String(xPortGetCoreID()));
  wifiInit();
  while (true) {
    HTTPClient http;

    String sensorDataTemp = "Z4W7yCJQ6wxxs8CAaDdG";
    String location1 = "84";
    String ruta1 = "https://" + String(host1) + "/" + sensorDataTemp + "/" + t1 + "/" + h1 + "/" + location1;
    Serial.print("URL para ESPNOW1: ");
    Serial.println(ruta1);
    http.begin(ruta1.c_str());
    http.addHeader("Content-Type", "text/plain");
    int httpResponseCode1 = http.POST("send");
    Serial.print("Código de respuesta para ESPNOW1: ");
    Serial.println(httpResponseCode1);
    if (httpResponseCode1 == 200) {
      // El servidor respondió correctamente
      Serial.println("Solicitud HTTP exitosa ESPNOW1");
      delay(500);
      delay(500);
    } else {
      // Hubo un problema en la solicitud HTTP
      Serial.println("Error en la solicitud HTTP ESPNOW1");
      delay(500);
      http.end();
    }

    String sensorDataTemp2 = "4GIr8z5IIfCnG7knBN8I";
    String location2 = "84";
    String ruta2 = "https://" + String(host1) + "/" + sensorDataTemp2 + "/" + t2 + "/" + h2 + "/" + location2;
    Serial.print("URL para ESPNOW2: ");
    Serial.println(ruta2);
    http.begin(ruta2.c_str());
    http.addHeader("Content-Type", "text/plain");
    int httpResponseCode2 = http.POST("send");
    Serial.print("Código de respuesta para ESPNOW2: ");
    Serial.println(httpResponseCode2);
    if (httpResponseCode2 == 200) {
      // El servidor respondió correctamente
      Serial.println("Solicitud HTTP exitosa ESPNOW2");
      delay(500);
      delay(500);
    } else {
      // Hubo un problema en la solicitud HTTP
      Serial.println("Error en la solicitud HTTP ESPNOW2");
      delay(500);
      http.end();
    }

    http.end();
  }
}

//_______________________________________________________________________________________//
 
void setup() {
  // Configurar el Monitor en Serie
  Serial.begin(115200);

  // Iniciar el ESP32 en modo Estación
  WiFi.mode(WIFI_STA);

  // Inicializar ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error al inicializar ESP-NOW");
    return;
  }
   
  // Registrar la función de devolución de llamada
  esp_now_register_recv_cb(OnDataRecv);

  xTaskCreatePinnedToCore(
    TaskSendDataToServer,
    "Send WiFi Data",
    10000,                    // Stack size
    NULL,                    // When no parameter is used, simply pass NULL
    1,                       // Priority
    &send_wifi_task_handle,  // With task handle we will be able to manipulate with this task.
    0                        // Core on which the task will run
  );
  Serial.println("loop() running on core: " + String(xPortGetCoreID()));
}

//---------------------------------------------------------------------------------//

void loop() {
  vTaskDelay(10);
}
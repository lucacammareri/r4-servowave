#include <WiFiS3.h>

const char* ssid     = "TU_WIFI";
const char* password = "TU_PASSWORD";

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  delay(1500);

  Serial.println();
  Serial.println("=== DIAGNOSTICO WIFI R4 ===");

  // Chequear el modulo
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("ERROR: No se detecta el modulo WiFi.");
    while (true) delay(1000);
  }

  String fv = WiFi.firmwareVersion();
  Serial.print("Version de firmware del modulo: ");
  Serial.println(fv);
  if (fv != WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.print("Hay una version mas nueva disponible: ");
    Serial.println(WIFI_FIRMWARE_LATEST_VERSION);
    Serial.println("Te recomiendo actualizarlo desde el Arduino IDE (Herramientas > Firmware Updater).");
  }

  Serial.print("Conectando a: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 40) {
    delay(500);
    Serial.print(".");
    intentos++;
  }
  Serial.println();

  int status = WiFi.status();
  Serial.print("Status final (codigo): ");
  Serial.println(status);
  // 3 = WL_CONNECTED, 1 = WL_NO_SSID_AVAIL, 4 = WL_CONNECT_FAILED, 6 = WL_DISCONNECTED

  if (status == WL_CONNECTED) {
    delay(1000); // dar tiempo a que el DHCP asigne IP
    Serial.println("CONECTADO!");
    Serial.print("IP asignada: ");
    Serial.println(WiFi.localIP());
    Serial.print("Señal (RSSI): ");
    Serial.println(WiFi.RSSI());
  } else {
    Serial.println("NO SE PUDO CONECTAR.");
    Serial.println("Revisa: SSID, password, y que la red sea 2.4GHz (no 5GHz).");
  }
}

void loop() {
  // Reimprime la IP cada 3 segundos por si tarda en asignarse
  delay(3000);
  Serial.print("IP actual: ");
  Serial.println(WiFi.localIP());
  Serial.print("Status: ");
  Serial.println(WiFi.status());
}

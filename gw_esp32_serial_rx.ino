/******************************************************************************/
/* HUZZAH32 (ESP32) — Gateway backend                                         */
/* Reçoit les données série depuis le Feather M0                              */
/* Protocole série : <t=23.98:r=-67>                                          */
/* Envoie les données au Raspberry Pi via HTTP GET                            */
/*                                                                            */
/* Branchement :                                                              */
/*   Feather M0 TX/D1  →  ESP32 RX/GPIO16                                    */
/*   Feather M0 RX/D0  ←  ESP32 TX/GPIO17                                    */
/*   GND               —  GND                                                 */
/******************************************************************************/
#include <WiFi.h>
#include <HTTPClient.h>

// --- WiFi ---
#define WIFI_SSID     "SAE24_S203_1"
#define WIFI_PASSWORD "sae24_s203_1"

// --- IP statique ESP32 ---
IPAddress localIP(192, 168, 1, 215);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);

// --- Raspberry Pi ---
#define RPI_IP   "192.168.1.115"
#define RPI_URL  "http://" RPI_IP "/measures.php"

// --- Liaison série Feather M0 ---
#define SERIAL_M0_RX   16
#define SERIAL_M0_TX   17
#define SERIAL_M0_BAUD 9600

#define BUF_SIZE 64

char serialBuf[BUF_SIZE];
uint8_t bufIndex = 0;

// ----------------------------------------------------------------
void connectWiFi() {
  Serial.printf("[WiFi] Connexion à %s ...\n", WIFI_SSID);
  WiFi.config(localIP, gateway, subnet, dns);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint8_t attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\n[WiFi] Connecté ! IP : %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\n[WiFi] Échec de connexion !");
  }
}

// ----------------------------------------------------------------
// Parse un message du type <t=23.98:r=-67>
bool parseMessage(const char *msg, float &temperature, int &rssi) {
  if (msg[0] != '<') return false;
  const char *end = strchr(msg, '>');
  if (end == nullptr) return false;

  float t = 0.0f;
  int r = 0;
  int matched = sscanf(msg, "<t=%f:r=%d>", &t, &r);
  if (matched != 2) return false;

  temperature = t;
  rssi = r;
  return true;
}

// ----------------------------------------------------------------
// Envoie une requête GET au Raspberry Pi
void sendToRaspberryPi(float temperature, int rssi) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Non connecté, tentative de reconnexion...");
    connectWiFi();
    return;
  }

  // Construire l'URL : http://192.168.1.115/measures.php?temperature=23.98&rssi=-67
  char url[128];
  snprintf(url, sizeof(url), "%s?temperature=%.2f&rssi=%d", RPI_URL, temperature, rssi);
  Serial.printf("[HTTP] GET %s\n", url);

  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.printf("[HTTP] Réponse : %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String response = http.getString();
      Serial.printf("[HTTP] Body : %s\n", response.c_str());
    }
  } else {
    Serial.printf("[HTTP] Erreur : %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

// ----------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial2.begin(SERIAL_M0_BAUD, SERIAL_8N1, SERIAL_M0_RX, SERIAL_M0_TX);

  connectWiFi();

  Serial.println("[INFO] ESP32 prêt, en attente de données série du Feather M0...");
}

// ----------------------------------------------------------------
void loop() {
  // Reconnexion WiFi si perdue
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Connexion perdue, reconnexion...");
    connectWiFi();
  }

  // Lire caractère par caractère depuis Serial2
  while (Serial2.available()) {
    char c = Serial2.read();

    if (c == '\n' || c == '\r') {
      if (bufIndex > 0) {
        serialBuf[bufIndex] = '\0';
        float temperature = 0.0f;
        int rssi = 0;

        if (parseMessage(serialBuf, temperature, rssi)) {
          Serial.printf("[DATA] Température : %.2f °C  |  RSSI : %d dBm\n", temperature, rssi);
          sendToRaspberryPi(temperature, rssi);
        } else {
          Serial.printf("[WARN] Message non reconnu : %s\n", serialBuf);
        }

        bufIndex = 0;
      }
    } else {
      if (bufIndex < BUF_SIZE - 1) {
        serialBuf[bufIndex++] = c;
      } else {
        Serial.println("[WARN] Buffer plein, reset");
        bufIndex = 0;
      }
    }
  }
}

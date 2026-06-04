/******************************************************************************/
/* Feather M0 RFM9x — Gateway                                                 */
/* Reçoit les données LoRa du End Device et les transmet en série à l'ESP32   */
/* Protocole série : <t=23.98:r=-67>                                          */
/*                                                                            */
/* Librairies nécessaires (Library Manager) :                                 */
/* - RadioHead by Mike McCauley                                                */
/******************************************************************************/
#include <RH_RF95.h>
#include <SPI.h>

#define RFM95_FREQ  869.525
#define RFM95_CS    8
#define RFM95_RST   4
#define RFM95_INT   3

#define SERIAL_ESP32_BAUD 9600  // liaison série vers l'ESP32 (Serial1 = TX/D1, RX/D0)

RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Décode les 2 octets de température (même encodage que l'émetteur)
float decodeTemperatureFrom2bytes(uint8_t msb, uint8_t lsb) {
  int16_t encoded = (int16_t)((msb << 8) | lsb);
  return encoded / 100.0f;
}

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);   // debug USB vers PC
  Serial1.begin(SERIAL_ESP32_BAUD); // liaison série vers ESP32

  delay(3000);

  // Reset LoRa
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init()) {
    Serial.println("[ERROR] LoRa initialization failed");
    while (1);
  }

  rf95.setFrequency(RFM95_FREQ);
  rf95.setModeRx(); // mode réception continue

  Serial.println("[INFO] Gateway Feather M0 prêt, en attente de paquets LoRa...");
}


void loop() {
  if (rf95.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
      if (len >= 6) {
        uint32_t counter = ((uint32_t)buf[0] << 24)
                         | ((uint32_t)buf[1] << 16)
                         | ((uint32_t)buf[2] <<  8)
                         |  (uint32_t)buf[3];

        float temperature = decodeTemperatureFrom2bytes(buf[4], buf[5]);
        int16_t rssi = rf95.lastRssi();

        // Debug sur port USB
        Serial.printf("[INFO] Paquet reçu — counter=%lu  temp=%.2f°C  RSSI=%d dBm\n",
                      counter, temperature, rssi);

        // Envoi vers ESP32 via Serial1 : format <t=23.98:r=-67>
        Serial1.printf("<t=%.2f:r=%d>\n", temperature, rssi);

      } else {
        Serial.printf("[WARN] Paquet trop court (%d octets)\n", len);
      }

    } else {
      Serial.println("[ERROR] Échec de réception du paquet");
    }
  }
}

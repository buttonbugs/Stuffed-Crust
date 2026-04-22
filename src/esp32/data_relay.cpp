#include "data_relay.h"
#include <esp_now.h>
#include <WiFi.h>

uint8_t receiverMacAddress[] = {0xDC, 0xDA, 0x0C, 0x28, 0x32, 0xA8};
esp_now_peer_info_t peerInfo;
volatile bool isSending = false;  // Flag to indicate if data is currently being sent

// ESP-NOW send callback
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  isSending = false;  // Reset the sending flag
  // Optional: uncomment to debug
  // Serial.print("Last Packet Send Status: ");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void init_relay() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  Serial.println("ESP-NOW initialized");
  
  // Register send callback
  esp_now_register_send_cb(OnDataSent);

  // Add peer
  memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  
  Serial.print("Peer added: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(receiverMacAddress[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
}

void relay_data(BotData data) {
  if (isSending) return; // Drop or queue the packet — previous TX still in flight

  isSending = true;
  esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *)&data, sizeof(BotData));

  if (result != ESP_OK) {
    isSending = false; // Reset if queueing itself failed
  }
}
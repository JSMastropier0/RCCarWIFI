/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  mac mando=0C:DC:7E:62:16:34
  mac robot=CC:50:E3:BF:04:70
*/

#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>

//Defineix els pins que utilitzarem

const int X_pin = 33; // analog pin connected to X output
const int Y_pin = 35; // analog pin connected to Y output

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0xCC, 0x50, 0xE3, 0xBF, 0x04, 0x70};

// Define variables to store joystick readings
int lecturaX;
int lecturaY;
char button;

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
  int eixX;
  int eixY;
  char buto;
} struct_message;

// Create a struct_message called BME280Readings to hold sensor readings
struct_message joyReadings;

// Create a struct_message to hold incoming sensor readings
struct_message incomingJoyReadings;

esp_now_peer_info_t peerInfo;


// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//  Serial.print("\r\nLast Packet Send Status:\t");
//  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingJoyReadings, incomingData, sizeof(incomingJoyReadings));
//  Serial.print("Bytes received: ");
//  Serial.println(len);
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
//    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
//    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {

  lecturaX = analogRead(X_pin);
  lecturaY = analogRead(Y_pin);
  
  Serial.print("LecturaX: ");
  Serial.print(lecturaX);
  Serial.print("||||| LecturaY: ");
  Serial.println(lecturaY);

  
  // Set values to send
  joyReadings.eixX = lecturaX;
  joyReadings.eixY = lecturaY;
  joyReadings.buto = 'A';

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &joyReadings, sizeof(joyReadings));
   
  if (result == ESP_OK) {
//    Serial.println("Sent with success");
  }
  else {
//    Serial.println("Error sending the data");
  }
 
//  delay(1000);
}

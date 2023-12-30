#line 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\calibMONITOR\\versiones\\v1\\espCELDA\\build\\sketch\\espCELDA.ino.cpp"
#include <Arduino.h>
#line 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
//* COMUNICACION MULTICELDA DESDE EL MASTER

#include <esp_now.h>
#include <WiFi.h>



// REPLACE WITH THE MAC Address of your receiver 
uint8_t macMASTER[] = {0xA0, 0xB7, 0x65, 0xDD, 0x9E, 0xD4};

// Define the message to be sent as a string
//String messageToSend = "string1";

esp_now_peer_info_t master;


// Callback when data is sent
#line 18 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
#line 24 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
#line 69 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void setup();
#line 102 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void loop();
#line 105 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void sendSTRING(String messageToSend, uint8_t* MAC);
#line 130 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void cargasCELDA();
#line 134 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void purgaCELDA();
#line 138 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void vertxCELDA(int cantidad);
#line 142 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void estadCELDA();
#line 146 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void driveCELDA();
#line 18 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  String receivedMessage = String((char*)incomingData);
  Serial.println(receivedMessage);
  String ORDEN      = receivedMessage.substring(0, 5);
  String CANTIDADstr   = receivedMessage.substring(5, 9);
  int CANTIDAD      = CANTIDADstr.toInt();


  //* Filtrado de comandos

  if (ORDEN == "CARGA"){
    cargasCELDA();
  }
  else if (ORDEN == "PURGA"){
    purgaCELDA();
  }
  else if (ORDEN == "VERTX"){
    vertxCELDA(CANTIDAD);
  }
  else if (ORDEN == "ESTAD"){
    estadCELDA();
  }
  else if (ORDEN == "DRIVE"){
    driveCELDA();
  }
  else if (ORDEN == "STOPX"){
    Serial.println("ORDEN DE STOP");
  }
  else{
    Serial.println("ERROR: comando desconocido");
  }





  //Serial.printf("\nOrden enviada: %s\n",ORDEN);

  digitalWrite(2,HIGH);
  delay(500);
  digitalWrite(2,LOW);
}



void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);



  //* REGISTRAR PEERS
  memcpy(master.peer_addr, macMASTER, 6);
  master.channel = 1;  
  master.encrypt = false;

  //* AÑADIR PEERS      
  if (esp_now_add_peer(&master) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);

}

void loop() {
}

void sendSTRING(String messageToSend, uint8_t* MAC){
  // Send message via ESP-NOW
  uint8_t messageLength = messageToSend.length() + 1; // Include the null terminator
  uint8_t messageBytes[messageLength];
  messageToSend.getBytes(messageBytes, messageLength);
  int retry = 0;
  bool sended = false;
  do
  {
  esp_err_t result = esp_now_send(MAC, messageBytes, messageLength);
  if (result == ESP_OK) {
    Serial.println("Mensaje enviado.");
    sended = true;
  }
  else {
    Serial.printf("Retry: %d \n",retry);
    retry++;
  }
  } while ((sended == false)||(retry > 3));
  if (sended == false){
    Serial.println("No se pudo enviar el mensaje");
  }
  delay(100);
}

void cargasCELDA(){
  Serial.println("CARGA iniciada");
}

void purgaCELDA(){
  Serial.println("PURGA iniciada");
}

void vertxCELDA(int cantidad){
  Serial.printf("VERTIMIENTO %d g.\n",cantidad);
}

void estadCELDA(){
  Serial.println("ESTADO iniciada");
}

void driveCELDA(){
  Serial.println("DRIVE iniciada");
}



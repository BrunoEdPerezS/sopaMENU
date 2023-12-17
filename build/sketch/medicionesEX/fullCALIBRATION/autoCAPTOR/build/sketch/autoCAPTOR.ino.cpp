#line 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoCAPTOR\\build\\sketch\\autoCAPTOR.ino.cpp"
#include <Arduino.h>
#line 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoCAPTOR\\autoCAPTOR.ino"
#include <esp_now.h>
#include <WiFi.h>


#define TXLED 27
#define RXLED 14
#define LEDBL 2

//Comunicacion por espNOW con dos CELDAS
// REPLACE WITH THE MAC Address of your receiver 
uint8_t probeMAC[6] = {0x94, 0xB7, 0x65, 0xDD, 0x04, 0x5C};
uint8_t captorMAC[6];
int numeroX = 1000;
volatile bool printENABLE = false;
// Define the message to be sent as a string
//String messageToSend = "string1";




esp_now_peer_info_t peerInfo;

// Callback when data is sent
#line 24 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoCAPTOR\\autoCAPTOR.ino"
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
#line 34 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoCAPTOR\\autoCAPTOR.ino"
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
#line 62 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoCAPTOR\\autoCAPTOR.ino"
void setup();
#line 89 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoCAPTOR\\autoCAPTOR.ino"
void loop();
#line 100 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoCAPTOR\\autoCAPTOR.ino"
void sendSTRING(String mensajeTOsend);
#line 111 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoCAPTOR\\autoCAPTOR.ino"
template <typename T>void wifiPRINT(T mensajeTOsend);
#line 126 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoCAPTOR\\autoCAPTOR.ino"
void addPEER();
#line 142 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoCAPTOR\\autoCAPTOR.ino"
void processCommand(String command);
#line 24 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoCAPTOR\\autoCAPTOR.ino"
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\nDatos enviados: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success\n" : "Delivery Fail\n");
  digitalWrite(TXLED, HIGH);
  delay(50);
  digitalWrite(TXLED, LOW);
  delay(50);
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  String receivedMessage = String((char*)incomingData);

  // Verifica si los primeros 5 caracteres del mensaje son "print"
  String HEADER = receivedMessage.substring(0, 5);
  String TEXTO  = receivedMessage.substring(5, len);
  if (HEADER.equalsIgnoreCase("PAIRR"))
  {
    memcpy(probeMAC, mac, 6);
    sendSTRING("PAIRA");
    addPEER();
    
  }
  if(printENABLE){
  if (HEADER.equalsIgnoreCase("print")) {
    Serial.println(TEXTO);
  } else {
    Serial.println("Mensaje recibido no válido: " + receivedMessage);
  }
  }
  digitalWrite(RXLED, HIGH);
  delay(50);
  digitalWrite(RXLED, LOW);
  delay(50);
}



void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  //Setup de LED TXRX
  pinMode(RXLED, OUTPUT);
  pinMode(TXLED, OUTPUT);
  pinMode(LEDBL,OUTPUT);
  digitalWrite(RXLED,LOW);
  digitalWrite(TXLED,LOW);
  digitalWrite(LEDBL,LOW);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.macAddress(captorMAC);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  addPEER();
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Lee la entrada hasta encontrar un salto de línea
    processCommand(command);
  }
  


delay(500);
}

void sendSTRING(String mensajeTOsend) {
  String mensajeString = String(mensajeTOsend);
  uint8_t messageLength = mensajeString.length() + 1; // Include the null terminator
  uint8_t messageBytes[messageLength];
  mensajeString.getBytes(messageBytes, messageLength);
  esp_err_t result = esp_now_send(probeMAC, messageBytes, messageLength);
  if (result != ESP_OK) {
    Serial.println("Error de envío.");
  }
}


template <typename T>
void wifiPRINT(T mensajeTOsend) {
  String mensajeString = String("print") + String(mensajeTOsend);
  uint8_t messageLength = mensajeString.length() + 1; // Include the null terminator
  uint8_t messageBytes[messageLength];
  mensajeString.getBytes(messageBytes, messageLength);
  esp_err_t result = esp_now_send(probeMAC, messageBytes, messageLength);
  Serial.println(mensajeString);
  
  if (result != ESP_OK) {
    Serial.println("Error de envío.");
  }
}

void addPEER(){
  // Register peer
  memcpy(peerInfo.peer_addr, probeMAC, 6);
  peerInfo.channel = 1;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }else{
    Serial.println("Peer added");
    digitalWrite(LEDBL,HIGH);
  }
}

void processCommand(String command) {
  
  String primerCOMANDO = command.substring(0, 5);
  
  if (primerCOMANDO.equals("print")) {
  String argumento = command.substring(5);
  if (argumento.equals("ENABLE")) {
    printENABLE = true;
  } else if ("DISABLE") {
    printENABLE = false;
  }
  }

  // SETVA
  if (primerCOMANDO.equals("setVA")) {
    int comaPos = command.indexOf(',');

    // Verificar si se encontró la coma y si hay al menos un carácter después de la coma
    if (comaPos != -1 && comaPos < command.length() - 1) {
      // Obtener el nombre y el valor separados por la coma
      String argumento = command.substring(6, comaPos);
      String valorStr = command.substring(comaPos + 1);
      //int valor = valorStr.toInt();  // Cambia a toFloat() si es un valor decimals
      String toSEND = String("setVA(") + String(argumento)+String(",") + String(valorStr);
      Serial.print("Se va a enviar el comando: ");
      Serial.println(toSEND);
      sendSTRING(toSEND);
    } else {
      Serial.println("Error en el formato del comando setVA");
    }
  }
}


#line 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\build\\sketch\\autoPROBE.ino.cpp"
#include <Arduino.h>
#line 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\autoPROBE.ino"
#include <esp_now.h>
#include <WiFi.h>


//Inicializar variables
#define BUTTON1 17

#define TXLED 27
#define RXLED 14
#define LEDBL 2

unsigned long buttonTIME = 0;  
unsigned long lastBTIME = 0; 
hw_timer_t *My_timer = NULL;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
volatile bool PAIRED  = false;
volatile bool PAIRING = false;


//Comunicacion por espNOW con dos CELDAS
// REPLACE WITH THE MAC Address of your receiver 
uint8_t captorMAC[] = {0xA0, 0xB7, 0x65, 0xDD, 0x9E, 0xD4};
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

volatile int numeX = 1000;
// Define the message to be sent as a string
//String messageToSend = "string1";

esp_now_peer_info_t peerBROADCAST;
esp_now_peer_info_t peerCAPTOR;



#line 57 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\autoPROBE.ino"
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
#line 63 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\autoPROBE.ino"
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
#line 99 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\autoPROBE.ino"
void setup();
#line 140 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\autoPROBE.ino"
void loop();
#line 158 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\autoPROBE.ino"
template <typename T>void wifiPRINT(T mensajeTOsend);
#line 178 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\autoPROBE.ino"
void pairREQUEST();
#line 194 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\autoPROBE.ino"
void addPEER();
#line 34 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\autoPROBE.ino"
void IRAM_ATTR BUTTONpress1(){
   buttonTIME = millis();
//Condicion para el debounce
if (buttonTIME - lastBTIME > 1000)
{
   PAIRING = true;
   Serial.println("Pairing pressed");
   
lastBTIME = buttonTIME;
}
}











// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\nDatos enviados: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success\n" : "Delivery Fail\n");
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  String receivedMessage = String((char*)incomingData);
  // Verifica si el mensaje recibido es "string2" y, si es así, imprímelo
  Serial.println(receivedMessage);
  String HEADER = receivedMessage.substring(0, 5);
  String TEXTO  = receivedMessage.substring(5, len);
  if (HEADER.equalsIgnoreCase("PAIRA"))
  {
    memcpy(captorMAC, mac, 6);
    addPEER();
  }else if (HEADER.equalsIgnoreCase("setVA")){
    int comaPos = TEXTO.indexOf(',');
    if (comaPos != -1 && comaPos < TEXTO.length() - 1) {
      // Obtener la variable y el valor separados por la coma
      String variable = TEXTO.substring(1, comaPos);
      String valorStr = TEXTO.substring(comaPos + 1);
      int valor = valorStr.toInt(); 
      // Asignar el valor a la variable correspondiente según el nombre obtenido
      if (variable.equalsIgnoreCase("numeX")) {
        numeX = valor;
      }
    }
  }



  Serial.println("Mensaje recibido.");
  Serial.println(receivedMessage);
  digitalWrite(RXLED,HIGH);
  delay(50);
  digitalWrite(RXLED,LOW);
  delay(50);
}



void setup() {
  //Interrupciones de los botones y leds
  pinMode(BUTTON1, INPUT_PULLUP);
  attachInterrupt(BUTTON1, BUTTONpress1, RISING);

  pinMode(RXLED, OUTPUT);
  pinMode(TXLED, OUTPUT);
  pinMode(LEDBL,OUTPUT);
  digitalWrite(RXLED,LOW);
  digitalWrite(TXLED,LOW);
  digitalWrite(LEDBL,LOW);

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

  
  // Register peer
  memcpy(peerBROADCAST.peer_addr, broadcastAddress, 6);
  peerBROADCAST.channel = 1;  
  peerBROADCAST.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerBROADCAST) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
}

void loop() {

if (PAIRING && (!PAIRED)){
Serial.println("Pairing started");
pairREQUEST();
delay(100);
PAIRING = false;
}

if(PAIRED){
wifiPRINT(numeX);
delay(1000);
}
delay(20);

}



template <typename T>
void wifiPRINT(T mensajeTOsend) {
  String mensajeString = String("print") + String(mensajeTOsend);
  uint8_t messageLength = mensajeString.length() + 1; // Include the null terminator
  uint8_t messageBytes[messageLength];
  mensajeString.getBytes(messageBytes, messageLength);
  esp_err_t result = esp_now_send(captorMAC, messageBytes, messageLength);
  Serial.println(mensajeString);
  
  if (result != ESP_OK) {
    Serial.println("Error de envío.");
  }
  digitalWrite(TXLED,HIGH);
  delay(50);
  digitalWrite(TXLED,LOW);
  delay(50);
}


void pairREQUEST() {
  String mensajeString = String("PAIRR");
  uint8_t messageLength = mensajeString.length() + 1; // Include the null terminator
  uint8_t messageBytes[messageLength];
  mensajeString.getBytes(messageBytes, messageLength);
  esp_err_t result = esp_now_send(broadcastAddress, messageBytes, messageLength);
  if (result != ESP_OK) {
    Serial.println("Error de envío PAIRING.");
  }
  digitalWrite(TXLED,HIGH);
  delay(50);
  digitalWrite(TXLED,LOW);
  delay(50);

}

void addPEER(){
  // Register peer
  memcpy(peerCAPTOR.peer_addr, captorMAC, 6);
  peerCAPTOR.channel = 1;  
  peerCAPTOR.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerCAPTOR) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }else{
    Serial.println("Peer added");
    PAIRED = true;
    digitalWrite(LEDBL,HIGH);
  }
}

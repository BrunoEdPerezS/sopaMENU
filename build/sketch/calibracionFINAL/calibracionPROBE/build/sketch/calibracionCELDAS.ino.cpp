#line 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\calibracionFINAL\\calibracionPROBE\\build\\sketch\\calibracionCELDAS.ino.cpp"
#include <Arduino.h>
#line 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\calibracionCELDAS\\calibracionCELDAS.ino"
//https://andrewshirley.tech/how-to-weigh-stuff-with-a-microcontroller/


#include "HX711.h"
#include <esp_now.h>
#include <WiFi.h>




// HX711 circuit wiring
const int LOADCELL_DOUT_PIN1 = 32;
const int LOADCELL_SCK_PIN1 = 33;
const int LOADCELL_DOUT_PIN2 = 25;
const int LOADCELL_SCK_PIN2 = 26;
const int LOADCELL_DOUT_PIN3 = 27;
const int LOADCELL_SCK_PIN3 = 14;
const int LOADCELL_DOUT_PIN4 = 12;
const int LOADCELL_SCK_PIN4 = 13;



volatile long gainCELDA1 = 1;
volatile long gainCELDA2 = 2;
volatile long gainCELDA3 = 3;
volatile long gainCELDA4 = 4;
long rawMEASURE       = 0;
long correctedMEASURE = 0;
int celdaMONITOR = 1;


uint8_t captorMAC[] = {0xA0, 0xB7, 0x65, 0xDD, 0x9E, 0xD4};

esp_now_peer_info_t peerInfo;

// Callback when data is sent
#line 37 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\calibracionCELDAS\\calibracionCELDAS.ino"
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
#line 43 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\calibracionCELDAS\\calibracionCELDAS.ino"
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
#line 102 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\calibracionCELDAS\\calibracionCELDAS.ino"
void setup();
#line 154 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\calibracionCELDAS\\calibracionCELDAS.ino"
void loop();
#line 236 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\calibracionCELDAS\\calibracionCELDAS.ino"
void sendSTRING(String messageToSend);
#line 250 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\calibracionCELDAS\\calibracionCELDAS.ino"
template <typename T>void wifiPRINT(T mensajeTOsend);
#line 37 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\calibracionCELDAS\\calibracionCELDAS.ino"
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
  if (HEADER.equalsIgnoreCase("setVA")){
    int comaPos = TEXTO.indexOf(',');
    if (comaPos != -1 && comaPos < TEXTO.length() - 1) {
      // Obtener la variable y el valor separados por la coma
      String variable = TEXTO.substring(1, comaPos);
      String valorStr = TEXTO.substring(comaPos + 1);
      int valor = valorStr.toInt(); 
      // Asignar el valor a la variable correspondiente según el nombre obtenido
      if (variable.equalsIgnoreCase("celdaMONITOR")) {
        //Condicion de limites
        if (valor <= 1){
          valor = 1;
          celdaMONITOR = valor;
        }else if (valor >= 4){
          valor = 4;
          celdaMONITOR = valor;
        }else{
          celdaMONITOR = valor;
        }
        
      }else if (variable.equalsIgnoreCase("gainCELDA1")) {
        gainCELDA1 = valor;
      }
      else if (variable.equalsIgnoreCase("gainCELDA2")) {
        gainCELDA2 = valor;
      }
      else if (variable.equalsIgnoreCase("gainCELDA3")) {
        gainCELDA3 = valor;
      }
      else if (variable.equalsIgnoreCase("gainCELDA4")) {
        gainCELDA4 = valor;
      }

    }
  }

  Serial.println("Mensaje recibido.");
  Serial.println(receivedMessage);
  digitalWrite(2,HIGH);
  delay(50);
  digitalWrite(2,LOW);
  delay(50);
}




HX711 celda1;
HX711 celda2;
HX711 celda3;
HX711 celda4;


void setup() {
  Serial.begin(115200);
   WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  memcpy(peerInfo.peer_addr, captorMAC, 6);
  peerInfo.channel = 1;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  //Celdas de carga
  Serial.printf("\n\nTaring celda1\n");
  celda1.begin(LOADCELL_DOUT_PIN1, LOADCELL_SCK_PIN1, 64);    // Set gain to 64, uses A- and A+ on the HX711 Board
  celda2.begin(LOADCELL_DOUT_PIN2, LOADCELL_SCK_PIN2, 64);
  celda3.begin(LOADCELL_DOUT_PIN3, LOADCELL_SCK_PIN3, 64);
  celda4.begin(LOADCELL_DOUT_PIN4, LOADCELL_SCK_PIN4, 64);

  //Init de unidades CELDA1
  celda1.set_scale();
  celda1.tare();
  //Init de unidades CELDA2
  celda1.set_scale();
  celda1.tare();           
  //Init de unidades CELDA3
  celda1.set_scale();
  celda1.tare();      
  //Init de unidades CELDA4
  celda1.set_scale();
  celda1.tare();  

 
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
}

// CALCULATING celda1:  ( celda1.read() - celda1.offset ) / KnownWeight
// Example:              ( 34811 - (-7209) ) / 203g = (  34811 + 7209  )  / 203g =  206
// celda1.set_celda1(206);

void loop() {

    
    //String myString1 = String("Celda1") + String("Raw:") + String(celda1.read()) + String(" Offset:") + String(celda1.get_offset()) + String(" celda1:") + String(celda1.get_scale()) + String(" Corrected:") + String(celda1.get_units(5));

   if (celdaMONITOR == 1){
    celda1.set_scale();
    celda1.tare();  
    rawMEASURE = celda1.get_units(10);
    celda1.set_scale(gainCELDA1); 
    correctedMEASURE = celda1.get_units(10);
    //Print resultados
    String bufferSTR = String("CELDA 1 - ") + String("RAW: ") + String(rawMEASURE) + String(" CORRECT: ") + String(correctedMEASURE) + String(" GAIN: ") + String(gainCELDA1);
    Serial.println(bufferSTR);
    wifiPRINT(bufferSTR);

   }
   else if (celdaMONITOR == 2){
    celda2.set_scale();
    celda2.tare();  
    rawMEASURE = celda2.get_units(10);
    celda2.set_scale(gainCELDA2); 
    correctedMEASURE = celda2.get_units(10);
    //Print resultados
    String bufferSTR = String("CELDA 2 - ") + String("RAW: ") + String(rawMEASURE) + String(" CORRECT: ") + String(correctedMEASURE) + String(" GAIN: ") + String(gainCELDA2);
    Serial.println(bufferSTR);
    wifiPRINT(bufferSTR);

   }
   else if (celdaMONITOR == 3){
    celda3.set_scale();
    celda3.tare();  
    rawMEASURE = celda3.get_units(10);
    celda3.set_scale(gainCELDA3); 
    correctedMEASURE = celda3.get_units(10);
    //Print resultados
    String bufferSTR = String("CELDA 3 - ") + String("RAW: ") + String(rawMEASURE) + String(" CORRECT: ") + String(correctedMEASURE) + String(" GAIN: ") + String(gainCELDA3);
    Serial.println(bufferSTR);
    wifiPRINT(bufferSTR);
   }   
   else if (celdaMONITOR == 4){
    celda4.set_scale();
    celda4.tare();  
    rawMEASURE = celda4.get_units(10);
    celda4.set_scale(gainCELDA4); 
    correctedMEASURE = celda4.get_units(10);
    //Print resultados
    String bufferSTR = String("CELDA 4 - ") + String("RAW: ") + String(rawMEASURE) + String(" CORRECT: ") + String(correctedMEASURE) + String(" GAIN: ") + String(gainCELDA4);
    Serial.println(bufferSTR);
    wifiPRINT(bufferSTR);
   }






    //String myString1 = String(" Medidas: ") + String("C1: ") + String(celda1.get_units(5)) + String("  C2: ") + String(celda2.get_units(5)) + String("  C3: ") + String(celda3.get_units(5)) + String("  C4: ") + String(celda4.get_units(5));

    //Serial.println("MEDICIONES: ");
    //Serial.println(myString1);
    //Serial.println(myString2);
    //Serial.println(myString3);
    //Serial.println(myString4);

    /*
    sendSTRING("ENVIANDO DATOS: ");
    sendSTRING(myString1);
    delay(500);
    sendSTRING(myString2);
    delay(500);
    sendSTRING(myString3);
    delay(500);
    sendSTRING(myString4);
    */
    delay(1000);
    
}




void sendSTRING(String messageToSend){
  // Send message via ESP-NOW
  uint8_t messageLength = messageToSend.length() + 1; // Include the null terminator
  uint8_t messageBytes[messageLength];
  messageToSend.getBytes(messageBytes, messageLength);
  esp_err_t result = esp_now_send(captorMAC, messageBytes, messageLength);
   
  if (result == ESP_OK) {
    Serial.println("Mensaje enviado.");
  }
  else {
    Serial.println("Error de envío.");
  }
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
  digitalWrite(2,HIGH);
  delay(50);
  digitalWrite(2,LOW);
  delay(50);
}


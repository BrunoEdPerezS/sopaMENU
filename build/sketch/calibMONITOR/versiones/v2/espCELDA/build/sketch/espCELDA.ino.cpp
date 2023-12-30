#line 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\calibMONITOR\\versiones\\v2\\espCELDA\\build\\sketch\\espCELDA.ino.cpp"
#include <Arduino.h>
#line 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
//* COMUNICACION MULTICELDA DESDE EL MASTER

#include <esp_now.h>
#include <WiFi.h>
#include "HX711.h"



volatile bool initCARGA = false;
volatile bool initPURGA = false;
volatile bool initVERTX = false;

volatile bool STOPX = false;

//Setting de mediciones
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN1 = 32;
const int LOADCELL_SCK_PIN1 = 33;
const int LOADCELL_DOUT_PIN2 = 25;
const int LOADCELL_SCK_PIN2 = 26;
const int LOADCELL_DOUT_PIN3 = 27;
const int LOADCELL_SCK_PIN3 = 14;
const int LOADCELL_DOUT_PIN4 = 12;
const int LOADCELL_SCK_PIN4 = 13;

//Mediciones y buffers de media movil
float rawMEASURE        = 0;  
float correctedMEASURE = 0;
const int numDatos = 30;  // Número de datos para el cálculo de la media móvil
float buff1[numDatos];      // Array para almacenar los datos
float buff2[numDatos];      // Array para almacenar los datos
float buff3[numDatos];      // Array para almacenar los datos
float buff4[numDatos];      // Array para almacenar los datos
float mean1,mean2,mean3,mean4;
float offset1 =0, offset2 =0, offset3 =0, offset4 =0;
float tarado1, tarado2, tarado3, tarado4;

int indice = 0;           // Índice actual en el array
int CANTIDADVERT = 0;



// REPLACE WITH THE MAC Address of your receiver 
uint8_t macMASTER[] = {0xA0, 0xB7, 0x65, 0xDD, 0x9E, 0xD4};

// Define the message to be sent as a string
//String messageToSend = "string1";

esp_now_peer_info_t master;

HX711 celda1;
HX711 celda2;
HX711 celda3;
HX711 celda4;

// Callback when data is sent
#line 57 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
#line 63 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
#line 112 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void setup();
#line 151 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void loop();
#line 230 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void sendSTRING(String messageToSend, uint8_t* MAC);
#line 255 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void cargasCELDA();
#line 274 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void purgaCELDA();
#line 287 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void vertxCELDA(int cantidad);
#line 291 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void estadCELDA();
#line 295 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void driveCELDA();
#line 299 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
float calcularMediaMovil(float datosIN[10]);
#line 307 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void tareCELLS();
#line 314 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void cellMEASURE();
#line 57 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espCELDA\\espCELDA.ino"
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  String receivedMessage = String((char*)incomingData);
  Serial.print("Mensaje recibido: ");
  Serial.println(receivedMessage);
  String ORDEN         = receivedMessage.substring(0, 5);
  String CANTIDADstr   = receivedMessage.substring(5, 9);
  CANTIDADVERT         = CANTIDADstr.toInt();


  //* Filtrado de comandos

  if (ORDEN == "CARGA"){
    initCARGA = true;
  }
  else if (ORDEN == "PURGA"){
    initPURGA = true;
  }
  else if (ORDEN == "VERTX"){
    initVERTX = true;

  }
  else if (ORDEN == "ESTAD"){
  //  estadCELDA();
    tareCELLS();
  }
  else if (ORDEN == "DRIVE"){
    driveCELDA();
  }
  else if (ORDEN == "STOPX"){
    STOPX = true;
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
  //Init de las celdas
  celda1.begin(LOADCELL_DOUT_PIN1, LOADCELL_SCK_PIN1, 64);    // Set gain to 64, uses A- and A+ on the HX711 Board
  celda2.begin(LOADCELL_DOUT_PIN2, LOADCELL_SCK_PIN2, 64);
  celda3.begin(LOADCELL_DOUT_PIN3, LOADCELL_SCK_PIN3, 64);
  celda4.begin(LOADCELL_DOUT_PIN4, LOADCELL_SCK_PIN4, 64);

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

if(initCARGA){
cargasCELDA();
initCARGA = false;
}
else if (initPURGA)
{
  purgaCELDA();
  initPURGA = false;
}
else if (initVERTX)
{
  vertxCELDA(CANTIDADVERT);
  initVERTX = false;
}
else
{
  //Serial.println("Celda inactiva");
}


//Medicion constante de las celdas
//Serial.println("Medidas celdas: ");
/* Medidas RAW
rawMEASURE = celda1.read();
Serial.println(rawMEASURE);
rawMEASURE = celda2.read();
Serial.println(rawMEASURE);
rawMEASURE = celda3.read();
Serial.println(rawMEASURE);
rawMEASURE = celda4.read();
Serial.println(rawMEASURE);
*/
// Buffers de medida
/*
  // Almacenar el nuevo dato en el array
  buff1[indice] = float(celda1.read());
  buff2[indice] = float(celda2.read());
  buff3[indice] = float(celda3.read());
  buff4[indice] = float(celda4.read());
  // Calcular la media móvil
  mean1 = calcularMediaMovil(buff1);
  mean2 = calcularMediaMovil(buff2);
  mean3 = calcularMediaMovil(buff3);
  mean4 = calcularMediaMovil(buff4);
  //Mean - offset
  tarado1 = mean1-offset1;
  tarado2 = mean2-offset2;
  tarado3 = mean3-offset3;
  tarado4 = mean4-offset4; 

  // Imprimir resultado
  Serial.println("Mean de cada uno: ");
  Serial.println(tarado1);
  Serial.println(tarado2);
  Serial.println(tarado3);
  Serial.println(tarado4);
  float finalMEAN = (tarado1+tarado2+tarado3)/3;
  Serial.println(finalMEAN);


indice = (indice + 1) % numDatos;
*/
cellMEASURE();
// Imprimir resultado
/*
Serial.println("Mediciones: ");
Serial.println(tarado1);
Serial.println(tarado2);
Serial.println(tarado3);
Serial.println(tarado4);
float finalMEAN = (tarado1+tarado2+tarado3)/3;
Serial.println(finalMEAN);
*/

delay(10);
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
  Serial.println("PESO TARADO");
  tareCELLS();
  while (STOPX == false)
  {
    Serial.println("CARGUE EL CONTENEDOR");
    delay(200);
  }
  STOPX = false;
  //Medir
  cellMEASURE();
  //Guardar
  Serial.println("Se guardo la medicion");
  Serial.println("Carga realizada.");


}

void purgaCELDA(){
  Serial.println("PURGA iniciada");
  while (STOPX == false)
  {
    Serial.println("PURGANDO EL CONTENEDOR");
    delay(200);
  }
  STOPX = false;
  //Guardar
  Serial.println("CONTENEDOR PURGADO");
  Serial.println("Purga finalizada.");
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

float calcularMediaMovil(float datosIN[10]) {
  float suma = 0;
  for (int i = 0; i < numDatos; i++) {
    suma += datosIN[i];
  }
  return suma / numDatos;
}

void tareCELLS(){
  offset1 = mean1;
  offset2 = mean2;
  offset3 = mean3;
  offset4 = mean4;
}

void cellMEASURE(){
  // Almacenar el nuevo dato en el array
  buff1[indice] = float(celda1.read());
  buff2[indice] = float(celda2.read());
  buff3[indice] = float(celda3.read());
  buff4[indice] = float(celda4.read());
  // Calcular la media móvil
  mean1 = calcularMediaMovil(buff1);
  mean2 = calcularMediaMovil(buff2);
  mean3 = calcularMediaMovil(buff3);
  mean4 = calcularMediaMovil(buff4);
  //(Mean - offset)*GAIN
  tarado1 = mean1-offset1;
  tarado2 = mean2-offset2;
  tarado3 = mean3-offset3;
  tarado4 = mean4-offset4; 

  indice = (indice + 1) % numDatos;
}


//? VERSION MODIFICADA SIN ERROR DE MEDICION!!!
//* COMUNICACION MULTICELDA DESDE EL MASTER
//MAC ADRESS DEL MASTER HOUSE: A0:B7:65:DD:9E:D4
//MAC ADRESS DEL MASTER SOPA: {0xA0, 0xB7, 0x65, 0xDD, 0x9E, 0xD4}
//MAC ADRESS DEL MASTER HACMONITOR:



#include <Preferences.h>
#include <esp_now.h>
#include <WiFi.h>
#include "HX711.h"
#define PINDRIVER1 18
#define PINDRIVER2 19
#define LEDCARGA 17
#define LEDPURGA 16
#define LEDVERTX 4


//* Variables de control
volatile bool initCARGA = false;
volatile bool initPURGA = false;
volatile bool initVERTX = false;
volatile bool printENABLE = false;
volatile bool STOPX = false;


//* Variables de estado de celda
bool ocupado = false;
bool empty      = false;

//Setting de mediciones
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN1 = 32;
const int LOADCELL_SCK_PIN1 = 33;
const int LOADCELL_DOUT_PIN2 = 25;
const int LOADCELL_SCK_PIN2 = 26;
const int LOADCELL_DOUT_PIN3 = 27;
const int LOADCELL_SCK_PIN3 = 14;

//Estos pines pueden estar midiendo mal preferible cambiarlos a 
const int LOADCELL_DOUT_PIN4 = 23; //18
const int LOADCELL_SCK_PIN4 = 13;  //19

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
float gainC1=1,gainC2=1,gainC3=1,gainC4=1;
float meanCELDA =0; //Valor medio medido en la celda
float CONTENIDO =0; //Carga que posee el contenedor
float meanSCALED =0;
float gainMEAN;
float meanOFFSET;
float meanALL =0;


int indice = 0;           // Índice actual en el array
int CANTIDADVERT = 0;

// REPLACE WITH THE MAC Address of your receiver 
//uint8_t macMASTER[] = {0xA0, 0xB7, 0x65, 0xDD, 0x9E, 0xD4}; // MASTER CASA
//uint8_t macMASTER[] = {0xC0,0x49,0xEF,0xD3,0xE9,0xBC}; // MASTER SOPA
uint8_t macMASTER[] = {0xA8,0x42,0xE3,0xAB,0x4A,0x28};

// Define the message to be sent as a string
//String messageToSend = "string1";

esp_now_peer_info_t master;
Preferences preferences;

HX711 celda1;
HX711 celda2;
HX711 celda3;
HX711 celda4;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
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

  //?FUNCIONES PRINCIPALES
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
    estadCELDA();
  }
  //? ORDENES DEL CONTROL MANUAL DEL DRIVER
  else if (ORDEN == "DRIV+"){
    driverACTIVE(true);
  }
  else if (ORDEN == "DRIV-"){
    driverACTIVE(false);
  }
  else if (ORDEN == "DRIVS"){
    driverSTOP();
  }
  //? ORDEN DE STOP
  else if (ORDEN == "STOPX"){
    STOPX = true;
    Serial.println("ORDEN DE STOP");
  }
  else if (ORDEN == "PRINT"){
    printENABLE = !(printENABLE);
    if(printENABLE){
    Serial.println("Print activado");}
    else{
    Serial.println("Print desactivado");}

  //? ORDENES PARA CALIBRACION
  }else if (ORDEN == "TAREX"){
    tareCELLS();
  }else if (ORDEN == "RESET"){
    resetCELLS();
  }
  else if (ORDEN == "CALIB"){
    Serial.println(ORDEN);
    Serial.println(CANTIDADVERT);
    calibrarCELDAS(CANTIDADVERT);
  }else if (ORDEN == "MEASX"){
    sendMEASURE();
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
  //Setup de las preferences y EEPROM
  cargarValoresDesdePreferences();




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
  pinMode(PINDRIVER1,OUTPUT);
  pinMode(PINDRIVER1,OUTPUT);
  pinMode(LEDCARGA,OUTPUT);
  pinMode(LEDVERTX,OUTPUT);
  pinMode(LEDPURGA,OUTPUT);



  digitalWrite(2,LOW);
  digitalWrite(PINDRIVER1,LOW);
  digitalWrite(PINDRIVER1,LOW);
  digitalWrite(LEDCARGA,LOW);
  digitalWrite(LEDVERTX,LOW);
  digitalWrite(LEDPURGA,LOW);
}

void loop() {
ocupado = false;
if(initCARGA){
cargaCELDA();
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
//if (printENABLE){
Serial.println("Mediciones: ");
Serial.printf("C1: %.4f\n",mean1);
Serial.printf("C2: %.4f\n",mean2);
Serial.printf("C3: %.4f\n",mean3);
Serial.printf("C4: %.4f\n",mean4);
Serial.printf("----------\n");

Serial.printf("MEANRAW: %.4f\n",meanALL);
Serial.printf("Offset:  %.4f\n",meanOFFSET);
Serial.printf("GAIN:  %.4f\n",gainMEAN);
Serial.printf("Promedio CORREX:  %.4f\n",meanSCALED);
//float finalMEAN = (tarado1+tarado2+tarado3+tarado4)/4;
//Serial.println(finalMEAN);
//sendDATOSLCD();
//}



delay(20);
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
    //Serial.println("Mensaje enviado.");
    sended = true;
  }
  else {
    Serial.printf("Retry: %d \n",retry);
    retry++;
  }
  } while ((sended == false)&&(retry < 3));
  if (sended == false){
    Serial.println("No se pudo enviar el mensaje");
  }
  delay(100);
}

void cargaCELDA(){
  ocupado = true;
  Serial.println("CARGA iniciada");
  Serial.println("PESO TARADO");
  tareCELLS();
  STOPX = false;
  while (STOPX == false)
  {
    Serial.println("CARGUE EL CONTENEDOR");
    digitalWrite(LEDCARGA,HIGH);
    delay(200);
  }
  STOPX = false;
  digitalWrite(LEDCARGA,LOW);
  //Medir
  cellMEASURE();
  CONTENIDO = meanCELDA;
  //Guardar
  Serial.println("Se guardo la medicion");
  Serial.println("Carga realizada.");
}

void purgaCELDA(){
  ocupado = true;
  Serial.println("PURGA iniciada");
  driverACTIVE(true);
  while (STOPX == false)
  {
    Serial.println("PURGANDO EL CONTENEDOR");
    digitalWrite(LEDPURGA,HIGH);
    delay(200);
  }
  STOPX = false;
  digitalWrite(LEDPURGA,LOW);
  driverSTOP();
  //Guardar
  Serial.println("CONTENEDOR PURGADO");
  Serial.println("Purga finalizada.");
}

void vertxCELDA(int cantidad){
  ocupado = true;
  Serial.println("Iniciando vertimiento");
  bool COMPLETE = false;
  //TODO   Ciclo de vertimiento, se vierte material hasta que se cumpla la cota de vertimiento (CONTENIDO TOTAL - CANTIDAD)
  //TODO   este ciclo solo funciona una vez que se ha cargado el contenedor, de lo contrario se acciona el driver por dos segundos.
  float CONTENIDO = meanSCALED;
  
  //! Funcion modificada para verter siempre y hacia arriba
  while ((STOPX == false))
  {
    driverACTIVE(true);
    Serial.println("VERTIENDO MATERIAL");
    Serial.printf("Total: %.4f \n",CONTENIDO);
    Serial.printf("Mean celda: %.4f \n",meanSCALED);
    Serial.printf("Vertido: %d \n",cantidad);
    cellMEASURE();
    //delay(20);
    if(meanSCALED > (CONTENIDO+cantidad))
    {
      //COMPLETE = true;
      STOPX = true;
    }
    /*
    else if (meanSCALED<100){
      STOPX = true;
      empty = true;
    }*/
    digitalWrite(LEDVERTX,HIGH);
    //delay(2000);
    //break;
  }
  driverSTOP();
  STOPX = false;
  //COMPLETE = false;
  digitalWrite(LEDVERTX,LOW);
  Serial.printf("Se vertieron %d g. de material\n",cantidad);
}

void estadCELDA(){
  String buffer = String("STATE") + String(int(ocupado));
  Serial.println(ocupado);
  Serial.println(buffer);
  sendSTRING(buffer,macMASTER);
}

void driveCELDA(){
  Serial.println("DRIVE iniciada");
}

float calcularMediaMovil(float datosIN[30]) {
  float suma = 0;
  for (int i = 0; i < numDatos; i++) {
    suma += datosIN[i];
  }
  return suma / numDatos;
}

void tareCELLS(){
  meanOFFSET = meanALL;
}

void resetCELLS(){
  
  meanOFFSET = 0;
  gainMEAN = 1;
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

  meanALL = (mean3);

  //(Mean - offset)*GAIN
  meanSCALED = (meanALL-meanOFFSET)*gainMEAN;
  if (meanSCALED <=0){
    meanSCALED = 0;
  }
  indice = (indice + 1) % numDatos;
}

void driverACTIVE(bool sentido){
  if(sentido){
    digitalWrite(PINDRIVER1,HIGH);
    digitalWrite(PINDRIVER2,LOW);
    Serial.println("Driver SENTIDO1");
  }else{
    digitalWrite(PINDRIVER1,LOW);
    digitalWrite(PINDRIVER2,HIGH);
    Serial.println("Driver SENTIDO2");
  }
}

void driverSTOP(){
    digitalWrite(PINDRIVER1,LOW);
    digitalWrite(PINDRIVER2,LOW);
    Serial.println("Driver DETENIDO");
}

void calibrarCELDAS(int PESO){
  float PESOF = float(PESO);
  if (CANTIDADVERT == 9999){
  Serial.printf("Reseteando ganancias a 1\n");
  gainMEAN = 1;
  }else{
  Serial.printf("Calibrando celdas con %d g.\n",CANTIDADVERT);
  if (meanALL - meanOFFSET != 0) {
    gainMEAN = PESOF / (meanALL - meanOFFSET);
  } else {
    gainMEAN = 1.0; // Si el denominador es cero, establecer gainMEAN en 1
  }
  Serial.printf("Ganancia obtenida G1:%.4f \n",gainMEAN);
  }
  guardarValoresEnPreferences();
}

void sendMEASURE(){
  String buffer = String("MEASX") + String(meanSCALED);
  sendSTRING(buffer,macMASTER);
}

//? MANEJO DE MEMORIA EEPROM 

void cargarValoresDesdePreferences() {
  preferences.begin("my-app", false);  // El segundo argumento indica si borrar las preferencias al arrancar

  // Leer los valores de gainMEAN y meanOffset desde las preferencias
  gainMEAN = preferences.getFloat("gainMEAN", 1.0);  // Si no está inicializado, se establece en 1.0
  meanOFFSET = preferences.getFloat("meanOffset", 1.0);  // Si no está inicializado, se establece en 1.0

  preferences.end();
}

void guardarValoresEnPreferences() {
  preferences.begin("my-app", false);  // El segundo argumento indica si borrar las preferencias al arrancar

  // Guardar los valores de gainMEAN y meanOffset en las preferencias
  preferences.putFloat("gainMEAN", gainMEAN);
  preferences.putFloat("meanOffset", meanOFFSET);

  preferences.end();
}
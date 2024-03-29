# 1 "c:\\Users\\bruno\\OneDrive\\Desktop\\SOPA\\sopaMENU\\funcDEFINITIVAS\\espMASTER\\espMASTER.ino"
//* COMUNICACION MULTICELDA DESDE EL MASTER
//* Control con comandos


# 6 "c:\\Users\\bruno\\OneDrive\\Desktop\\SOPA\\sopaMENU\\funcDEFINITIVAS\\espMASTER\\espMASTER.ino" 2
# 7 "c:\\Users\\bruno\\OneDrive\\Desktop\\SOPA\\sopaMENU\\funcDEFINITIVAS\\espMASTER\\espMASTER.ino" 2
# 8 "c:\\Users\\bruno\\OneDrive\\Desktop\\SOPA\\sopaMENU\\funcDEFINITIVAS\\espMASTER\\espMASTER.ino" 2

// Init del LCD y del display
int lcdColumns = 20;
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);






unsigned long buttonTIME = 0;
unsigned long lastBTIME = 0;
hw_timer_t *My_timer = 
# 21 "c:\\Users\\bruno\\OneDrive\\Desktop\\SOPA\\sopaMENU\\funcDEFINITIVAS\\espMASTER\\espMASTER.ino" 3 4
                      __null
# 21 "c:\\Users\\bruno\\OneDrive\\Desktop\\SOPA\\sopaMENU\\funcDEFINITIVAS\\espMASTER\\espMASTER.ino"
                          ;
portMUX_TYPE mux = {.owner = 0xB33FFFFF,.count = 0} /**< Spinlock initializer */;


//Constantes del menu
volatile bool UP = false;
volatile bool DOWN = false;
volatile bool SELECT = false;
volatile bool EVENT = true;
volatile bool initCARGA = false;
String substrings[4];
//INTERRUPCION BOTON UP
void __attribute__((section(".iram1" "." "28"))) BUTTONpress1(){
   buttonTIME = millis();
//Condicion para el debounce
if (buttonTIME - lastBTIME > 500)
{
   UP = true;
   DOWN = false;
   SELECT = false;
   EVENT = true;
   Serial.println("UP  presionado");

lastBTIME = buttonTIME;
 }
}

//INTERRUPCION BOTON DOWN
void __attribute__((section(".iram1" "." "29"))) BUTTONpress2(){
   buttonTIME = millis();
//Condicion para el debounce
if (buttonTIME - lastBTIME > 500)
{
   UP = false;
   DOWN = true;
   SELECT = false;
   EVENT = true;
   Serial.println("DOWN  presionado");

lastBTIME = buttonTIME;
 }
}

//INTERRUPCION BOTON SELECT
void __attribute__((section(".iram1" "." "30"))) BUTTONpress3(){
   buttonTIME = millis();
//Condicion para el debounce
if (buttonTIME - lastBTIME > 500)
{
   UP = false;
   DOWN = false;
   SELECT = true;
   EVENT = true;
   Serial.println("SELECT  presionado");

lastBTIME = buttonTIME;
 }
}





// REPLACE WITH THE MAC Address of your receiver 
//uint8_t macCELDA_A[] = {0xA0, 0xB7, 0x65, 0xDD, 0x4, 0x5C};
uint8_t macCELDA_A[] = {0x94, 0xB5, 0x55, 0xF9, 0x12, 0x68};
uint8_t macCELDA_B[] = {0xA0, 0xB7, 0x65, 0xDC, 0x15, 0xA8};
uint8_t macCELDA_C[] = {0xA1, 0xB7, 0x65, 0xDC, 0x15, 0xA8};
uint8_t macCELDA_D[] = {0xA2, 0xB7, 0x65, 0xDD, 0x4, 0x5C};
uint8_t macCELDA_E[] = {0xA3, 0xB7, 0x65, 0xDC, 0x15, 0xA8};
uint8_t macCELDA_F[] = {0xA4, 0xB7, 0x65, 0xDC, 0x15, 0xA8};
uint8_t macCELDA_G[] = {0xA4, 0xB7, 0x65, 0xDC, 0x15, 0xA8};

uint8_t macCeldas[][6] = {
  {0x94, 0xB5, 0x55, 0xF9, 0x12, 0x68},
  {0xA0, 0xB7, 0x65, 0xDC, 0x15, 0xA8},
  {0xA1, 0xB7, 0x65, 0xDC, 0x15, 0xA8},
  {0xA2, 0xB7, 0x65, 0xDD, 0x04, 0x5C},
  {0xA3, 0xB7, 0x65, 0xDC, 0x15, 0xA8},
  {0xA4, 0xB7, 0x65, 0xDC, 0x15, 0xA8},
  {0xA4, 0xB7, 0x65, 0xDC, 0x15, 0xA8}
};


// Define the message to be sent as a string
//String messageToSend = "string1";

esp_now_peer_info_t celda1;
esp_now_peer_info_t celda2;
esp_now_peer_info_t celda3;
esp_now_peer_info_t celda4;
esp_now_peer_info_t celda5;
esp_now_peer_info_t celda6;
esp_now_peer_info_t celda7;


// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\rLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  String receivedMessage = String((char*)incomingData);

  // Verifica si los primeros 5 caracteres del mensaje son "print"
  String HEADER = receivedMessage.substring(0, 5);
  String TEXTO = receivedMessage.substring(5, len);
  if (HEADER.equalsIgnoreCase("print")) {
    Serial.println(TEXTO);
    digitalWrite(2, 0x1);
    delay(500);
    digitalWrite(2, 0x0);
    delay(500);
  } else if (HEADER.equalsIgnoreCase("LCDPR")){
    lcdPRINT(TEXTO);
  } else {
    Serial.println("Mensaje recibido no válido: " + receivedMessage);
  }
}



void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  //Interrupciones
  pinMode(18, 0x05);
  attachInterrupt((((18)<40)?(18):-1), BUTTONpress1, 0x02);

  pinMode(19, 0x05);
  attachInterrupt((((19)<40)?(19):-1), BUTTONpress2, 0x02);

  pinMode(17, 0x05);
  attachInterrupt((((17)<40)?(17):-1), BUTTONpress3, 0x02);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_MODE_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0 /*!< esp_err_t value indicating success (no error) */) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  //* REGISTRAR PEERS
  memcpy(celda1.peer_addr, macCeldas[0], 6);
  celda1.channel = 1;
  celda1.encrypt = false;

  memcpy(celda2.peer_addr, macCeldas[1], 6);
  celda2.channel = 1;
  celda2.encrypt = false;

  memcpy(celda3.peer_addr, macCeldas[2], 6);
  celda3.channel = 1;
  celda3.encrypt = false;

  memcpy(celda4.peer_addr, macCeldas[3], 6);
  celda4.channel = 1;
  celda4.encrypt = false;

  memcpy(celda5.peer_addr, macCeldas[4], 6);
  celda5.channel = 1;
  celda5.encrypt = false;

  memcpy(celda6.peer_addr, macCeldas[5], 6);
  celda6.channel = 1;
  celda6.encrypt = false;

  memcpy(celda6.peer_addr, macCeldas[6], 6);
  celda7.channel = 1;
  celda7.encrypt = false;

  //* AÑADIR PEERS      
  if (esp_now_add_peer(&celda1) != 0 /*!< esp_err_t value indicating success (no error) */){
    Serial.println("Failed to add peer");
    return;
  }

  if (esp_now_add_peer(&celda2) != 0 /*!< esp_err_t value indicating success (no error) */){
    Serial.println("Failed to add peer");
    return;
  }

  if (esp_now_add_peer(&celda3) != 0 /*!< esp_err_t value indicating success (no error) */){
    Serial.println("Failed to add peer");
    return;
  }

  if (esp_now_add_peer(&celda4) != 0 /*!< esp_err_t value indicating success (no error) */){
    Serial.println("Failed to add peer");
    return;
  }

  if (esp_now_add_peer(&celda5) != 0 /*!< esp_err_t value indicating success (no error) */){
    Serial.println("Failed to add peer");
    return;
  }

  if (esp_now_add_peer(&celda6) != 0 /*!< esp_err_t value indicating success (no error) */){
    Serial.println("Failed to add peer");
    return;
  }

    if (esp_now_add_peer(&celda7) != 0 /*!< esp_err_t value indicating success (no error) */){
    Serial.println("Failed to add peer");
    return;
  }

  pinMode(2,0x03);
  digitalWrite(2,0x0);
  lcd.init();
  lcd.backlight();



}

void loop() {

  Serial.println("\nCELDA A");
  //sendSTRING("Celda A request",macCELDA_A);
  if (UP){
  //funcCARGA(macCeldas[0]);
  resetCELL(macCeldas[0]);
  delay(2000);
  UP = false;
  }
  delay(500);
  if (DOWN){
  //funcCARGA(macCELDA_A);
  //sendSTRING("ESTAD",macCeldas[0]);
  sendSTRING("TAREX",macCeldas[0]);
  delay(2000);
  DOWN = false;
  }
  delay(500);
  if (SELECT){
  calibCELL(macCeldas[0],500);
  delay(2000);
  SELECT = false;
  }
  delay(500);

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Lee la entrada hasta encontrar un salto de línea
    processCommand(command);
  }



  /*

  funcPURGA(macCELDA_A);

  delay(2000);

  funcVERTX(macCELDA_A,5000);

  delay(2000);

  sendSTRING("ESTAD",macCELDA_A);

  delay(2000);

  sendSTRING("STOPX",macCELDA_A);

  delay(2000);

  */
# 289 "c:\\Users\\bruno\\OneDrive\\Desktop\\SOPA\\sopaMENU\\funcDEFINITIVAS\\espMASTER\\espMASTER.ino"
  /*

  Serial.println("\nCELDA B");

  sendSTRING("Celda B request",macCELDA_B);

  delay(2000);

  Serial.println("\nCELDA C");

  sendSTRING("Celda C request",macCELDA1);

  delay(2000);

  Serial.println("\nCELDA D");

  sendSTRING("Celda D request",macCELDA2);

  delay(2000);

  Serial.println("\nCELDA E");

  sendSTRING("Celda E request",macCELDA1);

  delay(2000);

  Serial.println("\nCELDA F");

  sendSTRING("Celda F request",macCELDA2);

  delay(2000);

  Serial.println("\nCELDA G");

  sendSTRING("Celda G request",macCELDA2);

  delay(2000);

  */
# 309 "c:\\Users\\bruno\\OneDrive\\Desktop\\SOPA\\sopaMENU\\funcDEFINITIVAS\\espMASTER\\espMASTER.ino"
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
  if (result == 0 /*!< esp_err_t value indicating success (no error) */) {
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


void funcCARGA(uint8_t *cellADDRESS){
  // Buffer para convertir el número a cadena
  char buffer[10];
  // Concatenar "CARGA" con el número
  sprintf(buffer, "CARGA");
  sendSTRING(buffer,cellADDRESS);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Funcion de carga");
  lcd.setCursor(0,1);
  lcd.print("Carga en curso");
  lcd.setCursor(0,2);
  lcd.print("Pulsa para detener");
}

void funcPURGA(uint8_t *cellADDRESS){
  // Buffer para convertir el número a cadena
  char buffer[10];
  // Concatenar "CARGA" con el número
  sprintf(buffer, "PURGA");
  sendSTRING(buffer,cellADDRESS);
}

void funcVERTX(uint8_t *cellADDRESS,int cantidad){
  // Buffer para convertir el número a cadena
  char buffer[10];
  // Concatenar "CARGA" con el número
  sprintf(buffer, "VERTX%d",cantidad);
  Serial.println(buffer);
  sendSTRING(buffer,cellADDRESS);
}

void calibCELL(uint8_t *cellADDRESS,int cantidad){
  // Buffer para convertir el número a cadena
  char buffer[10];
  // Concatenar "CARGA" con el número
  sprintf(buffer, "CALIB%d",cantidad);
  Serial.println(buffer);
  sendSTRING(buffer,cellADDRESS);
}

void resetCELL(uint8_t *cellADDRESS){
  // Buffer para convertir el número a cadena
  char buffer[10];
  // Concatenar "CARGA" con el número
  sprintf(buffer, "RESET");
  Serial.println(buffer);
  sendSTRING(buffer,cellADDRESS);
}

void processCommand(String command) {

  separarCadena(command,substrings);
  String HEADER = substrings[0]; //command.substring(0, 3);
  String COMANDO = substrings[1];//command.substring(4, 9);
  String CELDA = substrings[2];//command.substring(10, 16);
  String CANTIDAD = substrings[3];
  int CANTIDADnum = CANTIDAD.toInt();
  //Determinar CELDAOBJETIVO
  int numeroCelda = CELDA.charAt(5) - '0';

  if (HEADER.equals("RUN")){
    //Serial.print("Correr funcion: ");
    if (COMANDO.equals("CARGA")){
      //Serial.printf(" de CARGA en la CELDA %d \n",numeroCelda);
      funcCARGA(macCeldas[numeroCelda]);
    }else if (COMANDO.equals("VERTX"))
    {
      //Serial.printf(" de VERTX en la CELDA %d \n",numeroCelda);
      funcVERTX(macCeldas[numeroCelda],CANTIDADnum);
    }else if (COMANDO.equals("PURGA"))
    {
      //Serial.printf(" de VERTX en la CELDA %d \n",numeroCelda);
      funcPURGA(macCeldas[numeroCelda]);
    }else if (COMANDO.equals("TAREX"))
    {
      //Serial.printf(" de VERTX en la CELDA %d \n",numeroCelda);
      sendSTRING("TAREX",macCeldas[numeroCelda]);
    }else if (COMANDO.equals("CALIB"))
    {
      calibCELL(macCeldas[numeroCelda],CANTIDADnum);
    }else if (COMANDO.equals("RESET"))
    {
      resetCELL(macCeldas[numeroCelda]);
    }


  }else if(HEADER.equals("SND")){
    //Serial.println("Enviar comando: ");
    if (COMANDO.equals("STOPX")){
      //Serial.printf(" de STOPX a la CELDA %d \n",numeroCelda);
      sendSTRING("STOPX",macCeldas[numeroCelda]);
    }else if(COMANDO.equals("PRINT")){
      sendSTRING("PRINT",macCeldas[numeroCelda]);
    }
  }






  Serial.println(HEADER);
  Serial.println(COMANDO);
  Serial.println(CELDA);
  Serial.println(CANTIDAD);
  /*

  if (COMANDO.equals("CARGA")) {

    funcCARGA(macCELDA_A);

    delay(2000);

  }

  */
# 450 "c:\\Users\\bruno\\OneDrive\\Desktop\\SOPA\\sopaMENU\\funcDEFINITIVAS\\espMASTER\\espMASTER.ino"
}



void separarCadena(String cadena, String* resultado) {
  int espacio1, espacio2;
  espacio1 = cadena.indexOf(' ');

  if (espacio1 != -1) {
    resultado[0] = cadena.substring(0, espacio1);
    espacio2 = cadena.indexOf(' ', espacio1 + 1);

    if (espacio2 != -1) {
      resultado[1] = cadena.substring(espacio1 + 1, espacio2);
      if((cadena.substring(espacio1 + 1, espacio2) == "VERTX")||(cadena.substring(espacio1 + 1, espacio2) == "CALIB")){
        int espacio3 = cadena.indexOf(' ', espacio2 + 1);
        resultado[2] = cadena.substring(espacio2 + 1,espacio3);
        resultado[3] = cadena.substring(espacio3 + 1);
      }else{
      int espacio3 = cadena.indexOf(' ', espacio2 + 1);
      if (espacio3 != -1){
      resultado[2] = cadena.substring(espacio2 + 1,espacio3);
      resultado[3] = "";
      }else{
      resultado[2] = cadena.substring(espacio2 + 1);
      resultado[3] = "";
      }
      }
    }
  }
}

void lcdPRINT(String data){
    lcd.clear();
    char separador = ',';
    char *str = strdup(data.c_str());
    char *token = strtok(str, &separador);
    int fila = 0;
    while (token != 
# 488 "c:\\Users\\bruno\\OneDrive\\Desktop\\SOPA\\sopaMENU\\funcDEFINITIVAS\\espMASTER\\espMASTER.ino" 3 4
                   __null 
# 488 "c:\\Users\\bruno\\OneDrive\\Desktop\\SOPA\\sopaMENU\\funcDEFINITIVAS\\espMASTER\\espMASTER.ino"
                        && fila < 4) {
        lcd.setCursor(0, fila);
        lcd.print(atoi(token));
        token = strtok(
# 491 "c:\\Users\\bruno\\OneDrive\\Desktop\\SOPA\\sopaMENU\\funcDEFINITIVAS\\espMASTER\\espMASTER.ino" 3 4
                      __null
# 491 "c:\\Users\\bruno\\OneDrive\\Desktop\\SOPA\\sopaMENU\\funcDEFINITIVAS\\espMASTER\\espMASTER.ino"
                          , &separador);
        fila++;
    }
    free(str);
}

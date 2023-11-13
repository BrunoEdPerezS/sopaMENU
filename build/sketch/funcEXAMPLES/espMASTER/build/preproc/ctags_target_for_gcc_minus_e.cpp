#line 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espMASTER\\build\\preproc\\ctags_target_for_gcc_minus_e.cpp"
# 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espMASTER\\espMASTER.ino"
//* COMUNICACION MULTICELDA DESDE EL MASTER

# 4 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espMASTER\\espMASTER.ino" 2
# 5 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espMASTER\\espMASTER.ino" 2
# 6 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espMASTER\\espMASTER.ino" 2

// Init del LCD y del display
int lcdColumns = 20;
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);






unsigned long buttonTIME = 0;
unsigned long lastBTIME = 0;
hw_timer_t *My_timer = 
# 19 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espMASTER\\espMASTER.ino" 3 4
                      __null
# 19 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espMASTER\\espMASTER.ino"
                          ;
portMUX_TYPE mux = {.owner = 0xB33FFFFF,.count = 0} /**< Spinlock initializer */;


//Constantes del menu
volatile bool UP = false;
volatile bool DOWN = false;
volatile bool SELECT = false;
volatile bool EVENT = true;

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
uint8_t macCELDA_A[] = {0xA0, 0xB7, 0x65, 0xDD, 0x4, 0x5C};
uint8_t macCELDA_B[] = {0xA0, 0xB7, 0x65, 0xDC, 0x15, 0xA8};
uint8_t macCELDA_C[] = {0xA1, 0xB7, 0x65, 0xDC, 0x15, 0xA8};
uint8_t macCELDA_D[] = {0xA2, 0xB7, 0x65, 0xDD, 0x4, 0x5C};
uint8_t macCELDA_E[] = {0xA3, 0xB7, 0x65, 0xDC, 0x15, 0xA8};
uint8_t macCELDA_F[] = {0xA4, 0xB7, 0x65, 0xDC, 0x15, 0xA8};
uint8_t macCELDA_G[] = {0xA4, 0xB7, 0x65, 0xDC, 0x15, 0xA8};




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
  Serial.println("Mensaje recibido: ");
  Serial.println(receivedMessage);
  digitalWrite(2,0x1);
  delay(500);
  digitalWrite(2,0x0);
  // Verifica si el mensaje recibido es "string2" y, si es así, imprímelo
  /*

  if (receivedMessage.equals("RESPUESTA1")) {

    Serial.println("Mensaje recibido");

  } else {

    Serial.println("Received message: " + receivedMessage);

  }

  */
# 127 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espMASTER\\espMASTER.ino"
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
  memcpy(celda1.peer_addr, macCELDA_A, 6);
  celda1.channel = 1;
  celda1.encrypt = false;

  memcpy(celda2.peer_addr, macCELDA_B, 6);
  celda2.channel = 1;
  celda2.encrypt = false;

  memcpy(celda3.peer_addr, macCELDA_C, 6);
  celda3.channel = 1;
  celda3.encrypt = false;

  memcpy(celda4.peer_addr, macCELDA_D, 6);
  celda4.channel = 1;
  celda4.encrypt = false;

  memcpy(celda5.peer_addr, macCELDA_E, 6);
  celda5.channel = 1;
  celda5.encrypt = false;

  memcpy(celda6.peer_addr, macCELDA_F, 6);
  celda6.channel = 1;
  celda6.encrypt = false;

  memcpy(celda6.peer_addr, macCELDA_G, 6);
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




}

void loop() {

  Serial.println("\nCELDA A");
  //sendSTRING("Celda A request",macCELDA_A);
  if (UP){
  funcCARGA(macCELDA_A);
  delay(2000);
  UP = false;
  }
  delay(500);





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
# 257 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espMASTER\\espMASTER.ino"
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
# 277 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espMASTER\\espMASTER.ino"
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
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Funcion de carga");
  lcd.setCursor(0,1);
  lcd.print("Carga en curso");
# 325 "C:\\Users\\bruno\\Desktop\\sopaMENU\\funcEXAMPLES\\espMASTER\\espMASTER.ino"
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
  sendSTRING(buffer,cellADDRESS);
}

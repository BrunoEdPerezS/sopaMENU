#line 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\build\\preproc\\ctags_target_for_gcc_minus_e.cpp"
# 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\autoPROBE.ino"
# 2 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\autoPROBE.ino" 2
# 3 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\autoPROBE.ino" 2


//Inicializar variables






unsigned long buttonTIME = 0;
unsigned long lastBTIME = 0;
hw_timer_t *My_timer = 
# 14 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\autoPROBE.ino" 3 4
                      __null
# 14 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\autoPROBE.ino"
                          ;
portMUX_TYPE mux = {.owner = 0xB33FFFFF,.count = 0} /**< Spinlock initializer */;
volatile bool PAIRED = false;
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



void __attribute__((section(".iram1" "." "28"))) BUTTONpress1(){
   buttonTIME = millis();
//Condicion para el debounce
if (buttonTIME - lastBTIME > 1000)
{
   PAIRING = true;
   Serial.println("Pairing pressed");

lastBTIME = buttonTIME;
}
}
# 56 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoPROBE\\autoPROBE.ino"
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
  String TEXTO = receivedMessage.substring(5, len);
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
  digitalWrite(14,0x1);
  delay(50);
  digitalWrite(14,0x0);
  delay(50);
}



void setup() {
  //Interrupciones de los botones y leds
  pinMode(17, 0x05);
  attachInterrupt(17, BUTTONpress1, 0x01);

  pinMode(14, 0x03);
  pinMode(27, 0x03);
  pinMode(2,0x03);
  digitalWrite(14,0x0);
  digitalWrite(27,0x0);
  digitalWrite(2,0x0);

  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_MODE_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0 /*!< esp_err_t value indicating success (no error) */) {
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
  if (esp_now_add_peer(&peerBROADCAST) != 0 /*!< esp_err_t value indicating success (no error) */){
    Serial.println("Failed to add peer");
    return;
  }
  pinMode(2,0x03);
  digitalWrite(2,0x0);
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

  if (result != 0 /*!< esp_err_t value indicating success (no error) */) {
    Serial.println("Error de envío.");
  }
  digitalWrite(27,0x1);
  delay(50);
  digitalWrite(27,0x0);
  delay(50);
}


void pairREQUEST() {
  String mensajeString = String("PAIRR");
  uint8_t messageLength = mensajeString.length() + 1; // Include the null terminator
  uint8_t messageBytes[messageLength];
  mensajeString.getBytes(messageBytes, messageLength);
  esp_err_t result = esp_now_send(broadcastAddress, messageBytes, messageLength);
  if (result != 0 /*!< esp_err_t value indicating success (no error) */) {
    Serial.println("Error de envío PAIRING.");
  }
  digitalWrite(27,0x1);
  delay(50);
  digitalWrite(27,0x0);
  delay(50);

}

void addPEER(){
  // Register peer
  memcpy(peerCAPTOR.peer_addr, captorMAC, 6);
  peerCAPTOR.channel = 1;
  peerCAPTOR.encrypt = false;

  // Add peer        
  if (esp_now_add_peer(&peerCAPTOR) != 0 /*!< esp_err_t value indicating success (no error) */){
    Serial.println("Failed to add peer");
    return;
  }else{
    Serial.println("Peer added");
    PAIRED = true;
    digitalWrite(2,0x1);
  }
}

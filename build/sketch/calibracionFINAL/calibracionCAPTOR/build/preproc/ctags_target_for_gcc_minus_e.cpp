#line 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\calibracionFINAL\\calibracionCAPTOR\\build\\preproc\\ctags_target_for_gcc_minus_e.cpp"
# 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoCAPTOR\\autoCAPTOR.ino"
# 2 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoCAPTOR\\autoCAPTOR.ino" 2
# 3 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\fullCALIBRATION\\autoCAPTOR\\autoCAPTOR.ino" 2






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
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\nDatos enviados: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success\n" : "Delivery Fail\n");
  digitalWrite(27, 0x1);
  delay(50);
  digitalWrite(27, 0x0);
  delay(50);
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  String receivedMessage = String((char*)incomingData);

  // Verifica si los primeros 5 caracteres del mensaje son "print"
  String HEADER = receivedMessage.substring(0, 5);
  String TEXTO = receivedMessage.substring(5, len);
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
  digitalWrite(14, 0x1);
  delay(50);
  digitalWrite(14, 0x0);
  delay(50);
}



void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  //Setup de LED TXRX
  pinMode(14, 0x03);
  pinMode(27, 0x03);
  pinMode(2,0x03);
  digitalWrite(14,0x0);
  digitalWrite(27,0x0);
  digitalWrite(2,0x0);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_MODE_STA);
  WiFi.macAddress(captorMAC);
  // Init ESP-NOW
  if (esp_now_init() != 0 /*!< esp_err_t value indicating success (no error) */) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  addPEER();
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  pinMode(2,0x03);
  digitalWrite(2,0x0);
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
  if (result != 0 /*!< esp_err_t value indicating success (no error) */) {
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

  if (result != 0 /*!< esp_err_t value indicating success (no error) */) {
    Serial.println("Error de envío.");
  }
}

void addPEER(){
  // Register peer
  memcpy(peerInfo.peer_addr, probeMAC, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;

  // Add peer        
  if (esp_now_add_peer(&peerInfo) != 0 /*!< esp_err_t value indicating success (no error) */){
    Serial.println("Failed to add peer");
    return;
  }else{
    Serial.println("Peer added");
    digitalWrite(2,0x1);
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

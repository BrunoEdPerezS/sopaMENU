#line 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\singleMEAS\\build\\preproc\\ctags_target_for_gcc_minus_e.cpp"
# 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\singleMEAS\\singleMEAS.ino"
# 2 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\singleMEAS\\singleMEAS.ino" 2
# 3 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\singleMEAS\\singleMEAS.ino" 2
# 4 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\singleMEAS\\singleMEAS.ino" 2




// HX711 circuit wiring
const int LOADCELL_DOUT_PIN1 = 32;
const int LOADCELL_SCK_PIN1 = 33;
const int LOADCELL_DOUT_PIN2 = 25;
const int LOADCELL_SCK_PIN2 = 26;
const int LOADCELL_DOUT_PIN3 = 27;
const int LOADCELL_SCK_PIN3 = 14;
const int LOADCELL_DOUT_PIN4 = 12;
const int LOADCELL_SCK_PIN4 = 13;




const long LOADCELL_OFFSET = 7135;
const long LOADCELL_DIVIDER = 207;


uint8_t remoteTXRX1[] = {0xA0, 0xB7, 0x65, 0xDD, 0x4, 0x5C};

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\nDatos enviados: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success\n" : "Delivery Fail\n");
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  String receivedMessage = String((char*)incomingData);
  // Verifica si el mensaje recibido es "string2" y, si es así, imprímelo
  Serial.println("Mensaje recibido.");
  if (receivedMessage.equals("Print")) {
    Serial.println("Se ha recibido: " + receivedMessage);
  }
  digitalWrite(2,0x1);
  delay(500);
  digitalWrite(2,0x0);
  delay(500);
}




HX711 celda1;
HX711 celda2;
HX711 celda3;
HX711 celda4;


void setup() {
  Serial.begin(115200);
   WiFi.mode(WIFI_MODE_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0 /*!< esp_err_t value indicating success (no error) */) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  memcpy(peerInfo.peer_addr, remoteTXRX1, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;

  // Add peer        
  if (esp_now_add_peer(&peerInfo) != 0 /*!< esp_err_t value indicating success (no error) */){
    Serial.println("Failed to add peer");
    return;
  }

  //Celdas de carga
  Serial.printf("\n\nTaring celda1\n");
  celda1.begin(LOADCELL_DOUT_PIN1, LOADCELL_SCK_PIN1, 64); // Set gain to 64, uses A- and A+ on the HX711 Board
  celda2.begin(LOADCELL_DOUT_PIN2, LOADCELL_SCK_PIN2, 64);
  celda3.begin(LOADCELL_DOUT_PIN3, LOADCELL_SCK_PIN3, 64);
  celda4.begin(LOADCELL_DOUT_PIN4, LOADCELL_SCK_PIN4, 64);

  //celda1.set_gain(32);
  celda1.tare(10); // Set ZERO using an averaged 10 readings
  celda1.set_scale(LOADCELL_DIVIDER); // Reads out in grams, approximately (for my test setup)
  celda2.tare(10); // Set ZERO using an averaged 10 readings
  celda2.set_scale(LOADCELL_DIVIDER); // Reads out in grams, approximately (for my test setup)
  celda3.tare(10); // Set ZERO using an averaged 10 readings
  celda3.set_scale(LOADCELL_DIVIDER); // Reads out in grams, approximately (for my test setup)
  celda4.tare(10); // Set ZERO using an averaged 10 readings
  celda4.set_scale(LOADCELL_DIVIDER); // Reads out in grams, approximately (for my test setup)


  pinMode(2,0x03);
  digitalWrite(2,0x0);
}

// CALCULATING celda1:  ( celda1.read() - celda1.offset ) / KnownWeight
// Example:              ( 34811 - (-7209) ) / 203g = (  34811 + 7209  )  / 203g =  206
// celda1.set_celda1(206);

void loop() {


    String myString1 = String("Celda1") + String("Raw:") + String(celda1.read()) + String(" Offset:") + String(celda1.get_offset()) + String(" celda1:") + String(celda1.get_scale()) + String(" Corrected:") + String(celda1.get_units(5));
    String myString2 = String("Celda2") + String("Raw:") + String(celda2.read()) + String(" Offset:") + String(celda2.get_offset()) + String(" celda1:") + String(celda2.get_scale()) + String(" Corrected:") + String(celda2.get_units(5));
    String myString3 = String("Celda3") + String("Raw:") + String(celda3.read()) + String(" Offset:") + String(celda3.get_offset()) + String(" celda1:") + String(celda3.get_scale()) + String(" Corrected:") + String(celda3.get_units(5));
    String myString4 = String("Celda4") + String("Raw:") + String(celda4.read()) + String(" Offset:") + String(celda4.get_offset()) + String(" celda1:") + String(celda4.get_scale()) + String(" Corrected:") + String(celda4.get_units(5));

    Serial.println("MEDICIONES: ");
    Serial.println(myString1);
    Serial.println(myString2);
    Serial.println(myString3);
    Serial.println(myString4);

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
# 131 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\singleMEAS\\singleMEAS.ino"
    delay(1000);

}




void sendSTRING(String messageToSend){
  // Send message via ESP-NOW
  uint8_t messageLength = messageToSend.length() + 1; // Include the null terminator
  uint8_t messageBytes[messageLength];
  messageToSend.getBytes(messageBytes, messageLength);
  esp_err_t result = esp_now_send(remoteTXRX1, messageBytes, messageLength);

  if (result == 0 /*!< esp_err_t value indicating success (no error) */) {
    Serial.println("Mensaje enviado.");
  }
  else {
    Serial.println("Error de envío.");
  }

}

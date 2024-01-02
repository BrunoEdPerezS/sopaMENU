# 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\calibracionCELDAS\\calibracionCELDAS.ino"
# 2 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\calibracionCELDAS\\calibracionCELDAS.ino" 2

Preferences preferences;

void setup() {
  Serial.begin(115200);

  preferences.begin("my-app", false); // El segundo argumento indica si borrar las preferencias al arrancar

  // Restablecer las preferencias (borrar la "EEPROM")
  preferences.clear();

  Serial.println("Memoria flash (EEPROM) restablecida");

  preferences.end();
}

void loop() {
  // Tu código aquí
}

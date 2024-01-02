#include <Arduino.h>
#line 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\calibracionCELDAS\\calibracionCELDAS.ino"
#include <Preferences.h>

Preferences preferences;

#line 5 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\calibracionCELDAS\\calibracionCELDAS.ino"
void setup();
#line 18 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\calibracionCELDAS\\calibracionCELDAS.ino"
void loop();
#line 5 "C:\\Users\\bruno\\Desktop\\sopaMENU\\medicionesEX\\calibracionCELDAS\\calibracionCELDAS.ino"
void setup() {
  Serial.begin(115200);

  preferences.begin("my-app", false);  // El segundo argumento indica si borrar las preferencias al arrancar

  // Restablecer las preferencias (borrar la "EEPROM")
  preferences.clear();

  Serial.println("Memoria flash (EEPROM) restablecida");

  preferences.end();
}

void loop() {
  // Tu código aquí
}

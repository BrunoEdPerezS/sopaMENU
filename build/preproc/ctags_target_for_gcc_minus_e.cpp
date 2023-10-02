# 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
# 2 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino" 2
# 3 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino" 2



# 5 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
//! Estructura base para implementar una vista de scroll------------
  /*

  *Para activar el scrolling se usa la funcion:

  scrolling(menu0F);



  *Para generar la vista se utilizan la funcion generarVISTA y scrollSIGN

  *generar vista se encarga de generar la vista del menu y scrollSIGN de implementar

  *el signo de scroll y seleccion:

  generarVISTA(menu0,menu0F);

  scrollSIGN(menu0F);

  */
# 17 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
//TODO     OJITO CON ESTOO!!!
 /*

 * Cuidaito con las vistas, avece los lcd.clear() se pifean y la pantalla no actualiza correctamente

 * la solucion mas simple es borrar el clear de la funcion de cambio de vista y aliminar la condicion del clear en el generador de vista

*/
//Inicializar botones y variables globales




unsigned long buttonTIME = 0;
unsigned long lastBTIME = 0;
hw_timer_t *My_timer = 
# 32 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino" 3 4
                      __null
# 32 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
                          ;
portMUX_TYPE mux = {.owner = 0xB33FFFFF,.count = 0} /**< Spinlock initializer */;

volatile bool UP = false;
volatile bool DOWN = false;
volatile bool SELECT = false;
volatile bool EVENT = true;
int scrollSTATE = 0;
int scrollOFFSET = 0;

int numeroVISTA = 0;



//INTERRUPCION BOTON UP
void __attribute__((section(".iram1" "." "28"))) BUTTONpress1(){
   buttonTIME = millis();
//Condicion para el debounce
if (buttonTIME - lastBTIME > 250)
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
if (buttonTIME - lastBTIME > 250)
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
if (buttonTIME - lastBTIME > 250)
{
   UP = false;
   DOWN = false;
   SELECT = true;
   EVENT = true;
   Serial.println("SELECT  presionado");

lastBTIME = buttonTIME;
 }
}
# 115 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
// Init del LCD y del display
int lcdColumns = 20;
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

//Crear arrays con las vistas
int menu0F = 12;
char menu0[12][20];

int menu1F = 4;
char menu1[4][20];

int menu2F = 4;
char menu2[4][20];

int menu2AF = 6;
char menu2A[6][20];

char displaySHOW[4][20];

void setup(){
Serial.begin(115200);
//Interrupciones
pinMode(18, 0x09);
attachInterrupt(18, BUTTONpress1, 0x02);

pinMode(19, 0x09);
attachInterrupt(19, BUTTONpress2, 0x02);

pinMode(17, 0x09);
attachInterrupt(17, BUTTONpress3, 0x02);



// Init de las vistas
// Asignar valores a los elementos del array
  strcpy(menu0[0], "M0 - Menu 1");
  strcpy(menu0[1], "M0 - Menu 2");
  strcpy(menu0[2], "M0 - 3");
  strcpy(menu0[3], "M0 - 4");
  strcpy(menu0[4], "M0 - 5");
  strcpy(menu0[5], "M0 - 6");
  strcpy(menu0[6], "M0 - 7");
  strcpy(menu0[7], "M0 - 8");
  strcpy(menu0[8], "M0 - 9");
  strcpy(menu0[9], "M0 - 10");
  strcpy(menu0[10], "M0 - 11");
  strcpy(menu0[11], "M0 - 12");

  strcpy(menu1[0], "M1 - 1");
  strcpy(menu1[1], "M1 - 2");
  strcpy(menu1[2], "M1 - 3");
  strcpy(menu1[3], "M1 - Volver");

  strcpy(menu2[0], "M2 - 1");
  strcpy(menu2[1], "M2 - 2");
  strcpy(menu2[2], "M2 - Menu2A");
  strcpy(menu2[3], "M2 - Volver");

  strcpy(menu2A[0], "M2A - 1");
  strcpy(menu2A[1], "M2A - 2");
  strcpy(menu2A[2], "M2A - 3");
  strcpy(menu2A[3], "M2A - 4");
  strcpy(menu2A[4], "M2A - 5");
  strcpy(menu2A[5], "M2A - Volver");





  //Setup del lcd
  lcd.init();
  lcd.backlight();
}




void loop(){

  switch (numeroVISTA)
  {
  case 0:
   scrolling(menu0F);
   generarVISTA(menu0,menu0F);
   scrollSIGN(menu0F);
   //Cambio de vista 
   cambioVISTA(0,1);
   //Cambio de vista
   cambioVISTA(1,2);
   break;

  case 1:
   scrolling(menu1F);
   generarVISTA(menu1,menu1F);
   scrollSIGN(menu1F);
   //Cambio de vista
   cambioVISTA(3,0);
   break;

  case 2:
   scrolling(menu2F);
   generarVISTA(menu2,menu2F);
   scrollSIGN(menu2F);
   //Cambio de vista
   cambioVISTA(3,0);
   //Cambio de vista
   cambioVISTA(2,3);
   break;

  case 3:
   scrolling(menu2AF);
   generarVISTA(menu2A,menu2AF);
   scrollSIGN(menu2AF);
   cambioVISTA(5,2);
   break;

  default:
   Serial.print("Problemas con las vistas...");
   break;
  }



/*

  scrolling(menu0F);

  generarVISTA(menu0,menu0F);

  scrollSIGN(menu0F);

*/
# 245 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
  delay(200);
}



//? Funciones de uso general 

/* Asigna una ventana de 4 mensajes a la matriz de vista del menu */
void asignarVISTA(char entrada[][20], char salida[4][20], int scroll, int filas) {

   int wdwSTART = 0;
   int endINDEX = (filas-1);
   if ((scroll+3) >= (endINDEX)){
      wdwSTART = endINDEX-3;
   }
   else
   {
      wdwSTART = scroll;
   }

   int i=0;
   for (int fila = wdwSTART; fila <= wdwSTART+3; fila++) {
       strcpy(salida[i++], entrada[fila]);
   }
}

/*Esta funcion desplaza la variable de scroll a lo largo de las opciones*/
void scrolling(int filas){

   int endINDEX = (filas-1);
   if (UP){
   scrollSTATE = scrollSTATE+1;
   if(scrollSTATE >= endINDEX){
      scrollSTATE = endINDEX;
   }
  UP = false;
  }
  if (DOWN){
   scrollSTATE = scrollSTATE-1;
   if(scrollSTATE <= 0){
      scrollSTATE = 0;
   }
  DOWN = false;
  }
}

/*Esta funcion se encarga de inmplementar el movimiento del scroll sign*/
void scrollSIGN(int filas){

   int endINDEX = (filas-1);
   if ((scrollSTATE+3) >= endINDEX){
    scrollOFFSET = (3-(endINDEX-scrollSTATE));
    if (scrollOFFSET > 3){
       scrollOFFSET = 3;
   }
    Serial.print("ScrollSIGN: ");
    Serial.println(scrollOFFSET);
    lcd.setCursor(19, scrollOFFSET);
    lcd.print("<");
   }
   else{
    lcd.setCursor(19, 0);
    lcd.print("<");
   }
}

/*Esta funcion se encarga de crear la vista en el LCD*/
void generarVISTA(char menu[][20],int opciones) {

   asignarVISTA(menu, displaySHOW, scrollSTATE, opciones);
   Serial.print("Vista cambiada a:");
   Serial.println(opciones);
   if (EVENT){
   lcd.clear();
   EVENT = false;
   }

   for (int i = 0; i <= 3; i++) {
      lcd.setCursor(0, i);
      lcd.print(displaySHOW[i]);
   }



}

/*Esta funcion realiza el cambio de vista, para ello se le entrega el estado de "Scroll" en el que realizara, y la vista hacia la que 

se deriva dicho cambio*/
# 333 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
void cambioVISTA(int opcion, int destino){

   if ((scrollSTATE == opcion)&&(SELECT)){
      SELECT = false;
      scrollSTATE=0;
      numeroVISTA = destino;
      lcd.clear();
   }
}

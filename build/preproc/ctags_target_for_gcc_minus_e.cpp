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
//! Estructura base para implementar una vista de SETTING------------
  /*

  *Todas las vistas de setting se componene de 4 lineas

    * EXPLICACION DE VARIABLE(que valor setea)

    * VALOR (el numero)

    * CONFIRMAR (lo guarda en memoria)

    * VOLVER (retorna a la vista anterior)

  



  *Para generar la vista se utilizan la funcion settingMENU()y cambioSETTING(), la primer

  *se encarga de generar la vista y del setting menu, la segunda crea la instancia del "setting", implementando una nueva

  *rama en el arbol de vistas, la cual hace variar el valor que se ve en pantalla.

   settingMENU(menu0);

   cambioSETTING(5); (el parámetro de esta funcion debe ser la vista a la cual responde)

  */
# 33 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
//! Estructura base para implementar una notificacion (como un cuadro de dialogo)------------
/*

  *Basicament un mensaje que se muestra a modo de notificacion, cualquier evento desencadena el "goBACK"



*/
//TODO     OJITO CON ESTOO!!!
 /*

 * Cuidaito con las vistas, aveces los lcd.clear() se pifean y la pantalla no actualiza correctamente

 * la solucion mas simple es borrar el clear de la funcion de cambio de vista y aliminar la condicion del clear en el generador de vista

 * en este caso la pantalla se actualizara constantemente de acuerdo al Ts del main loop.

 * 

 * OJO CON LOS CAMBIOS EN LAS VARIABLES DE SETTING, FALTA PROGRAMAR LAS ADICIONES A MEMORIA EPROM

 * 

 * FALTA PROGRAMAR EL MODELO PARA LAS FUNCIONES DE CARGA Y DESCARGA Y PURGA

 * 

 * FALTA PROGRAMAR EL MONITOREO DE ESTADO

*/
# 56 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
//Inicializar botones y variables globales




unsigned long buttonTIME = 0;
unsigned long lastBTIME = 0;
hw_timer_t *My_timer = 
# 63 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino" 3 4
                      __null
# 63 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
                          ;
portMUX_TYPE mux = {.owner = 0xB33FFFFF,.count = 0} /**< Spinlock initializer */;

volatile bool UP = false;
volatile bool DOWN = false;
volatile bool SELECT = false;
volatile bool EVENT = true;
int scrollSTATE = 0;
int scrollOFFSET = 0;

int numeroVISTA = 0;
int numeroVISTAant = 0;

char VISTA[12] = "MAIN";


int valuetoSET = 0;
int settingLOOP = false;



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
# 153 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
// Init del LCD y del display
int lcdColumns = 20;
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);


//? Declarar las vistas
//Display de 4 filas 20 caracteres
char displaySHOW[4][20];




//Crear arrays con las vistas 
int menuMAIN_F = 12; //Declarar opciones (filas del menu)
char menuMAIN[12][20]; //Declarar la matriz de la vista

int menuA0_F = 5;
char menuA0[5][20]; //Declarar la matriz de la vista

int menuB0_F = 7;
char menuB0[7][20]; //Declarar la matriz de la vista

//Crear menu de setting 
char menuA0A0[4][20];

//NOTIFICACION
int menuA0A0Conf_F = 4;
char menuA0A0Conf[4][20];

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

//MAIN MENU
  strcpy(menuMAIN[0], "M0 - Menu 1");
  strcpy(menuMAIN[1], "M0 - Menu 2");
  strcpy(menuMAIN[2], "M0 - Menu 3");
  strcpy(menuMAIN[3], "M0 - 4");
  strcpy(menuMAIN[4], "M0 - 5");
  strcpy(menuMAIN[5], "M0 - 6");
  strcpy(menuMAIN[6], "M0 - 7");
  strcpy(menuMAIN[7], "M0 - 8");
  strcpy(menuMAIN[8], "M0 - 9");
  strcpy(menuMAIN[9], "M0 - 10");
  strcpy(menuMAIN[10], "M0 - 11");
  strcpy(menuMAIN[11], "M0 - 12");

//Menu A0
  strcpy(menuA0[0], "MA0 - A0A0");
  strcpy(menuA0[1], "MA0 - OPT 2");
  strcpy(menuA0[2], "MA0 - OPT 3");
  strcpy(menuA0[3], "MA0 - OPT 4");
  strcpy(menuA0[4], "MA0 - Volver");

//Menu A0A0 (SETTING MENU)
  strcpy(menuA0A0[0], "SETEE VALOR");
  strcpy(menuA0A0[1], "");
  strcpy(menuA0A0[2], "Confirmar");
  strcpy(menuA0A0[3], "Volver");

//Menu B0
  strcpy(menuB0[0], "MB0 - A0A0");
  strcpy(menuB0[1], "MB0 - OPT 2");
  strcpy(menuB0[2], "MB0 - OPT 3");
  strcpy(menuB0[3], "MB0 - OPT 4");
  strcpy(menuB0[4], "MB0 - OPT 5");
  strcpy(menuB0[5], "MB0 - OPT 6");
  strcpy(menuB0[6], "MB0 - Volver");



//NOTIFICACION
  strcpy(menuA0A0Conf[0], "Valor guardado");
  strcpy(menuA0A0Conf[1], "correctamente!");
  strcpy(menuA0A0Conf[2], "");
  strcpy(menuA0A0Conf[3], "");



  //Setup del lcd
  lcd.init();
  lcd.backlight();
}




void loop(){

if (strcmp(VISTA, "MAIN") == 0) {
   //!VISTA
   scrolling(menuMAIN_F);
   generarVISTA(menuMAIN,menuMAIN_F);
   scrollSIGN(menuMAIN_F);
   //!MODELO



   //Cambio de vista 
   cambioVISTA(0,"A0");
   cambioVISTA(1,"B0");
}
else if (strcmp(VISTA, "A0") == 0) {
   //!VISTA
   scrolling(menuA0_F);
   generarVISTA(menuA0,menuA0_F);
   scrollSIGN(menuA0_F);
   //!MODELO


   //Cambio de vista 
   cambioVISTA(0,"A0A0");
   //go BACK
   cambioVISTA(4,"MAIN");
}
else if (strcmp(VISTA, "B0") == 0) {
   //!VISTA
   scrolling(menuB0_F);
   generarVISTA(menuB0,menuB0_F);
   scrollSIGN(menuB0_F);
   //!MODELO

   //go BACK
   cambioVISTA(6,"MAIN");
}



else if (strcmp(VISTA, "A0A0") == 0) {
   //! VISTA
   settingMENU(menuA0A0);
   //Cambio de vista al setting loop (indicar CASE del setting LOOP)


   //!MODELO
   cambioSETTING("A0A0-S");
   //Cambio de vista confirmacion
   cambioVISTA(2,"A0A0-S+");
   //Cambio de vista al menu anterior
   cambioVISTA(3,"A0");

}
else if (strcmp(VISTA, "A0A0-S") == 0){
   //!VISTA + MODELO
   setterFUNC(10,100,1000);
   settingBACK("A0A0");
}
else if (strcmp(VISTA, "A0A0-S+") == 0) {
   //!VISTA
   //scrolling(menuA0A0Conf_F);
   generarVISTA(menuA0A0Conf,menuA0A0Conf_F);
   //scrollSIGN(menuA0A0Conf_F);

   //!MODELO
   //Cambio de vista 
   cambioEVENTO("A0");
}
else {
  Serial.print("Problemas con las vistas...");
}

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
# 416 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
void cambioVISTA(int opcion, char destino[12] ){

   if ((scrollSTATE == opcion)&&(SELECT)){
      SELECT = false;
      scrollSTATE=0;
      valuetoSET = 0;
      strcpy(VISTA, destino);
      lcd.clear();
   }
}



//? Funciones para los setting menu
/*Inicializa un setting menú de forma estandar*/
void settingMENU(char menu[4][20]){
   scrollinSETMENU();
   generarVISTA(menu,4);
   //Display de la variable del set
   lcd.setCursor(0, 1);
   lcd.print(valuetoSET);
   scrollSIGN(4);
}

/*Deriva a una vista de setting*/
void cambioSETTING(char destino[12]){
   if ((scrollSTATE == 1)&&(SELECT)){
      SELECT = false;
      scrollSTATE=0;
      strcpy(VISTA, destino);
   }
}

/*Activa la vista del setting mode (el step modifica cuanto añadimos con cada UP-DOWN)*/
void setterFUNC(int steps, int setSTEP, int limit){

   lcd.setCursor(19, 1);
   lcd.print("-");
   lcd.setCursor(0, 1);
   lcd.print("      ");


   int endINDEX = (steps);
   if (UP){
   scrollSTATE = scrollSTATE+1;
   if(scrollSTATE >= endINDEX){
      scrollSTATE = endINDEX;
   }
   UP = false;
   valuetoSET = valuetoSET + setSTEP;
   }
   if (DOWN){
   scrollSTATE = scrollSTATE-1;
   if(scrollSTATE <= 0){
      scrollSTATE = 0;
   }
   DOWN = false;
   valuetoSET = valuetoSET - setSTEP;
   }
   if ((valuetoSET > limit)||(valuetoSET<0)){
   valuetoSET = 0;
   }
   lcd.setCursor(0, 1);
   lcd.print(valuetoSET);
}

/*Devuelve a la vista originaL*/
void settingBACK(char destino[12]){
   if (SELECT){
      SELECT = false;
      scrollSTATE=0;
      strcpy(VISTA, destino);
      lcd.clear();
   }
}

/*Permite scrollear de forma cómoda en el setting menu*/
void scrollinSETMENU(){

   int endINDEX = 3;
   if (UP){
   scrollSTATE = scrollSTATE+1;
   if(scrollSTATE >= endINDEX){
      scrollSTATE = endINDEX;
   }
   UP = false;
   }
   if (DOWN){
   scrollSTATE = scrollSTATE-1;
   DOWN = false;
   }
  if(scrollSTATE <= 1){
      scrollSTATE = 1;
   }
}

//? Cambio de vista triggeado por evento
void cambioEVENTO(char destino[12]){
   if (UP||DOWN||SELECT){
      EVENT = false;
      UP = false;
      DOWN = false;
      SELECT = false;
      scrollSTATE=0;
      strcpy(VISTA, destino);
      lcd.clear();
   }
}

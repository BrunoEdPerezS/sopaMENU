# 1 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
# 2 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino" 2
# 3 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino" 2
# 4 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino" 2


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
# 18 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
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
# 34 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
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
# 55 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
//Inicializar botones y variables globales




unsigned long buttonTIME = 0;
unsigned long lastBTIME = 0;
hw_timer_t *My_timer = 
# 62 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino" 3 4
                      __null
# 62 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
                          ;
portMUX_TYPE mux = {.owner = 0xB33FFFFF,.count = 0} /**< Spinlock initializer */;


//Constantes del menu
volatile bool UP = false;
volatile bool DOWN = false;
volatile bool SELECT = false;
volatile bool EVENT = true;
int scrollSTATE = 0;
int scrollOFFSET = 0;
char VISTA[12] = "MAIN";
int valuetoSET = 0;
int settingLOOP = false;

//Constante condicionales de la operacion
volatile bool contenedoresCARGADOS = true;
int receta_seleccionada = 1;


int addREC_slot = 1;
bool slotDISPONIBLE = false;
int addREC_ingSELECT;
int addRECparam [9] = {0,0,0,0,0,0,0,99999,99999};
int addRECshowparam[4];
int addRECcantidad = 0;

//La primera fila del array es inaccesible para el programa ya que esta reservada para edicion
int RECETA[11][9] = {
  {1, 0, 0, 0, 0, 0, 0, 99999, 99999},
  {1, 0, 0, 0, 0, 0, 0, 99999, 99999},
  {2, 0, 0, 0, 0, 0, 0, 99999, 99999},
  {3, 0, 0, 0, 0, 0, 0, 99999, 99999},
  {4, 0, 0, 0, 0, 0, 0, 99999, 99999},
  {5, 0, 0, 0, 0, 0, 0, 99999, 99999},
  {6, 0, 0, 0, 0, 0, 0, 99999, 99999},
  {7, 0, 0, 0, 0, 0, 0, 99999, 99999},
  {8, 0, 0, 0, 0, 0, 0, 99999, 99999},
  {9, 0, 0, 0, 0, 0, 0, 99999, 99999},
  {10, 0, 0, 0, 0, 0, 0, 99999, 99999}
};


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


// Init del LCD y del display
int lcdColumns = 20;
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);


//? Declarar las vistas
//Display de 4 filas 20 caracteres
char displaySHOW[4][20];

//! Crear arrays con las vistas 

//? Main MENU
char mainMENU[6][20]; //Declarar la matriz de la vista
int mainMENU_F = 6; //Declarar opciones (filas del menu)

//? Menus rama A
//* scroll A0
char A0scroll[4][20]; //Declarar la matriz de la vista
int A0scroll_f = 4; //Declarar opciones (filas del menu)

//* scroll A0A0
char A0A0scroll[4][20]; //Declarar la matriz de la vista
int A0A0scroll_f = 4; //Declarar opciones (filas del menu)

//* setting A0B0
char A0B0sett[4][20]; //Declarar la matriz de la vista

//* notif A0B1
char A0B1notif[4][20]; //Declarar la matriz de la vista
int A0B1notif_f = 4; //Declarar opciones (filas del menu)

//* scroll A0C0 (ELIMINABLE)
char A0C0scroll[4][20]; //Declarar la matriz de la vista
int A0C0scroll_f = 4; //Declarar opciones (filas del menu)

//* scroll A0C1 (ELIMINABLE)
char A0C1scroll[4][20]; //Declarar la matriz de la vista
int A0C1scroll_f = 4; //Declarar opciones (filas del menu)

//? Menus de la rama B
//* scroll B0a
char B0ascroll[4][20]; //Declarar la matriz de la vista
int B0ascroll_f = 4; //Declarar opciones (filas del menu)

//* notif B0b
char B0bnotif[4][20]; //Declarar la matriz de la vista
int B0bnotif_f = 4; //Declarar opciones (filas del menu)

//* setting B1
char B1scroll[4][20]; //Declarar la matriz de la vista
int B1scroll_f = 4; //Declarar opciones (filas del menu)

//* scroll B2
char B2scroll[4][20]; //Declarar la matriz de la vista
int B2scroll_f = 4; //Declarar opciones (filas del menu)

//* scroll B3
char B3scroll[4][20]; //Declarar la matriz de la vista
int B3scroll_f = 4; //Declarar opciones (filas del menu)

//? Menus de la rama C
//* scroll C0
char C0scroll[4][20]; //Declarar la matriz de la vista
int C0scroll_f = 4; //Declarar opciones (filas del menu)

//* sett C0A0
char C0A0sett[4][20]; //Declarar la matriz de la vista

//* scroll C0A1X
char C0A1Xscroll[4][20]; //Declarar la matriz de la vista
int C0A1Xscroll_f = 4; //Declarar opciones (filas del menu)

//* scroll C0A1
char C0A1scroll[9][20]; //Declarar la matriz de la vista
int C0A1scroll_f = 9; //Declarar opciones (filas del menu)

//* scroll C0A2I
char C0A2Isett[4][20]; //Declarar la matriz de la vista

//* scroll C0A2T
char C0A2Tsett[4][20]; //Declarar la matriz de la vista

//* notif C0A3
char C0A3notif[4][20]; //Declarar la matriz de la vista
int C0A3notif_f = 4; //Declarar opciones (filas del menu)

//* sett C0B0
char C0B0sett[4][20]; //Declarar la matriz de la vista
int C0B0sett_f = 4; //Declarar opciones (filas del menu)

//* notif C0B2
char C0B1notif[4][20]; //Declarar la matriz de la vista
int C0B1notif_f = 4; //Declarar opciones (filas del menu)
/*

int menuA0_F = 5;

char menuA0[5][20]; //Declarar la matriz de la vista



int menuB0_F = 7;

char menuB0[7][20]; //Declarar la matriz de la vista



//Crear menu de setting 

char menuA0A0[4][20]; 



//NOTIFICACIONES

int menuA0A0Conf_F = 4;

char menuA0A0Conf[4][20]; 

*/
# 264 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
void setup(){
Serial.begin(115200);
//Interrupciones
pinMode(18, 0x05);
attachInterrupt((((18)<40)?(18):-1), BUTTONpress1, 0x02);

pinMode(19, 0x05);
attachInterrupt((((19)<40)?(19):-1), BUTTONpress2, 0x02);

pinMode(17, 0x05);
attachInterrupt((((17)<40)?(17):-1), BUTTONpress3, 0x02);



// Init de las vistas
// Asignar valores a los elementos del array

//? MAIN MENU
  strcpy(mainMENU[0], "Preparar");
  strcpy(mainMENU[1], "Iniciar");
  strcpy(mainMENU[2], "Recetas");
  strcpy(mainMENU[3], "Control");
  strcpy(mainMENU[4], "Calibración");
  strcpy(mainMENU[5], "About");

//? Menus de la rama A

  //* A0 
  strcpy(A0scroll[0], "Cargar conts.");
  strcpy(A0scroll[1], "Sel. receta");
  strcpy(A0scroll[2], "Purgar conts.");
  strcpy(A0scroll[3], "Volver");
  //* A0A0
  strcpy(A0A0scroll[0], "Carga - 1");
  strcpy(A0A0scroll[1], "Carga - 2");
  strcpy(A0A0scroll[2], "Carga - 3");
  strcpy(A0A0scroll[3], "Volver");
  //* A0B0
  strcpy(A0B0sett[0], "Seleccione receta");
  strcpy(A0B0sett[1], "  /20");
  strcpy(A0B0sett[2], "Confirmar");
  strcpy(A0B0sett[3], "Volver");
  //* A0B1
  strcpy(A0B1notif[0], "Receta guardada");
  strcpy(A0B1notif[1], "correctamente.");
  strcpy(A0B1notif[2], "");
  strcpy(A0B1notif[3], "Volver");
  //* A0C0 (ELIMINABLE)
  strcpy(A0C0scroll[0], "Confirme proceso de purga");
  strcpy(A0C0scroll[1], "");
  strcpy(A0C0scroll[2], "Confirmar");
  strcpy(A0C0scroll[3], "Volver");
  //* A0C1 (ELIMINABLE)
  strcpy(A0C1scroll[0], "PURGA-1");
  strcpy(A0C1scroll[1], "PURGA-1");
  strcpy(A0C1scroll[2], "PURGA-2");
  strcpy(A0C1scroll[3], "Volver");

//? Menus de la rama B
  //* B0a
  strcpy(B0ascroll[0], "Confirme proceso");
  strcpy(B0ascroll[1], "de dispensado.");
  strcpy(B0ascroll[2], "Confirmar");
  strcpy(B0ascroll[3], "Volver");
  //* B0b
  strcpy(B0bnotif[0], "Hay una carga de");
  strcpy(B0bnotif[1], "contenedores");
  strcpy(B0bnotif[2], "pendiente.");
  strcpy(B0bnotif[3], "Volver");
  //* B1
  strcpy(B1scroll[0], "Preparar receta:");
  strcpy(B1scroll[1], "");
  strcpy(B1scroll[2], "Confirmar");
  strcpy(B1scroll[3], "Volver");
  //* B2
  strcpy(B2scroll[0], "MEZCLA-1");
  strcpy(B2scroll[1], "MEZCLA-2");
  strcpy(B2scroll[2], "Confirmar");
  strcpy(B2scroll[3], "Volver");
  //* B3
  strcpy(B3scroll[0], "DISPENSADO-1");
  strcpy(B3scroll[1], "DISPENSADO-2");
  strcpy(B3scroll[2], "Confirmar");
  strcpy(B3scroll[3], "Volver");

//? Menus de la rama C
  //* C0 
  strcpy(C0scroll[0], "Editor de recetas");
  strcpy(C0scroll[1], "Agregar/Editar");
  strcpy(C0scroll[2], "Eliminar");
  strcpy(C0scroll[3], "Volver");
  //* C0A0
  strcpy(C0A0sett[0], "Seleccione receta");
  strcpy(C0A0sett[1], "  /10");
  strcpy(C0A0sett[2], "Confirmar");
  strcpy(C0A0sett[3], "Volver");
  //* C0A1X 
  strcpy(C0A1Xscroll[0], "Espacio ocupado");
  strcpy(C0A1Xscroll[1], "desea editarlo?");
  strcpy(C0A1Xscroll[2], "Confirmar");
  strcpy(C0A1Xscroll[3], "Volver");
  //* C0A1
  strcpy(C0A1scroll[0], "Ing 1  -      g");
  strcpy(C0A1scroll[1], "Ing 2  -      g");
  strcpy(C0A1scroll[2], "Ing 3  -      g");
  strcpy(C0A1scroll[3], "Ing 4  -      g");
  strcpy(C0A1scroll[4], "Ing 5  -      g");
  strcpy(C0A1scroll[5], "Ing 6  -      g");
  strcpy(C0A1scroll[6], "Tiempo -      seg");
  strcpy(C0A1scroll[7], "Confirmar");
  strcpy(C0A1scroll[8], "Cancelar");
  //* C0A2I
  strcpy(C0A2Isett[0], "Selec. cantidad");
  strcpy(C0A2Isett[1], "      /5000 grs");
  strcpy(C0A2Isett[2], "Confirmar");
  strcpy(C0A2Isett[3], "Volver");
  //* C0A2T
  strcpy(C0A2Tsett[0], "Tiempo de mezcla");
  strcpy(C0A2Tsett[1], "      /5000 segs");
  strcpy(C0A2Tsett[2], "Confirmar");
  strcpy(C0A2Tsett[3], "Volver");
  //* C0A3
  strcpy(C0A3notif[0], "Receta guardada");
  strcpy(C0A3notif[1], "correctamente.");
  strcpy(C0A3notif[2], "");
  strcpy(C0A3notif[3], "Volver");
  //* C0B0
  strcpy(C0B0sett[0], "Seleccione receta;");
  strcpy(C0B0sett[1], "  /10");
  strcpy(C0B0sett[2], "Confirmar");
  strcpy(C0B0sett[3], "Volver");
  //* C0B1
  strcpy(C0B1notif[0], "Receta eliminada");
  strcpy(C0B1notif[1], "correctamente.");
  strcpy(C0B1notif[2], "");
  strcpy(C0B1notif[3], "Volver");


/*

//Menu A0

  strcpy(menuA0[0],  "MA0 - A0A0");

  strcpy(menuA0[1],  "MA0 - OPT 2");

  strcpy(menuA0[2],  "MA0 - OPT 3");

  strcpy(menuA0[3],  "MA0 - OPT 4");

  strcpy(menuA0[4],  "MA0 - Volver");



//Menu A0A0 (SETTING MENU)

  strcpy(menuA0A0[0],  "SETEE VALOR");

  strcpy(menuA0A0[1],  "");

  strcpy(menuA0A0[2],  "Confirmar");

  strcpy(menuA0A0[3],  "Volver");



//Menu B0

  strcpy(menuB0[0],  "MB0 - A0A0");

  strcpy(menuB0[1],  "MB0 - OPT 2");

  strcpy(menuB0[2],  "MB0 - OPT 3");

  strcpy(menuB0[3],  "MB0 - OPT 4");

  strcpy(menuB0[4],  "MB0 - OPT 5");

  strcpy(menuB0[5],  "MB0 - OPT 6");

  strcpy(menuB0[6],  "MB0 - Volver");



  



//NOTIFICACION

  strcpy(menuA0A0Conf[0],  "Valor guardado");

  strcpy(menuA0A0Conf[1],  "correctamente!");

  strcpy(menuA0A0Conf[2],  "");

  strcpy(menuA0A0Conf[3],  "");

*/
# 435 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
  //Setup del lcd
  lcd.init();
  lcd.backlight();
}




void loop(){

//? MAIN MENU
if (strcmp(VISTA, "MAIN") == 0) {
   //!VISTA
   scrolling(mainMENU_F);
   generarVISTA(mainMENU,mainMENU_F);
   scrollSIGN(mainMENU_F);
   //!MODELO

   //Cambio de vista 
   cambioVISTA(0,"A0");
   cambioVISTA(1,"B0");
   cambioVISTA(2,"C0");
}

//? Vistas de rama A
else if (strcmp(VISTA, "A0") == 0) {
   //!VISTA SCROLL
   scrolling(A0scroll_f);
   generarVISTA(A0scroll,A0scroll_f);
   scrollSIGN(A0scroll_f);
   //!MODELO


   //Cambio de vista 
   cambioVISTA(0,"A0A0");
   cambioVISTA(1,"A0B0");
   cambioVISTA(2,"A0C0");
   //go BACK
   cambioVISTA(3,"MAIN");
}
else if (strcmp(VISTA, "A0A0") == 0) {
   //!VISTA SCROLL
   scrolling(A0A0scroll_f);
   generarVISTA(A0A0scroll,A0A0scroll_f);
   scrollSIGN(A0A0scroll_f);
   //!MODELO


   //Cambio de vista 

   //go BACK
   cambioVISTA(3,"A0");
}
else if (strcmp(VISTA, "A0B0") == 0) {
   //! VISTA SETTING
   settingMENU(A0B0sett,receta_seleccionada);
   //Cambio de vista al setting loop (indicar CASE del setting LOOP)
   Serial.println(RECETA[receta_seleccionada][0]);
   //!MODELO
   cambioSETTING("A0B0set");
   //Cambio de vista confirmacion
   cambioVISTA(2,"A0B1");
   //Cambio de vista al menu anterior
   cambioVISTA(3,"A0");
}
else if (strcmp(VISTA, "A0B0set") == 0){
   //!VISTA SETTING LOOP
   setterFUNC(10,1,1,10);
   receta_seleccionada = valuetoSET;
   settingBACK("A0B0");
   //Printear receta seleccionada

}
else if (strcmp(VISTA, "A0B1") == 0) {
   //!VISTA NOTIF
   //scrolling(menuA0A0Conf_F);
   generarVISTA(A0B1notif,A0B1notif_f);
   //scrollSIGN(menuA0A0Conf_F);

   //!MODELO
   //Cambio de vista 
   cambioEVENTO("A0");
}
else if (strcmp(VISTA, "A0C0") == 0) {
   //!VISTA SCROLL
   scrolling2();
   generarVISTA(A0C0scroll,A0C0scroll_f);
   scrollSIGN(A0C0scroll_f);
   //!MODELO


   //Cambio de vista 
   cambioVISTA(2,"A0C1");
   //go BACK
   cambioVISTA(3,"A0");
}
else if (strcmp(VISTA, "A0C1") == 0) {
   //!VISTA SCROLL
   scrolling(A0C1scroll_f);
   generarVISTA(A0C1scroll,A0C1scroll_f);
   scrollSIGN(A0C1scroll_f);
   //!MODELO


   //Cambio de vista 
   //go BACK
   cambioVISTA(3,"A0C0");
}

//? Vistas de rama B
else if (strcmp(VISTA, "B0") == 0) {
   //! ESTA VISTA ES UNA VISTA CONDICIONADA TIENE VERSION a Y VERSION b
   //! dependiendo de si se cumple la "condicion de entrada" se activa una o la otra

   //!VISTAa SCROLL con 2 opciones (se activa si los contenedores estan cargados)
   if(contenedoresCARGADOS){
   scrolling2();
   generarVISTA(B0ascroll,B0ascroll_f);
   scrollSIGN(B0ascroll_f);
   //!MODELO


   //Cambio de vista 
   cambioVISTA(2,"B1");
   //go BACK
   cambioVISTA(3,"MAIN");
   }
   //!VISTAb con notificacion (se activa si los contenedores no estan)
   else{
   //!VISTA NOTIF
   generarVISTA(B0bnotif,B0bnotif_f);
   //!MODELO
   //Cambio de vista 
   cambioEVENTO("MAIN");
   }
}
else if (strcmp(VISTA, "B1") == 0) {
   //!VISTA SCROLL
   scrolling2();
   generarVISTA(B1scroll,B1scroll_f);
   //!Vista de estado
   lcd.setCursor(0, 1);
   lcd.print(receta_seleccionada);

   scrollSIGN(B1scroll_f);
   //!MODELO


   //Cambio de vista 
   cambioVISTA(2,"B2");
   //go BACK
   cambioVISTA(3,"B0");
}
else if (strcmp(VISTA, "B2") == 0) {
   //!VISTA SCROLL
   scrolling(B2scroll_f);
   generarVISTA(B2scroll,B2scroll_f);
   scrollSIGN(B2scroll_f);
   //!MODELO


   //Cambio de vista 
   cambioVISTA(2,"B2");
   //go BACK
   cambioVISTA(3,"B1");
}
else if (strcmp(VISTA, "B3") == 0) {
   //!VISTA SCROLL
   scrolling(B3scroll_f);
   generarVISTA(B3scroll,B3scroll_f);
   scrollSIGN(B3scroll_f);
   //!MODELO


   //Cambio de vista 

   //go BACK
   cambioVISTA(3,"B2");
}

//? Vistas rama C
else if (strcmp(VISTA, "C0") == 0) {
   //!VISTA SCROLL
   //scrolling(C0scroll_f);
   scrollingSETMENU();
   generarVISTA(C0scroll,C0scroll_f);
   scrollSIGN(C0scroll_f);
   //!MODELO


   //Cambio de vista 
   cambioVISTA(1,"C0A0");
   cambioVISTA(2,"C0B0");
   //go BACK
   cambioVISTA(3,"MAIN");
}
else if (strcmp(VISTA, "C0A0") == 0) {
   //! VISTA SETTING
   settingMENU(C0A0sett,addREC_slot);
   //Cambio de vista al setting loop (indicar CASE del setting LOOP)
   //!MODELO 

   //Asegurar que si no se entre al setting loop se parta desde 1 
   cambioSETTING("C0A0set");
   //Cambio de vista confirmacion
   if (slotDISPONIBLE){
   for (int i = 0; i < 9; i++) {
   RECETA[0][i] = RECETA[addREC_slot][i];
   }
   cambioVISTA(2,"C0A1");
   }
   else{
   cambioVISTA(2,"C0A1X");
   for (int i = 0; i < 9; i++) {
   RECETA[0][i] = RECETA[addREC_slot][i];
   }
   }
   //Cambio de vista al menu anterior
   cambioVISTA(3,"C0");
}
else if (strcmp(VISTA, "C0A0set") == 0){
   //!VISTA SETTING LOOP
   setterFUNC(10,1,1,10);
   addREC_slot = valuetoSET;
   //Asegurar que no acceda al array de setting
   settingBACK("C0A0");
}
else if (strcmp(VISTA, "C0A1X") == 0) {
   //!VISTA SCROLL
   //scrolling(C0scroll_f);
   scrolling2();
   generarVISTA(C0A1Xscroll,C0A1Xscroll_f);
   scrollSIGN(C0A1Xscroll_f);
   //!MODELO


   //Cambio de vista 
   cambioVISTA(2,"C0A1");
   //cambioVISTA(1,"C0B0");
   //cambioVISTA(2,"C0C0");
   //go BACK
   cambioVISTA(3,"C0");
}
else if (strcmp(VISTA, "C0A1") == 0) {
   //!VISTA SCROLL
   //scrolling(C0scroll_f);
   scrolling(C0A1scroll_f);
   //generarVISTA(C0A1scroll,C0A1scroll_f);
   generarVISTAparams(C0A1scroll,RECETA[0],C0A1scroll_f);
   scrollSIGN(C0A1scroll_f);
   //!MODELO
   addREC_ingSELECT = scrollSTATE;

   //Cambio de vista 
   //* En este caso todo los cambios llevan al mismo setting menu, exceoto que la variable cambia de acuerdo al scrollSTATE
   if (scrollSTATE == 0){
   addRECcantidad = RECETA[0][addREC_ingSELECT];
   cambioVISTA(0,"C0A2I");}
   if (scrollSTATE == 1){
   addRECcantidad = RECETA[0][addREC_ingSELECT];
   cambioVISTA(1,"C0A2I");}
   if (scrollSTATE == 2){
   addRECcantidad = RECETA[0][addREC_ingSELECT];
   cambioVISTA(2,"C0A2I");}
   if (scrollSTATE == 3){
   addRECcantidad = RECETA[0][addREC_ingSELECT];
   cambioVISTA(3,"C0A2I");}
   if (scrollSTATE == 4){
   addRECcantidad = RECETA[0][addREC_ingSELECT];
   cambioVISTA(4,"C0A2I");}
   if (scrollSTATE == 5){
   addRECcantidad = RECETA[0][addREC_ingSELECT];
   cambioVISTA(5,"C0A2I");}
   if (scrollSTATE == 6){
   addRECcantidad = RECETA[0][addREC_ingSELECT];
   cambioVISTA(6,"C0A2T");}

   //GUARDAR EDICION
   if ((scrollSTATE == 7)&&(SELECT)){
   for (int i = 0; i < 7; i++) {
   RECETA[addREC_slot][i] = RECETA[0][i];
   }
   cambioVISTA(7,"C0");}
   //go BACK
   cambioVISTA(8,"C0");
}
else if (strcmp(VISTA, "C0A2I") == 0) {
   //! VISTA SETTING
   settingMENU(C0A2Isett,addRECcantidad);
   //Cambio de vista al setting loop (indicar CASE del setting LOOP)
   //!MODELO
   cambioSETTING("C0A2Iset");
   //Cambio de vista confirmacion
   cambioVISTA(2,"C0A1");
   //Cambio de vista al menu anterior
   cambioVISTA(3,"C0");
}
else if (strcmp(VISTA, "C0A2Iset") == 0){
   //!VISTA SETTING LOOP
   setterFUNC(10,100,0,1000);
   addRECcantidad = valuetoSET;
   RECETA[0][addREC_ingSELECT] = valuetoSET;
   settingBACK("C0A2I");

}
else if (strcmp(VISTA, "C0A2T") == 0) {
   //! VISTA SETTING
   settingMENU(C0A2Tsett,addRECcantidad);
   //Cambio de vista al setting loop (indicar CASE del setting LOOP)
   //!MODELO
   cambioSETTING("C0A2Tset");
   //Cambio de vista confirmacion
   cambioVISTA(2,"C0A1");
   //Cambio de vista al menu anterior
   cambioVISTA(3,"C0");
}
else if (strcmp(VISTA, "C0A2Tset") == 0){
   //!VISTA SETTING LOOP
   setterFUNC(10,10,0,300);
   addRECcantidad = valuetoSET;
   RECETA[0][addREC_ingSELECT] = valuetoSET;
   settingBACK("C0A2T");

}
else if (strcmp(VISTA, "C0A3") == 0){
   //!VISTA NOTIF
   generarVISTA(C0A3notif,C0A3notif_f);

   //!MODELO
   //Cambio de vista 
   cambioEVENTO("C0");
}
else if (strcmp(VISTA, "C0B0") == 0) {
   //! VISTA SETTING
   settingMENU(C0B0sett,addREC_slot);
   //Cambio de vista al setting loop (indicar CASE del setting LOOP)

   //!MODELO
   cambioSETTING("C0B0set");
   //Cambio de vista confirmacion
   cambioVISTA(2,"C0B1");
   //Cambio de vista al menu anterior
   cambioVISTA(3,"C0");
}
else if (strcmp(VISTA, "C0B0set") == 0){
   //!VISTA SETTING LOOP
   setterFUNC(10,1,1,10);
   addREC_slot = valuetoSET;
   //Asegurar que no acceda al array de setting
   settingBACK("C0B0");
}
else if (strcmp(VISTA, "C0B1") == 0){
   //!VISTA NOTIF
   generarVISTA(C0B1notif,C0B1notif_f);

   //!MODELO
   //Cambio de vista 
   for (int i = 0; i < 7; i++) {
   RECETA[addREC_slot][i] = 0;
   }

   Serial.println(addREC_slot);
   Serial.println(RECETA[addREC_slot][0]);
   cambioEVENTO("C0");
}



/*

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

   //! VISTA SETTING

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

   //!VISTA SETTING LOOP

   setterFUNC(10,100,1000);

   settingBACK("A0A0");

}

else if (strcmp(VISTA, "A0A0-S+") == 0) {

   //!VISTA NOTIF

   //scrolling(menuA0A0Conf_F);

   generarVISTA(menuA0A0Conf,menuA0A0Conf_F);

   //scrollSIGN(menuA0A0Conf_F);



   //!MODELO

   //Cambio de vista 

   cambioEVENTO("A0");

} 

*/
# 858 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
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
    //Serial.print("ScrollSIGN: ");
    //Serial.println(scrollOFFSET);
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
   //Serial.print("Vista cambiada a:");
   //Serial.println(opciones);
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
# 950 "C:\\Users\\bruno\\Desktop\\sopaMENU\\sopaMENU.ino"
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
void settingMENU(char menu[4][20], int valor){
   scrollingSETMENU();
   generarVISTAsettmenu(menu,4,valor);
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
void setterFUNC(int steps, int setSTEP, int startIndex, int limit){

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
   if(scrollSTATE <= startIndex){
      scrollSTATE = startIndex;
   }
   DOWN = false;
   valuetoSET = valuetoSET - setSTEP;
   }
   if ((valuetoSET > limit)||(valuetoSET<startIndex)){
   valuetoSET = startIndex;
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
void scrollingSETMENU(){

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

/*Permite scrollear en menus de 2 opciones*/
void scrolling2(){

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
      scrollSTATE = 2;
   }
}

/*Genera la vista del sett menu con el parametro respectivo*/
void generarVISTAsettmenu(char menu[][20],int opciones, int valor) {

   asignarVISTA(menu, displaySHOW, scrollSTATE, opciones);
   //Serial.print("Vista cambiada a:");
   //Serial.println(opciones);
   if (EVENT){
   lcd.clear();
   EVENT = false;
   }

   for (int i = 0; i <= 3; i++) {
      lcd.setCursor(0, i);
      lcd.print(displaySHOW[i]);
      lcd.setCursor(0, 1);
      lcd.print(valor);
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


//? Generar la vista "display del editor de recetas"

void asignarPARAMS(int entrada[9], int salida_int[4], int scroll, int filas) {

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
       salida_int[i++] = entrada[fila];
   }

}

void generarVISTAparams(char menu[][20],int entrada[9] ,int opciones) {

   asignarVISTA(menu, displaySHOW, scrollSTATE, opciones);
   asignarPARAMS(entrada, addRECshowparam, scrollSTATE, opciones);
   //Serial.print("Vista cambiada a:");
   //Serial.println(opciones);
   if (EVENT){
   lcd.clear();
   EVENT = false;
   }

   for (int i = 0; i <= 3; i++) {
      lcd.setCursor(0, i);
      lcd.print(displaySHOW[i]);
      lcd.setCursor(9, i);
      if (addRECshowparam[i] != 99999)
      {
         lcd.print(addRECshowparam[i]);
      }
   }
}

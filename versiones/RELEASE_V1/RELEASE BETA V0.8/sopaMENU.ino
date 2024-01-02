#include <LiquidCrystal_I2C.h>
#include <string> 
#include <EEPROM.h>


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

//Inicializar botones y variables globales
#define BUTTON1 18
#define BUTTON2 19
#define BUTTON3 17

unsigned long buttonTIME = 0;  
unsigned long lastBTIME = 0; 
hw_timer_t *My_timer = NULL;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;


//Constantes del menu
volatile bool UP     = false;
volatile bool DOWN   = false;
volatile bool SELECT = false;
volatile bool EVENT  = true;
int scrollSTATE = 0;
int scrollOFFSET = 0;
char VISTA[12] = "MAIN";
int valuetoSET = 0;
int settingLOOP = false;
int cantPORCIONES = 1;
volatile bool cellCHECK = false;

volatile int contSELECT=0;

//Constante condicionales de la operacion
volatile bool contenedoresCARGADOS = true;
int receta_seleccionada = 1;


int  addREC_slot = 1;
bool slotDISPONIBLE = false;
int  addREC_ingSELECT;
int  addRECparam [9] = {0,0,0,0,0,0,0,99999,99999};
int  addRECshowparam[4];
int  addRECcantidad = 0;

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
void IRAM_ATTR BUTTONpress1(){
   buttonTIME = millis();
//Condicion para el debounce
if (buttonTIME - lastBTIME > 500)
{
   UP     = true;
   DOWN   = false;
   SELECT = false;
   EVENT  = true;
   Serial.println("UP  presionado");
   
lastBTIME = buttonTIME;
 }
}

//INTERRUPCION BOTON DOWN
void IRAM_ATTR BUTTONpress2(){
   buttonTIME = millis();
//Condicion para el debounce
if (buttonTIME - lastBTIME > 500)
{
   UP     = false;
   DOWN   = true;
   SELECT = false;
   EVENT  = true;
   Serial.println("DOWN  presionado");
   
lastBTIME = buttonTIME;
 }
}

//INTERRUPCION BOTON SELECT
void IRAM_ATTR BUTTONpress3(){
   buttonTIME = millis();
//Condicion para el debounce
if (buttonTIME - lastBTIME > 500)
{
   UP     = false;
   DOWN   = false;
   SELECT = true;
   EVENT  = true;
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
char mainMENU[5][20]; //Declarar la matriz de la vista
int mainMENU_F = 5;   //Declarar opciones (filas del menu)

//? Menus rama A
//* scroll A0
char A0scroll[4][20]; //Declarar la matriz de la vista
int A0scroll_f = 4;   //Declarar opciones (filas del menu)

//* scroll conSELEC
char conSELECTscroll[8][20]; //Declarar la matriz de la vista
int conSELECTscroll_f = 8;   //Declarar opciones (filas del menu)

//* notif A0A1
char A0A1notif[4][20]; //Declarar la matriz de la vista
int A0A1notif_f = 4;   //Declarar opciones (filas del menu)

//* notif A0A2
char A0A2notif[4][20]; //Declarar la matriz de la vista
int A0A2notif_f = 4;   //Declarar opciones (filas del menu)

//* notif A0B1
char A0B1notif[4][20]; //Declarar la matriz de la vista
int A0B1notif_f = 4;   //Declarar opciones (filas del menu)

//* notif A0B2
char A0B2notif[4][20]; //Declarar la matriz de la vista
int A0B2notif_f = 4;   //Declarar opciones (filas del menu)


//? Menus de la rama B
//* scroll B0a
char B0ascroll[4][20]; //Declarar la matriz de la vista
int B0ascroll_f = 4;   //Declarar opciones (filas del menu)

//* notif B0b
char B0bnotif[4][20]; //Declarar la matriz de la vista
int B0bnotif_f = 4;   //Declarar opciones (filas del menu)

//* setting B0A0
char B0A0sett[4][20]; //Declarar la matriz de la vista

//* notif B0A1
char B0A1notif[4][20]; //Declarar la matriz de la vista
int B0A1notif_f = 4;   //Declarar opciones (filas del menu)

//* scroll B0B0
char B0B0scroll[4][20]; //Declarar la matriz de la vista
int B0B0scroll_f = 4;   //Declarar opciones (filas del menu)

//* setting B0B0A0
char B0B0A0sett[4][20]; //Declarar la matriz de la vista

//* notif B0B0A1
char B0B0A1notif[4][20]; //Declarar la matriz de la vista
int B0B0A1notif_f = 4;   //Declarar opciones (filas del menu)

//* notif B0B0A2a
char B0B0A2anotif[4][20]; //Declarar la matriz de la vista
int B0B0A2anotif_f = 4;   //Declarar opciones (filas del menu)

//* notif B0B0A2a
char B0B0A2bnotif[4][20]; //Declarar la matriz de la vista
int B0B0A2bnotif_f = 4;   //Declarar opciones (filas del menu)

//* notif B0B0B0
char B0B0B0notif[4][20]; //Declarar la matriz de la vista
int B0B0B0notif_f = 4;   //Declarar opciones (filas del menu)

//* scroll B0B0C0
char B0B0C0scroll[4][20]; //Declarar la matriz de la vista
int B0B0C0scroll_f = 4;   //Declarar opciones (filas del menu)

//* notif B0B0C1
char B0B0C1notif[4][20]; //Declarar la matriz de la vista
int B0B0C1notif_f = 4;   //Declarar opciones (filas del menu)


//? Menus de la rama C
//* scroll C0
char C0scroll[4][20]; //Declarar la matriz de la vista
int C0scroll_f = 4;   //Declarar opciones (filas del menu)

//* sett C0A0
char C0A0sett[4][20]; //Declarar la matriz de la vista

//* scroll C0A1X
char C0A1Xscroll[4][20]; //Declarar la matriz de la vista
int C0A1Xscroll_f = 4;   //Declarar opciones (filas del menu)

//* scroll C0A1
char C0A1scroll[9][20]; //Declarar la matriz de la vista
int C0A1scroll_f = 9;   //Declarar opciones (filas del menu)

//* scroll C0A2I
char C0A2Isett[4][20]; //Declarar la matriz de la vista

//* scroll C0A2T
char C0A2Tsett[4][20]; //Declarar la matriz de la vista

//* notif C0A3
char C0A3notif[4][20]; //Declarar la matriz de la vista
int C0A3notif_f = 4;   //Declarar opciones (filas del menu)

//* sett C0B0
char C0B0sett[4][20]; //Declarar la matriz de la vista
int C0B0sett_f = 4;   //Declarar opciones (filas del menu)

//* notif C0B2
char C0B1notif[4][20]; //Declarar la matriz de la vista
int C0B1notif_f = 4;   //Declarar opciones (filas del menu)

//? Menus de la rama D

//* scroll D0
char D0scroll[4][20]; //Declarar la matriz de la vista
int  D0scroll_f = 4;   //Declarar opciones (filas del menu)

//* scroll D0A1
char D0A1scroll[5][20]; //Declarar la matriz de la vista
int  D0A1scroll_f = 5;   //Declarar opciones (filas del menu)

//* notif D0A1a
char D0A1anotif[4][20]; //Declarar la matriz de la vista
int  D0A1anotif_f = 4;   //Declarar opciones (filas del menu)

//* notif D0A1b
char D0A1bnotif[4][20]; //Declarar la matriz de la vista
int  D0A1bnotif_f = 4;   //Declarar opciones (filas del menu)

//* notif D0A1c
char D0A1cnotif[4][20]; //Declarar la matriz de la vista
int  D0A1cnotif_f = 4;   //Declarar opciones (filas del menu)

//* notif D0A1d
char D0A1dnotif[4][20]; //Declarar la matriz de la vista
int  D0A1dnotif_f = 4;   //Declarar opciones (filas del menu)

//* scroll D0B1
char D0B1scroll[4][20]; //Declarar la matriz de la vista
int  D0B1scroll_f = 4;   //Declarar opciones (filas del menu)

//* scroll D0C0
char D0C0scroll[8][20]; //Declarar la matriz de la vista
int  D0C0scroll_f = 8;   //Declarar opciones (filas del menu)

//? Menus rama E
char E0notif[4][20]; //Declarar la matriz de la vista
int  E0notif_f = 4;   //Declarar opciones (filas del menu)

void setup(){
Serial.begin(115200);
//Interrupciones
pinMode(BUTTON1, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(BUTTON1), BUTTONpress1, FALLING);

pinMode(BUTTON2, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(BUTTON2), BUTTONpress2, FALLING);

pinMode(BUTTON3, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(BUTTON3), BUTTONpress3, FALLING);



// Init de las vistas
// Asignar valores a los elementos del array

//? MAIN MENU
  strcpy(mainMENU[0],  "Carga/Descarga");
  strcpy(mainMENU[1],  "Prep. receta");
  strcpy(mainMENU[2],  "Adm. recetas");
  strcpy(mainMENU[3],  "Configuraciones");
  strcpy(mainMENU[4],  "About");

//? Menus de la rama A

  //* A0 
  strcpy(A0scroll[0],  "Seleccione accion:");
  strcpy(A0scroll[1],  "Cargar");
  strcpy(A0scroll[2],  "Purgar");
  strcpy(A0scroll[3],  "Volver");
  //* CONT SELECT  --- COMUN PARA LAS OPCIONES DE CARGA Y PURGA
  strcpy(conSELECTscroll[0],  "Contenedor 1");
  strcpy(conSELECTscroll[1],  "Contenedor 2");
  strcpy(conSELECTscroll[2],  "Contenedor 3");
  strcpy(conSELECTscroll[3],  "Contenedor 4");
  strcpy(conSELECTscroll[4],  "Contenedor 5");
  strcpy(conSELECTscroll[5],  "Contenedor 6");
  strcpy(conSELECTscroll[6],  "Contenedor 7");
  strcpy(conSELECTscroll[7],  "Volver");
  //* A0A1 
  strcpy(A0A1notif[0],  "Pulse cuando el");
  strcpy(A0A1notif[1],  "contenedor");
  strcpy(A0A1notif[2],  "hay sido");
  strcpy(A0A1notif[3],  "cargado.");
  //* A0A2
  strcpy(A0A2notif[0],  "EL contenedor");
  strcpy(A0A2notif[1],  "ha sido cargado.");
  strcpy(A0A2notif[2],  "");
  strcpy(A0A2notif[3],  "Volver.");
  //* A0B1 
  strcpy(A0B1notif[0],  "Pulse cuando el");
  strcpy(A0B1notif[1],  "contenedor");
  strcpy(A0B1notif[2],  "hay sido");
  strcpy(A0B1notif[3],  "purgado.");
  //* A0B2
  strcpy(A0B2notif[0],  "EL contenedor");
  strcpy(A0B2notif[1],  "ha sido purgado.");
  strcpy(A0B2notif[2],  "");
  strcpy(A0B2notif[3],  "Volver.");


//? Menus de la rama B

  //* B0a
  strcpy(B0ascroll[0],  "Seleccione accion:");
  strcpy(B0ascroll[1],  "Selec. receta");
  strcpy(B0ascroll[2],  "Preparar");
  strcpy(B0ascroll[3],  "Volver");
  //* B0b
  strcpy(B0bnotif[0],  "Hay una carga de");
  strcpy(B0bnotif[1],  "contenedores");
  strcpy(B0bnotif[2],  "pendiente.");
  strcpy(B0bnotif[3],  "Volver");
  //* B0A0
  strcpy(B0A0sett[0],  "Seleccione receta");
  strcpy(B0A0sett[1],  "  /10");
  strcpy(B0A0sett[2],  "Confirmar");
  strcpy(B0A0sett[3],  "Volver");
  //* B0A1
  strcpy(B0A1notif[0],  "Receta guardada");
  strcpy(B0A1notif[1],  "correctamente.");
  strcpy(B0A1notif[2],  "");
  strcpy(B0A1notif[3],  "Volver.");
  //* B0B0
  strcpy(B0B0scroll[0],  "Preparar");
  strcpy(B0B0scroll[1],  "Mezclar.");
  strcpy(B0B0scroll[2],  "Dispensar");
  strcpy(B0B0scroll[3],  "Volver.");
  //* B0B0A0
  strcpy(B0B0A0sett[0],  "Numero porciones");
  strcpy(B0B0A0sett[1],  "  /10");
  strcpy(B0B0A0sett[2],  "Confirmar");
  strcpy(B0B0A0sett[3],  "Volver");
  //* B0B0A1
  strcpy(B0B0A1notif[0],  "Preparando...");
  strcpy(B0B0A1notif[1],  "");
  strcpy(B0B0A1notif[2],  "PULSE PARA");
  strcpy(B0B0A1notif[3],  " CANCELAR.");
  //* B0B0A2a
  strcpy(B0B0A2anotif[0],  "Porciones");
  strcpy(B0B0A2anotif[1],  "preparadas.");
  strcpy(B0B0A2anotif[2],  "");
  strcpy(B0B0A2anotif[3],  "Volver.");
  //* B0B0A2b
  strcpy(B0B0A2bnotif[0],  " PROCESO ");
  strcpy(B0B0A2bnotif[1],  "CANCELADO.");
  strcpy(B0B0A2bnotif[2],  "");
  strcpy(B0B0A2bnotif[3],  "Volver.");
  //* B0B0B0
  strcpy(B0B0B0notif[0],  "Mezclando material");
  strcpy(B0B0B0notif[1],  "");
  strcpy(B0B0B0notif[2],  "Pulse para");
  strcpy(B0B0B0notif[3],  "detener.");
  //* B0B0C0
  strcpy(B0B0C0scroll[0],  "Seleccione accion:");
  strcpy(B0B0C0scroll[1],  "");
  strcpy(B0B0C0scroll[2],  "Dispensar porcion");
  strcpy(B0B0C0scroll[3],  "Volver");
  //* B0B0C1
  strcpy(B0B0C1notif[0],  "Porcion dispensada");
  strcpy(B0B0C1notif[1],  "correctamente.");
  strcpy(B0B0C1notif[2],  "");
  strcpy(B0B0C1notif[3],  "Volver.");

//? Menus de la rama C
  //* C0 
  strcpy(C0scroll[0],  "Editor de recetas");
  strcpy(C0scroll[1],  "Agregar/Editar");
  strcpy(C0scroll[2],  "Eliminar");
  strcpy(C0scroll[3],  "Volver");
  //* C0A0
  strcpy(C0A0sett[0],  "Seleccione receta");
  strcpy(C0A0sett[1],  "  /10");
  strcpy(C0A0sett[2],  "Confirmar");
  strcpy(C0A0sett[3],  "Volver");
  //* C0A1X 
  strcpy(C0A1Xscroll[0],  "Espacio ocupado");
  strcpy(C0A1Xscroll[1],  "desea editarlo?");
  strcpy(C0A1Xscroll[2],  "Confirmar");
  strcpy(C0A1Xscroll[3],  "Volver");
  //* C0A1
  strcpy(C0A1scroll[0],  "Ing 1   -      g");
  strcpy(C0A1scroll[1],  "Ing 2   -      g");
  strcpy(C0A1scroll[2],  "Ing 3   -      g");
  strcpy(C0A1scroll[3],  "Ing 4   -      g");
  strcpy(C0A1scroll[4],  "Ing 5   -      g");
  strcpy(C0A1scroll[5],  "Ing 6   -      g");
  strcpy(C0A1scroll[6],  "Porcion -      g");
  strcpy(C0A1scroll[7],  "Confirmar");
  strcpy(C0A1scroll[8],  "Cancelar"); 
  //* C0A2I
  strcpy(C0A2Isett[0],  "Selec. cantidad");
  strcpy(C0A2Isett[1],  "      /5000 grs");
  strcpy(C0A2Isett[2],  "Confirmar");
  strcpy(C0A2Isett[3],  "Volver");
  //* C0A2T
  strcpy(C0A2Tsett[0],  "Tiempo de mezcla");
  strcpy(C0A2Tsett[1],  "      /5000 segs");
  strcpy(C0A2Tsett[2],  "Confirmar");
  strcpy(C0A2Tsett[3],  "Volver");
  //* C0A3
  strcpy(C0A3notif[0],  "Receta guardada");
  strcpy(C0A3notif[1],  "correctamente.");
  strcpy(C0A3notif[2],  "");
  strcpy(C0A3notif[3],  "Volver");
  //* C0B0
  strcpy(C0B0sett[0],  "Seleccione receta;");
  strcpy(C0B0sett[1],  "  /10");
  strcpy(C0B0sett[2],  "Confirmar");
  strcpy(C0B0sett[3],  "Volver");
  //* C0B1
  strcpy(C0B1notif[0],  "Receta eliminada");
  strcpy(C0B1notif[1],  "correctamente.");
  strcpy(C0B1notif[2],  "");
  strcpy(C0B1notif[3],  "Volver");


//? Menus de la rama D
//*C0
  strcpy(D0scroll[0],  "Calibracion");
  strcpy(D0scroll[1],  "Control");
  strcpy(D0scroll[2],  "Estado");
  strcpy(D0scroll[3],  "Volver");
//*C0A1
  strcpy(D0A1scroll[0],  "Reiniciar");
  strcpy(D0A1scroll[1],  "Tarar");
  strcpy(D0A1scroll[2],  "Calibrar");
  strcpy(D0A1scroll[3],  "Medicion");
  strcpy(D0A1scroll[4],  "Volver");
//*D0A1a
  strcpy(D0A1anotif[0],  "Contenedor");
  strcpy(D0A1anotif[1],  "reiniciado");
  strcpy(D0A1anotif[2],  "");
  strcpy(D0A1anotif[3],  "Volver");
//*D0A1b
  strcpy(D0A1bnotif[0],  "Contenedor");
  strcpy(D0A1bnotif[1],  "tarado");
  strcpy(D0A1bnotif[2],  "");
  strcpy(D0A1bnotif[3],  "Volver");
//*D0A1c
  strcpy(D0A1cnotif[0],  "Contenedor");
  strcpy(D0A1cnotif[1],  "calibrado");
  strcpy(D0A1cnotif[2],  "");
  strcpy(D0A1cnotif[3],  "Volver");
//*D0A1d
  strcpy(D0A1dnotif[0],  "Contenedor");
  strcpy(D0A1dnotif[1],  "midiendo:");
  strcpy(D0A1dnotif[2],  "         g.");
  strcpy(D0A1dnotif[3],  "Volver");
//*C0B1
  strcpy(D0B1scroll[0],  "GIRAR sentido +");
  strcpy(D0B1scroll[1],  "GIRAR sentido -");
  strcpy(D0B1scroll[2],  "DETENER");
  strcpy(D0B1scroll[3],  "Volver");
//*C0C0
  strcpy(D0C0scroll[0],  "CONT1: ");
  strcpy(D0C0scroll[1],  "CONT2: ");
  strcpy(D0C0scroll[2],  "CONT3: ");
  strcpy(D0C0scroll[3],  "CONT4: ");
  strcpy(D0C0scroll[4],  "CONT5: ");
  strcpy(D0C0scroll[5],  "CONT6: ");
  strcpy(D0C0scroll[6],  "CONT7: ");
  strcpy(D0C0scroll[7],  "Volver");


//? Menus de la rama E
  strcpy(E0notif[0],  "HAC INGENIERIA");
  strcpy(E0notif[1],  "SOPA SOFTWARE v1.0");
  strcpy(E0notif[2],  "         ");
  strcpy(E0notif[3],  "Volver");
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
   cambioVISTA(3,"D0");
   cambioVISTA(4,"E0");
} 

//? Vistas de rama A
else if (strcmp(VISTA, "A0") == 0) {
   //!VISTA SCROLL
   //scrolling(A0scroll_f);
   scrollingSETMENU();
   generarVISTA(A0scroll,A0scroll_f);
   scrollSIGN(A0scroll_f);
   //!MODELO


   //Cambio de vista 
   cambioVISTA(1,"A0A0");
   cambioVISTA(2,"A0B0");
   //cambioVISTA(2,"A0C0");
   //go BACK
   cambioVISTA(3,"MAIN");
} 
else if (strcmp(VISTA, "A0A0") == 0) {
   //!VISTA SCROLL
   scrolling(conSELECTscroll_f);
   generarVISTA(conSELECTscroll,conSELECTscroll_f);
   scrollSIGN(conSELECTscroll_f);
   //!MODELO
   contSELECT = scrollSTATE;
   Serial.printf("Seleccionado contenedor: %d\n",contSELECT);

   //Cambio de vista 
   
   //go BACK
   cambioVISTA(0,"A0A1");
   cambioVISTA(1,"A0A1");
   cambioVISTA(2,"A0A1");
   cambioVISTA(3,"A0A1");
   cambioVISTA(4,"A0A1");
   cambioVISTA(5,"A0A1");
   cambioVISTA(6,"A0A1");
   cambioVISTA(7,"A0");
} 
else if (strcmp(VISTA, "A0A1") == 0) {
   //!VISTA NOTIF
   //scrolling(menuA0A0Conf_F);
   generarVISTA(A0A1notif,A0A1notif_f);
   //scrollSIGN(menuA0A0Conf_F);

   //!MODELO
   //TODO ENVIAR MENSAJE DE CARGA AL CONTENEDOR CORRESPONDIENTE



   //Cambio de vista 
   cambioEVENTO("A0A2");
} 
else if (strcmp(VISTA, "A0A2") == 0) {
   //!VISTA NOTIF
   //scrolling(menuA0A0Conf_F);
   generarVISTA(A0A2notif,A0A2notif_f);
   //scrollSIGN(menuA0A0Conf_F);

   //!MODELO
   //Cambio de vista 
   cambioEVENTO("A0");
} 
else if (strcmp(VISTA, "A0B0") == 0) {
   //!VISTA SCROLL
   scrolling(conSELECTscroll_f);
   generarVISTA(conSELECTscroll,conSELECTscroll_f);
   scrollSIGN(conSELECTscroll_f);
   //!MODELO
   contSELECT = scrollSTATE;
   Serial.printf("Seleccionado contenedor: %d\n",contSELECT);

   //Cambio de vista 
   
   //go BACK
   cambioVISTA(0,"A0B1");
   cambioVISTA(1,"A0B1");
   cambioVISTA(2,"A0B1");
   cambioVISTA(3,"A0B1");
   cambioVISTA(4,"A0B1");
   cambioVISTA(5,"A0B1");
   cambioVISTA(6,"A0B1");
   cambioVISTA(7,"A0");
} 
else if (strcmp(VISTA, "A0B1") == 0) {
   //!VISTA NOTIF
   //scrolling(menuA0A0Conf_F);
   generarVISTA(A0B1notif,A0B1notif_f);
   //scrollSIGN(menuA0A0Conf_F);

   //!MODELO
   //TODO ENVIAR MENSAJE DE CARGA AL CONTENEDOR CORRESPONDIENTE



   //Cambio de vista 
   cambioEVENTO("A0B2");
} 
else if (strcmp(VISTA, "A0B2") == 0) {
   //!VISTA NOTIF
   //scrolling(menuA0A0Conf_F);
   generarVISTA(A0B2notif,A0B2notif_f);
   //scrollSIGN(menuA0A0Conf_F);

   //!MODELO
   //Cambio de vista 
   cambioEVENTO("A0");
} 

//? Vistas de rama B
else if (strcmp(VISTA, "B0") == 0) {
   //! ESTA VISTA ES UNA VISTA CONDICIONADA TIENE VERSION a Y VERSION b
   //! dependiendo de si se cumple la "condicion de entrada" se activa una o la otra
   
   //!VISTAa SCROLL con 2 opciones (se activa si los contenedores estan cargados)
   if(contenedoresCARGADOS){
   scrollingSETMENU();
   generarVISTA(B0ascroll,B0ascroll_f);
   scrollSIGN(B0ascroll_f);
   //!MODELO
   //Cambio de vista 
   cambioVISTA(1,"B0A0");
   cambioVISTA(2,"B0B0");
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
else if (strcmp(VISTA, "B0A0") == 0) {
   //! VISTA SETTING
   settingMENU(B0A0sett,receta_seleccionada);
   //Cambio de vista al setting loop (indicar CASE del setting LOOP)
   //Serial.println(RECETA[receta_seleccionada][0]);
   //!MODELO
   cambioSETTING("B0A0set");
   //Cambio de vista confirmacion
   cambioVISTA(2,"B0A1");
   //Cambio de vista al menu anterior
   cambioVISTA(3,"B0");
} 
else if (strcmp(VISTA, "B0A0set") == 0){
   //!VISTA SETTING LOOP
   setterFUNC(10,1,1,10);
   receta_seleccionada = valuetoSET;
   settingBACK("B0A0");
   //Printear receta seleccionada
   
}
else if (strcmp(VISTA, "B0A1") == 0) {
   //!VISTA NOTIF
   //scrolling(menuA0A0Conf_F);
   generarVISTA(B0A1notif,B0A1notif_f);
   //scrollSIGN(menuA0A0Conf_F);

   //!MODELO
   //Cambio de vista 
   cambioEVENTO("B0");
} 
else if (strcmp(VISTA, "B0B0") == 0) {
   //!VISTA SCROLL
   scrolling(B0B0scroll_f);
   generarVISTA(B0B0scroll,B0B0scroll_f);
   scrollSIGN(B0B0scroll_f);
   //!MODELO

   //Cambio de vista 
   
   //go BACK
   cambioVISTA(0,"B0B0A0");
   cambioVISTA(1,"B0B0B0");
   cambioVISTA(2,"B0B0C0");
   cambioVISTA(3,"B0");

} 
else if (strcmp(VISTA, "B0B0A0") == 0) {
   //! VISTA SETTING
   settingMENU(B0B0A0sett,cantPORCIONES);
   //Cambio de vista al setting loop (indicar CASE del setting LOOP)
   //Serial.println(RECETA[receta_seleccionada][0]);
   //!MODELO
   cambioSETTING("B0B0A0set");
   //Cambio de vista confirmacion
   cambioVISTA(2,"B0B0A1");
   //Cambio de vista al menu anterior
   cambioVISTA(3,"B0B0");
} 
else if (strcmp(VISTA, "B0B0A0set") == 0){
   //!VISTA SETTING LOOP
   setterFUNC(10,1,1,10);
   cantPORCIONES = valuetoSET;
   settingBACK("B0B0A0");
   //Printear receta seleccionada
   
}
else if (strcmp(VISTA, "B0B0A1") == 0) {
   //!VISTA NOTIF
   //scrolling(menuA0A0Conf_F);
   generarVISTA(B0B0A1notif,B0B0A1notif_f);
   //scrollSIGN(menuA0A0Conf_F);

   //!MODELO
   //Cambio de vista 
   //Correr cell check
   if (cellCHECK){
      UP = true;
      delay(5000);
   }

   cambioEVENTO("B0B0A2");
} 
else if (strcmp(VISTA, "B0B0A2") == 0) {
    //! ESTA VISTA ES UNA VISTA CONDICIONADA TIENE VERSION a Y VERSION b
   //! dependiendo de si se cumple la "condicion de entrada" se activa una o la otra
   
   //!VISTAa SCROLL con 2 opciones (se activa si los contenedores estan cargados)
   if(cellCHECK){
   //!VISTA NOTIF
   generarVISTA(B0B0A2anotif,B0B0A2anotif_f);
   //!MODELO
   //Cambio de vista 
   cambioEVENTO("B0B0");
   }
   //!VISTAb con notificacion (se activa si los contenedores no estan)
   else{
   //!VISTA NOTIF
   generarVISTA(B0B0A2bnotif,B0B0A2bnotif_f);
   //!MODELO
   //Cambio de vista 
   cambioEVENTO("B0B0");
   }
} 
else if (strcmp(VISTA, "B0B0B0") == 0) {
   //!VISTA NOTIF
   //scrolling(menuA0A0Conf_F);
   generarVISTA(B0B0B0notif,B0B0B0notif_f);
   //scrollSIGN(menuA0A0Conf_F);

   //!MODELO
   //Cambio de vista 
   //Correr cell check
   cambioEVENTO("B0B0");
} 
else if (strcmp(VISTA, "B0B0C0") == 0) {
   //!VISTA SCROLL
   scrolling2();
   generarVISTA(B0B0C0scroll,B0B0C0scroll_f);
   scrollSIGN(B0B0C0scroll_f);
   //!MODELO

   //Cambio de vista 
   
   //go BACK
   cambioVISTA(2,"B0B0C1");
   cambioVISTA(3,"B0B0");

} 
else if (strcmp(VISTA, "B0B0C1") == 0) {
   //!VISTA NOTIF
   //scrolling(menuA0A0Conf_F);
   generarVISTA(B0B0C1notif,B0B0C1notif_f);
   //scrollSIGN(menuA0A0Conf_F);

   //!MODELO
   //Cambio de vista 
   //Correr cell check
   cambioEVENTO("B0B0C0");
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

//? Vistas rama D
else if (strcmp(VISTA, "D0") == 0) {
   //!VISTA SCROLL
   //scrolling(C0scroll_f);
   scrolling(D0scroll_f);
   generarVISTA(D0scroll,D0scroll_f);
   scrollSIGN(D0scroll_f);
   //!MODELO


   //Cambio de vista 
   cambioVISTA(0,"D0A0");
   cambioVISTA(1,"D0B0");
   cambioVISTA(2,"D0C0");
   //go BACK
   cambioVISTA(3,"MAIN");
} 
else if (strcmp(VISTA, "D0A0") == 0) {
   //!VISTA SCROLL
   scrolling(conSELECTscroll_f);
   generarVISTA(conSELECTscroll,conSELECTscroll_f);
   scrollSIGN(conSELECTscroll_f);
   //!MODELO
   contSELECT = scrollSTATE;
   Serial.printf("Seleccionado contenedor: %d\n",contSELECT);

   //Cambio de vista 
   
   //go BACK
   cambioVISTA(0,"D0A1");
   cambioVISTA(1,"D0A1");
   cambioVISTA(2,"D0A1");
   cambioVISTA(3,"D0A1");
   cambioVISTA(4,"D0A1");
   cambioVISTA(5,"D0A1");
   cambioVISTA(6,"D0A1");
   cambioVISTA(7,"D0");
} 
else if (strcmp(VISTA, "D0A1") == 0) {
   //!VISTA SCROLL
   scrolling(D0A1scroll_f);
   generarVISTA(D0A1scroll,D0A1scroll_f);
   scrollSIGN(D0A1scroll_f);
   //!MODELO
   
   //Cambio de vista 
   
   //go BACK
   cambioVISTA(0,"D0A1a");
   cambioVISTA(1,"D0A1b");
   cambioVISTA(2,"D0A1c");
   cambioVISTA(3,"D0A1d");
   cambioVISTA(4,"D0");
}
else if (strcmp(VISTA, "D0A1a") == 0){
   //!VISTA NOTIF
   generarVISTA(D0A1anotif,D0A1anotif_f);

   //!MODELO
   //TODO REINICIAR CONTENEDOR
   cambioEVENTO("D0A1");
}
else if (strcmp(VISTA, "D0A1b") == 0){
   //!VISTA NOTIF
   generarVISTA(D0A1bnotif,D0A1bnotif_f);

   //!MODELO
   //TODO TARAR CONTENEDOR
   cambioEVENTO("D0A1");
}
else if (strcmp(VISTA, "D0A1c") == 0){
   //!VISTA NOTIF
   generarVISTA(D0A1cnotif,D0A1cnotif_f);

   //!MODELO
   //TODO CALIBRAR CONTENEDOR
   cambioEVENTO("D0A1");
}
else if (strcmp(VISTA, "D0A1d") == 0){
   //!VISTA NOTIF
   generarVISTA(D0A1dnotif,D0A1dnotif_f);

   //!MODELO
   //TODO EXTRAER MEDICION
   cambioEVENTO("D0A1");
}


else if (strcmp(VISTA, "D0B0") == 0) {
   //!VISTA SCROLL
   scrolling(conSELECTscroll_f);
   generarVISTA(conSELECTscroll,conSELECTscroll_f);
   scrollSIGN(conSELECTscroll_f);
   //!MODELO
   contSELECT = scrollSTATE;
   Serial.printf("Seleccionado contenedor: %d\n",contSELECT);

   //Cambio de vista 
   
   //go BACK
   cambioVISTA(0,"D0B1");
   cambioVISTA(1,"D0B1");
   cambioVISTA(2,"D0B1");
   cambioVISTA(3,"D0B1");
   cambioVISTA(4,"D0B1");
   cambioVISTA(5,"D0B1");
   cambioVISTA(6,"D0B1");
   cambioVISTA(7,"D0");
} 
else if (strcmp(VISTA, "D0B1") == 0) {
   //!VISTA SCROLL
   //scrolling(C0scroll_f);
   scrolling(D0B1scroll_f);
   generarVISTA(D0B1scroll,D0B1scroll_f);
   scrollSIGN(D0B1scroll_f);
   //!MODELO
   //TODO GIRAR DRIVER DEL CONTENEDOR SELECCIONADO

   //go BACK
   cambioVISTA(3,"D0");
} 


else if (strcmp(VISTA, "D0C0") == 0) {
   //!VISTA SCROLL
   //scrolling(C0scroll_f);
   scrolling(D0C0scroll_f);
   generarVISTA(D0C0scroll,D0C0scroll_f);
   scrollSIGN(D0C0scroll_f);
   //!MODELO
   //DISPLAY DEL STATUS CHECK

   //Cambio de vista 
   //go BACK
   cambioVISTA(7,"D0");
} 

//? Vista rama E
else if (strcmp(VISTA, "E0") == 0){
   generarVISTA(E0notif,E0notif_f);
   cambioEVENTO("MAIN");
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
      lcd.setCursor(10, i);
      if (addRECshowparam[i] != 99999)
      {
         lcd.print(addRECshowparam[i]);
      }
   }   
}




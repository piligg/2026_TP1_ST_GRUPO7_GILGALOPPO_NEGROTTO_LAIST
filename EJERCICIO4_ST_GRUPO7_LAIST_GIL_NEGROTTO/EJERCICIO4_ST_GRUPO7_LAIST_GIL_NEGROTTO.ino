//Grupo 7-Gil Galoppo, Laist, Negrotto
#include <Adafruit_Sensor.h> 
#include <DHT.h> 
#include <U8g2lib.h> 

#define LED 25 
#define BOTON_SUMAR_SW1 35 
#define BOTON_RESTAR_SW2 34
#define DHTPIN 23
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE); 

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE); //objeto para manejar la pantalla

void print_temp_umbral(); // Muestra solo el umbral
void print_temp_umbral_y_actual(float temp); // Muestra temp actual y umbral
void temp_led(float temp); // Controla el LED

// Definición de estados (máquina de estados)
typedef enum {
RESET,
ESPERA, 
CONFIRMAR, 
SIN_PRESION, 
INCREMENTAR, 
DECREMENTAR, 
SALIDA
} est_t;

est_t temperature = RESET; // Variable de estado inicial

float umbral = 28; // Valor inicial del umbral de temperatura

void setup() {
Serial.begin(9600);

pinMode(LED, OUTPUT); 
pinMode(BOTON_SUMAR_SW1, INPUT); 
pinMode(BOTON_RESTAR_SW2, INPUT); 

u8g2.begin(); // Inicializa pantalla OLED
dht.begin(); // Inicializa sensor DHT

u8g2.clearBuffer(); // Limpia pantalla
digitalWrite(LED, LOW); 
}

void loop() {
Serial.print(" Valor "); 

int SW1 = digitalRead(BOTON_SUMAR_SW1); 
int SW2 = digitalRead(BOTON_RESTAR_SW2); 

float t = dht.readTemperature(); // Lee temperatura
float h = dht.readHumidity(); // Lee humedad

if (isnan(t) || isnan(h)) { // Si t o h no son números (isnan=is not a number)
Serial.println(F("mala lectura")); // Mensaje de error
return; // Sale del loop actual
}

float st = dht.computeHeatIndex(t, h, false); // Calcula sensación térmica en celsius

Serial.println(st); // Muestra st en serial

temp_led(st); // Controla el LED según temperatura

switch (temperature) { // Máquina de estados

case RESET:
Serial.println("RESET");
temperature = ESPERA; // Cambia a espera
break;

case ESPERA:
Serial.println("ESPERA");
print_temp_umbral_y_actual(st); // LLama a la función y muestra temp y umbral en el oled
if ((SW1 == LOW) && (SW2 == LOW)) temperature = CONFIRMAR;
break; 

case CONFIRMAR:
Serial.println("CONFIRMAR");
if ((SW1 == HIGH) && (SW2 == HIGH)) temperature = SIN_PRESION;
break;

case SIN_PRESION:
Serial.println("SIN_PRESION");
print_temp_umbral(); // Muestra solo el umbral
if (SW1 == LOW) temperature = INCREMENTAR; 
else if (SW2 == LOW) temperature = DECREMENTAR;
break;

case INCREMENTAR:
Serial.println("INCREMENTAR");
if (SW1 == HIGH) { // Cuando suelta botón
temperature = SIN_PRESION;
umbral = umbral + 1; // Aumenta el umbral
}
else if (SW2 == LOW) temperature = SALIDA; // Sale si presiona otro
break;

case DECREMENTAR:
Serial.println("DECREMENTAR");
if (SW2 == HIGH) { // Cuando suelta botón
temperature = SIN_PRESION;
umbral = umbral - 1; 
}
else if (SW1 == LOW) temperature = SALIDA; // Sale si presiona otro
break;

case SALIDA:
Serial.println("SALIDA");
if ((SW1 == HIGH) && (SW2 == HIGH)) temperature = ESPERA;
break;
}
}

// Función para mostrar solo el umbral
void print_temp_umbral() {
char stringtemp1[5]; // Variable para texto

u8g2.clearBuffer(); // Limpia pantalla

u8g2.setFont(u8g2_font_ncenB08_tr); // Fuente
u8g2.drawStr(0, 40, "T UMBRAL"); // Texto fijo

sprintf(stringtemp1, "%.2f", umbral); // Convierte umbral a string
u8g2.drawStr(70, 40, stringtemp1); // Muestra valor

u8g2.sendBuffer(); // Actualiza pantalla
}

// Función para mostrar temperatura y umbral
void print_temp_umbral_y_actual(float temp) {
char stringtemp1[5];

u8g2.clearBuffer();

u8g2.setFont(u8g2_font_ncenB08_tr);

u8g2.drawStr(0, 15, "T ACTUAL"); // Texto temp actual
sprintf(stringtemp1, "%.2f", temp); // Convierte el float ‘temp’ a string
u8g2.drawStr(70, 15, stringtemp1);

u8g2.drawStr(0, 40, "T UMBRAL"); // Texto umbral
sprintf(stringtemp1, "%.2f", umbral);
u8g2.drawStr(70, 40, stringtemp1);

u8g2.sendBuffer();
}

// Función que controla el LED según el umbral
void temp_led(float temp) {
if (temp < umbral) digitalWrite(LED, LOW); // Apaga LED si está por debajo
else digitalWrite(LED, HIGH); // Enciende si supera umbral
}






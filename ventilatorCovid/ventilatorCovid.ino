/*
    The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Button 1 (decrease BPM) pin 10
 Button 2 (increase BPM) pin 9
 Button 3 (change ratio) pin 8
 Relay (valve) controller (pin 13)
 */


#include <LiquidCrystal.h>
//Pin's Const declaration / Declaracion de constantes para los pines
const int b1 = 10;
const int b2 = 9;
const int b3 = 8;
const int reley = 13;
//Var Declaration /Declaracion de variables
long tempMillis= 0;         //Used to calculate "tiempo" on each loop / Se usa para calcular "tiempo" en cada loop
long tempo = 0;            //Used to save the valve on/off time / Variable usada para guardar el tiempo que lleva encendida/apagada la valvula
int ratio =0;               //Inhale/Exhale Ratio 1:1 (default) / Relación Inhalar/Exhalar  1:1 por default
int bpm = 8;                //Breaths Cycles Per Minute - Ciclos de respiracion por minuto
int state = 0;             //Indicates the state of valve 0 - off  1 - on  / Indica el estado de la valvula, 0 apagado  1 prendido
int itime = 3750;           //Inhale time-Tiempo de inhalacion
int etime = 3750;           //Exhale time-Tiempo de exhalacion
String ieratio= "1:1";      //lcd text
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //initialize lcd / inicializa la pantalla lcd


void changeTimes() //calculate inhale/exhale time according to BPM and ratio / Calcula los tiempos segun los BPM y la relacion i/e
{
  switch (ratio)
  {
    case 0 : itime = (60000/bpm) / 2;  //ratio 1:1
             etime = (60000/bpm) / 2;
             ieratio="1:1";
             break;
   
    case 1 : itime = (60000/bpm) / 3;  //ratio 1:2
             etime = 2*(60000/bpm) / 3;
             ieratio="1:2";
             break;
   
    case 2 : itime = (60000/bpm) / 4;  //ratio 1:3
             etime = 3*(60000/bpm) / 4;
             ieratio="1:3";
             break;
             
   case 3 : itime = (60000/bpm) / 5;  //ratio 1:4
            etime = 4*(60000/bpm) / 5;
            ieratio="1:4";
            break;
             
   case 4 : itime = 2*(60000/bpm) / 3;  //ratio 2:1
            etime = (60000/bpm) / 3;
            ieratio="2:1";
            break;
  }
}
void changeRatio() // Changes ratio var, possible values: 0 - 4 / Cambia el valor de la variable ratio, los valores posibles son del 0 - 4
{
  if(ratio <4){
    ratio ++;
  }
  else{
    ratio = 0;
  }
  changeTimes();
}
int status_change (int s) //change between inhale and exhale status / Cambia la variable para indicar cuando esta inhalando o exhalando
{
  if(s == 0){
    return 1;
  }
  else{
    return 0;
  }
}
int change_bpm (int s)  // Increase or decrease BPM by 2 in a range between 8 and 40 - Incrementa o decrementa los BPM en un rango de entre 8 y 40
{                       // Rhe function receives a int parameter, 1 to incease, 0 to decrease - Recibe un parametro de 1 para aumentar o 0 para disminuir
  int nbpm = 0;
  if (s == 1){
    if(bpm > 39){
      nbpm = 40;
    }
    else{
      nbpm = bpm + 2;
    }
  }
  
  if (s == 0){
    if(bpm <= 8){
      nbpm = 8;
    }
    else{
      nbpm = bpm - 2;
    }
  }
  return nbpm;
}
void initLCD() //Funciton to print texts on LCD - Funcion para imprimir los textos en la pantalla LCD
{
    lcd.clear();
    lcd.print("Ventilador!");
    lcd.setCursor(0,1);
    lcd.print("BPM:");
    if(bpm < 10){
      lcd.setCursor(5,1);
    }
    else{
      lcd.setCursor(4,1);
    }
//    lcd.setCursor(4,1);
    lcd.print(bpm);
    lcd.setCursor(7,1);
    lcd.print("I/E:");
    lcd.setCursor(11,1);
    lcd.print(ieratio);
}
void setup() {
  //Set buttons and relee - Declarar Botones y reley
  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);
  pinMode(b3, INPUT_PULLUP);
  pinMode(reley, OUTPUT);
  
  // set up the LCD's number of columns and rows: - Setea el numero de filas y columnas del LCD
  
  lcd.begin(16, 2);
  // Print a message to the LCD - Imprime el mensaje inicial en el LCD
  lcd.print("Ventilador!");
  lcd.setCursor(0,1);
  lcd.print("BPM:");
  lcd.setCursor(4,1);
  lcd.print(bpm);
  lcd.setCursor(7,1);
  lcd.print("I/E:");
  lcd.setCursor(11,1);
  lcd.print(ieratio);
}
void loop() {
  tempMillis = millis(); //check miliseconds at the begining - Guarda los milisegundos al inicio del ciclo
//Check status and time spent on that status - Revisa el estado actual y cuanto tiempo lleva en el
  if (state == 0){
    if (tempo >= etime)  {
      state = status_change(state);
      digitalWrite(reley, HIGH);
      tempo = 0;
    }
  }
  else{
    if (tempo >= itime){
      state = status_change(state);
      digitalWrite(reley, LOW);
      tempo = 0;
    }
  }
  //Check if buttons are pressed to modify global values - Revisa si se presionan botones para hacer el cambio de las variables globales
  if(digitalRead(b1)==LOW){
    bpm = change_bpm(0);
    initLCD();
    changeTimes();
    delay(250);
  }
  if(digitalRead(b2)==LOW){
    bpm = change_bpm(1);
    initLCD();
    changeTimes();
    delay(250);
  }
  if(digitalRead(b3)==LOW){
    changeRatio();
    initLCD();
    delay(250);
  }
  delay(100);
  tempo = tempo + (millis() - tempMillis); //calculates time spent in the whole loop - Calcula el tiempo que tardo en completar el ciclo
}

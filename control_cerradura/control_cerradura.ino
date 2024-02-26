#define DIR 8     // pin DIR de A4988 a pin 8
#define STEP 9      // pin STEP de A4988 a pin 9

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

boolean estado_limpieza_pantalla = false;

//variables de pulsador
int boton = 2;
int estado_boton;

//variables de limitadores
int limitador_derecha = 10;
int limitador_izquierda = 11;

int fase; // existen tres fases durante un solo pulso: localizar (100) si se va la luz, luego calibrar (200), girar 180 grados (300) o en limitadores (400 no existente).
boolean inicio_fase_uno; // apagar fases en fase 400.
boolean inicio_fase_dos; // apagar fases en fase 400.
boolean movimiento = false; // comprobar movimiento de bobinas en acciones (metodos) sin bucles.
int posicion_de_leva; //derecha (500) e izquierda (600).

void girar_para_localizar() {
  digitalWrite(STEP, HIGH);
  delayMicroseconds(3000);
  digitalWrite(STEP, LOW);
  delayMicroseconds(3000);
}

void girar_para_calibrar() {
  digitalWrite(DIR, HIGH);          //giro en un sentido
  for(int i = 0; i < 30; i++){      //30 pasos para motor de 0.9 grados de angulo de paso
    digitalWrite(STEP, HIGH);       //nivel alto
    delay(3);                       //por 3 mseg
    digitalWrite(STEP, LOW);        //nivel bajo
    delay(3);                       //por 3 mseg
  }/*
  if (inicio_fase_uno == true) {
    Serial.println("300");
    fase = 300;
  }
  if (inicio_fase_uno == false) {
    digitalWrite(DIR, LOW);
    Serial.println("100");
    fase = 100;
    inicio_fase_uno = true;
  }*/
  fase = 300;
  delay(1000);                      //demora de 1 segundo
}

void girar_180() {
  digitalWrite(DIR, LOW);           //giro en un sentido
  for(int i = 0; i < 100; i++){     //100 pasos para motor de 0.9 grados de angulo de paso
    digitalWrite(STEP, HIGH);       //nivel alto
    delay(6);                       //por 6 mseg
    digitalWrite(STEP, LOW);        //nivel bajo
    delay(6);                       //por 6 mseg
  }
  fase = 100;
  delay(1000);                      //demora de 1 segundo
}

void girar_alejandose_de_limitadores() {
  if (digitalRead(limitador_derecha) == LOW || digitalRead(limitador_izquierda) == LOW) {
    movimiento = true;
    if (movimiento == true) {
      digitalWrite(DIR, HIGH);
      while (digitalRead(limitador_derecha) == LOW || digitalRead(limitador_izquierda) == LOW) {
        digitalWrite(STEP, HIGH);
        delayMicroseconds(3000);
        digitalWrite(STEP, LOW);
        delayMicroseconds(3000);
      }/*
      for(int i =0; i < 5; i++) {
        digitalWrite(STEP, HIGH);
        delay(3);
        digitalWrite(STEP, LOW);
      }*/
      movimiento = false;
      digitalWrite(DIR, LOW);
      fase = 100;
      inicio_fase_uno = true;
    }
  }
}

void setup() {  
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  Serial.println(posicion_de_leva);
  pinMode(limitador_derecha, INPUT_PULLUP);
  pinMode(limitador_izquierda, INPUT_PULLUP);
  pinMode(boton, INPUT);
  digitalWrite(boton, HIGH);
  //digitalWrite(limitador_derecha, HIGH);
  //digitalWrite(limitador_izquierda, HIGH);
  pinMode(DIR, OUTPUT); //pin 8 como salida
  pinMode(STEP, OUTPUT); //pin 9 como salida
  if (digitalRead(limitador_derecha) == HIGH && digitalRead(limitador_izquierda) == HIGH) {
    inicio_fase_uno = true;
    inicio_fase_dos = true;
    fase = 100;
  }
  else {
    inicio_fase_uno = false;
    inicio_fase_dos = false;
    lcd.setCursor(0,0);
    lcd.print("fase uno false");
    lcd.setCursor(0,1);
    lcd.print("fase dos false");
    estado_limpieza_pantalla = true;
    girar_alejandose_de_limitadores();
    //digitalWrite(DIR, LOW);
    //movimiento = false;
    //inicio_fase_uno = true;
    //inicio_fase_dos = true;
  }/**/
  /*
  if (digitalRead(limitador_derecha) == HIGH && digitalRead(limitador_izquierda) == HIGH) {
    inicio_fase_uno = true;
    fase = 100;
  }
  else {
    inicio_fase_uno = false;
  }
  */
  //fase = 100;
}

void loop() {
  if (estado_limpieza_pantalla == true) {
    lcd.clear();
    estado_limpieza_pantalla = false;
  }/*
  lcd.setCursor(0,0);
  lcd.print("fase uno true");
  lcd.setCursor(0,1);
  lcd.print("fase dos true");*/
  
  estado_boton = digitalRead(boton);
  if (fase == 100 && inicio_fase_uno == true/**/) {
    if (digitalRead(limitador_derecha) && digitalRead(limitador_izquierda) == HIGH) {
      if(estado_boton == LOW){
        inicio_fase_dos = true;
        movimiento = true;
      }
      if(movimiento == true) {
        girar_para_localizar();
      }
    }
  }
  
  if (digitalRead(limitador_derecha) == LOW) {
    movimiento = false;
    fase = 200;
    posicion_de_leva = 500;
  }
  
  if (digitalRead(limitador_izquierda) == LOW) {
    movimiento = false;
    fase = 200;
    posicion_de_leva = 600;
  }
  
  if (fase == 200) {
    if (inicio_fase_dos == true) {
      girar_para_calibrar();
    }
    //girar_para_calibrar();
  }
  
  if (fase == 300) {
    girar_180();
  }
}

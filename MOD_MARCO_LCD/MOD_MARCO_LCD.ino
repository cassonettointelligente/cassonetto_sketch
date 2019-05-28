#include <LiquidCrystal_I2C.h>
#include <MFRC522.h> //libreria RFID
#include <Servo.h>
#include <Stepper.h>
#include <SPI.h>
#include "HX711.h"

//Definizione bottoni
#define BUTTON1 22
#define BUTTON2 23
#define BUTTON3 24
#define BUTTON4 25
#define BUTTON5 26
#define BUTTON6 27
#define BUTTON7 28

#define sospensione 64

//Definizione pin per motori servo
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;
Servo servo7;
// Pin sensori ir
int ir1 = 30;
int ir2 = 31;
int ir3 = 32;
int ir4 = 33;
int ir5 = 34;
int ir6 = 35;
int ir7 = 36;
// variabili stato sensori ir
int sir1 ;
int sir2 ;
int sir3 ;
int sir4 ;
int sir5 ;
int sir6 ;
int sir7 ;
//Pin ausigliari
int Gall_umi = 49;
int Gall_ind = 69;
int s_fiamma = 29;
int cicalino = 63;

//rele
int E_valv = 9;
int E_magn = 67;
int trita1 = 68;
int trita2 = 53;

int val1 = 0;
int val2 = 0;
int val3 = 0;
int val4 = 0;
int val5 = 0;
int val6 = 0;
int val7 = 0;
int stato = 0;
int tempo;
int tempo1;
int r = 0 ;

//stati
int sosp;
int incendio;
int ind_pieno;
int umi_pieno;
int arresto = 0;
float giric = 4.5; //giri del carrello
float gdfc;
float girip = 25; //giri della pressa
float gdfp;

//motori passo passo
const int stepsPerRevolution = 200;
Stepper pressa(stepsPerRevolution, 10, 11, 12, 13);
Stepper carrello(stepsPerRevolution, 57, 56, 55, 54);

//celle di carico
HX711 scala1(37, 38);
HX711 scala2(39, 40);
HX711 scala3(41, 42);
HX711 scala4(43, 44);
HX711 scala5(45, 46);
HX711 scala7(47, 48);


float calibration_factor1 = 2042.46;
float units1;
float calibration_factor2 = 2042.46;
float units2;
float calibration_factor3 = 2078.48;
float units3;
float calibration_factor4 = 2024.81;
float units4;
float calibration_factor5 = 2025.17;
float units5;
float calibration_factor7 = 8779.88;
float units7;

MFRC522 rfid(58, 62 ); //pin lettori tessere
LiquidCrystal_I2C lcd(0x27, 20, 4);

String users[] = {"6bd6961b", "2b52891b", "0b32881b", "9baf620a"}; //schede abilitate
int usersSize = sizeof(users) / sizeof(String);

/*
  //Caratteri home-made perchÃ¨ Unicode non Ã¨ swag abbastanza.
  byte okok[8] = {B01010, B01010, B00000, B00100, B00100, B10001, B10001, B01110};
  byte triste[8] = {B00000, B11011, B00000, B00100, B00100, B00000, B01110, B10001};
  byte occhiolino[8] = {B01000, B01011, B00000, B00100, B00100, B10001, B10001, B01110};
*/
byte okok[] = {B01010, B01010, B00000, B00100, B00100, B10001, B10001, B01110};
byte freccia1[] = { B00000, B00000, B00000, B01111, B01111, B00000, B00000, B00000};
byte freccia2[] = { B01000, B01100, B01110, B11111, B11111, B01110, B01100, B01000};
byte chiuso[] = {B00000, B00100, B11111, B11111, B10101, B10101, B10101, B11111};
byte aperto[] = {B00010, B01100, B10000, B11111, B10101, B10101, B10101, B11111};
byte as[] = { B00000, B00000, B00000, B00111, B00100, B00100, B00100, B00100};
byte ad[] = { B00000, B00000, B00000, B11100, B00100, B00100, B00100, B00100};
byte bd[] = { B00100, B00100, B00100, B11100, B00000, B00000, B00000, B00000};
byte bs[] = { B00100, B00100, B00100, B00111, B00000, B00000, B00000, B00000};
byte ver[] = { B00100, B00100, B00100, B00100, B00100, B00100, B00100, B00100};
byte escla[] = { B00000, B00100, B00100, B00100, B00100, B00000, B00100, B00000};
byte fuoco[] = { B00100, B10110, B11110, B10110, B11001, B11101, B11111, B01110};
byte lente[] = { B01110, B10001, B10011, B10111, B01110, B00100, B00100, B00100};
byte raee1[] = { B11000, B10100, B11000, B10100, B00010, B00101, B00111, B00101};
byte raee2[] = { B11000, B10000, B11000, B10011, B11010, B00011, B00010, B00011};
byte errore[] = { B00000, B11011, B11011, B00000, B00000, B01010, B10101, B00000};
byte liquido[] = { B00000, B10001, B11001, B11111, B11111, B11111, B11111, B11111};
byte zigzag[] = { B00000, B00000, B00100, B01010, B10001, B00000, B00000, B00000};
byte enne[] = {B10001, B10001, B11001, B10101, B10011, B10001, B10001, B00000};

//RFID riconoscimento
String getUID() {
  String uid = "";
  for (int i = 0; i < rfid.uid.size; i++) {
    uid += rfid.uid.uidByte[i] < 0x10 ? "0" : "";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  rfid.PICC_HaltA();
  return uid;
}

bool checkUID(String uid) {
  for (int i = 0; i < usersSize; i++) {
    if (users[i] == uid) {
      return true;
    }
  }
  return false;
}

void typewriting(String messaggio) {
  int lunghezza = messaggio.length();
  for (int i = 0; i < lunghezza; i++) {
    lcd.print(messaggio[i]);
    delay(150);
  }
}

void lcdquadratososp() {
  lcd.createChar(0, as);
  lcd.createChar(1, ad);
  lcd.createChar(2, bd);
  lcd.createChar(3, bs);
  lcd.createChar(4, ver);
  lcd.createChar(5, fuoco);
  lcd.createChar(6, escla);

  lcd.setCursor(0, 0);
  lcd.write(0);
  lcd.print("------------------");
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.write(4);
  lcd.print("   SOSPENSIONE    ");
  lcd.write(4);
  lcd.setCursor(0, 2);
  lcd.write(4);
  lcd.print("       ");
  lcd.write(6); lcd.write(6); lcd.write(6); lcd.write(6);
  lcd.print("       ");
  lcd.write(4);
  lcd.setCursor(0, 3);
  lcd.write(3);
  lcd.print("------------------");
  lcd.write(2);
}

void lcdquadratoinc() {
  lcd.createChar(0, as);
  lcd.createChar(1, ad);
  lcd.createChar(2, bd);
  lcd.createChar(3, bs);
  lcd.createChar(4, ver);
  lcd.createChar(5, fuoco);
  lcd.createChar(6, escla);

  lcd.setCursor(0, 0);
  lcd.write(0);
  lcd.print("------------------");
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.write(4);
  lcd.print(" ALLARME INCENDIO ");
  lcd.write(4);
  lcd.setCursor(0, 2);
  lcd.write(4);
  lcd.print("       ");
  lcd.write(5); lcd.write(5); lcd.write(5); lcd.write(5);
  lcd.print("       ");
  lcd.write(4);
  lcd.setCursor(0, 3);
  lcd.write(3);
  lcd.print("------------------");
  lcd.write(2);
}

void lcdquadratoelab() {
  lcd.createChar(0, as);
  lcd.createChar(1, ad);
  lcd.createChar(2, bd);
  lcd.createChar(3, bs);
  lcd.createChar(4, ver);
  lcd.createChar(5, fuoco);
  lcd.createChar(6, escla);

  lcd.setCursor(0, 0);
  lcd.write(0);
  lcd.print("------------------");
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.write(4);
  lcd.print("   ELABORAZIONE   ");
  lcd.write(4);
  lcd.setCursor(0, 2);
  lcd.write(4);
  lcd.print("   ...attendi...  ");
  lcd.write(4);
  lcd.setCursor(0, 3);
  lcd.write(3);
  lcd.print("------------------");
  lcd.write(2);
}

void lcdfail() {
  lcd.createChar(0, enne);
  lcd.createChar(1, zigzag);
  lcd.setCursor(1, 0);
  lcd.print("NON RIESCO A        ");
  lcd.setCursor(0, 1);
  lcd.print("        RICONOSCERTI");
  lcd.setCursor(0, 2);
  lcd.print("  XX                 ");
  lcd.setCursor(0, 3);
  lcd.print("  ");
  lcd.write(1);
  lcd.write(1);
  lcd.print("      *riprova*     ");
}

void lcdriconoscimento() {
  lcd.createChar(0, lente);
  lcd.setCursor(0, 0);
  lcd.print("FATTI RICONOSCERE   ");
  lcd.setCursor(0, 1);
  lcd.print("Avvicina il tuo TAG ");
  lcd.setCursor(0, 2);
  lcd.print("RFID                ");
  lcd.setCursor(0, 3);
  lcd.print("------------------ ");
  lcd.write(0);
}

void lcdbutta() {
  lcd.createChar(0, raee1);
  lcd.createChar(1, raee2);
  lcd.setCursor(0, 0);
  lcd.print("CIAO                "); //lcd.print(nome); lcd.print("!");
  lcd.setCursor(0, 1);
  lcd.print("Cosa vuoi buttare?  ");
  lcd.setCursor(0, 2);
  lcd.print("--------------------");
  lcd.setCursor(0, 3);
  lcd.print("UM ");
  lcd.write(0);
  lcd.write(1);
  lcd.print(" CR PL VT MT IN ");
}

void lcdbuttaaltro() {
  lcd.createChar(0, raee1);
  lcd.createChar(1, raee2);
  lcd.setCursor(0, 0);
  lcd.print("C'e altro           "); //lcd.print(nome); lcd.print("!");
  lcd.setCursor(0, 1);
  lcd.print("per noi?            ");
  lcd.setCursor(0, 2);
  lcd.print("--------------------");
  lcd.setCursor(0, 3);
  lcd.print("UM ");
  lcd.write(0);
  lcd.write(1);
  lcd.print(" CR PL VT MT IN ");
}

void lcdpieno() {
  lcd.createChar(0, errore);
  lcd.createChar(1, liquido);
  lcd.createChar(2, escla);
  lcd.setCursor(0, 0);
  lcd.print("Serbatoio PIENO");
  lcd.write(2);
  lcd.write(2);
  lcd.write(2);
  lcd.print(" ");
  lcd.write(0);
  lcd.setCursor(0, 1);
  lcd.print("Riprova piu tardi   ");
  lcd.setCursor(0, 2);
  lcd.print("                    ");
  lcd.setCursor(0, 3);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
}

void lcdapertochiuso () {
  lcd.createChar(1, chiuso);
  lcd.createChar(2, freccia1);
  lcd.createChar(3, freccia2);
  lcd.createChar(4, aperto);

  lcd.setCursor(0, 3);
  lcd.print("       ");
  lcd.write(1);
  lcd.print(" ");
  lcd.write(2);
  lcd.write(3);
  lcd.print(" ");
  lcd.write(4);
  lcd.print("       ");
}

void intro() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("                    Cassonetto");
  lcd.setCursor(0, 1);
  lcd.print("                   Intelligente");

  for (int positionCounter = 0; positionCounter < 25; positionCounter++) {
    lcd.scrollDisplayRight();
    delay(65);
  }
  delay(3500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     Cassonetto");
  lcd.blink();
  lcd.setCursor(0, 1);
  lcd.print("    Intelligente");
  lcd.setCursor(0, 2);
  lcd.print("Welcome 2 automation");
  lcd.setCursor(0, 3);
  lcd.print("   Parma 05/2019  ");
  lcd.blink();
  delay(6000);
  lcd.noBlink();
  lcd.clear();
}

void reset() {
  scala1.set_scale();
  scala1.tare();
  scala2.set_scale();
  scala2.tare();
  scala3.set_scale();
  scala3.tare();
  scala4.set_scale();
  scala4.tare();
  scala5.set_scale();
  scala5.tare();
  scala7.set_scale();
  scala7.tare();
}

void setup() {
  Serial.begin(9600);
  carrello.setSpeed(300);
  pressa.setSpeed(240);

  servo1.attach(2);
  servo2.attach(3);
  servo3.attach(4);
  servo4.attach(5);
  servo5.attach(6);
  servo6.attach(7);
  servo7.attach(8);

  pinMode(cicalino, OUTPUT);
  pinMode(E_magn, OUTPUT);
  pinMode(E_valv, OUTPUT);
  pinMode(trita1, OUTPUT);
  pinMode(trita2, OUTPUT);

  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  pinMode(BUTTON3, INPUT);
  pinMode(BUTTON4, INPUT);
  pinMode(BUTTON5, INPUT);
  pinMode(BUTTON6, INPUT);
  pinMode(BUTTON7, INPUT);
  pinMode(sospensione, INPUT_PULLUP);
  pinMode(Gall_umi, INPUT_PULLUP);
  pinMode(Gall_ind, INPUT_PULLUP);
  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  pinMode(ir3, INPUT);
  pinMode(ir4, INPUT);
  pinMode(ir5, INPUT);
  pinMode(ir6, INPUT);
  pinMode(ir7, INPUT);
  pinMode(s_fiamma, INPUT);
  digitalWrite(cicalino, 0);
  digitalWrite(E_magn, 0);
  digitalWrite(E_valv, 0);
  digitalWrite(trita1, 0);
  digitalWrite(trita2, 0);
  reset();

  servo1.write(0);
  delay(135);
  servo2.write(0);
  delay(135);
  servo3.write(0);
  delay(135);
  servo4.write(0);
  delay(135);
  servo5.write(0);
  delay(135);
  servo6.write(0);
  delay(135);
  servo7.write(0);

  SPI.begin();
  rfid.PCD_Init();

  //da rivedere: aggiungere, togliere o modificare caratteri
  lcd.begin();
  //lcd.home();
  //intro();
}

void loop () {
  ind_pieno = digitalRead(Gall_ind);
  umi_pieno = digitalRead(Gall_umi);
  val1 = digitalRead(BUTTON1);
  val2 = digitalRead(BUTTON2);
  val3 = digitalRead(BUTTON3);
  val4 = digitalRead(BUTTON4);
  val5 = digitalRead(BUTTON5);
  val6 = digitalRead(BUTTON6);
  val7 = digitalRead(BUTTON7);
  sosp = digitalRead(sospensione);
  incendio = digitalRead (s_fiamma);

  //Allarme incendio
  if (incendio == 0) {
    lcdquadratososp();
    lcd.setCursor(1, 1);
    lcd.print(" ALLARME INCENDIO ");
    lcd.setCursor(1, 2);
    lcd.print("     ");
    lcd.write(5); lcd.print(" "); lcd.write(5); lcd.print(" "); lcd.write(5); lcd.print(" "); lcd.write(5);
    lcd.print("    ");
    digitalWrite(cicalino, 1);
    digitalWrite(E_valv, 1);
    delay(500);
    digitalWrite(E_valv, 0);
    delay(500);
    digitalWrite(cicalino, 0);
    stato = 0;
    delay(450);
  }

  //Sospesione
  if (sosp == 1) {
    lcdquadratososp();
    //reset();
    stato = 0;
  }
  //Normale funzionamento
  else
  {
    //Inizializzazione
    if (stato == 0 && r == 0)
    {
      //lcd.clear();
      lcdriconoscimento();
      if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
      {

        tempo = millis();
        String uid = getUID();
        Serial.println(getUID());
        if (checkUID(uid))
        {
          //lcd.clear(); //INT.DISPLAY
          lcdbutta();
          stato = 1;
          r = 0;
        }
        else {
          lcdfail();
          delay(3500);
          r = 0;
        }
      }
    }
  }
  if (stato >= 2)
  {
    //lcd.clear(); //INT.DISPLAY
    lcdbuttaaltro();
    r = 0;
    tempo1 = millis();
    tempo1 = tempo1 - tempo;
    //Serial.println(tempo1);
    if (tempo1 >= 15000)
    {
      stato = 0;
    }
  }

  val1 = digitalRead(BUTTON1);
  val2 = digitalRead(BUTTON2);
  val3 = digitalRead(BUTTON3);
  val4 = digitalRead(BUTTON4);
  val5 = digitalRead(BUTTON5);
  val6 = digitalRead(BUTTON6);
  val7 = digitalRead(BUTTON7);
  tempo1 = millis();
  tempo1 = tempo1 - tempo;
  if (tempo1 >= 13000)
  {
    stato = 0;
  }

  //1 PULSANTE INDIFFERENZIATO
  if (val1 == 1 && stato >= 1)
  {
    if (ind_pieno == 0)
    {
      gdfc = giric * stepsPerRevolution;

      delay (50);
      lcd.clear(); //INT.DISPLAY
      lcd.setCursor(0, 0);
      lcd.print("    Butta il tuo    ");
      lcd.setCursor(0, 1);
      lcd.print("  INDIFFERENZIATO   ");
      lcdapertochiuso();


      delay (500);
      servo1.write(150);
      delay(2000);
      sir1 = digitalRead(ir1);
      while (sir1 == 0)
      {
        sir1 = digitalRead(ir1);
      }
      delay(2000);
      digitalWrite(E_magn, 1);
      servo1.write(0);
      delay(50);

      lcd.clear(); //INT.DISPLAY
      lcdquadratoelab();
      digitalWrite(trita2, 1);
      delay(3500);
      digitalWrite(trita2, 0);
      delay(2000);
      digitalWrite(trita1, 1);
      delay(500);
      digitalWrite(trita1, 0);
      delay(50);
      carrello.step(gdfc);//I passi "orari" saranno uguali a quelli "antiorari"
      digitalWrite(54, 0);
      digitalWrite(55, 0);
      digitalWrite(56, 0);
      digitalWrite(57, 0);
      delay(250);
      digitalWrite(E_magn, 0);
      delay(5000);
      carrello.step(-gdfc);
      digitalWrite(54, 0);
      digitalWrite(55, 0);
      digitalWrite(56, 0);
      digitalWrite(57, 0);

      float b = 1.35;
      units1 = (random (2, 9)) ;
      units1 = units1 / b;
      Serial.print("Hai buttato: ");
      Serial.print(units1);
      Serial.print(" g di INDIFFERENZIATO");
      Serial.println(" ");

      stato = 2;
      tempo = millis();
    }
    else
    {
      //lcd.clear(); //INT.DISPLAY
      lcdpieno();
      digitalWrite(cicalino, 1);
      delay(500);
      digitalWrite(cicalino, 0);
      delay(500);
      digitalWrite(cicalino, 1);
      delay(500);
      digitalWrite(cicalino, 0);
      delay(500);
      digitalWrite(cicalino, 1);
      delay(500);
      digitalWrite(cicalino, 0);
      delay(1500);
      stato = 2;
    }
  }
  //2 PULSANTE METALLO
  if (val2 == 1 && stato >= 1) {
    delay (50);
    lcd.clear(); //INT.DISPLAY
    lcd.setCursor(0, 0);
    lcd.print("    Butta il tuo    ");
    lcd.setCursor(0, 1);
    lcd.print("       METALLO      ");
    lcdapertochiuso();

    delay (500);
    servo2.write(150);
    delay(2000);
    sir2 = digitalRead(ir2);
    while (sir2 == 0)
    {
      sir2 = digitalRead(ir2);
    }
    delay(2000);
    servo2.write(0);
    delay(20);

    //peso metallo
    scala2.set_scale(calibration_factor2);
    Serial.print("Hai buttato: ");
    units2 = scala2.get_units();
    if (units2 < 0) {
      units2 = 0.00;
    }
    Serial.print(units2);
    Serial.print(" g di METALLO");
    Serial.println(" ");
    scala2.tare();
    tempo = millis();
    stato = 2;
  }
  //3 PULSANTE VETRO
  if (val3 == 1 && stato >= 1) {
    delay (50);
    lcd.clear(); //INT.DISPLAY
    lcd.setCursor(0, 0);
    lcd.print("    Butta il tuo    ");
    lcd.setCursor(0, 1);
    lcd.print("       VETRO        ");
    lcdapertochiuso();

    delay (500);
    servo3.write(140);
    delay(2000);
    sir3 = digitalRead(ir3);
    while (sir3 == 0)
    {
      sir3 = digitalRead(ir3);
    }
    delay(2000);
    servo3.write(0);
    delay(20);
    tempo = millis();
    stato = 2;

    //peso vetro
    scala3.set_scale(calibration_factor3);
    Serial.print("Hai buttato: ");
    units3 = scala3.get_units();
    if (units3 < 0) {
      units3 = 0.00;
    }
    Serial.print(units3);
    Serial.print(" g di VETRO");
    Serial.println(" ");
    scala2.tare();

    tempo = millis();
    stato = 3;
  }
  // 4 PULSANTE PLASTICA
  if (val4 == 1 && stato >= 1) {
    delay(50);
    lcd.clear(); //INT.DISPLAY
    lcd.setCursor(0, 0);
    lcd.print("    Butta la tua    ");
    lcd.setCursor(0, 1);
    lcd.print("      PLASTICA      ");
    lcdapertochiuso();

    delay (500);
    servo4.write(150);
    delay(2000);
    sir4 = digitalRead(ir4);
    while (sir4 == 0)
    {
      sir4 = digitalRead(ir4);
    }
    delay(2000);
    servo4.write(0);
    delay(20);

    //peso vetro
    scala4.set_scale(calibration_factor4);
    Serial.print("Hai buttato: ");
    units4 = scala4.get_units();
    if (units4 < 0) {
      units4 = 0.00;
    }
    Serial.print(units4);
    Serial.print(" g di plastica");
    Serial.println(" ");
    scala4.tare();

    tempo = millis();
    stato = 2;
  }

  //5 PULSANTE CARTA
  if (val5 == 1 && stato >= 1) {

    lcd.clear(); //INT.DISPLAY
    lcd.setCursor(0, 0);
    lcd.print("    Butta la tua    ");
    lcd.setCursor(0, 1);
    lcd.print("       CARTA        ");
    lcdapertochiuso();

    delay (500);
    servo5.write(140);
    delay(2000);
    sir5 = digitalRead(ir5);
    while (sir5 == 0)
    {
      sir5 = digitalRead(ir5);
    }
    delay(2000);
    servo5.write(0);
    delay(150);

    //peso vetro
    scala5.set_scale(calibration_factor5);
    Serial.print("Hai buttato: ");
    units5 = scala5.get_units();
    if (units5 < 0) {
      units5 = 0.00;
    }
    Serial.print(units5);
    Serial.print(" g di CARTA");
    Serial.println(" ");
    scala5.tare();

    tempo = millis();
    stato = 2;
  }
  //6 PULSANTE RAEE
  if (val6 == 1 && stato >= 1) {
    delay (50);

    lcd.clear(); //INT.DISPLAY
    lcd.setCursor(0, 0);
    lcd.print("    Butta il tuo    ");
    lcd.setCursor(0, 1);
    lcd.print("      R.A.E.E.      ");
    lcdapertochiuso();

    delay (500);
    servo6.write(140);
    delay(2000);
    sir6 = digitalRead(ir6);
    while (sir6 == 0)
    {
      sir6 = digitalRead(ir6);
    }
    delay(2000);
    servo6.write(0);
    delay(20);

    tempo = millis();
    stato = 2;
  }
  //7 PULSANTE UMIDO
  if (val7 == 1 && stato >= 1) {
    if (umi_pieno == 0)
    {
      gdfp = girip * stepsPerRevolution;
      delay (50);
      lcd.clear(); //INT.DISPLAY
      lcd.clear(); //INT.DISPLAY
      lcd.setCursor(0, 0);
      lcd.print("    Butta il tuo    ");
      lcd.setCursor(0, 1);
      lcd.print("       UMIDO        ");
      lcdapertochiuso();

      delay (500);
      servo7.write(160);
      delay(2000);
      sir7 = digitalRead(ir7);
      while (sir7 == 0)
      {
        sir7 = digitalRead(ir7);
      }
      delay(2000);
      servo7.write(0);
      delay(2000);

      lcd.clear(); //INT.DISPLAY
      lcdquadratoelab();
      delay(500);
      pressa.step(-gdfp);//I passi "orari" saranno uguali a quelli "antiorari"
      digitalWrite(10, 0);
      digitalWrite(11, 0);
      digitalWrite(12, 0);
      digitalWrite(13, 0);
      delay(1000);
      pressa.step(gdfp);
      digitalWrite(10, 0);
      digitalWrite(11, 0);
      digitalWrite(12, 0);
      digitalWrite(13, 0);

      //peso umido
      scala7.set_scale(calibration_factor7);
      Serial.print("Hai buttato: ");
      units7 = scala7.get_units();
      if (units7 < 0) {
        units7 = 0.00;
      }
      Serial.print(units7);
      Serial.print(" g di UMIDO");
      Serial.println(" ");
      scala7.tare();

      float acaso = 1.35;
      float ph = (random (8, 11)) ;
      ph = ph / acaso;
      Serial.print("ph percolato: ");
      Serial.print(ph);
      Serial.println(" ");

      tempo = millis();
      stato = 2;
    }
    else
    {
      lcd.clear(); //INT.DISPLAY
      lcdpieno();
      digitalWrite(cicalino, 1);
      delay(500);
      digitalWrite(cicalino, 0);
      delay(500);
      digitalWrite(cicalino, 1);
      delay(500);
      digitalWrite(cicalino, 0);
      delay(500);
      digitalWrite(cicalino, 1);
      delay(500);
      digitalWrite(cicalino, 0);
      delay(1500);
      stato = 2;
    }
  }

}
//Serial.println(umi_pieno);
//Serial.println(ind_pieno);

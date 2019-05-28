
unsigned long previousMillis = 0;        // will store last time LED was updated

const long interval = 5000;


void setup() {
  Serial3.begin(115200);
  Serial.begin(115200);
}

void loop() {
  unsigned long currentMillis = millis();
/*
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    String data = "{ \"rfid\": 123456 }";
    Serial3.print(data + "%");

    //Serial3.print("URL/cassonetto/auth.php");
    //Serial3.print("1234%");
  }
  */
  if(Serial.available()){
    Serial3.print(Serial.readString() + "%");
  }
  
  if(Serial3.available()){
    Serial.println(Serial3.readString());
    //QUI ELABORAZIONE
  }
  //Serial.println(stato);
}

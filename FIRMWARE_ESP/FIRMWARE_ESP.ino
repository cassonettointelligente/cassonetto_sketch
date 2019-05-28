/**
   BasicHTTPClient.ino

    Created on: 24.05.2015

*/


#include <Arduino.h>


#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoHttpClient.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;
String stato, dataIn, postData;
String URL0 = "/cassonetto/auth.php";
String URL1 = "/cassonetto/insertdata.php";
int urlScelto=0;
char dato;
IPAddress server(192, 168, 1, 102);

void setup() {

  Serial.begin(115200);

  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  Serial.setTimeout(3000);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("TP-LINK_52EF", "95347226");

}

void loop()

{
  
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED))
  {

    WiFiClient wifi;

    HttpClient client = HttpClient(wifi, server, 80);
    
    if (Serial.available())
    {
      dataIn = Serial.readStringUntil('%');

      if (dataIn.startsWith("URL")) {
        String newURL = dataIn.substring(3);
        if(newURL=="0")
            urlScelto=0;
        if(newURL=="1")
            urlScelto=1;
      }
      else
      {
        postData = dataIn;
        //Serial.println("OK");
        //Serial.println("making POST request");
        String contentType = "application/json";
        //String postData = "{ \"rfid\": " + rfid + " }";
        Serial.println(postData);


        client.beginRequest();
        switch(urlScelto){
          case 0:
            client.post(URL0);
            break;
          case 1:
            client.post(URL1);
            break;
        }
        client.sendHeader("Content-Type", "application/json");
        client.sendHeader("Content-Length", postData.length());
        client.sendHeader("Host", "192.168.1.100");
        client.beginBody();
        client.print(postData);
        client.endRequest();

        // read the status code and body of the response
        int statusCode = client.responseStatusCode();
        String response = client.responseBody();

        Serial.print("Status code: ");
        Serial.println(statusCode);
        Serial.print("Response: ");
        Serial.println(response);

      }
      Serial.flush();
      dataIn="";
      client.flush();
    }
    
    /*
      while (Serial.available())
      {
      delay(15);
      dato = Serial.read();
      stato += dato;
      }
    */

    /*
      String rfid = "123456";

      if (stato == "1234%")
      {
      Serial.println("OK");
      Serial.println("making POST request");
      String contentType = "application/json";
      String postData = "{ \"rfid\": " + rfid + " }";
      Serial.println(postData);


      client.beginRequest();
      client.post("/cassonetto/auth.php");
      client.sendHeader("Content-Type", "application/json");
      client.sendHeader("Content-Length", postData.length());
      client.sendHeader("Host", "192.168.1.100");
      client.beginBody();
      client.print(postData);
      client.endRequest();

      // read the status code and body of the response
      int statusCode = client.responseStatusCode();
      String response = client.responseBody();

      Serial.print("Status code: ");
      Serial.println(statusCode);
      Serial.print("Response: ");
      Serial.println(response);

      Serial.println("Wait five seconds");
      delay(5000);
      }
    */
  }
}

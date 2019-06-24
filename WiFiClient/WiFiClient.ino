#include <WiFi.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <Esp.h>


const char* ssid     = "BT_HQ1";
const char* password = "";


void advertiseServices(const char* MyName)
{
  if(MDNS.begin(MyName))
  {
    Serial.println(F("mDNS responder started."));
    Serial.print(F("I am: "));
    Serial.println(MyName);

    //Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
  }
  else
  {
    while(1)
    {
      Serial.println(F("Error setting up MDNS responder"));
      delay(1000);
    }
  }
}


/*
 * Functie ce returneaza un ID unic in functie de
 * MAC-ul device-ului sau ID-ul cipului integrat.
 */
uint16_t getDeviceId()
{
#if defined(ARDUINO_ARCH_ESP32)
  return ESP.getEfuseMac();
#else
  return ESP.getChipId();
#endif
}


/*
 * makeMine returneaza un nume unic pentru device-ul care ruleaza.
 * Functia rezolva doar sisteme cu un numar mic de device-uri.
 * Pentru multe device-uri foloseste EEPROM-ul serverului si MegunoLink pentru
 * a le atribui nume unice.
 */
String makeMine(const char* nameTemplate)
{
  uint16_t uChipId = getDeviceId();
  String result = String(nameTemplate) + String(uChipId, HEX);

  return result;
}


void setup() 
{
  Serial.begin(9600);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print("Still trying to connect... \n");
  }

  Serial.println("");
  Serial.println("WiFi connected");
    
  Serial.println("Client IP address: \n");
  Serial.println(WiFi.localIP());

  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());

  //Urcam in retea serviciile device-ului si device-ul cu un nume unic
  String myName = makeMine("WiFi Device Client ");
  advertiseServices(myName.c_str());

  Serial.println("\nThe services are advertised on my unique client ID: ");
  Serial.print(myName.c_str());
  
  delay(6000);  
}


void loop() 
{
  char input;

  if(Serial.available())
  {
    
    input = Serial.read();

    /*
     * Simularea primirii unui semnal de comanda    
     * si rezolvarea acesteia prin crearea unui client
     * si rularea unui HTTP GET pe un endpoint al serverului.
     */ 
    if(input == '1')
    {
      Serial.println("\n\nClient: Sunt aici!\n");

      HTTPClient http;

      Serial.print("[HTTP] begin... \n");

      http.begin("http://192.168.0.110/deschide"); //HTTP

      Serial.print("[HTTP GET... \n");

      int httpCode = http.GET();

      if(httpCode > 0)
      {
        Serial.printf("[HTTP] GET... code %d\n", httpCode);

        if(httpCode == HTTP_CODE_OK)
        {
          String payload = http.getString();
          Serial.println(payload);
        }
        else
        {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str()); //For 404 ERROR
        } 

        http.end();
      }
    }

    if(input == '2')
    {
      Serial.println("\nClient: Sunt aici pentru a afla IP-ul serverului!\n");

      HTTPClient http;

      Serial.print("[HTTP] begin... \n");

      http.begin("http://bariera.local/"); //HTTP

      Serial.print("[HTTP GET... \n");

      int httpCode = http.GET();

      Serial.println(httpCode);

      if(httpCode > 0)
      {
        Serial.printf("[HTTP] GET... code %d\n", httpCode);

        if(httpCode == HTTP_CODE_OK)
        {
          String payload = http.getString();
          Serial.println(payload);
        }
        else
        {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str()); //For 404 ERROR
        } 

        http.end();
      }
    }
  }
  
  delay(500);
}

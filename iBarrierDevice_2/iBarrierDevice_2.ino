#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>


String      versiune = "| iBariera ver. 1.01 | TNT Computers SRL | www.tntcomputers.ro | office@tntcomputers.ro |";
 
const char* ssid     = "BT_HQ1";
const char* password = "";

//Portul de comunicare al serverului este 80
WebServer server(80);

//Cele doua relee de pe placuta Olimex se afla la pinii 32 si 33
const int PIN_deschide_bariera = 32;
const int PIN_inchide_bariera  = 33;

const int DURATA_IMPULS        = 2000; //was 5000

char input;
unsigned long loop_counter = 0;


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


void getClientIp(void)
{
  Serial.print("Server: Am primit cerere de la clientul cu IP-ul: ");
  Serial.println(server.client().remoteIP());
  Serial.print("\n");
  
  server.send(200, "text/plain", "Server: Am aflat IP-ul client.");
}


void deschideBariera(void)
{
  digitalWrite(PIN_deschide_bariera, 1);
  
  server.send(200, "text/plain", "Am deschis bariera, Salut Simi!");
  delay(DURATA_IMPULS);
  
  digitalWrite(PIN_deschide_bariera, 0);
}


void inchideBariera(void)
{
  digitalWrite(PIN_inchide_bariera, 1);
  
  server.send(200, "text/plain", "Am inchis bariera");
  delay(DURATA_IMPULS);
  
  digitalWrite(PIN_inchide_bariera, 0);
}


void handleRoot(void) {
  //server.send(200, "text/plain", versiune);
  server.send(200, "text/plain", WiFi.localIP().toString());
  server.send(200, "text/plain", "\nServer: Resolved by my mDNS");
}


void functie_id(void) 
{
  digitalWrite(PIN_deschide_bariera, 1);
  
  server.send(200, "text/plain", "Salut Luci, ID-ul este: ");
  delay(DURATA_IMPULS);
  
  digitalWrite(PIN_deschide_bariera, 0);
}


void handleNotFound(void) 
{
  String message = "File Not Found\n\n";
  
  message += "URI: ";
  message += server.uri();
  
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  
  for (uint8_t i = 0; i < server.args(); i++) 
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  
  server.send(404, "text/plain", message);
}


void updateBIOS(void)
{
    WiFiClient client;
    t_httpUpdate_return ret = httpUpdate.update(client, serverUpdate);
    
    // Or:
    // t_httpUpdate_return ret = httpUpdate.update(client, "server", 80, "file.bin");

    switch (ret) 
    {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
  
  ESP.restart();
}


void StartHTTPServer(void){

  server.on("/", handleRoot);
  //server.send(200, "text/plain", versiune);
  
  server.on("/deschide", deschideBariera);
  
  server.on("/inchide", inchideBariera);

  server.on("/scancode", inchideBariera);
  
  server.on("/afiseaza", inchideBariera);

  server.on("/id", functie_id);

  server.on("/update", updateBIOS);

  server.on("/getClientIp", getClientIp);

  server.onNotFound(handleNotFound);

  server.begin();
  
  Serial.println("HTTP server pornit");
} 

 
void setup(void) 
{
  pinMode(PIN_deschide_bariera, OUTPUT);
  digitalWrite(PIN_deschide_bariera, 0);
  
  pinMode(PIN_inchide_bariera, OUTPUT);
  digitalWrite(PIN_inchide_bariera, 0);

  /*if (MDNS.begin("bariera")) {
    Serial.println("MDNS responder started");
  }*/
  
  //Activarea serviciilor prin care putem lua legatura cu device-urile in functie de numele lor
  advertiseServices("bariera");
  
  
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());

  StartHTTPServer();
  Serial.println("Am terminat setup-ul.");
}


void loop(void) 
{
  server.handleClient();
  
  //Serial.println(WiFi.localIP()); // pentru testare
  
  delay(500);
}

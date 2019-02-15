///Simple http Server who serve SPIFFS
//////////////////////////////////////


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>

IPAddress ip;

///Create server////
AsyncWebServer server(80);
////////////////////
//boolean  init_file = true;
int index_file;
boolean allow_to_read=false;
int numbers_lines_to_erase[50];
//int numbers[50]; //number of line to delete (erase function)
////////////////
void setup() {
  pinMode(BUILTIN_LED,OUTPUT); // Status led
  Serial.begin(115200); //let's start Serial !
  SPIFFS.begin(); // Starting SPIFFS
  setup_wifi("Vot'Stamp", "SFR_06A8", "123456789", "10");
  serveur_html();
  server.begin();
  index_file=0;
//  init_file();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void serveur_html() { // Ici toutes les redirection vers les pages Web

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->redirect("/index.html");
    //delay(100);
  });

  server.onNotFound([](AsyncWebServerRequest * request) {
    request->redirect("/index.html");
    //delay(100);
  });


  server.serveStatic("/", SPIFFS, "/", "max-age=86400");
  /////////////requete SMS
  server.on("/add_sms", HTTP_GET, [](AsyncWebServerRequest * request) {
    String id_value = request->arg("id");
    String time_value = request->arg("time");
    String text_value= request->arg("text");
    add_SMS(index_file,id_value,time_value,text_value);
    index_file++;

    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Enregistrement sms OK" );
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });
  server.on("/allow", HTTP_GET, [](AsyncWebServerRequest * request) {
    allow_to_read=true;
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "autorisation de lire" );
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/erase", HTTP_GET, [](AsyncWebServerRequest * request) {
    init_file();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "effacement du fichier" );
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/allowed", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", String(allow_to_read) );
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
    allow_to_read=false;
  });

  server.on("/blacklist", HTTP_GET, [](AsyncWebServerRequest * request) {
    String erase_lines = request->arg("lines");
    moderation(erase_lines);
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Enregistrement sms OK" );
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });
  server.on("/blacklist_file", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/blacklist.json", "text/json" );
    response->addHeader("Access-Control-Allow-Origin", "*");
    //response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/data.csv", "application/octet-stream" );
    response->addHeader("Access-Control-Allow-Origin", "*");
    //response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });


}

void init_file(){
  File data;
  data = SPIFFS.open("/data.csv", "w");
  data.println("index;id;time;text");
  data.close();
  File blacklist_lines;
  blacklist_lines = SPIFFS.open("/blacklist.json", "w");
  blacklist_lines.println("{\"id\":[");
  blacklist_lines.println("1000]}");
  blacklist_lines.close();
  index_file=0;
}
void add_SMS(int index_line,String identity, String time_stamp, String SMS_text){
  File data;
  data = SPIFFS.open("/data.csv", "a");
  data.println(String(index_line)+";"+String(identity) + ";" + String(time_stamp) + ";" + SMS_text);
  data.close();
}
void moderation(String lines){
  Serial.println("moderation");
  File blacklist_lines;
  String current_line;
  int line_counter=0;
  blacklist_lines = SPIFFS.open("/blacklist.json", "w");
  for (int i=0; i<50;i++){
    numbers_lines_to_erase[i]=0;
  }
  split(lines,",");
  blacklist_lines.println("{\"id\":[");

  for (int i=0; i<50;i++){
    if(numbers_lines_to_erase[i]!=0){
      blacklist_lines.print(numbers_lines_to_erase[i]);
      blacklist_lines.println(",");
      Serial.println(numbers_lines_to_erase[i]);
    }
  }
  blacklist_lines.println("1000]}");
  blacklist_lines.close();
}

void split(String line_to_split_str, char* delimiter){
  // initialize first part (string, delimiter)
  unsigned int bufSize = line_to_split_str.length() + 1; //String length + null terminator
	char* line_to_split = new char[bufSize];
	line_to_split_str.toCharArray(line_to_split, bufSize);

  char* ptr = strtok(line_to_split, delimiter);
  int i=0;
  while(ptr != NULL) {
    i++;
    String ptr_str = String(ptr);
    numbers_lines_to_erase[i] = ptr_str.toInt();
    //Serial.println(numbers_lines_to_erase[i]);
    ptr = strtok(NULL, delimiter);
  }
}

void setup_wifi(String name, String ssid, String mdp, String canal) {
  int timeout=0;
  WiFi.hostname(name.c_str());
  WiFi.begin(ssid.c_str(), mdp.c_str());
  Serial.println("connection en cours...");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(BUILTIN_LED,LOW);
    delay(180);
    timeout++;
    Serial.print(".");
    digitalWrite(BUILTIN_LED,HIGH);
    delay(80);
    if (timeout > 60) { // On reboucle en point d'acces si la connexion a échouée
    Serial.println("TimeOUT");
    digitalWrite(BUILTIN_LED,LOW);
    return;
  }
}
WiFi.softAP(name.c_str(), "", 10);
ip = WiFi.localIP();
Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(ip);
}

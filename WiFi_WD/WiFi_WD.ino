//Add to repo bitbucket 1

#include <ESP8266WiFi.h>
#include <DHT.h>
#include <WiFiClientSecure.h>

// Replace with your network details
const char* ssid = "Setochka";
const char* password = "T+NPX^18";

#define DHTPIN D4
#define DHTTYPE DHT21   // DHT 21 (AM2301)

//account artem.kryhin@gmail.com
const char WEBSITE[] = "api.pushingbox.com"; //pushingbox API server
const String devid = "vCBDA10F84968BAC"; //device ID from Pushingbox 

const int RED = 15;
const int GREEN = 12;
const int BLUE = 13;

const int http_port = 80;


DHT dht(DHTPIN, DHTTYPE, 11);

float h;
float t;
float f;

char hum[6];
char temp[6];
char light[6];

String location;

char http_site;

int counter_COM;
int counter_STAT;
int posted;
int STAT_interval;
int COM_interval;

WiFiServer server(80);

// only runs once on boot
void setup() {
  Serial.begin(115200);
  delay(10);
   
  dht.begin();
  
  pinMode(A0, INPUT);
  
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    analogWrite(RED, 100);
    delay(500);
    analogWrite(RED, 0);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
  analogWrite(RED, 0);
  analogWrite(GREEN, 100);

  STAT_interval = 120000;
  COM_interval = 1000;
  counter_COM=0;
  counter_STAT=STAT_interval-5;
  posted=0;
  

}

void getTemperature() {
  float t;
  float h;
  int l;
  do {
     t = dht.readTemperature();
     dtostrf(t, 3, 2, temp);
     h = dht.readHumidity();
     dtostrf(h, 3, 2, hum);
     l=analogRead(A0);
     dtostrf(l, 3, 2, light);
     delay(100);
   } while (t == 85.0 || t == (-127.0));
}

void loop() {
  // Listenning for new clients
  delay(1);
  WiFiClient client = server.available();

  counter_STAT = counter_STAT + 1;
  if ( counter_STAT == STAT_interval ){
    getTemperature();
    WiFiClient cl;
    if (cl.connect(WEBSITE, 80))
      { 
         cl.print("GET /pushingbox?devid=" + devid
            + "&t=" + (String) temp
            + "&h="      + (String) hum
            + "&l="     + (String) light
            + "&loc="      + (String) location
         );

      cl.println(" HTTP/1.1"); 
      cl.print("Host: ");
      cl.println(WEBSITE);
      cl.println("User-Agent: ESP8266/1.0");
      cl.println("Connection: close");
      cl.println();

          analogWrite(GREEN, 0); 
          delay(300);
          analogWrite(GREEN, 100);      
      }
      posted=posted + 1;
      counter_STAT = 0;
    }
    
  if (client) {
    Serial.println("New client");
    // bolean to locate when the http request ends
    boolean blank_line = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && blank_line) {

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head>");
            
  client.println("<meta charset='utf-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  client.println("<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'>");
  client.println("<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js'></script>");
  client.println("<script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script>");       
            
            client.println("</head><body>");
            client.println("");
            
client.println("<div class='container'>");
client.println("<h1>ESP8266 - Temperature</h1>");
client.println("<ul class='nav nav-tabs'>");
client.println("<li class='active'><a data-toggle='tab' href='#view'>VIEW</a></li>");
client.println("<li><a data-toggle='tab' href='#menu1'>SET UP</a></li>");
client.println("</ul>");

  client.println("<div class='tab-content'>");
  client.println("<div id='view' class='tab-pane fade in active'>");

            client.println("<table>");
            client.println("<tr><td>");
            
            client.println("<h3>Temperature:</h3></td><td><h3><b>");
            client.println(temp);
            client.println("</b>℃</h3>");
            
            client.println("</td></tr><tr><td>");
            
            client.println("<h3>Humidity:</h3></td><td><h3><b>");
            client.println(hum);
            client.println("</b>%</h3>");

            client.println("</td></tr><tr><td>");

            client.println("<h3>Light:</h3></td><td><h3><b>");
            client.println(light);
            client.println("</b>(light from [0 - 1023])</h3>");

            client.println("</td></tr><tr><td>");

            client.println("<h3>The date will be posted via ~ </h3></td><td><h3><b>");
            client.println((STAT_interval-counter_STAT)/1000);
            client.println("</b>sec</h3>");

            client.println("</td></tr><tr><td>");
            
            client.println("<h3>Posted </h3></td><td><h3><b>");
            client.println(posted);
            client.println("</b>times</h3>");
            
            client.println("</td></tr>");
            client.println("</table>");
       
 client.println("</div>");
 client.println("<div id='menu1' class='tab-pane fade'>");
 client.println("<h3>Menu 1</h3>");
 client.println("<p>Some content in menu 1.</p>");
 client.println("</div>");
 client.println("</div>");
 
            client.println("<a href='https://docs.google.com/spreadsheets/d/1LpD0A50fb5p2S3zDntTLVkwz1obk54cMQURvVaVMROU/edit?usp=sharing'>you can review data here</a>");
            client.println("<p>");
            client.println("<a href='https://docs.google.com/spreadsheets/d/1LpD0A50fb5p2S3zDntTLVkwz1obk54cMQURvVaVMROU/pubchart?oid=322524499&format=interactive'>Chart temperature</a>");
            client.println("<p>");
            client.println("<a href='https://docs.google.com/spreadsheets/d/1LpD0A50fb5p2S3zDntTLVkwz1obk54cMQURvVaVMROU/pubchart?oid=1577253036&format=interactive'>Chart ligth</a>");
            client.println("</body></html>");
            
            break;
        }
        if (c == '\n') {
          // when starts reading a new line
          blank_line = true;
        }
        else if (c != '\r') {
          // when finds a character on the current line
          blank_line = false;
        }
      }
    }  
    // closing the client connection
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}   

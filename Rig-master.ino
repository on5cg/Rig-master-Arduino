#include "Arduino.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
//#include <Adafruit_BusIO-master.h>
//#include <Adafruit_SSD1306-master>
//#include <Adafruit_BusIO.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SSD1306.h>
/*
1-01
2-02
3-03
4-04
5-05
6-06
7-07
8-08
9-09
10-0A
11-0B
12-0C
13-0D
14-0E
15-0F
16-10
17-11
18-12
19-13-
20-14
21-15
22-16
23-17
24-18
25-19
*/
#define SCREEN_WIDTH 128  // Largeur de l'écran OLED
#define SCREEN_HEIGHT 32  // Hauteur de l'écran OLED

// Définir l'adresse I2C de l'écran OLED (0x3C est une valeur courante)
//const uint8_t slaveAddress = 0x3C;  // Adresse I2C unique de chaque ESP32 (modifiez-la pour chaque ESP32)
#define OLED_RESET -1  // Il est possible que votre écran n'ait pas de reset, utilisez -1 si c'est le cas
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 afficheur(128, 32, &Wire, -1);


// Configuration réseau
//const char* ssid = "WiFi-2.4-C980";        // SSID de votre réseau Wi-Fi
const char* ssid = "MikroTik-C6FD91";        // SSID de votre réseau Wi-Fi
const char* password = "7bz51ii7";          // Mot de passe de votre réseau Wi-Fi

IPAddress local_IP(192, 168, 1, 188);       // Adresse IP statique
IPAddress gateway(192, 168, 1, 1);          // Adresse de la passerelle
IPAddress subnet(255, 255, 255, 0);         // Masque de sous-réseau

WebServer server(80);

const unsigned long interval = 3000;       // Intervalle de temps entre chaque requête (10 secondes)
unsigned long previousMillis = 0;

String devicesInfo[25];  // Tableau pour stocker les informations des ESP32 esclaves
String ident = "";
String affich = "";
float str_rate = 0;
float str_time = 0;
float Hrate_total = 0;
float sec_total = 0;
float sec_moyenne = 0;
int nbr_esp32_ok = 0;

byte nbr_esp32_HTML_flag = 0;
byte Hrate_total_HTML_flag = 0;

int nbr_esp32_HTML_affich = 0;
int Hrate_total_HTML_affich = 0;

void scanI2CDevices();
void affich_oled();
void handleRoot();


/************************************************************************
*
*  SETUP
*
*************************************************************************/

void setup() {

  Serial.begin(115200);
  Wire.begin();  // Initialiser le bus I2C en tant que maître
  Serial.println("Master started...");

  // Configuration du WiFi
  //WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Configuration du service mDNS
  if (!MDNS.begin("esp32")) {
    Serial.println("Error starting mDNS");
    return;
  }

  // Configuration du serveur web
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

    // Initialisation de la communication I2C avec l'écran OLED
  if(!afficheur.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Échec de l'initialisation de l'écran SSD1306"));
    for(;;); // Boucle infinie si l'écran ne s'initialise pas correctement
  }

    // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  afficheur.display();
  delay(2000); // Pause for 2 seconds



  afficheur.clearDisplay(); // Effacer le tampon de l'écran
  afficheur.setTextSize(2); // Définir la taille du texte
//  display.setTextColor(SSD1306_WHITE);  // Définir la couleur du texte
  afficheur.setTextColor(1);  // Définir la couleur du texte
  afficheur.setCursor(0, 10); // Positionner le curseur pour afficher le texte

  // Afficher le texte
  afficheur.println("----");
  afficheur.setTextSize(2);
  afficheur.println("");
  afficheur.println("----");

  // Afficher le contenu du tampon sur l'écran
  afficheur.display();


}
/************************************************************************
*
*  loop
*
*************************************************************************/

void loop() {

  server.handleClient();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    scanI2CDevices();
    delay(20000);
    affich_oled();
  }

}

/************************************************************************
*
*  reset
*
*************************************************************************/

/************************************************************************
*
*  affich_oled
*
*************************************************************************/

void affich_oled() {
	afficheur.clearDisplay(); // Effacer le tampon de l'écran
	afficheur.setTextSize(2); // Définir la taille du texte
	afficheur.setTextColor(1);  // Définir la couleur du texte
	afficheur.setCursor(0, 10); // Positionner le curseur pour afficher le texte

  // Afficher le texte
	afficheur.print(Hrate_total,3); afficheur.println(" MHs");
//	afficheur.setTextSize(2);
//	afficheur.println("");
  //display.print(sec_moyenne,1);
//	afficheur.print("ESP :"); afficheur.print(nbr_esp32_ok,0); afficheur.println("/25");

	afficheur.display();  // Afficher le contenu du tampon sur l'écran
}


/************************************************************************
*
*  scanI2CDevices : sera utilisé pour les rst
*
*************************************************************************/
void scanI2CDevices() {
  for (byte address = 1; address <= 25; address++) {
//  for (byte address = 0x01; address <= 0x19; address++) {
    // convertir 'address' en hexa
    if (Wire.endTransmission() == 0) {  // Si l'adresse répond
      Wire.requestFrom(address, 40);  // Demande à l'esclave de lui envoyer son identifiant (40 caractères)
      String ident = "";
      while (Wire.available()) {
        char c = Wire.read();  // Lire les caractères envoyés par l'esclave
        ident += c;
        //Serial.println(ident);
      }
//      Serial.println(ident.length());
//      Serial.println(ident);
      if (ident.length() >= 8) {
        //Serial.println(address);
        int position = (ident.substring(6, 8)).toInt();  // Extraire les 7ème et 8ème caractères pour déterminer la position
        affich = ident.substring(9, 35);
        str_rate = (ident.substring(15, 21)).toFloat();   // extrait le Hrate     // faire un reset si le rate est 0
        str_time =  (ident.substring(28, 32)).toFloat();  // extrait le temps

        Hrate_total += str_rate/1000;                   // Hrate en MH/s
        sec_total = sec_total + str_time;
        nbr_esp32_ok = nbr_esp32_ok + 1;
        if (position >= 1 && position <= 25) {
          devicesInfo[position - 1] = affich;  // Stocker l'identifiant à la position correspondante (index de 0 à 24)
        }
      }
    }
  }

  sec_moyenne = sec_total/nbr_esp32_ok;    // calcul temps moyen

/*  Serial.print(Hrate_total,3); Serial.print("  -  nbr_ESP : "); Serial.print(nbr_esp32_ok,0);   Serial.print("  -  Sec_total : "); Serial.println(sec_moyenne,2) ;
  Serial.println("-") ;Serial.println("-") ;Serial.println("-") ;
*/
  if (nbr_esp32_HTML_flag == 0) {nbr_esp32_HTML_affich = nbr_esp32_ok;}
  if (Hrate_total_HTML_flag == 0) {Hrate_total_HTML_affich = Hrate_total;}

  nbr_esp32_HTML_flag = 1;
  Hrate_total_HTML_flag = 1;
}




/************************************************************************
*
*  PAGE WEB
*
*************************************************************************/
void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
      <head>
        <title>DUCO-rig ESP32</title>
        <style>
          body {
            background-color: yellow;
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
          }
          h1 {
            text-align: center;
            margin-top: 20px;
          }
          .line {
            margin-left: 20px;
            margin-top: 10px;
            font-size: 18px;
          }
        </style>
      </head>
      <body>
        <h1>DUCO-rig ESP32</h1>
        <div>
  )rawliteral";

  for (int i = 1; i <= 25; i++) {
    html += "<div class='line'>" + String(i < 10 ? "0" : "") + String(i) + " - </div>";
  }

  html += R"rawliteral(
        </div>
      </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

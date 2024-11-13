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

#define SCREEN_WIDTH 128  // Largeur de l'écran OLED
#define SCREEN_HEIGHT 64  // Hauteur de l'écran OLED

// Définir l'adresse I2C de l'écran OLED (0x3C est une valeur courante)
//const uint8_t slaveAddress = 0x3C;  // Adresse I2C unique de chaque ESP32 (modifiez-la pour chaque ESP32)
#define OLED_RESET -1  // Il est possible que votre écran n'ait pas de reset, utilisez -1 si c'est le cas
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 afficheur(128, 64, &Wire, -1);


// Configuration réseau
const char* ssid = "WiFi-2.4-C980";        // SSID de votre réseau Wi-Fi
const char* password = "7bz51ii7";          // Mot de passe de votre réseau Wi-Fi
//const char* ssid = "MikroTik-C6FD91";        // SSID de votre réseau Wi-Fi
//const char* password = "7bz51ii7";          // Mot de passe de votre réseau Wi-Fi
IPAddress local_IP(192, 168, 1, 190);       // Adresse IP statique
IPAddress gateway(192, 168, 1, 1);          // Adresse de la passerelle
IPAddress subnet(255, 255, 255, 0);         // Masque de sous-réseau

WebServer server(80);

const unsigned long interval = 4000;       // Intervalle de temps entre chaque requête (4 secondes)
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

#define RESET_ESP01 4
#define RESET_ESP02 5
#define RESET_ESP03 12
#define RESET_ESP04 13
#define RESET_ESP05 14
#define RESET_ESP06 15
#define RESET_ESP07 16
#define RESET_ESP08 17
#define RESET_ESP09 18
#define RESET_ESP10 19
#define RESET_ESP11 25
#define RESET_ESP12 27

void scanI2CDevices();
void affich_oled();
void handleRoot();


/************************************************************************
*
*  SETUP
*
*************************************************************************/

void setup() {
  pinMode(RESET_ESP01, OUTPUT);   digitalWrite(RESET_ESP01, HIGH);
  pinMode(RESET_ESP02, OUTPUT);   digitalWrite(RESET_ESP02, HIGH);
  pinMode(RESET_ESP03, OUTPUT);   digitalWrite(RESET_ESP03, HIGH);
  pinMode(RESET_ESP04, OUTPUT);   digitalWrite(RESET_ESP04, HIGH);
  pinMode(RESET_ESP05, OUTPUT);   digitalWrite(RESET_ESP05, HIGH);
  pinMode(RESET_ESP06, OUTPUT);   digitalWrite(RESET_ESP06, HIGH);
  pinMode(RESET_ESP07, OUTPUT);   digitalWrite(RESET_ESP07, HIGH);
  pinMode(RESET_ESP08, OUTPUT);   digitalWrite(RESET_ESP08, HIGH);
  pinMode(RESET_ESP09, OUTPUT);   digitalWrite(RESET_ESP09, HIGH);
  pinMode(RESET_ESP10, OUTPUT);   digitalWrite(RESET_ESP10, HIGH);
  pinMode(RESET_ESP11, OUTPUT);   digitalWrite(RESET_ESP11, HIGH);
  pinMode(RESET_ESP12, OUTPUT);   digitalWrite(RESET_ESP12, HIGH);


  Serial.begin(115200);
  Wire.begin();  // Initialiser le bus I2C en tant que maître
  Serial.println("Master started...");

  // Configuration du WiFi
  WiFi.config(local_IP, gateway, subnet);
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
/*  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Échec de l'initialisation de l'écran SSD1306"));
    for(;;); // Boucle infinie si l'écran ne s'initialise pas correctement
  }*/

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
	afficheur.setTextSize(2);
	afficheur.println("");
  //display.print(sec_moyenne,1);
	afficheur.print("ESP :"); afficheur.print(nbr_esp32_ok,0); afficheur.println("/25");

	afficheur.display();  // Afficher le contenu du tampon sur l'écran
}


/************************************************************************
*
*  scanI2CDevices
*
*************************************************************************/
void scanI2CDevices() {
  for (byte address = 1; address <= 30; address++) {
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

void handleRoot() {   // Fonction pour gérer la racine de la page web
 // String html = "<html><head><title>DUCO-rig ESP32</title>";
String html PROGMEM =  "<html><head><title>DUCO-rig ESP32</title>";
/*  html += "<script>";
  html += "var timeLeft = 10;";  // Décompt Serial.print("i+1= ");    Serial.println(i+1);e de 3 secondes
  html += "function countdown() {";
  html += "  document.getElementById('countdown').innerHTML = 'Refresh in ' + timeLeft + ' seconds';";
  html += "  document.getElementById('progressBar').value = timeLeft;";  // Met à jour la barre de progression
  html += "  if (timeLeft <= 0) {";
  html += "    clearInterval(timerId);";
  html += "    location.reload();";  // Rafraîchit la page après le décompte
  html += "  } else {";
  html += "    timeLeft--;";  // Décrémente le temps restant
  html += "  }";
  html += "}";
*/
//  html += "var timerId = setInterval(countdown, 1000);";  // Déclenche le décompte chaque seconde
//  html += "</script>";
  html += "</head><body onload='countdown()' style='background-color: #FFFFE0; position: relative; min-height: 100vh;'>";  // Fond jaune pâle (#FFFFE0)

  html += "<div style='text-align: center;'>";  // Centrer le titre et la variable
//  html += "<h1>DUCO-rig : " + String(nbr_esp32_HTML_affich) + " x ESP32 -> " + String(Hrate_total_HTML_affich, 3) + " MHs</h1>";  // Nouveau titre
  html += "<h1>DUCO-rig : " + String(nbr_esp32_HTML_affich) + " x ESP32 -> " + String(Hrate_total, 3) + " MHs</h1>";  // Nouveau titre

  html += "<p style='text-align: center;'>&copy; 2024 ON5CG</p>";  // Copyright centré
  html += "<div style='display: grid; grid-template-columns: repeat(5, 1fr); gap: 10px;'>";

  for (int i = 0; i < 25; i++) {
    Serial.print(i+1); Serial.print(" -> deviceinfo : "); Serial.println(devicesInfo[i]);
    html += "<div style='border: 1px solid black; padding: 20px; text-align: center;'>";
    html += "ESP " + String(i + 1) + "<br>";  // Numérote les positions de 1 à 25
    if ((devicesInfo[i] != "")  or (devicesInfo[i].substring(17, 18) !="0")){
      html += devicesInfo[i];  // Affiche la chaîne de caractères recueillie
      Serial.print(i+1); Serial.print(" -> substring = "); Serial.println(devicesInfo[i].substring(17, 18));
    } //else
    if (devicesInfo[i].substring(17, 18) ="0"){

      //html += "-----------";  // prévoir de calculer le nombre de reset
      Serial.print("i= "); Serial.print(i);
      // Utiliser switch...case
      switch (i+1){
/*        case 1:
          Serial.print("reset du ESP "); Serial.println(i+1);
          html += "--RST 01--";  // prévoir de calculer le nombre de reset
          digitalWrite(RESET_ESP01, LOW); //reset, prévoir reset spécifique esp32 'i+1'
          delay(20);
          digitalWrite(RESET_ESP01, HIGH);
          break;

        case 2:
          Serial.print("reset du ESP "); Serial.println(i+1);
          html += "--RST 02--";  // prévoir de calculer le nombre de reset
          digitalWrite(RESET_ESP02, LOW); //reset, prévoir reset spécifique esp32 'i+1'
          delay(20);
          digitalWrite(RESET_ESP02, HIGH);
          break;

        case 3:
          Serial.print("reset du ESP "); Serial.println(i+1);
          html += "--RST 03--";  // prévoir de calculer le nombre de reset
          digitalWrite(RESET_ESP03, LOW); //reset, prévoir reset spécifique esp32 'i+1'
          delay(20);
          digitalWrite(RESET_ESP03, HIGH);
          break;
*/
        case 4:
          Serial.print("reset du ESP "); Serial.println(i+1);
          html += "--RST 04--";  // prévoir de calculer le nombre de reset
          digitalWrite(RESET_ESP04, LOW); //reset, prévoir reset spécifique esp32 'i+1'
          delay(50);
          digitalWrite(RESET_ESP04, HIGH);
          break;
/*
        case 5:
          Serial.print("reset du ESP "); Serial.println(i+1);
          html += "--RST 05--";  // prévoir de calculer le nombre de reset
          digitalWrite(RESET_ESP05, LOW); //reset, prévoir reset spécifique esp32 'i+1'
          delay(20);
          digitalWrite(RESET_ESP05, HIGH);

        case 6:
          Serial.print("reset du ESP "); Serial.println(i+1);
          html += "--RST 06--";  // prévoir de calculer le nombre de reset
          digitalWrite(RESET_ESP06, LOW); //reset, prévoir reset spécifique esp32 'i+1'
          delay(20);
          digitalWrite(RESET_ESP06, HIGH);
          break;

        case 7:
          Serial.print("reset du ESP "); Serial.println(i+1);
          html += "--RST 07--";  // prévoir de calculer le nombre de reset
          digitalWrite(RESET_ESP07, LOW); //reset, prévoir reset spécifique esp32 'i+1'
          delay(20);
          digitalWrite(RESET_ESP07, HIGH);
          break;

        case 8:
          Serial.print("reset du ESP "); Serial.println(i+1);
          html += "--RST 08--";  // prévoir de calculer le nombre de reset
          digitalWrite(RESET_ESP08, LOW); //reset, prévoir reset spécifique esp32 'i+1'
          delay(20);
          digitalWrite(RESET_ESP08, HIGH);
          break;

        case 9:
          Serial.print("reset du ESP "); Serial.println(i+1);
          html += "--RST 09--";  // prévoir de calculer le nombre de reset
          digitalWrite(RESET_ESP09, LOW); //reset, prévoir reset spécifique esp32 'i+1'
          delay(20);
          digitalWrite(RESET_ESP09, HIGH);

        case 10:
          Serial.print("reset du ESP "); Serial.println(i+1);
          html += "--RST 10--";  // prévoir de calculer le nombre de reset
          digitalWrite(RESET_ESP10, LOW); //reset, prévoir reset spécifique esp32 'i+1'
          delay(20);
          digitalWrite(RESET_ESP10, HIGH);
          break;

        case 11:
          Serial.print("reset du ESP "); Serial.println(i+1);
          html += "--RST 11--";  // prévoir de calculer le nombre de reset
          digitalWrite(RESET_ESP11, LOW); //reset, prévoir reset spécifique esp32 'i+1'
          delay(20);
          digitalWrite(RESET_ESP11, HIGH);
          break;

        case 12:
          Serial.print("reset du ESP "); Serial.println(i+1);
          html += "--RST 12--";  // prévoir de calculer le nombre de reset
          digitalWrite(RESET_ESP12, LOW); //reset, prévoir reset spécifique esp32 'i+1'
          delay(20);
          digitalWrite(RESET_ESP12, HIGH);
          break;
*/
        default:
          Serial.println("Valeur inattendue");
          break;
        }



    }
    html += "</div>";
  }
  // décompte et barre de progression
  /*
  html += "</div>";
  html += "<div style='position: absolute; bottom: 10px; width: 100%; text-align: center;'>";
  html += "<p id='countdown' style='display: inline-block; margin-right: 10px;'></p>";  // Décompte à gauche de la barre de progression
  html += "<progress id='progressBar' value='0' max='10'></progress>";  // Barre de progression
  html += "</div>";
  */

  html += "</body></html>";

  server.send(200, "text/html", html);
  nbr_esp32_HTML_flag = 0;
  Hrate_total_HTML_flag = 0;
}
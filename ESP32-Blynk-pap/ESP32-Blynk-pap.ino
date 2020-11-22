
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

//#include <IRremote.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "yourauth";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "yourssid";
char pass[] = "yourpass";

int x=512 ;
int y=512 ;
int speedtilt ;
int speedpan ;

BLYNK_WRITE(V1) {
  y = param[0].asInt();
  x = param[1].asInt();
  
  // Do something with x and y
  Serial.print("X = ");
  Serial.print(x);
  Serial.print("; Y = ");
  Serial.println(y);
}
// Constantes
const int adresse = 1; //configurer ici le N° de tourelle de 1 a 4

const int tiltdirpin = 12;
// la pin pour la LED IR est forcément la 3
const int pandirpin = 21;
const int tiltsteppin = 13;
const int pansteppin = 22;
const int nenable = 15;

int camid = 1; //N° de la tourelle gérée, par défaut la N°1
bool zoom;
bool dezoom ;
bool senstilt ;
bool senspan ;
int camled;
int pertilt;
int perpan;
unsigned long curmillis ;
unsigned long lastmillis ;
unsigned long previousTiltMillis = 0;
unsigned long previousPanMillis = 0;
unsigned long previousRadioMillis = 0;
unsigned long deltaPanMillis = 0;
int tiltstepstate;
int panstepstate;

void setup()
{
  // Debug console
  Serial.begin(115200);
  Serial.println("Setup");
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // config I/O
  pinMode(tiltdirpin, OUTPUT);
  pinMode(pandirpin, OUTPUT);
  pinMode(tiltsteppin, OUTPUT);
  pinMode(pansteppin, OUTPUT);
  pinMode(nenable, OUTPUT);
}

void loop()
{
  Blynk.run();
  
unsigned long currentMillis = micros();
speedtilt = map(x, 0, 1024, -512, 512);
speedpan = map(y, 0, 1024, -512, 512);

  if (speedtilt < 0) {
    speedtilt = -1 * speedtilt;
    senstilt = 1;
  }
  else {
    senstilt = 0;
  }
  if(speedtilt > 512){
    speedtilt=512;
  } 

  if (speedpan < 0) {
    speedpan = -1 * speedpan;
    senspan = 1; 
  }
  else {
    senspan = 0;
  }
  if(speedpan > 512){
    speedpan=512;
  }

 // if ( curentMillis - previousRadioMillis > 1000) {
 //   previousRadioMillis = currentMillis;

// Gestion du moteur haut/bas
    if (speedtilt > 70) {
      digitalWrite(nenable, LOW);
      if (senstilt == 1) {
        digitalWrite(tiltdirpin, HIGH);
      }
      else {
        digitalWrite(tiltdirpin, LOW);
      }
      // commande impulsions non bloquantes
      pertilt = map (speedtilt, 1, 512, 20000, 2000);
      if ((currentMillis - previousTiltMillis) > pertilt)
      {
        previousTiltMillis = currentMillis;
        if (tiltstepstate == LOW) {
          tiltstepstate = HIGH;
        }
        else {
          tiltstepstate = LOW;
        }
        digitalWrite(tiltsteppin, tiltstepstate);
      }
    }

    // Gestion du moteur gauche/droite
    if (speedpan > 70) {
      digitalWrite(nenable, LOW);
      if (senspan == 1) {
        digitalWrite(pandirpin, HIGH);
      }
      else {
        digitalWrite(pandirpin, LOW);
      }
      // commande impulsions non bloquantes
      perpan = map (speedpan, 1, 512, 15000, 800);
      deltaPanMillis = currentMillis - previousPanMillis ;
      if ( deltaPanMillis> perpan)
      {
        
        if (panstepstate == LOW) {
          panstepstate = HIGH;
        }
        else {
          panstepstate = LOW;
        }
        digitalWrite(pansteppin, panstepstate);
        previousPanMillis = currentMillis;
      }
  }
  //On desactive les moteurs si inutilises pour eviter la chauffe
     if (speedpan <10 && speedtilt < 10) {
        digitalWrite(nenable, HIGH);
     }
  // Données pour le moniteur série
     
}

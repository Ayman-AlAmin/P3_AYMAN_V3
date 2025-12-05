#include <Arduino.h>
#include <WiFi.h>

// Information de connexion WiFi
const char* ssid = "Ayman";
const char* password = "12345678"; // PAS SUR A REVOIR!!!!!!!!!!!!!!

// print interval
unsigned long lastIpPrint = 0;
//const unsigned long ipPrintInterval = 5000; // 5 seconds

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Connect to WiFi
  Serial.println();
  Serial.print("Connexion au WiFi: ");
  Serial.println(ssid);
  
  // Début de la connexion au WiFi de l'école
  WiFi.begin(ssid, password);

  // Attente de connection avec timeout simple
  unsigned long startAttemptTime = millis();
  const unsigned long timeoutMs = 15000; // 15 seconds max

  while (WiFi.status() != WL_CONNECTED && (millis() - startAttemptTime) < timeoutMs) {
    delay(500);
    Serial.print(".");
  }

  // Affichage de l'IP quand connexion réussite 
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Erreur: impossible de se connecter au WiFi. Verifiez le reseau ou le mot de passe.");
    return;
  }

  // Affichage de l'IP quand connexion réussite 
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  lastIpPrint = millis();
}

void loop() {
  unsigned long now = millis();

  if (now - lastIpPrint >= 5000) {  
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("WiFi not connected");
    }
    lastIpPrint = now;
  }

  delay(100);
}

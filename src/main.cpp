#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h> // ajout pour faire la requête HTTP

// Information de connexion WiFi
const char* ssid = "Ayman";
const char* password = "12345678"; // PAS SUR A REVOIR!!!!!!!!!!!!!!

// API (remplace par ton endpoint)
const char* apiUrl = "http://api.open-notify.org/iss-now.json";

// print interval
unsigned long lastIpPrint = 0;
const unsigned long ipPrintInterval = 5000; // 5 seconds

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

  if (now - lastIpPrint >= ipPrintInterval) {  
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());

      // GET request à l'API et lecture de certains headers
      HTTPClient http;

      // Déclare les headers que l'on souhaite lire
      const char* headerKeys[] = { "Content-Type", "Content-Length", "Server", "Date", "Connection" };
      const int headerCount = sizeof(headerKeys) / sizeof(headerKeys[0]);

      http.begin(apiUrl);
      // Indique à HTTPClient de collecter ces headers dans la réponse
      http.collectHeaders(headerKeys, headerCount);

      int httpCode = http.GET(); // on fait la requête
      if (httpCode > 0) {
        // La requête a bien reçu une réponse (httpCode contient le code HTTP)
        Serial.print("HTTP response code: ");
        Serial.println(httpCode);

        if (httpCode >= 200 && httpCode < 300) {
          Serial.println("Request successful (2xx).");
        
        } else if (httpCode >= 400 && httpCode < 500) {
          Serial.println("Client error (4xx).");
        } else if (httpCode >= 500) {
          Serial.println("Server error (5xx).");
        } else {
          Serial.println("Unexpected HTTP status code.");
        }

        // Affiche les headers collectés
        Serial.println("Response headers:");
        for (int i = 0; i < headerCount; ++i) {
          String value = http.header(headerKeys[i]);
          if (value.length() == 0) value = "<not present>";
          Serial.print("  ");
          Serial.print(headerKeys[i]);
          Serial.print(": ");
          Serial.println(value);
        }
      } else {
        // httpCode <= 0 : erreur lors de la requête (connexion, DNS, timeouts...)
        Serial.print("HTTP request failed: ");
        Serial.println(http.errorToString(httpCode));
      }
      http.end();
    } else {
      Serial.println("WiFi not connected");
    }
    lastIpPrint = now;
  }

  delay(100);
}

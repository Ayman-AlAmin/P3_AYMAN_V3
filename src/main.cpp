#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> 

// Information de connexion WiFi
const char* ssid = "IOT-6220";
const char* password = "6220M@cSelection"; // PAS SUR A REVOIR!!!!!!!!!!!!!!

// API (remplace par ton endpoint)
const char* apiUrl = "http://api.open-notify.org/iss-now.json";

// print interval
unsigned long lastIpPrint = 0;
const unsigned long ipPrintInterval = 5000; // 5 seconds

// Structures pour contenir la réponse JSON
struct IssPosition {
  String latitude;
  String longitude;
};

struct IssResponse {
  String message;
  long timestamp;
  IssPosition iss_position;
};

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

      // GET request à l'API, lecture des headers et du corps
      HTTPClient http;

      const char* headerKeys[] = { "Content-Type", "Content-Length", "Server", "Date", "Connection" };
      const int headerCount = sizeof(headerKeys) / sizeof(headerKeys[0]);

      http.begin(apiUrl);
      http.collectHeaders(headerKeys, headerCount);

      int httpCode = http.GET(); // on fait la requête
      if (httpCode > 0) {
        Serial.print("HTTP response code: ");
        Serial.println(httpCode);

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

        // Lit le corps de la réponse
        String payload = http.getString();
        Serial.println("Response payload:");
        if (payload.length() > 0) {
          Serial.println(payload);
        } else {
          Serial.println("<empty body>");
        }

        // Désérialise le JSON dans une structure et affiche chaque champ
        // Ajuster la taille du document si nécessaire; 512 suffit pour cette réponse
        StaticJsonDocument<512> doc;
        DeserializationError err = deserializeJson(doc, payload);
        if (err) {
          Serial.print("JSON deserialization failed: ");
          Serial.println(err.c_str());
        } else {
          IssResponse resp;
          // message
          if (doc.containsKey("message")) resp.message = String((const char*)doc["message"]);
          else resp.message = "<absent>";

          // timestamp
          if (doc.containsKey("timestamp")) resp.timestamp = doc["timestamp"].as<long>();
          else resp.timestamp = 0;

          // iss_position
          JsonObject pos = doc["iss_position"].as<JsonObject>();
          if (!pos.isNull()) {
            if (pos.containsKey("latitude")) resp.iss_position.latitude = String((const char*)pos["latitude"]);
            else resp.iss_position.latitude = "<absent>";
            if (pos.containsKey("longitude")) resp.iss_position.longitude = String((const char*)pos["longitude"]);
            else resp.iss_position.longitude = "<absent>";
          } else {
            resp.iss_position.latitude = "<absent>";
            resp.iss_position.longitude = "<absent>";
          }

          // Affiche chaque champ séparément
          Serial.println("Parsed JSON fields:");
          Serial.print("  message: ");
          Serial.println(resp.message);
          Serial.print("  timestamp: ");
          Serial.println(resp.timestamp);
          Serial.println("  iss_position:");
          Serial.print("    latitude: ");
          Serial.println(resp.iss_position.latitude);
          Serial.print("    longitude: ");
          Serial.println(resp.iss_position.longitude);
        }

      } else {
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

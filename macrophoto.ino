#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>  // Pour ESP32
#include <math.h> // Inclure la bibliothèque math pour utiliser floor
#include "appWeb.h" // Inclusion du fichier HTML contenant l'interface utilisateur
#include "gestionMoteur.h" // Inclusion du fichier de gestion du moteur

// Informations pour la configuration Wi-Fi
const char* ssid = "MicroMotion";          // Nom du réseau Wi-Fi
const char* password = "123456789";  // Mot de passe du réseau Wi-Fi

// Définition de la broche pour le relais
#define RELAY_PIN 13

// Variables globales
double avanceMillimetre;       // Distance à parcourir (en mm)
int nbrPhotos = 0;             // Nombre total de photos à prendre
int photosRealisees = 0;       // Compteur de photos prises
bool enCours = false;          // Indique si une séquence est en cours
long debutPosition = 0;        // Position initiale du moteur
long finPosition = 0;        // Position finale du moteur
long positionActuelle = 0;     // Position actuelle du moteur
double distanceParcourue = 0.0; // Distance totale parcourue en mm
float erreurAccumulee = 0.0;   // Correction pour arrondir les pas
int currentPhoto = 0;         // Compteur de photos actuellement prises
bool lastPhotoDelay = false;  // Indique si on attend après la dernière photo

// Variables de gestion du temps
unsigned long previousMillis = 0; // Dernière fois qu'une action a été effectuée
unsigned long intervalDeclenchement = 2000;    // attente après le déclenchement (en ms)
unsigned long intervalMoteur = 4000;    //  attente après le mouvement du moteur (en ms)
int sequenceState = 0;            // État de la séquence : 0=Relais, 1=Temporisation, 2=Moteur, 3=Pause

// Serveur Web pour l'interface utilisateur
WebServer server(80);

// *** Fonctions ***

// Page principale (HTML) pour l'interface utilisateur
void handleRoot() {
  server.send(200, "text/html", htmlPage); // Envoi de la page HTML au client
}

// Fonction pour tester le déclencheur (relais)
void handleTestShutter() {
  Serial.println("Test shutter activé");
  digitalWrite(RELAY_PIN, HIGH); // Activer le relais
  delay(200);                    // Maintenir l'état pendant 200 ms
  digitalWrite(RELAY_PIN, LOW);  // Désactiver le relais
  server.send(200, "text/plain", "Test shutter effectué"); // Confirmation au client
}

// Stopper la séquence en cours et revenir à la position initiale
void handleStop() {
  Serial.println("Requête STOP reçue");
  enCours = false;

  long stepsToDebut = debutPosition - positionActuelle;
  avancerMoteur((stepsToDebut * pasDeVis) / stepsPerRevolution);
  positionActuelle = debutPosition;

  Serial.println("Retour à la position de début effectué");
  server.send(200, "text/plain", "Séquence arrêtée");
}

void handleGetIntervalMoteur() {
  // Envoyer la valeur de intervalMoteur en secondes
  server.send(200, "text/plain", String(intervalMoteur / 1000)); // Convertir en secondes et limiter à 1 décimale
}

// Mettre à jour l'intervalle du moteur
void handleSetIntervalMoteur() {
  if (server.hasArg("interval")) { // Vérifier si le paramètre "interval" est fourni
    unsigned long newInterval = server.arg("interval").toInt() * 1000; // Convertir la valeur saisie (en secondes) en millisecondes
    if (newInterval >= 1000 && newInterval <= 10000) { // Vérifier si la valeur est dans une plage acceptable (1-10 secondes)
      intervalMoteur = newInterval; // Mettre à jour la variable globale
      Serial.print("Nouvel intervalle moteur défini : ");
      Serial.println(intervalMoteur);
      server.send(200, "text/plain", "Intervalle mis à jour avec succès !");
    } else {
      server.send(400, "text/plain", "Valeur hors plage (1-10 secondes)");
    }
  } else {
    server.send(400, "text/plain", "Paramètre 'interval' manquant");
  }
}

// Déplacement manuel du moteur
void handleMove() {
  if (server.hasArg("step") && server.hasArg("direction")) {
    double step = server.arg("step").toFloat();
    String direction = server.arg("direction");

    int nombreDePas = round((step / pasDeVis) * stepsPerRevolution);
    digitalWrite(DIR_PIN, (direction == "AvManu") ? HIGH : LOW);

    for (int i = 0; i < abs(nombreDePas); i++) {
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(speedDelay);
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(speedDelay);
    }

    positionActuelle += (nombreDePas * (direction == "AvManu" ? 1 : -1));
  }
}

// Enregistrer la position de début
void handleSetDebut() {
  debutPosition = positionActuelle; // Mémoriser la position actuelle comme point de départ
  server.send(200, "text/plain", "Position de début enregistrée");
  Serial.print("Position de début enregistrée : ");
  Serial.println(debutPosition);
}

// Enregistrer la position de fin
void handleSetFin() {
  finPosition = positionActuelle; // Mémoriser la position actuelle comme point de fin
  long distanceSteps = abs(finPosition - debutPosition); // Calcul de la distance en pas
  double distance_mm = (distanceSteps * pasDeVis) / stepsPerRevolution; // Conversion en mm
  server.send(200, "text/plain", String(distance_mm)); // Retourner la distance parcourue
  Serial.print("Position de fin enregistrée : ");
  Serial.println(finPosition);
  Serial.print("Distance calculée (mm) : ");
  Serial.println(distance_mm);
}

// Valider la configuration pour une séquence
void handleValider() {
  if (server.hasArg("avance")) {
    avanceMillimetre = server.arg("avance").toFloat();
    long distanceSteps = finPosition - debutPosition; // Distance totale en pas
    nbrPhotos = round(distanceSteps / (avanceMillimetre * stepsPerRevolution / pasDeVis)); // Nombre de photos

    // Calcul de l'avance réelle par pas entiers
    int nombreDePas = ceil(avanceMillimetre * stepsPerRevolution / pasDeVis); // Nombre de pas pour l'avance
    double avanceReelle = (pasDeVis * nombreDePas) / stepsPerRevolution;      // Avancée réelle en mm

    // Calcul de la distance totale parcourue
    double distanceTotale = avanceReelle * nbrPhotos;

    // Envoyer les résultats au client (nombre de photos, avance réelle et distance totale)
    String result = String(nbrPhotos) + "," + String(avanceReelle, 2) + "," + String(distanceTotale, 2);
    server.send(200, "text/plain", result); // Format : "nbrPhotos,avanceReelle,distanceTotale"
  } else {
    server.send(400, "text/plain", "Paramètre manquant");
  }
}

void handleDistanceTotale() {
  server.send(200, "text/plain", String(distanceParcourue, 2)); // Envoyer la distance totale parcourue
}

void handleReset() {
  // Réinitialiser les variables globales
  avanceMillimetre = 0.0;
  nbrPhotos = 0;
  photosRealisees = 0;
  debutPosition = 0;
  finPosition = 0;
  positionActuelle = 0;
  distanceParcourue = 0.0;
  previousMillis = 0;
  sequenceState = 0;
  currentPhoto = 0;
  lastPhotoDelay = false;

  Serial.println(avanceMillimetre);
  Serial.println(nbrPhotos);
  Serial.println(photosRealisees);
  Serial.println(debutPosition);
  Serial.println(finPosition);
  Serial.println(positionActuelle);
  Serial.println(distanceParcourue);
  Serial.println(enCours);
  Serial.println("Tous les paramètres ont été réinitialisés.");

  // Envoyer une réponse au client et actualiser la page
  server.send(200, "text/plain", "RESET_COMPLETE");
}

void configureServer() {
  server.on("/", handleRoot);               // Page principale
  server.on("/testShutter", handleTestShutter); // Test du relais
  server.on("/start", startSequence);       // Démarrage de la séquence
  server.on("/stop", handleStop);           // Arrêt de la séquence
  server.on("/move", handleMove);           // Mouvement manuel du moteur
  server.on("/setDebut", handleSetDebut);   // Enregistrer la position de début
  server.on("/setFin", handleSetFin);       // Enregistrer la position de fin
  server.on("/valider", handleValider);     // Validation des paramètres
  server.on("/reset", handleReset);         // Réinitialisation des paramètres
  server.on("/distanceTotale", handleDistanceTotale); // Récupérer la distance totale
  server.on("/setIntervalMoteur", handleSetIntervalMoteur); //Change le delais avance moteur
  server.on("/getIntervalMoteur", handleGetIntervalMoteur); //envoie la valeur par defaut dans le champ html
  server.begin();                           // Démarrage du serveur Web
  Serial.println("Serveur Web configuré et démarré.");
}

// *** Programme principal ***
void setup() {
  // Initialisation du moniteur série pour le débogage
  Serial.begin(115200);
  Serial.println("Démarrage du programme...");

  // Configuration de l'adresse IP fixe pour le point d'accès
  IPAddress local_IP(192, 168, 4, 1);    // Adresse IP fixe
  IPAddress gateway(192, 168, 4, 1);      // Passerelle
  IPAddress subnet(255, 255, 255, 0);     // Masque de sous-réseau

  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("Erreur lors de la configuration de l'adresse IP fixe.");
  }

  // Configuration du point d'accès Wi-Fi
  WiFi.softAP(ssid, password); // Créer un réseau Wi-Fi avec SSID et mot de passe
  Serial.println("Point d'accès démarré avec succès.");

  // Obtenir l'adresse IP du point d'accès
  Serial.print("Adresse IP du point d'accès : ");
  Serial.println(WiFi.softAPIP());

  WiFi.softAP(ssid, password); // Point d'accès Wi-Fi
    Serial.println("Point d'accès démarré.");

    // Initialisation de mDNS
    if (!MDNS.begin("micromotion")) { // Remplacez "micromotion" par le nom désiré
        Serial.println("Erreur : mDNS n'a pas démarré.");
    } else {
        Serial.println("mDNS activé. Accédez à http://micromotion.local");
    }

  // Initialisation du moteur pas à pas
  initMotor();

  // Initialisation du relais
  pinMode(RELAY_PIN, OUTPUT);  // Définir la broche du relais comme sortie
  digitalWrite(RELAY_PIN, LOW); // Relais désactivé au démarrage

  // Configuration des routes du serveur
  configureServer();

  // Configuration initiale
  Serial.println("Configuration terminée. En attente de commandes.");
}

void loop() {
  server.handleClient(); // Gérer les requêtes entrantes
  processSequence();     // Exécuter la séquence si en cours
}

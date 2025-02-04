#ifndef GESTIONMOTEUR_H
#define GESTIONMOTEUR_H

#include <Arduino.h>

// Déclarations des fonctions et variables liées à la gestion du moteur
void initMotor();
void avancerMoteur(double distance_mm);
void startSequence();
void processSequence();

// Définition des broches pour le moteur
#define STEP_PIN 16
#define DIR_PIN 17
#define ENABLE_PIN 18
#define RELAY_PIN 13

// Paramètres du moteur pas à pas
const int stepsPerRevolution = 4 * 200; // Nombre de pas par tour (moteur standard)
const float pasDeVis = 1.00;        // Distance parcourue (en mm) pour un tour complet
const int speedDelay = 250;  // Temps entre chaque pas (en microsecondes) vitesse élevée si valeur faible

// Variables globales
extern double avanceMillimetre;       // Distance à parcourir (en mm)
extern int nbrPhotos;             // Nombre total de photos à prendre
extern int photosRealisees;       // Compteur de photos prises
extern bool enCours;          // Indique si une séquence est en cours
extern long debutPosition;        // Position initiale du moteur
extern long finPosition;        // Position finale du moteur
extern long positionActuelle;     // Position actuelle du moteur
extern double distanceParcourue; // Distance totale parcourue en mm
extern float erreurAccumulee;   // Correction pour arrondir les pas
extern int currentPhoto;         // Compteur de photos actuellement prises
extern bool lastPhotoDelay;  // Indique si on attend après la dernière photo

// Variables de gestion du temps
extern unsigned long previousMillis; // Dernière fois qu'une action a été effectuée
extern unsigned long intervalDeclenchement;    // Intervalle après le déclenchement (en ms)
extern unsigned long intervalMoteur;    // Intervalle après le mouvement du moteur (en ms)
extern int sequenceState;            // État de la séquence : 0=Relais, 1=Temporisation, 2=Moteur, 3=Pause

// Serveur Web pour l'interface utilisateur
extern WebServer server;

// Initialisation du moteur pas à pas
void initMotor() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW); // Activer le moteur
}

// Déplacement du moteur pas à pas sur une distance donnée (en mm)
void avancerMoteur(double distance_mm) {
  double nombreDeTours = distance_mm / pasDeVis;             // Convertir la distance en tours
  double pasFlottant = nombreDeTours * stepsPerRevolution;   // Convertir en pas
  int nombreDePas = ceil(abs(pasFlottant));                  // Arrondi à l'entier supérieur

  digitalWrite(DIR_PIN, (pasFlottant > 0) ? HIGH : LOW);     // Choisir la direction

  // Effectuer le déplacement en pas
  for (int i = 0; i < nombreDePas; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(speedDelay);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(speedDelay);
  }

  // Mettre à jour la position et calculer la distance parcourue
  positionActuelle += (nombreDePas * (digitalRead(DIR_PIN) == HIGH ? 1 : -1));
  distanceParcourue = abs((positionActuelle - debutPosition) * pasDeVis) / stepsPerRevolution;

  // Afficher les informations
  Serial.print("Position actuelle (pas) : ");
  Serial.println(positionActuelle);
  Serial.print("Distance parcourue depuis le début (mm) : ");
  Serial.println(distanceParcourue);
}

// Démarrer une séquence automatique
void startSequence() {
  if (enCours) return; // Ne rien faire si une séquence est déjà en cours
  enCours = true;
  photosRealisees = 0; // Réinitialiser le compteur de photos
  distanceParcourue = 0.0;
  erreurAccumulee = 0.0;

  long stepsToDebut = debutPosition - positionActuelle; // Calculer la distance jusqu'à la position initiale
  avancerMoteur((stepsToDebut * pasDeVis) / stepsPerRevolution);
  positionActuelle = debutPosition;

  Serial.println("Séquence démarrée");
  sequenceState = 0;
  previousMillis = millis(); // Initialiser le minuteur
}

// Traiter chaque étape de la séquence automatique
void processSequence() {
  if (!enCours) return; // Ne rien faire si aucune séquence n'est en cours

  unsigned long currentMillis = millis(); // Récupérer le temps actuel

  switch (sequenceState) {
    case 0: // Attendre intervalDeclenchement après le déclenchement
      if (currentMillis - previousMillis >= intervalDeclenchement) {
        digitalWrite(RELAY_PIN, HIGH);
        delay(200); // Maintenir le relais activé pendant 200 ms
        digitalWrite(RELAY_PIN, LOW);

        Serial.print("Photo ");
        Serial.print(currentPhoto + 1);
        Serial.print(" / ");
        Serial.println(nbrPhotos);

        photosRealisees = currentPhoto + 1;
        previousMillis = currentMillis;

        // Si c'est la dernière photo, ajouter une pause avant le retour
        if (currentPhoto + 1 >= nbrPhotos) {
          lastPhotoDelay = true; // Marquer qu'on attend après la dernière photo
          sequenceState = 4;     // Passer à l'état de pause avant le retour
        } else {
          sequenceState = 1; // Passer à l'avance du moteur
        }
      }
      break;

    case 1: // Avancer le moteur
      if (currentMillis - previousMillis >= intervalDeclenchement) {
        avancerMoteur(avanceMillimetre);
        previousMillis = currentMillis;
        sequenceState = 2; // Passer à la temporisation après le mouvement
      }
      break;

    case 2: // Temporisation après le mouvement
      if (currentMillis - previousMillis >= intervalMoteur) {
        currentPhoto++;
        sequenceState = 0; // Reprendre la séquence pour la prochaine photo
      }
      break;

    case 4: // Pause d'une seconde avant de revenir à "Set Début"
      if (lastPhotoDelay) {
        if (currentMillis - previousMillis >= 1500) { // Attendre 1.5 seconde
          lastPhotoDelay = false;
          sequenceState = 5; // Passer au retour à la position initiale
        }
      }
      break;

    case 5: // Retour à la position "Set Début"
      long stepsToReturn = debutPosition - positionActuelle;
      avancerMoteur((stepsToReturn * pasDeVis) / stepsPerRevolution);
      positionActuelle = debutPosition;
      enCours = false; // Terminer la séquence

      Serial.println("Séquence terminée et retour à la position de départ effectué.");

      // Calcul de l'avance réelle par photo
      int nombreDePas = ceil(avanceMillimetre * stepsPerRevolution / pasDeVis); // Nombre de pas pour l'avance
      double avanceReelle = (pasDeVis * nombreDePas) / stepsPerRevolution;      // Avancée réelle par photo

      // Calcul de la distance totale parcourue
      double distanceTotale = avanceReelle * photosRealisees;

      // Réinitialiser les variables globales
      avanceMillimetre = 0.0;
      nbrPhotos = 0;
      photosRealisees = 0;
      debutPosition = 0;
      finPosition = 0;
      positionActuelle = 0;
      distanceParcourue = distanceTotale; // Stocker la distance totale parcourue
      previousMillis = 0;
      sequenceState = 0;
      currentPhoto = 0;
      lastPhotoDelay = false;

      Serial.print("Distance totale parcourue pendant la séquence : ");
      Serial.println(distanceParcourue);

      // Envoyer une commande au client pour afficher la distance totale
      server.send(200, "text/plain", "RESET_COMPLETE");
      break;
  }
}

#endif // GESTIONMOTEUR_H

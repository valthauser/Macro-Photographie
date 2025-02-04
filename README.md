# Micro Motion Controller

## Description
Micro Motion Controller est un projet d'automatisation de la macrophotographie utilisant un rail motorisé. Le système repose sur un ESP32, contrôlant un moteur pas à pas et un relais pour déclencher une séquence de prises de vues avec des intervalles définis. Une interface Web permet de gérer les réglages et de contrôler le processus.

## Fonctionnalités
- Interface Web intuitive pour le contrôle du rail et des paramètres.
- Déplacement manuel du moteur avec définition des positions de début et de fin.
- Configuration des intervalles de déplacement et de déclenchement.
- Démarrage et arrêt de séquences automatiques.
- Retour automatique à la position initiale après la fin de la séquence.
- Test du déclencheur pour vérifier la connexion avec l'appareil photo.

## Matériel Requis
- ESP32
- Moteur pas à pas + driver (**TB6600**)
- Rail motorisé
- Relais pour déclencher l'appareil photo
- Câblage et alimentation adaptée

## Installation
### 1. Dépendances logicielles
Avant de téléverser le code sur l'ESP32, assurez-vous d'installer les bibliothèques nécessaires dans l'IDE Arduino :
```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <math.h>
```
### 2. Téléversement du code
1. Ouvrez l'IDE Arduino et chargez `macrophoto.ino`.
2. Vérifiez que l'ESP32 est sélectionné comme carte.
3. Téléversez le programme sur l'ESP32.

### 3. Connexion à l'interface Web
- Une fois le programme lancé, un réseau Wi-Fi nommé **MicroMotion** sera disponible.
- Connectez-vous avec le mot de passe : `123456789`.
- Accédez à l'interface Web via l'adresse : `http://micromotion.local` ou `192.168.4.1`.

## Structure des fichiers
```
├── macrophoto.ino      # Programme principal (contrôle moteur, gestion Wi-Fi, serveur Web)
├── appWeb.h            # Interface utilisateur en HTML/CSS/JavaScript
├── gestionMoteur.h     # Fonctions de contrôle du moteur
```

## Usage
### Contrôle manuel du rail
1. Définissez une distance en mm dans l'interface.
2. Utilisez les boutons `Av Manu` et `Rec Manu` pour avancer ou reculer.
3. Définissez les positions de début et de fin avant de lancer une séquence.

### Séquence automatique
1. Définissez l'intervalle entre chaque photo en mm.
2. Cliquez sur `Résultats séquence` pour voir le nombre de prises de vues prévues.
3. Lancez la séquence avec `START`.
4. Arrêtez la séquence à tout moment avec `STOP`.

## Contributions
Les contributions sont les bienvenues ! N'hésitez pas à soumettre des pull requests ou à ouvrir des issues pour signaler des améliorations ou des bugs.

## Licence
Ce projet est sous licence MIT. Voir le fichier `LICENSE` pour plus d'informations.

---

#ifndef APPWEB_H
#define APPWEB_H

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Micro Motion Controller</title>
  <meta charset="utf-8">
  <style>
        body {font-family: Arial, sans-serif;text-align: center;margin: 0;padding: 0;width: 100%;background-color: #dcdcc3;color: #228B22;}
        h1 {margin-top: 20px;margin-bottom: 10px;font-size: 40px !important;}
        h2 {font-size: 36px}
        h3 {font-size: 30px}
        p {margin: 10px;text-align: left; font-size:26px;}
        span {font-size:24px; text-align:left;}
        input {padding: 5px;width: 110px; font-size: 38px;}
        label {font-size: 38px;}
        button {padding: 10px;margin: 10px; width: 40vw; height: 80px;font-weight: 500; font-size: 34px; border-radius: 10px;}
        button:disabled {background-color: grey;color: white; cursor: not-allowed;opacity: 0.6;border-radius: 10px;}
        .custom-alert {
          display: none;
          position: fixed;
          top: 50%;
          left: 50%;
          transform: translate(-50%, -50%);
          background: white;
          padding: 24px;
          border-radius: 10px;
          box-shadow: 0px 4px 6px rgba(0, 0, 0, 0.2);
          text-align: center;
          z-index: 1000;
          width: 80%;
         }

        .custom-alert-message {
            text-align: center;
            font-size: 32px;
            font-weight: bold;
            color: black;
            margin-bottom: 15px;
        }

        .button-modal {
            margin-top: 10px;
            padding: 10px 20px;
            font-size: 30px; /* Important pour que le texte du bouton soit bien visible */
            border: none;
            background-color: #228B22;
            color: white;
            border-radius: 5px;
            cursor: pointer;
        }
        #distanceModal {
          display: none;
          position: fixed;
          top: 50%;
          left: 50%;
          transform: translate(-50%, -50%);
          background: white;
          padding: 24px;
          border-radius: 10px;
          box-shadow: 0px 4px 6px rgba(0, 0, 0, 0.2);
          text-align: center;
          z-index: 1000;
          width: 65%;
       }

      #distanceModal .custom-alert-message {
          text-align: center;
          font-size: 28px;
          font-weight: bold;
          color: black;
          margin-bottom: 15px;
      }

        .section {margin-top: 40px; width: 45%;}
        .green { background-color: #228B22;color: white;}
        .red {background-color: #A0522D;color: white;}
        .lightseagreen {background-color: #90EE90;color: black;}
        .blue { background-color: #87CEEB;color: black;}
        .orange { background-color: #FFD700;color: black;}
        .container {display: flex;flex-direction: row;justify-content: space-around;flex-wrap: wrap;}
        .inter-margin {margin-top: 15px !important;}
        .button-row {display: flex;flex-direction: row;justify-content: space-around;}
        .margin-usermode {margin-top: 80px;}
        li {text-align: left;padding: 10px;margin-left: 20px; font-size: 20px;}
        .interval-moteur{ display: flex; flex-direction: row;justify-content: space-around;}
        .moteur2 { display: flex; flex-direction: column;}
        .moteur1 {text-align:left;}
        .marg-bottom {margin-bottom: 20px;}
        @media only screen and (max-width: 1255px) {
            .button-row {display: flex;flex-direction: row;justify-content: space-around;}
        }
        @media only screen and (min-width: 1024px) {
            .container {flex-direction: column; align-items: center;}
        }
        @media only screen and (min-width: 768px) {
            .container {margin:auto;flex-direction: column;align-items:center; width:90%;}
            .section {width: 100%;}
        }
    </style>
</head>
<body>
    <div class="container">
    <h1>Micro Motion Controller</h1>
        <div class="section">
            <h2>Paramètres de réglages manuels</h2>
            <label for="avanceManuel">Déplacement en mm:</label>
            <input type="number" id="avanceManuel" step="0.1"><br>
            <div class="button-row inter-margin">
                <button class="green" onclick="moveManual('AvManu')">Av Manu</button>
                <button class="green" onclick="moveManual('RecManu')">Rec Manu</button>
            </div>
            <div class="button-row inter-margin">
                <button class="lightseagreen" onclick="setDebut()">Définir Début</button>
                <button class="lightseagreen" onclick="setFin()">Définir Fin</button>
            </div>

            <div class="button-row inter-margin">
                <button class="red" onclick="resetAll()">Reset tous paramètres</button>
            </div>
        </div>

        <div class="section interval-moteur">
            <div class="moteur2">
                <h2>Delai déclenchement</h2>
                <div>
                    <label for="intervalMoteur">Délai mouvement/photo :</label>
                    <input type="number" id="intervalMoteur" min="1" max="10" step="0.5" placeholder="" />
                    <button class="lightseagreen inter-margin" onclick="setIntervalMoteur()">Définir Délai</button>
                </div>
            </div>
        </div>

        <!-- Boîte modal -->
        <div id="distanceModal" class="custom-alert">
          <p class="custom-alert-message">Veuillez renseigner une valeur.</p>
          <button onclick="closeDistanceModal()" class="button-modal">OK</button>
        </div>

        <div class="section">
            <h2>Séquence Auto</h2>
            <label for="avanceAuto">Intervalle photos en mm:</label>
            <input type="number" id="avanceAuto" step="0.1"><br>
            <button class="blue inter-margin" onclick="valider()">Résultats séquence</button>
            <p id="nbrPhotos">Nombre de Photos : 0</p>
            <p id="avanceReelle">Intervalle réel photo : 0 mm</p>
            <p id="distanceParcourue">Distance parcourue : 0 mm</p>
            <div class="button-row">
                <button class="orange" onclick="testShutter()">Test déclencheur</button>
            </div>
            <div class="button-row inter-margin">
                <button class="blue" onclick="startSequence()">START</button>
                <button class="red" onclick="stopSequence()">STOP</button>
            </div>
        </div>

        <div class="section marg-bottom">
            <h2 class="margin-usermode">Mode d'emploi</h2>
            <h3>Réglages Manuels</h3>
            <ol>
                <li>Mettez votre appareil photo en mode Live View pour observer directement le comportement du système.</li>
                <li>Dans le champ <strong>"Déplacement en mm"</strong>, renseignez la distance en millimètres que vous souhaitez avancer
                    ou reculer pour obtenir une zone nette pour le stacking. Vous pouvez ajuster cette valeur pour
                    dégrossir les zones.</li>
                <li>Appuyez sur <strong>"Av Manu"</strong> ou <strong>"Rec Manu"</strong> pour définir les zones à photographier.</li>
                <li>Une fois la zone de départ trouvée, appuyez sur <strong>"Définir Début"</strong>. Lorsque vous avez trouvé la zone
                    finale, appuyez sur <strong>"Définir Fin"</strong>.</li>
                <li>Pour redéfinir les zones de début et de fin, appuyez sur <strong>"Reset tous paramètres"</strong> et
                    reprenez à l'étape 1.</li>
            </ol>

            <h3>Réglage du Délai mouvement/photo</h3>
            <ol>
                <li>Utilisez cette fonctionnalité pour définir le délai d'attente après chaque mouvement du moteur, avant que la photo ne soit prise. Ce délai est estimé pendant les mouvements manuels dans la section <strong>"Réglages Manuels"</strong>. Cela permet au système de se stabiliser complètement, éliminant les vibrations.</li>
                <li>Effectuez des avances manuelles à l'aide des boutons <strong>"Av Manu"</strong> ou <strong>"Rec Manu"</strong> dans la section <strong>"Réglages Manuels"</strong> pour détecter les vibrations.</li>
                <li>Estimez le temps nécessaire pour que le système soit parfaitement stable après un mouvement. Pour ce faire, observez les vibrations après avoir appuyé sur <strong>"Av Manu"</strong> ou <strong>"Rec Manu"</strong> et notez le temps nécessaire pour que les vibrations cessent.</li>
                <li>Saisissez cette durée en secondes dans le champ <strong>"Délai mouvement/photo"</strong>, situé dans la section <strong>Délai déclenchement</strong>.</li>
                <li>Cliquez sur le bouton <strong>"Définir Délai"</strong> pour enregistrer le délai.</li>
            </ol>



            <h3>Séquence Automatique</h3>
            <ol>
                <li>Saisissez la distance en millimètres que vous souhaitez entre chaque photo.</li>
                <li>Pour obtenir le nombre de photos réelles prises pendant la séquence automatique, l'intervalle réel
                    et la distance parcourue réelle, appuyez sur <strong>"Résultats séquence"</strong>.</li>
                <li>Le bouton <strong>"Test déclencheur"</strong> permet de vérifier si l'appareil photo déclenche la photo. Cela prendra
                    une photo test.</li>
                <li>Pour démarrer la séquence, appuyez sur <strong>"START"</strong>.</li>
                <li>Si vous constatez une erreur ou un oubli pendant la séquence, appuyez sur <strong>"STOP"</strong>. Le système
                    reviendra à la position initiale. Dans ce cas, recommencez la procédure depuis le début, en
                    commençant par les paramètres de réglage manuels.</li>
            </ol>
        </div>
    </div>

  <script>
    function loadDefaultInterval() {
      fetch('/getIntervalMoteur')
        .then(response => response.text())
        .then(interval => {
          const input = document.getElementById('intervalMoteur');
          input.placeholder = interval;
          input.value = "";
        })
        .catch(error => {
          console.error("Erreur lors de la récupération de l'intervalle moteur :", error);
        });
    }

    window.onload = function () {
      loadDefaultInterval();
    };

    function setIntervalMoteur() {
      const intervalSeconds = document.getElementById('intervalMoteur').value;
      if (intervalSeconds === "") {
        showDistanceModal(); // Afficher la modal si le champ est vide
      } else if (intervalSeconds >= 1 && intervalSeconds <= 10) {
        fetch(`/setIntervalMoteur?interval=${intervalSeconds}`)
          .then(response => response.text())
          .then(result => {
            showCustomAlert(result);
            loadDefaultInterval();
          })
          .catch(error => {
            console.error("Erreur lors de la mise à jour de l'intervalle moteur :", error);
          });
      } else {
        showCustomAlert("Veuillez entrer une valeur entre 1 et 10 secondes.");
      }
    }

    // Fonction pour afficher la boîte de dialogue modale pour le champ vide
    function showDistanceModal() {
      document.getElementById("distanceModal").style.display = "block";
    }

    // Fonction pour fermer la boîte de dialogue modale pour le champ vide
    function closeDistanceModal() {
      document.getElementById("distanceModal").style.display = "none";
    }

    // Fonction pour afficher la boîte de dialogue modale
    function showCustomAlert(message) {
        document.getElementById("alertMessage").innerText = message;
        document.getElementById("customAlert").style.display = "block";
    }

    // Fonction pour fermer la boîte de dialogue
    function closeCustomAlert() {
        document.getElementById("customAlert").style.display = "none";
    }

    function startSequence() {
      fetch('/start')
        .then(response => response.text())
        .then(result => {
          if (result === "RESET_COMPLETE") {
            // Récupérer la distance totale depuis le serveur
            fetch('/distanceTotale')
              .then(response => response.text())
              .then(distance => {
                document.getElementById('distanceParcourue').innerText = `Distance parcourue : ${distance} mm`;
              });
            location.reload(); // Actualiser la page
          }
        })
        .catch(error => {
          console.error("Erreur lors de la séquence :", error);
        });

      // Désactiver les boutons
      document.querySelector("button[onclick='testShutter()']").disabled = true;
      document.querySelector("button[onclick='setIntervalMoteur()']").disabled = true;
      document.querySelector("button[onclick='valider()']").disabled = true;

      // Ajouter la classe CSS pour désactiver visuellement
      document.querySelector("button[onclick='testShutter()']").classList.add("button:disabled");
      document.querySelector("button[onclick='setIntervalMoteur()']").classList.add("button:disabled");
      document.querySelector("button[onclick='valider()']").classList.add("button:disabled");
    }

    function stopSequence() {
      fetch('/stop');
    }

    function moveManual(direction) {
      const avance = document.getElementById('avanceManuel').value;
      if (avance === "") {
        showDistanceModal(); // Afficher la modal si le champ est vide
      } else {
        fetch(`/move?direction=${direction}&step=${avance}`);
      }
    }

    function setDebut() {
      fetch('/setDebut')
        .then(response => response.text())
        .then(() => {
          const boutonDebut = document.querySelector("button[onclick='setDebut()']");
          boutonDebut.innerText = "Debut OK";
          boutonDebut.className = "blue";
          boutonDebut.disabled = true; // Désactiver le bouton
        });
    }

    function setFin() {
      fetch('/setFin')
        .then(response => response.text())
        .then(() => {
          const boutonFin = document.querySelector("button[onclick='setFin()']");
          boutonFin.innerText = "Fin OK";
          boutonFin.className = "blue";
          boutonFin.disabled = true; // Désactiver le bouton "Définir Fin"

          // Désactiver les boutons "Av Manu" et "Rec Manu"
          document.querySelectorAll("button[onclick*='moveManual']").forEach(button => {
            button.disabled = true; // Désactiver le bouton
            button.classList.add("button:disabled"); // Ajouter la classe CSS pour changer leur apparence
          });
        });
    }

    function valider() {
      const avance = document.getElementById('avanceAuto').value;
      if (avance === "") {
        showDistanceModal(); // Afficher la modal si le champ est vide
      } else {
        fetch(`/valider?avance=${avance}`)
          .then(response => response.text())
          .then(result => {
            const [nbrPhotos, avanceReelle, distanceParcourue] = result.split(',');
            document.getElementById('nbrPhotos').innerText = `Nombre de Photos : ${nbrPhotos}`;
            document.getElementById('avanceReelle').innerText = `Intervalle réel photo : ${avanceReelle} mm`;
            document.getElementById('distanceParcourue').innerText = `Distance parcourue : ${distanceParcourue} mm`;
          })
          .catch(error => {
            console.error("Erreur lors de la validation :", error);
          });
      }
    }

    function testShutter() {
      fetch('/testShutter');
    }

    function resetAll() {
      fetch('/reset')
        .then(() => {
          location.reload(true); // Recharger la page pour refléter les changements

          // Réactiver les boutons "Av Manu" et "Rec Manu"
          document.querySelectorAll("button[onclick*='moveManual']").forEach(button => {
            button.disabled = false; // Réactiver le bouton
            button.classList.remove("button:disabled"); // Retirer la classe CSS
          });

          // Réactiver le bouton "Définir Fin" si nécessaire
          const boutonFin = document.querySelector("button[onclick='setFin()']");
          if (boutonFin) {
            boutonFin.disabled = false;
            boutonFin.className = "lightseagreen"; // Restaurer la classe d'origine
            boutonFin.innerText = "Définir Fin";
          }

          // Réactiver les boutons "Test déclencheur", "Définir Intervalle", et "Résultats prises de vues"
          const boutons = [
            document.querySelector("button[onclick='testShutter()']"),
            document.querySelector("button[onclick='setIntervalMoteur()']"),
            document.querySelector("button[onclick='valider()']")
          ];
          boutons.forEach(button => {
            button.disabled = false; // Réactiver le bouton
            button.classList.remove("button:disabled"); // Retirer la classe CSS
          });
        });
    }

  </script>
</body>
</html>

)rawliteral";

#endif

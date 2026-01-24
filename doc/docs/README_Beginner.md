---
icon: simple/arduino
---

# Doc for beginer

Salut ! Si tu découvres la programmation aujourd'hui, ne t'inquiète pas. Ce projet est comme un système d'alarme intelligent pour des stations de service. Imagine des boîtes magiques (appelées "cartes ESP8266") qui surveillent l'air pour détecter du gaz dangereux. Si elles en trouvent, elles envoient un message à un ordinateur distant (le serveur) qui prévient toutes les autres boîtes.

## Qu'est-ce que ce projet fait ?

- **Détection de gaz** : Les cartes ESP8266 ont un capteur qui "sent" si l'air contient du gaz.
- **Alerte lumineuse et sonore** : Si du gaz est détecté, une lumière s'allume et un buzzer fait du bruit.
- **Communication entre stations** : Toutes les cartes parlent entre elles via Internet. Si une détecte du gaz, toutes les autres sont alertées.
- **Serveur en ligne** : Un ordinateur sur Internet (hébergé sur Render) reçoit les messages et les redistribue.

## Comment ça marche en gros ?

1. **La carte ESP8266** :
   - Se connecte à ton WiFi (comme ton téléphone).
   - Se connecte à un serveur spécial sur Internet (via WebSocket, c'est comme un chat en temps réel).
   - Envoie son nom (comme "Station_A") au serveur pour s'inscrire.
   - Vérifie régulièrement si l'air est propre. Si pas, elle envoie une alerte.
   - Écoute les alertes des autres et allume sa lumière/buzzer si besoin.

2. **Le serveur** :
   - C'est un programme qui tourne sur un ordinateur distant.
   - Il reçoit les inscriptions des cartes.
   - Quand une alerte arrive, il la dit à toutes les autres cartes connectées.

## Comment installer et utiliser ?

### Pour la carte ESP8266

1. **Matériel nécessaire** :
   - Une carte ESP8266 (comme NodeMCU).
   - Un capteur de gaz (connecté à la broche A0).
   - Une LED (broche 2) et un buzzer (broche 4).
   - Câbles pour connecter.

2. **Logiciel** :
   - Télécharge l'Arduino IDE (c'est gratuit, comme un éditeur de texte pour code).
   - Installe les bibliothèques : ESP8266WiFi et WebSockets (dans l'IDE, va dans Outils > Gérer les bibliothèques).
   - Ouvre le fichier `arduino/TFC_station_service_online/TFC_station_service_online.ino`.
   - Change le SSID et mot de passe WiFi (lignes 5-6).
   - Change le nom de la station si tu veux (ligne 11).
   - Télécharge le code sur la carte (branche-la avec un câble USB, sélectionne la bonne carte dans l'IDE, et clique sur Téléverser).

3. **Test** :
   - Ouvre le Moniteur Série dans l'IDE (Outils > Moniteur Série).
   - La carte devrait se connecter au WiFi et au serveur.
   - Si tu touches le capteur (ou simules du gaz), elle enverra une alerte.

### Pour le serveur

1. **Hébergement** : Le serveur est déjà sur Render (un service gratuit pour sites web). Il tourne automatiquement.
2. **Si tu veux le changer** : Le code est dans `server/serverOnline.js`. Pour le tester localement :
   - Installe Node.js (gratuit).
   - Dans un terminal, va dans le dossier `server` et tape `npm install` puis `node serverOnline.js`.
   - Le serveur tournera sur <http://localhost:3000>.

## Que faire si ça ne marche pas ?

- Vérifie que le WiFi est bon.
- Regarde les messages dans le Moniteur Série : ils te disent ce qui se passe.
- Assure-toi que le serveur Render est en ligne (va sur <https://tfc-station-service-alerte.onrender.com> pour voir "Serveur WebSocket en ligne...").
- Si le fingerprint SSL change (rare), tu devras le mettre à jour (demande à un ami programmeur).

## Idées pour améliorer ?

- Ajouter plus de capteurs (fumée, température).
- Faire une app mobile pour voir les alertes.
- Stocker les données dans une base de données.

Amuse-toi bien ! La programmation, c'est comme construire des Lego : commence petit, et tu feras des trucs cools.

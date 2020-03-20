[:gb:](https://github.com/Zlika/theodore/blob/master/README.md)
[:fr:](https://github.com/Zlika/theodore/blob/master/README-FR.md)

Theodore - Emulateur Thomson MO/TO
=====================================

[![Build status](https://travis-ci.org/Zlika/theodore.svg?branch=master)](https://travis-ci.org/Zlika/theodore)
[![Build status](https://ci.appveyor.com/api/projects/status/7lo7cohkpmn50ogk?svg=true)](https://ci.appveyor.com/project/Zlika/theodore)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/15677/badge.svg)](https://scan.coverity.com/projects/zlika-theodore)
[![GitHub tag](https://img.shields.io/github/tag/Zlika/theodore.svg)](https://github.com/Zlika/theodore/releases)

Theodore est un "core" [libretro](https://github.com/libretro) émulant un ordinateur [Thomson](https://fr.wikipedia.org/wiki/Gamme_MOTO). Cet émulateur est dérivé des émulateurs [DCTO8D](http://dcto8.free.fr/), [DCTO9P](http://dcto9p.free.fr/) et [DCMO5](http://dcmo5.free.fr/) de Daniel Coulom, et supporte les modèles suivants: TO7, TO7/70, TO8, TO8D, TO9, TO9+, MO5, MO6 ainsi que l'Olivetti Prodest PC128 (un dérivé du MO6 pour le marché italien).

### Instructions d'installation

Cet émulateur est disponible sur les plateformes suivantes :
* RetroArch : [Installer RetroArch](http://www.retroarch.com/?page=platforms), puis démarrer RetroArch et télécharger le core "theodore" via le "Core Updater".
* [Recalbox](https://www.recalbox.com/) : Theodore est inclus dans votre console de retrogaming préférée depuis la version 2018.06.27.
* [Lakka](http://www.lakka.tv/): Theodore est inclus dans cette distribution Linux légère à partir de la version 2.2.2.
* [Kodi](https://kodi.tv/): Depuis la version 18 de Kodi, celui-ci permet de télécharger et d'exécuter des émulateurs basés sur libretro, dont Theodore.

Il devrait être compatible avec toutes les plateformes supportées par RetroArch (Android, Linux, MacOS, Raspberry Pi, Windows, Nintendo...).

### Instructions de compilation

Sur Linux (et la plupart des autres plateformes) :
```
make
```
Le "core" peut alors s'exécuter avec un "frontend" libretro tel que RetroArch :
```
retroarch -L theodore_libretro.so
```

Pour compiler la version Android, le [NDK Android](https://developer.android.com/ndk/downloads/) doit être utilisé :
```
cd jni
ndk-build
```

### :video_game: Correspondance des boutons de la manette

B => Bouton "Action"

Start => "Démarrer le programme". Simule la frappe d'une ou plusieurs touches sur le clavier pour démarrer un jeu. Cette fonctionnalité permet de démarrer la plupart des jeux sans avoir besoin d'un clavier. La touche simulée dépend du type de media chargé et de l'ordinateur émulé. Sur MO5/MO6/TO7/TO7-70, la commande utilisée dépend du format détecté pour le premier fichier de la cassette (BAS => RUN", BIN => LOADM"",,R).

| Media chargé | Modèle Thomson    | Touche                 |
| ------------ | ----------------- | ---------------------- |
| Disquette    | TO8/TO8D/TO9+     | Touche 'B' (BASIC 512) |
|              | TO9               | Touche 'D' (BASIC 128) |
|              | MO5               | RUN" + Entrée          |
| Cassette     | TO8/TO8D/TO9+     | Touche 'C' (BASIC 1.0) |
|              | TO9               | Touche 'E' (BASIC 1.0) |
|              | MO5/MO6/TO7       | RUN" ou LOADM"",,R + Entrée |
| Cartouche    | Tous sauf MO5/TO7 | Touche '0'             |
|              | TO7/TO7-70        | Touche '1'             |
|              | MO5               | Rien (cartouches déjà démarrées automatiquement sur le MO5) |

Si le jeu ne démarre pas, c'est sans doute qu'un autre BASIC doit être utilisé.

TO7 et TO7/70 : Quand une cartouche n'est pas chargée, la cartouche BASIC est automatiquement chargée (BASIC 1 pour le TO7, BASIC 128 pour le TO7/70).

**Fonctionnalité de clavier virtuel :** les boutons Y/X de la manette permettent de faire défiler des chiffres/lettres (Y=défilement vers le bas, X=défilement vers le haut). Le bouton "Select" simule un appui sur la touche choisie.
L'ordre des touches du clavier virtuel est : chiffres (0 à 9), puis lettres (A à Z) puis "Espace" puis "Entrée".

### Correspondance des touches spéciales du clavier

| Clavier Thomson | Clavier PC |
| ------------- | ------------- |
| STOP  | TAB  |
| CNT  | CTRL  |
| CAPSLOCK  | CAPSLOCK  |
| ACC  | BACKSPACE  |
| HOME  | HOME  |
| Flèches  | Flèches  |
| INS  | INSERT  |
| EFF  | DEL  |
| RAZ  | ALT  |
| F1-F5  | F1-F5  |
| F6-F10  | SHIFT+F1-F5  |
| Touche jaune (MO5) | SHIFT Gauche |
| BASIC (MO5/MO6) | SHIFT Droit |

RetroArch utilise beaucoup de raccourcis clavier, ce qui interfère avec l'émulation du clavier de ce core. Pour éviter ce problème, il suffit de configurer une "Hotkey" pour RetroArch, comme indiqué dans [Introduction to Hotkeys](https://docs.libretro.com/guides/retroarch-keyboard-controls/#introduction-to-hotkeys), et/ou basculer en mode "Game Focus" (touche "Arrêt Défil./Scroll Lock").

### :floppy_disk: Formats de fichiers supportés

L'émulateur peut lire les formats de fichiers suivants : *.fd et *.sap (disquettes), *.k7 (cassettes), *.m7/*.m5 et *.rom (cartouches).

### :computer: Modèles Thomson émulés

Par défaut, l'émulateur essaye de déduire le modèle d'ordinateur à émuler en se basant sur le nom du fichier chargé (par exemple : saphir_to8.fd utilisera un TO8, pulsar_mo5.k7 utilisera un MO5, etc...). En dernier recours, un TO8 est émulé. En utilisant l'option "Thomson model", il est possible de forcer l'émulation d'un modèle particulier, ou d'utiliser l'option "Auto" décrite précédemment.

### :rewind: Sauvegardes instantanées et rembobinage

L'émulateur supporte la fonctionnalité de "save state" (sauvegarde instantanée) de libretro. Sur RetroArch, cette fonctionnalité est accessible par défaut via les touches F2 (sauvegarde), F4 (chargement), F6/F7 (changement du slot de sauvegarde). Sur Recalbox, cette fonctionnalité est accessible par défaut via les boutons Hotkey + Y (sauvegarde), Hotkey + X (chargement), et Hotkey + "Flèche haut/bas" (changement du slot de sauvegarde).
L'émulateur supporte également la fonctionnalité de "rewind" (rembobinage) de libretro. Sur RetroArch, cette fonctionnalité est accessible par défaut en pressant et maintenant la touche "R". Sur Recalbox, cette fonctionnalité est accessible par défaut en pressant et maintenant les boutons Hotkey et "Flèche Gauche".

### :innocent: Codes de triche

L'émulateur est compatible avec la fonctionnalité "cheat code" de RetroArch (cf. l'article en anglais [Cheat code searching creation interface](https://www.libretro.com/index.php/upcoming-retroarch-1-7-4-cheat-code-searchingcreation-interface-with-rumble-features/) pour savoir comment utiliser cette fonctionnalité).

Le dépôt [libretro-database-thomson](https://github.com/Zlika/libretro-database-thomson) contient des cheat codes pour certains jeux Thomson.

### :beetle: Désassembleur / Debugger

L'émulateur dispose d'un petit désassembleur / debugger. Il nécessite l'accès à la ligne de commande, et n'est donc pas disponible sur les plateformes pour lesquelles les entrée/sortie standards ne sont pas disponibles ou facilement accessibles.
L'émulateur doit être compilé avec l'option "DASM=1" pour activer cette fonctionnalité (les binaires produits par le buildbot de libretro n'activent pas cette fonctionnalité, donc vous devez compiler l'émulateur par vous-même).
```
make DASM=1
```
RetroArch doit être lancé depuis la ligne de commande :
```
retroarch -L theodore_libretro.so /chemin/vers/jeu
```
Ensuite, il faut ouvrir le menu de RetroArch et activer l'option "Interactive disassembler" de l'émulateur.
Depuis la ligne de commande, les commandes suivantes sont ensuite disponibles :
* Appuie sur la touche `Entrée` : debuggage pas-à-pas. A chaque pas sont affichés l'adresse courante (registre Program Counter), l'instruction exécutée, et la valeur courante des registres du processeur.
* `trace` ou `t` : mode "trace". L'émulateur va afficher toutes les instructions exécutées au fil de l'eau et sans arrêt (sauf si un point d'arrêt est rencontré).
* `run` ou `r` : mode "run". L'émulateur va exécuter toutes les instructions au fil de l'eau mais sans les afficher et sans arrêt (sauf si un point d'arrêt est rencontré).
* `exit` ou `quit` ou `q` : quitte le désassembleur / debugger.
* `bp clear` : efface tous les points d'arrêts.
* `bp list` : affiche la list des points d'arrêt actuellement définis.
* `bp pc xxxx` (avec xxxx un nombre hexadécimal) : ajout d'un point d'arrêt pour la valeur donnée du registre Program Counter.
* `bp read xxxx` (avec xxxx un nombre hexadécimal) : ajout d'un point d'arrêt lors de la lecture à l'adresse donnée.
* `bp write xxxx` (avec xxxx un nombre hexadécimal) : ajout d'un point d'arrêt lors de l'écriture à l'adresse donnée.
* `read xxxx` (avec xxxx un nombre hexadécimal): lit la valeur en mémoire à l'adresse données.
* `write xxxx yy` (avec xxxx et yy des nombres hexadécimaux): écrit la valeur yy en mémoire à l'adresse xxxx.

Une autre option de l'émulateur, "Break on illegal opcode", permet de passer automatiquement en mode pas-à-pas quand un opcode illégal est rencontré.

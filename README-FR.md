[:gb:](https://github.com/Zlika/theodore/blob/master/README.md)
[:fr:](https://github.com/Zlika/theodore/blob/master/README-FR.md)

Theodore - Emulateur Thomson TO8/TO8D
=====================================

[![Build status](https://travis-ci.org/Zlika/theodore.svg?branch=master)](https://travis-ci.org/Zlika/theodore)
[![Build status](https://ci.appveyor.com/api/projects/status/7lo7cohkpmn50ogk?svg=true)](https://ci.appveyor.com/project/Zlika/theodore)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/15677/badge.svg)](https://scan.coverity.com/projects/zlika-theodore)
[![GitHub tag](https://img.shields.io/github/tag/Zlika/theodore.svg)](https://github.com/Zlika/theodore/releases)

Theodore est un "core" [libretro](https://github.com/libretro) émulant un ordinateur [Thomson TO8/TO8D](https://fr.wikipedia.org/wiki/Thomson_TO8). Cet émulateur est dérivé de l'émulateur [DCTO8D](http://dcto8.free.fr/) de Daniel Coulom.

### Instructions d'installation

[Installer RetroArch](http://www.retroarch.com/?page=platforms), puis démarrer RetroArch et télécharger le core "theodore" via le "Core Updater".

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

A => Bouton "Action"

B => Simule la frappe sur la touche 'B' du clavier (permet de démarrer la plupart des jeux sans avoir besoin d'un clavier)

**Fonctionnalité de clavier virtuel :** les boutons Y/X de la manette permettent de faire défiler des chiffres/lettres (Y=défilement vers le bas, X=défilement vers le haut). La bouton "start" simule un appui sur la touche choisie. Pour les manettes sans boutons Y/X, le bouton "select" peut être aussi utilisé pour faire défiler les touches du clavier virtuel.
L'ordre des touches du clavier virtuel est : chiffres (0 à 9), puis lettres (A à Z) puis "Entrée".

### Correspondance des touches spéciales du clavier

| Clavier Thomson | Clavier PC |
| ------------- | ------------- |
| STOP  | TAB  |
| CNT  | CTRL  |
| CAPSLOCK  | CAPSLOCK  |
| ACC  | ALT  |
| HOME  | HOME  |
| Flèches  | Flèches  |
| INS  | INSERT  |
| EFF  | DEL  |
| F1-F5  | F1-F5  |
| F6-F10  | SHIFT+F1-F5  |

### :floppy_disk: Formats de fichiers supportés

L'émulateur peut lire les formats de fichiers suivants : *.fd et *.sap (disquettes), *.k7 (cassettes), *.m7 et *.rom (cartouches).

**Fichiers SAP :** les fichiers *.sap sont convertis à la volé au format *.fd (sauf si un fichier *.fd de même nom existe déjà). Le fichier *.fd est ensuite chargé par l'émulateur.

### Variantes Thomson émulées

Par défaut, un ordinateur T08 est émulé. En utilisant l'option "Thomson flavor", il est possible d'émuler un TO8D.

### Sauvegardes instantanées

L'émulateur supporte la fonctionnalité de "save state" (sauvegarde instantanée) de libretro. Sur RetroArch, cette fonctionnalité est accessible par défaut via les touches F2 (sauvegarde), F4 (chargement), F6/F7 (changement du slot de sauvegarde).

### :innocent: Codes de triche

L'émulateur permet d'utiliser des codes de triche ("cheat codes") au format suivant : 0AAAAA:DD, avec AAAAA l'adresse en RAM (en hexadécimal) et DD la valeur (en hexadécimal) qui sera écrite à l'adresse en question après chaque invocation de la fonction retro_run().

Cf. le répertoire "cheat" qui contient des exemples de cheat codes ainsi qu'un script Python permettant de trouver des cheat codes à partir de plusieurs fichiers de sauvegarde instantanée (save states).

### Compatibilité

Le code source est portable et devrait compiler et s'exécuter sur la plupart des plateformes.
La tâche Travis vérifie la compilation sous Linux et MacOS, et la tâche AppVeyor vérifie la compilation sous Windows.

| Platforme | Compile | Exécute |
| --- | :---: | :---: |
| Linux / amd64 | :heavy_check_mark: | :heavy_check_mark: |
| Raspbian / Raspberry Pi | :heavy_check_mark: | :heavy_check_mark: |
| Android | :heavy_check_mark: | :heavy_check_mark: |
| MacOS | :heavy_check_mark: | :question: |
| Windows | :heavy_check_mark: | :heavy_check_mark: |
| Autres | :question: | :question: |


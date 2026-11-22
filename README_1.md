# Projet-3 — Outil de stéganographie BMP en C++

Outil en ligne de commande qui **cache un message texte dans une image BMP** et le **récupère**, avec un chiffrement **XOR** optionnel par clé répétée.
L'image produite reste lisible et les modifications sont invisibles à l'œil nu (au plus ±1 sur certaines composantes de couleur).

Astor et Noah D-M
---

## Compilation

### Visual Studio (environnement utilisé en cours)

1. Ouvrir `Projet-3.slnx` avec Visual Studio 2022 (17.x) ou plus récent.
2. Choisir la configuration `Debug` ou `Release` et la plateforme `x64`.
3. *Générer → Générer la solution* (`Ctrl+Maj+B`).
4. L'exécutable est produit dans `x64/Debug/Projet-3.exe` (ou `x64/Release/`).

Le projet est configuré en **C++20** (`/std:c++20`), application **console**, sans bibliothèque externe.

### Alternative en ligne de commande (g++ / clang)

Le code n'utilise que la bibliothèque standard, il compile aussi sous Linux/macOS :

```bash
g++ -std=c++20 -Wall -Wextra -o Projet-3 Projet-3/Main.cpp Projet-3/Bmp.cpp Projet-3/CommandLine.cpp Projet-3/Stegano.cpp Projet-3/XOR.cpp
```

---

## Commandes

```text
Projet-3.exe help
Projet-3.exe encode <source.bmp> <destination.bmp> "<message>" [clé]
Projet-3.exe decode <source.bmp> [clé]
```

| Commande | Rôle |
|----------|------|
| `help` | Affiche l'aide et des exemples. |
| `encode` | Charge `source.bmp`, cache le message (chiffré si une clé est donnée) et écrit `destination.bmp`. |
| `decode` | Lit le message caché dans `source.bmp` et l'affiche sur la sortie standard. La clé doit être la même qu'à l'encodage. |

La clé est **optionnelle** ; si elle est fournie, elle ne doit pas être vide.
Les arguments contenant des espaces doivent être placés **entre guillemets**.

### Exemples

```bash
Projet-3.exe encode Test_images/Apple.bmp sortie.bmp "Bonjour tout le monde"
```

```bash
Projet-3.exe decode sortie.bmp
```

```bash
Projet-3.exe encode Test_images/Apple.bmp secret.bmp "Message secret avec espaces" "ma clé"
```

```bash
Projet-3.exe decode secret.bmp "ma clé"
```

Code de retour : `0` en cas de succès, `1` en cas d'erreur (le message d'erreur est écrit sur `stderr`).

---

## Format supporté

Seul le format suivant est accepté ; toute autre variante est **explicitement rejetée** :

| Champ | Valeur exigée |
|-------|---------------|
| Signature | `BM` |
| Taille du DIB header | 40 octets (`BITMAPINFOHEADER`) |
| Plans | 1 |
| Bits par pixel | 24 |
| Compression | 0 (`BI_RGB`, non compressé) |
| Largeur / hauteur | strictement positives (images *bottom-up* uniquement) |
| Offset des pixels | ≥ 54 |

Le chargement vérifie aussi que le fichier fait au moins 54 octets et qu'il contient bien `hauteur × rowStride` octets de pixels à partir de l'offset (sinon : fichier tronqué → rejet).

**Rappels sur le format BMP 24 bits :**

- Chaque pixel occupe 3 octets dans l'ordre **B, G, R**.
- Chaque ligne est complétée par du **padding** pour que sa taille soit un multiple de 4 octets :
  `rowStride = (largeur × 3 + 3) / 4 × 4`
- Avec une hauteur positive, les lignes sont stockées **de bas en haut** (la première ligne du fichier est la ligne du bas de l'image).

**Sauvegarde :** le fichier est réécrit octet pour octet à partir du tampon chargé. Les en-têtes, le padding et d'éventuelles données après les pixels sont donc conservés à l'identique ; seuls les bits de poids faible des composantes utilisées changent. Le programme refuse d'écraser l'image source ou un fichier de destination déjà existant.

---

## Organisation du code

```text
Projet-3.slnx               Solution Visual Studio
Projet-3/
├── Main.cpp                Point d'entrée : parse → vérifie → exécute, gère le code de retour
├── CommandLine.h / .cpp    Analyse des arguments, vérifications, exécution des commandes, aide
├── Bmp.h / .cpp            Structure BmpImage : chargement, validation des en-têtes, sauvegarde
├── Stegano.h / .cpp        Capacité, écriture/lecture des bits LSB, métadonnées, encode/decode
├── XOR.h / .cpp            Chiffrement / déchiffrement XOR à clé répétée
└── Test_images/
    └── Apple.bmp           Image de test (1200 × 794, 24 bits)
```

Le flux d'exécution est séquentiel et sans héritage :

```text
main()
 ├─ CommandLineParse()    argv → CommandLineArguments (commande, source, destination, message, clé)
 ├─ CommandLineVerify()   champs obligatoires, source ≠ destination, destination inexistante
 └─ CommandLineExecute()
      ├─ BmpImage::loadImage()
      ├─ encodeMessage() / decodeMessage()   (→ applyXor() si clé)
      └─ BmpImage::saveImage()               (encode uniquement)
```

Le parsing du BMP est fait à la main : les entiers des en-têtes sont lus en **little-endian** avec des décalages (`readUint16`, `readUint32` dans `Bmp.cpp`), sans aucune bibliothèque d'image.

---

## Métadonnées embarquées et ordre de parcours

### Structure du bloc caché

Le message est précédé d'un en-tête de **9 octets** :

| Octet(s) | Contenu | Représentation |
|----------|---------|----------------|
| 0 – 3 | Signature | ASCII `S` `T` `E` `G` |
| 4 | Drapeau de chiffrement | `0` = sans clé, `1` = XOR |
| 5 – 8 | Longueur du message | entier non signé 32 bits, **little-endian** |
| 9 … | Message | octets bruts (chiffrés si drapeau = 1) |

Le texte est stocké **tel quel, octet par octet**, sans conversion d'encodage : les espaces et caractères accentués (UTF-8 ou autre) sont restitués à l'identique.

### Insertion des bits

- **1 bit par composante de couleur** (le bit de poids faible, LSB), donc **3 bits par pixel**.
- Écriture d'un bit : `composante = (composante & 0xFE) | bit`
- Lecture d'un bit : `bit = composante & 1`
- Chaque octet est découpé du **bit de poids fort au bit de poids faible** (`(valeur >> bitPos) & 1`, `bitPos` de 7 à 0) et reconstruit dans le même ordre (`valeur = (valeur << 1) | bit`).

### Ordre de parcours

Le bit n° `i` du flux (en-tête puis message) est placé dans la composante :

```text
pixel   = i / 3
canal   = i % 3              (0 = B, 1 = G, 2 = R)
ligne   = pixel / largeur
colonne = pixel % largeur
index   = pixelOffset + ligne × rowStride + colonne × 3 + canal
```

Le parcours suit donc l'ordre physique du fichier : ligne après ligne en partant de la **ligne du bas de l'image**, de gauche à droite, composantes **B → G → R**. Le calcul via `rowStride` **saute le padding** : aucune donnée n'est écrite dans les en-têtes ni dans les octets de remplissage.

---

## Capacité

```text
capacité totale (octets) = largeur × hauteur × 3 / 8      (division entière)
taille max du message    = capacité totale − 9            (9 octets de métadonnées)
```

Exemple avec `Apple.bmp` (1200 × 794) : 1200 × 794 × 3 / 8 = 357 300 octets, soit **357 291 octets** de message au maximum.

- À l'encodage, un message trop long est rejeté (`Message is too long for this image`).
- Au décodage, la longueur lue est contrôlée : elle doit être non nulle et ≤ capacité − 9, sinon `Invalid message length`. Cela évite toute lecture hors limites sur une image corrompue.

---

## Chiffrement XOR

Si une clé est fournie, chaque octet du message est combiné avec un octet de la clé, répétée autant que nécessaire :

```text
chiffré[i] = message[i] XOR clé[i % taille_clé]
```

Le XOR étant son propre inverse (`(a ^ k) ^ k = a`), la même fonction `applyXor` sert au chiffrement et au déchiffrement. La longueur du message n'est pas modifiée.

> ⚠️ **Stéganographie ≠ chiffrement.** La stéganographie *cache l'existence* du message ; le XOR le rend *illisible* sans la clé. Ce XOR à clé répétée est un mécanisme **pédagogique** : il se casse facilement (analyse de fréquence, texte clair connu) et **ne protège pas des données sensibles**.

---

## Gestion des erreurs

| Situation | Message affiché |
|-----------|-----------------|
| Aucune commande | `Missing command` |
| Commande inconnue | `Unknown command` |
| Mauvais nombre d'arguments / clé vide | `Invalid encode arguments` / `Invalid decode arguments` |
| Message vide | `Message is required for encoding` |
| Source = destination | `Destination file cannot be the same as source file` |
| Destination déjà existante | `Destination file already exists` |
| Fichier absent, tronqué ou format non supporté | `Cannot open or invalid BMP file` |
| Message trop long | `Message is too long for this image` |
| Image sans message caché | `No hidden message found in this image` |
| Message chiffré décodé sans clé | `A key is required to decode this message` |
| Message non chiffré décodé avec clé | `This message was encoded without a key` |
| Métadonnées incohérentes | `Invalid metadata in image` / `Invalid message length` |
| Échec d'écriture | `Cannot save destination BMP file` |

Les fichiers sont fermés dans tous les cas et chaque accès aux pixels reste dans les bornes validées au chargement.

---

## Démonstration

- **Image source :** `Projet-3/Test_images/Apple.bmp` (1200 × 794, 24 bits, 2 858 454 octets)
- **Message de test :** `Message secret avec espaces`
- **Clé :** `ma clé`

```bash
Projet-3.exe encode Projet-3/Test_images/Apple.bmp secret.bmp "Message secret avec espaces" "ma clé"
```

```bash
Projet-3.exe decode secret.bmp "ma clé"
```

Résultat attendu : `Message secret avec espaces`.
L'image `secret.bmp` a la même taille et les mêmes dimensions que la source ; seuls 95 octets diffèrent, chacun d'au plus 1.

---

## Vérification (scénarios exécutés)

| # | Scénario | Entrée | Résultat attendu | Résultat obtenu |
|---|----------|--------|------------------|-----------------|
| 1 | Aide | `help` | Affiche les commandes et exemples | ✅ OK |
| 2 | Sans commande | *(aucun argument)* | `Missing command`, code 1 | ✅ OK |
| 3 | Encodage sans clé, accents | `encode Apple.bmp out.bmp "Bonjour, ça va ? Élève à l'école"` | `Message encoded in out.bmp` | ✅ OK |
| 4 | Décodage sans clé | `decode out.bmp` | Texte identique, accents inclus | ✅ OK |
| 5 | Encodage avec clé | `encode Apple.bmp out2.bmp "Message secret avec espaces" "ma clé"` | Encodage réussi | ✅ OK |
| 6 | Décodage avec la bonne clé | `decode out2.bmp "ma clé"` | `Message secret avec espaces` | ✅ OK |
| 7 | Décodage sans la clé | `decode out2.bmp` | `A key is required…` | ✅ OK |
| 8 | Décodage avec une mauvaise clé | `decode out2.bmp "mauvaise"` | Texte illisible (pas de détection) | ✅ OK (comportement documenté) |
| 9 | Clé sur message non chiffré | `decode out.bmp "cle"` | `This message was encoded without a key` | ✅ OK |
| 10 | Image sans message | `decode Apple.bmp` | `No hidden message found…` | ✅ OK |
| 11 | BMP avec padding | image 7 × 10 (21 o de pixels + 3 o de padding par ligne), message de 17 o | Message restitué, padding et en-têtes inchangés | ✅ OK |
| 12 | Capacité maximale | image 7 × 10 : 17 o / image 50 × 50 : 928 o | Encodage et décodage réussis | ✅ OK |
| 13 | Dépassement de capacité | 18 o (7 × 10) / 929 o (50 × 50) | `Message is too long…` | ✅ OK |
| 14 | Destination existante | `encode Apple.bmp out.bmp "x"` | `Destination file already exists` | ✅ OK |
| 15 | Source = destination | `encode Apple.bmp Apple.bmp x` | Refus | ✅ OK |
| 16 | Message vide | `encode Apple.bmp o.bmp ""` | `Message is required…` | ✅ OK |
| 17 | Fichier absent | `encode nofile.bmp o.bmp hi` | `Cannot open or invalid BMP file` | ✅ OK |
| 18 | Fichier tronqué | 30 premiers octets d'un BMP | `Cannot open or invalid BMP file` | ✅ OK |
| 19 | Échec d'écriture | destination dans un dossier inexistant | `Cannot save destination BMP file` | ✅ OK |
| 20 | Commande inconnue | `foo` | `Unknown command` | ✅ OK |
| 21 | Invisibilité | `Apple.bmp` vs `out2.bmp` | Taille identique, écarts ≤ 1 | ✅ 95 octets modifiés, écart max 1 |

**Vérification croisée :** chaque membre a relu et exécuté les scénarios portant sur la partie écrite par l'autre (voir [Répartition du travail](#répartition-du-travail)).

---

## Limitations et défauts connus

- **Mauvaise clé non détectée :** le décodage avec une clé incorrecte renvoie un texte illisible au lieu d'une erreur (aucun contrôle d'intégrité n'est stocké). Limitation acceptée par le cahier des charges.
- **XOR faible :** chiffrement uniquement pédagogique (voir plus haut).
- **Pas de commande de copie simple :** le cycle chargement / sauvegarde sans transformation est garanti par le code (réécriture octet pour octet), mais il n'existe pas de commande dédiée pour le démontrer depuis la ligne de commande.
- **Encodage des arguments sous Windows :** `argv` est fourni dans la page de code de la console. Selon la configuration, des caractères accentués saisis dans le terminal peuvent être convertis avant même d'arriver au programme ; le programme, lui, conserve les octets reçus sans les modifier.
- **Formats non gérés :** BMP 32 bits, palettes, compression, en-têtes V4/V5 et images *top-down* (hauteur négative) sont refusés.
- **Message vide refusé :** un message de longueur 0 ne peut pas être caché.
- **Lecture / écriture des fichiers :** l'implémentation utilise `std::ifstream` / `std::ofstream` en mode binaire.
- **Vérification d'existence de la destination :** elle repose sur une tentative d'ouverture en lecture ; un fichier existant mais illisible (droits) ne serait pas détecté comme existant.

---

## Sources

- Microsoft Learn — *Bitmap Storage / BITMAPFILEHEADER / BITMAPINFOHEADER* : structure des en-têtes BMP.
- Microsoft Learn — *fread* : paramètres, valeurs de retour et gestion des erreurs.
- cppreference.com — `std::ifstream`, `std::ofstream`, opérateurs bit à bit, types `<cstdint>`.
- Manuel GIMP — *Exporter une image* : préparation des BMP de test.
- Supports de cours du module (9 au 13 novembre 2026).

---

## Répartition du travail

> *À compléter / ajuster par l'équipe.*

| Partie | Développeur principal | Relecture / tests |
|--------|-----------------------|-------------------|
| Lecture / sauvegarde BMP (`Bmp.h/.cpp`) | *À compléter* | *À compléter* |
| Stéganographie LSB et métadonnées (`Stegano.h/.cpp`) | *À compléter* | *À compléter* |
| Chiffrement XOR (`XOR.h/.cpp`) | *À compléter* | *À compléter* |
| Ligne de commande et erreurs (`CommandLine.h/.cpp`, `Main.cpp`) | *À compléter* | *À compléter* |
| Scénarios de vérification et README | Astor & Noah | Astor & Noah |

**Équipe :** Astor · Noah

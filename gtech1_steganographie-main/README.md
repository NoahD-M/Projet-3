# GTech 1 : Fiche de révision C++20
## Manipulation de bits, fichiers binaires et stéganographie BMP

**Objectif :** comprendre comment lire les octets d'un fichier BMP, modifier les bits des composantes couleur pour dissimuler un message, puis retrouver ce message.

Cette fiche reprend les fondamentaux C++ vus en cours et les notions nécessaires à la réalisation du projet.

---

## 1. Représentation des données

En informatique, les données sont représentées par des bits (`0` ou `1`).

Un **octet** contient 8 bits et peut représenter 256 valeurs différentes.

| Base | Exemple | Signification |
|---|---|---|
| Décimale | `85` | Base 10 |
| Binaire | `0b01010101` | Base 2 |
| Hexadécimale | `0x55` | Base 16 |

Ces trois écritures représentent exactement la même valeur.

### Les entiers sur 8 bits

```cpp
#include <cstdint>

std::uint8_t value = 0b01010101;
```

`std::uint8_t` est un type entier non signé sur exactement 8 bits, lorsqu'il est disponible.

Il représente les valeurs de **0 à 255**.

Attention aux dépassements de capacité :

```cpp
std::uint8_t a = 1234;   // 210
std::uint8_t b = 0x12AF; // 0xAF
```

Lorsqu'un entier est converti vers un type non signé sur 8 bits, la valeur est ramenée modulo 256.

### Afficher une valeur dans différentes bases

```cpp
#include <bitset>
#include <cstdint>
#include <iostream>

std::uint8_t value = 85;

std::cout << std::dec << +value << '\n';
std::cout << std::hex << +value << '\n';
std::cout << std::bitset<8>(value) << '\n';
```

Résultat :

```text
85
55
01010101
```

Le `+` unaire provoque une promotion entière, utile pour éviter qu'un `std::uint8_t` soit affiché comme un caractère.

`std::bitset<8>` est particulièrement pratique pour **visualiser** les bits d'un octet.

---

## 2. Les opérateurs binaires

Les opérateurs bit à bit travaillent directement sur la représentation binaire des valeurs.

Prenons :

```cpp
std::uint8_t a = 0b01010101;
std::uint8_t b = 0b11111111;
```

| Opérateur | Description | Exemple de résultat |
|---|---|---|
| `&` | AND : les deux bits doivent valoir 1 | `a & b` → `01010101` |
| `\|` | OR : au moins un bit doit valoir 1 | `a \| b` → `11111111` |
| `^` | XOR : les deux bits doivent être différents | `a ^ b` → `10101010` |
| `~` | NOT : inverse les bits | Inversion de `01010101` |
| `<<` | Décalage vers la gauche | `a << 1` → `10101010` |
| `>>` | Décalage vers la droite | `a >> 1` → `00101010` |

**Attention aux promotions entières :** les opérations réalisées sur des `std::uint8_t` sont généralement évaluées sur le type `int`.

Pour récupérer un résultat sur 8 bits, on peut utiliser une conversion :

```cpp
std::uint8_t inverted =
    static_cast<std::uint8_t>(~a);
```

### Masques binaires

Un masque permet d'observer ou de modifier certains bits sans affecter les autres.

```cpp
std::uint8_t value = 0b01010101;
std::uint8_t mask  = 0b00000100;
```

Opérations à connaître :

```cpp
// Tester le bit d'indice 2
bool is_set = (value & mask) != 0;

// Mettre le bit à 1
value = value | mask;

// Mettre le bit à 0
value = value & ~mask;

// Inverser le bit
value = value ^ mask;
```

Les bits sont numérotés à partir de **0**, en commençant par le bit de poids faible (tout à droite).

Pour construire un masque :

```cpp
unsigned index = 3;
unsigned mask = 1u << index;
```

Pour un octet, l'indice doit être compris entre 0 et 7.

### Outils de débogage

Dans Visual Studio, la fenêtre **Watch** permet d'inspecter les valeurs dans différentes bases.

Exemples :

- `value,b` : affichage binaire.
- `value,x` : affichage hexadécimal.
- `value,d` : affichage décimal.

Ces représentations facilitent la vérification des masques et des décalages.

---

## 3. Le XOR et son utilisation

Le XOR possède une propriété particulièrement intéressante :

**Appliquer deux fois le même XOR restaure la valeur initiale.**

```cpp
std::uint8_t message = 0x41; // 'A'
std::uint8_t key = 0x2A;

std::uint8_t encrypted = message ^ key;
std::uint8_t decrypted = encrypted ^ key;
```

En binaire :

```text
Message   01000001
Clé       00101010
          --------
XOR       01101011

Résultat  01101011
Clé       00101010
          --------
XOR       01000001
```

On retrouve bien `0x41`, soit le caractère `A`.

Pour un message de plusieurs octets, on peut appliquer le XOR successivement à chacun d'eux. Avec une clé composée de plusieurs octets, cette clé peut être répétée.

**Important :** ce XOR sert ici à comprendre une transformation réversible. Ce n'est pas un chiffrement sécurisé pour protéger des informations sensibles.

---

## 4. Lecture et écriture de fichiers binaires

Les deux classes principales sont :

- `std::ofstream` : écrire dans un fichier.
- `std::ifstream` : lire un fichier.

Elles sont fournies par `<fstream>`.

### Écriture

```cpp
#include <fstream>
#include <stdexcept>
#include <string>

std::ofstream file("example.bin", std::ios::binary);

if (!file.is_open())
{
    throw std::runtime_error("Cannot open file");
}

std::string text = "Hello World!";

file.write(text.data(), text.size());
file.close();
```

`write()` écrit un nombre exact d'octets.

Le caractère nul final d'une chaîne C n'est pas inclus ici : `text.size()` renvoie 12 pour `"Hello World!"`.

Le mode `std::ios::binary` évite les transformations propres au mode texte, notamment les conversions de fins de ligne sous Windows.

Un `std::ofstream` ouvert normalement remplace le contenu du fichier existant. Le mode `std::ios::app`, lui, écrit à la fin.

### Lecture

```cpp
#include <fstream>
#include <string>

std::ifstream file("example.bin", std::ios::binary);

if (!file.is_open())
{
    throw std::runtime_error("Cannot open file");
}

constexpr std::size_t buffer_size = 500;
char buffer[buffer_size];

file.read(buffer, buffer_size);

std::streamsize bytes_read = file.gcount();

std::string result(
    buffer,
    static_cast<std::size_t>(bytes_read)
);
```

`read()` tente de lire le nombre d'octets demandé.

`gcount()` indique combien d'octets ont effectivement été lus.

**Ne pas écrire :**

```cpp
std::string result(buffer);
```

Le buffer n'est pas nécessairement terminé par un caractère nul `'\0'`. Cette construction pourrait lire au-delà des données valides.

La construction `std::string(buffer, size)` précise explicitement le nombre d'octets à copier.

Pour manipuler un fichier binaire entier, `std::vector<std::uint8_t>` est également adapté : il possède ses données et sa taille peut varier dynamiquement.

### Position du curseur

```cpp
file.clear();
file.seekg(10, std::ios::beg);

std::streampos position = file.tellg();
```

- `seekg()` déplace le curseur de lecture.
- `tellg()` indique sa position.
- `std::ios::beg` désigne le début du fichier.
- `std::ios::cur` désigne la position actuelle.
- `std::ios::end` désigne la fin du fichier.

Exemple :

```cpp
file.seekg(10, std::ios::beg);
```

Positionne le curseur à l'offset 10, c'est-à-dire après les 10 premiers octets du fichier.

**Pourquoi `clear()` ?**

Une lecture qui demande plus d'octets que le fichier n'en possède active des indicateurs d'erreur, notamment `eofbit` et `failbit`.

`clear()` réinitialise ces indicateurs avant un repositionnement. Il ne modifie pas les octets du fichier.

### Ne pas oublier de contrôler les erreurs

L'ouverture réussie ne garantit pas que toutes les opérations de lecture ou d'écriture réussiront.

Une lecture d'en-tête doit notamment vérifier qu'elle a obtenu tous les octets demandés.

---

## 5. Working directory dans Visual Studio

Le **working directory** (répertoire de travail courant) est le dossier utilisé pour résoudre les chemins relatifs.

Il n'est pas nécessairement identique au dossier contenant l'exécutable.

Pour le connaître :

```cpp
#include <filesystem>
#include <iostream>

int main()
{
    std::cout
        << "Working Directory : "
        << std::filesystem::current_path()
        << '\n';
}
```

### Chemin relatif et chemin absolu

```cpp
std::ifstream file("image.bmp", std::ios::binary);
```

Le programme cherche `image.bmp` dans le répertoire de travail courant.

En revanche :

```cpp
std::ifstream file(
    R"(C:\Images\image.bmp)",
    std::ios::binary
);
```

Utilise un chemin absolu.

Exemple :

```text
Working Directory : C:\Projects\Steganography
```

Dans ce cas, `"image.bmp"` désigne :

```text
C:\Projects\Steganography\image.bmp
```

### Configurer Visual Studio

Dans les propriétés du projet C++ :

**Configuration Properties → Debugging → Working Directory**

Il est possible de définir le répertoire à utiliser lors du lancement depuis Visual Studio.

La macro `$(ProjectDir)` désigne le dossier du projet.

Le répertoire de travail peut être différent lorsque l'exécutable est lancé depuis un terminal : il dépend alors du répertoire courant de ce terminal.

**À retenir :** lorsqu'un fichier est introuvable, vérifier le working directory avant de supposer que la lecture du fichier est incorrecte.

---

## 6. Arguments de ligne de commande : argc et argv

La fonction `main()` peut recevoir les arguments transmis au lancement du programme.

```cpp
int main(int argc, char* argv[])
{
}
```

- `argc` : nombre d'arguments.
- `argv` : tableau contenant les arguments sous forme de chaînes C.
- `argv[0]` : nom ou chemin utilisé pour lancer le programme.
- `argv[1]` : premier argument utilisateur.
- `argv[argc]` : pointeur nul (`nullptr`).

Les chaînes C pointées par `argv[i]` sont terminées par `'\0'`.

### Conversion vers std::string

```cpp
#include <string>

std::string command = argv[1];
```

`std::string` peut être construit directement à partir d'une chaîne C terminée par `'\0'`.

Contrairement aux chaînes C, un `std::string` conserve explicitement sa longueur et peut contenir des octets nuls.

### Exemple de CLI

```text
Steganography.exe encode input.bmp output.bmp "Hello World!"
Steganography.exe decode output.bmp
```

Pour l'encodage :

| Index | Valeur |
|---|---|
| `argv[0]` | `Steganography.exe` |
| `argv[1]` | `encode` |
| `argv[2]` | `input.bmp` |
| `argv[3]` | `output.bmp` |
| `argv[4]` | `Hello World!` |

`argc` vaut donc 5.

Les guillemets permettent au terminal de transmettre `"Hello World!"` comme un seul argument.

### Validation des arguments

Toujours vérifier `argc` avant d'accéder à un indice de `argv`.

```cpp
if (argc < 2)
{
    std::cerr << "Missing command\n";
    return 1;
}

std::string command = argv[1];

if (command == "encode")
{
    if (argc != 5)
    {
        std::cerr << "Invalid encode arguments\n";
        return 1;
    }
}
else if (command == "decode")
{
    if (argc != 3)
    {
        std::cerr << "Invalid decode arguments\n";
        return 1;
    }
}
else
{
    std::cerr << "Unknown command\n";
    return 1;
}
```

Dans Visual Studio, les arguments se configurent dans :

**Configuration Properties → Debugging → Command Arguments**

Note : les arguments de `main()` ne sont pas universellement garantis en UTF-8, notamment sous Windows. Pour les premiers tests, des chemins et messages ASCII permettent d'éviter cette difficulté supplémentaire.

---

## 7. Structure d'un fichier BMP

Pour le projet, nous nous intéressons au format **BMP 24 bits non compressé** utilisant un `BITMAPINFOHEADER` de 40 octets.

Le BMP contient plusieurs zones distinctes.

| Partie | Taille | Rôle |
|---|---:|---|
| BITMAPFILEHEADER | 14 octets | Identification et informations générales |
| BITMAPINFOHEADER | 40 octets | Dimensions, profondeur, compression |
| Données des pixels | Variable | Couleurs de l'image |

### Champs importants

Les offsets suivants sont exprimés depuis le début du fichier.

| Offset | Taille | Signification |
|---|---:|---|
| 0 | 2 octets | Signature `BM` |
| 2 | 4 octets | Taille déclarée du fichier |
| 10 | 4 octets | Offset des données des pixels |
| 14 | 4 octets | Taille de l'en-tête DIB |
| 18 | 4 octets | Largeur |
| 22 | 4 octets | Hauteur |
| 26 | 2 octets | Nombre de plans |
| 28 | 2 octets | Bits par pixel |
| 30 | 4 octets | Type de compression |

Pour notre cas :

- Signature : `BM`, soit `0x42 0x4D`.
- En-tête DIB : 40 octets.
- Nombre de plans : 1.
- Bits par pixel : 24.
- Compression : 0 (`BI_RGB`).
- Dimensions positives.

Les entiers multioctets sont stockés en **little-endian**.

Exemple :

```text
36 00 00 00
```

Représente la valeur `0x00000036`, soit 54 en décimal.

Un BMP minimal avec ces deux en-têtes commence souvent ses pixels à l'offset 54. Toutefois, le véritable offset doit être lu dans l'en-tête du fichier.

### Couleurs : ordre BGR

Dans un BMP 24 bits, chaque pixel possède trois composantes sur un octet :

```text
Bleu | Vert | Rouge
  B  |   G  |   R
```

Chaque composante possède une valeur entre 0 et 255.

Ainsi, les octets :

```text
00 00 FF
```

Représentent un pixel rouge en BGR.

### Padding des lignes

Chaque ligne de pixels doit occuper un nombre d'octets multiple de 4.

Exemple : une image de 3 pixels de large.

```text
Pixel 1   Pixel 2   Pixel 3   Padding
B G R     B G R     B G R     -- -- --
```

Trois pixels représentent :

```text
3 × 3 = 9 octets
```

La ligne occupe donc 12 octets, avec 3 octets de padding.

Formule pour un BMP 24 bits :

```cpp
std::size_t stride = (width * 3 + 3) / 4 * 4;
```

Cette formule suppose une largeur valide et l'absence de dépassement arithmétique.

**Le padding ne représente pas des pixels et ne doit pas servir à cacher des bits dans notre algorithme.**

### Ordre des lignes

Pour une hauteur positive, un BMP est généralement stocké en **bottom-up** : les octets de la première ligne correspondent à la ligne inférieure de l'image.

Cela signifie que l'ordre des lignes dans le fichier n'est pas forcément celui que l'on utilise pour afficher l'image à l'écran.

### Retrouver une composante

Si `row` correspond à un indice de ligne dans l'ordre de stockage du fichier :

```cpp
std::size_t index =
    pixel_offset +
    row * stride +
    column * 3 +
    channel;
```

Avec :

- `channel = 0` : bleu.
- `channel = 1` : vert.
- `channel = 2` : rouge.

Avant d'accéder à cet index, les dimensions, l'offset et les limites du buffer doivent être vérifiés.

---

## 8. Stéganographie : le principe LSB

La **stéganographie** consiste à dissimuler une information dans un autre support.

Dans notre cas, nous cachons les bits d'un message dans les composantes couleur d'une image.

**LSB** signifie *Least Significant Bit*, ou bit de poids faible.

C'est le bit le plus à droite d'un octet.

### Modifier une composante

Exemple :

```text
Avant : 10100110 = 0xA6 = 166
Après : 10100111 = 0xA7 = 167
```

Un seul bit a changé.

La valeur de la composante varie de 1, ce qui produit généralement une variation visuelle imperceptible.

### Insérer un bit

```cpp
component = (component & 0xFEu) | bit;
```

Explication :

1. `0xFE` correspond à `11111110`.
2. Le AND met le dernier bit de `component` à 0.
3. Le OR insère le nouveau bit, qui doit valoir 0 ou 1.

### Extraire un bit

```cpp
std::uint8_t bit = component & 1u;
```

On récupère uniquement le dernier bit.

### Reconstruire un octet

Si les bits ont été insérés du bit de poids fort vers le bit de poids faible :

```cpp
value = static_cast<std::uint8_t>((value << 1) | bit);
```

On décale les bits précédemment récupérés puis on ajoute le nouveau bit.

**Attention à l'ordre :** l'encodage et le décodage doivent utiliser exactement le même ordre de lecture et d'écriture.

### Exemple avec le caractère A

```text
'A' = 0x41 = 01000001
```

Il faut cacher 8 bits, à raison d'un bit par composante couleur.

Un pixel BMP 24 bits contient 3 composantes. Il faut donc parcourir 3 pixels pour disposer d'au moins 8 composantes.

On ne modifie que les octets B, G et R des pixels, jamais les octets d'en-tête ou de padding.

---

## 9. Capacité de l'image

Avec un bit caché par composante et trois composantes par pixel :

```text
1 pixel = 3 bits cachés
```

Pour une image de largeur `W` et hauteur `H` :

\[
\text{Capacité brute en bits} = W \times H \times 3
\]

\[
\text{Capacité brute en octets} =
\left\lfloor \frac{W \times H \times 3}{8} \right\rfloor
\]

Exemple : BMP de 8 × 8 pixels.

```text
8 × 8 = 64 pixels
64 × 3 = 192 bits
192 / 8 = 24 octets
```

Il est donc possible de cacher **24 octets au maximum avant de réserver l'espace des métadonnées**.

Ce maximum n'est pas nécessairement la taille du texte utile : les informations permettant de retrouver le message occupent elles aussi de la place.

Le programme doit vérifier la capacité **avant toute modification du fichier**.

---

## 10. Identifier et retrouver un message

Une image contient déjà des bits de poids faible, même lorsqu'aucun message n'a été caché.

Le décodeur ne peut donc pas simplement lire tous les LSB et supposer qu'ils représentent un message valide.

Il faut définir un format commun à l'encodage et au décodage.

Le projet prévoit notamment :

| Information | Utilité |
|---|---|
| Signature | Identifier la présence d'un message |
| Longueur | Connaître la quantité de données à extraire |
| Indicateur XOR | Savoir si le message est chiffré |
| Données | Contenu du message |

L'ordre des champs, leurs tailles et leur représentation binaire doivent être identiques pour les deux opérations.

### Attention à la longueur des chaînes

`std::string::size()` renvoie une longueur **en octets**, pas nécessairement en caractères visibles.

Par exemple, en UTF-8, certains caractères accentués utilisent plusieurs octets.

Pour le projet, la taille enregistrée doit correspondre au nombre d'octets réellement insérés.

---

## 11. Méthode de travail pour le projet

Le projet peut être développé progressivement en validant chaque étape.

### Étape 1 — Vérifier les fichiers

- Ouvrir un véritable BMP en mode binaire.
- Lire sa signature.
- Vérifier les champs nécessaires de l'en-tête.
- Enregistrer une copie sans modification.
- Comparer les deux fichiers.

### Étape 2 — Identifier les pixels

- Récupérer l'offset des pixels.
- Calculer le stride.
- Parcourir les composantes BGR.
- Ne pas modifier les octets de padding.

### Étape 3 — Tester LSB

- Cacher un seul bit.
- Cacher un octet complet.
- Extraire cet octet.
- Vérifier que la valeur retrouvée est correcte.

### Étape 4 — Gérer un message

- Définir un format de message.
- Insérer la signature et la longueur.
- Ajouter le contenu.
- Extraire exactement les octets attendus.
- Ajouter l'option XOR.

### Étape 5 — Rendre le programme robuste

- Valider les arguments de la CLI.
- Vérifier le format BMP.
- Refuser un message trop grand.
- Gérer les erreurs de lecture et d'écriture.
- Ne pas écraser le fichier source.
- Tester les cas incorrects.
- Documenter l'utilisation dans le README.

---

## 12. Erreurs fréquentes à éviter

| Erreur | Pourquoi c'est problématique |
|---|---|
| Oublier `std::ios::binary` | Risque de transformations des octets en mode texte |
| Utiliser `std::ios::app` | Ajoute à la fin du fichier plutôt que de modifier les pixels |
| Utiliser `std::string(buffer)` sur un buffer brut | Suppose une terminaison `'\0'` |
| Ignorer `gcount()` | Suppose que tous les octets demandés ont été lus |
| Utiliser `seekg()` après un échec sans réinitialiser le flux | Le repositionnement peut échouer |
| Supposer que le working directory est le dossier de l'exécutable | Les chemins relatifs peuvent pointer ailleurs |
| Accéder à `argv[1]` sans vérifier `argc` | Risque d'accès invalide |
| Supposer que tous les BMP sont en 24 bits non compressés | Risque de mal interpréter les données |
| Supposer que les pixels commencent toujours à l'offset 54 | Ignore le champ d'offset réel |
| Traiter le padding comme une composante | Modifie des octets qui ne sont pas des couleurs |
| Confondre un pixel et un octet | Un pixel BMP 24 bits possède trois octets de couleur |
| Oublier la capacité du BMP | Risque de dépasser les données de l'image |
| Utiliser XOR comme mécanisme de sécurité | Le XOR répétitif ne constitue pas un chiffrement sécurisé |

---

## 13. Questions de révision

Avant de démarrer le développement, vous devriez pouvoir répondre à ces questions :

1. Quelle différence existe-t-il entre `0x55`, `85` et `0b01010101` ?
2. Comment tester si le bit d'indice 4 vaut 1 ?
3. Quelle opération permet d'inverser un bit donné ?
4. Pourquoi appliquer deux fois le même XOR retrouve-t-il la valeur initiale ?
5. Quelle différence existe-t-il entre `read()` et `gcount()` ?
6. Pourquoi utilise-t-on `std::ios::binary` pour un BMP ?
7. Que représente le working directory ?
8. Que contiennent `argc` et `argv` ?
9. Pourquoi un BMP 24 bits stocke-t-il les couleurs dans l'ordre BGR ?
10. À quoi sert le padding d'une ligne ?
11. Pourquoi ne faut-il pas modifier les en-têtes BMP ?
12. Comment cacher un bit dans une composante couleur ?
13. Comment reconstruire un octet depuis huit LSB ?
14. Pourquoi doit-on enregistrer la longueur du message ?
15. Comment vérifier qu'un message peut tenir dans une image ?

**Objectif final :** être capable de parcourir les octets utiles d'un fichier BMP, d'y insérer des données et de les récupérer, en vérifiant systématiquement la validité du fichier et les limites de chaque opération.

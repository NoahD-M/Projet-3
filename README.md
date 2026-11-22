
Astor Noah D-M

Building:
Visual Studio 2026 C++20


Commands:
help,
encode Test_images/Apple.bmp "message" "key",
decode Test_images/Apple.bmp "message" "key",
else: Unknown command,


Supported format:
image.bmp,
24 bits per pixels,
compression: O,

Code organisation:

Main: lit les arguments,
CommandLine: valide les arguments,
Bmp: charge et sauvegarde l'image,
Stegano: cache ou extrait le message,
XOR: chiffre ou déchiffre,

limitations:
un seul format d'image accepté

sources:
Document fourni par le Professeur,
Gemini/Claude --> Explication du projet simplifié et étape à suivre afin de mener à bien le projet et savoir où chercher les informations, 
Microsoft Learn/Stack Overflow --> Comprendre et utiliser les modules,

Répartition des tâches:
Noah D-M -- Création/organisation du repo, CommandLine.cpp / .h, stegano.cpp / .h, Test.images/Apple.bmp README.md,
Astor -- XOR.cpp / .h, Bmp.cpp / .h, main.cpp README.md,








# Règles du portage fidèle

Objectif : KICK original fonctionnel et portable sous Windows. Pas une application inspirée de KICK.

- Réutiliser l'interface, les modèles, les voix, les séquences, les effets, le format de projet et les règles musicales d'origine.
- Autoriser le code technique nécessaire à Windows : périphériques, routage des événements vers les commandes existantes, compilation, chemins et distribution.
- Ne pas ajouter de fonction musicale et ne pas remplacer un composant absent par une version simplifiée inventée.
- Distinguer explicitement une dépendance restaurée, une adaptation de plateforme et un élément encore indisponible.
- Ne pas considérer le chargement d'un écran, une réponse factice, une étape de configuration ou un test AudIO isolé comme une validation de la machine complète.
- Garder les sources publiées intactes dans `upstream/`. Les adaptations éventuelles sont séparées, traçables et appliquées à une copie de compilation.
- Ne pas lancer les scripts de flashage, de formatage ou de fabrication Buildroot dans le portage Windows.
- Conserver les anciennes expérimentations et leurs tags pour référence ; ne plus étendre leur séquenceur, leur éditeur ou leur format `.kicklab` pour prétendre compléter KICK.

Pour chaque étape : indiquer les fichiers d'origine, les modifications de plateforme, les dépendances, la compilation effectuée et les comportements réellement vérifiés. Une version ne sera présentée comme KICK Windows fonctionnel qu'après vérification de l'interface originale reliée au moteur original.

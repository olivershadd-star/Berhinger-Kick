# Portage progressif sans surface matérielle

## Base garantie

La base de chaque évolution est KICK Lab 0.2.0 : AudIO rend les seize lecteurs vers un bus stéréo, WinMM fournit la sortie Windows, et les commandes souris, clavier et MIDI déclenchent les pads. Le séquenceur, le chargement WAV, l'éditeur et les deux effets existants constituent le test de non-régression.

Une évolution ne remplace plus simultanément le périphérique audio, le graphe, les modèles et l'interface.

## Ce que la publication Music Tribe ne fournit pas

- aucun fichier audio d'usine ni son de métronome ;
- aucun programme jouable prêt à sélectionner ;
- cinq démos et cinq modèles dont les fichiers `.kickPrj` ne contiennent que des structures minimales ;
- plusieurs dépendances de compilation, constantes et contrats de périphérique ;
- la surface de contrôle physique utilisée par certains écrans.

Une catégorie visible dans l'interface ne prouve donc pas qu'un instrument, un preset ou un effet utilisable est fourni derrière cette catégorie.

## Ordre d'intégration

1. **Transport et longueur de séquence** : reprendre les modèles de temps KICK tout en conservant le rendu WinMM. Les pages et longueurs doivent être accessibles à la souris.
2. **Programmes de samples** : reprendre un type de programme KICK et son modèle de slices, avec création, sélection et suppression entièrement cliquables.
3. **Métronome** : reprendre le déclenchement KICK, fournir explicitement un son de test remplaçable puisque le son d'usine est absent, puis vérifier son routage dans le bus WinMM.
4. **Effets et inserts** : intégrer une chaîne originale à la fois. Les paramètres doivent être visibles, sélectionnables et contournables sans molette matérielle.
5. **Synthétiseurs** : intégrer seulement ceux dont le DSP et toutes les dépendances sont réellement publiés. Un écran ou une catégorie vide ne compte pas comme un synthétiseur porté.
6. **Écrans originaux** : reprendre ensuite les composants visuels correspondants, après validation de leur modèle et de leur audio dans la base jouable.

## Critères obligatoires pour chaque étape

- le programme démarre sans contrôleur KICK, MIDI ou périphérique externe ;
- chaque nouvelle commande est utilisable à la souris ;
- pads, samples, séquenceur et effets déjà acquis continuent de produire du son ;
- le nouveau module possède un scénario audio vérifiable ;
- la manipulation de l'interface pendant la lecture ne coupe ni ne déforme le son ;
- la version portable est testée, pas seulement compilée.

Le port complet conservé sur `port/kick-original-windows` reste un inventaire utile : il montre quelles unités compilent et quels écrans existent. Il ne doit plus être distribué comme version jouable avant que les modules aient passé ces étapes séparément.

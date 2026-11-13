# Passation — base Windows commune, KICK et Impulsion

État au 12 novembre 2026.

## Décision de produit

Le travail doit se séparer en deux interfaces, mais pas en deux moteurs :

- **KICK** reste le portage fidèle du logiciel Music Tribe/Behringer. Sa navigation, ses modes et ses fonctions doivent provenir du dépôt `0700-AAM`.
- **Impulsion** peut modifier l'organisation visuelle et l'ergonomie selon les choix du projet, tout en conservant le moteur AudIO et les concepts fonctionnels de KICK.
- Le moteur audio validé, le séquenceur, le stockage des projets, la bibliothèque et les adaptateurs Windows restent partagés tant qu'une divergence réelle n'est pas nécessaire.

La séparation des branches ne doit intervenir qu'après validation de la présente base commune. Noms proposés : `port/kick-windows` et `impulsion`.

## Base commune déjà en place

Le prototype stable se trouve dans `experiments/kicklab/source` :

- moteur AudIO à 48 kHz et sortie Windows WinMM ;
- 16 lecteurs de samples et séquenceur 16 pas ;
- quatre inserts par pad et quatorze effets issus du code distribué ;
- bibliothèque locale vide et import de WAV ;
- sauvegarde des projets ;
- interface redimensionnable, clavier et entrée MIDI.

Le fichier `src/control_surface.hpp` introduit maintenant la couche d'adaptation commune. La souris, le clavier, le MIDI et, plus tard, Maschine+ produisent des commandes logiques identiques : pads, pas, transport, encodeur central, Q-Links, Shift et retour. Le callback MIDI transmet ses événements au thread de l'interface avant d'agir sur l'application.

Deux `DisplayBinding` indépendants décrivent les deux écrans. Chaque écran choisit son module, son pad, son slot d'effet et sa page. Il ne crée aucun effet : il ne fait qu'afficher et commander le même état audio. Une page relie quatre paramètres aux quatre encodeurs fixes de l'écran.

Exemples possibles :

- écran A : insert 1, paramètres 1 à 4 ; écran B : insert 1, paramètres 5 à 8 ;
- écran A : navigateur de samples ; écran B : paramètres du sample ;
- écran A : insert 1 ; écran B : insert 2.

## Correspondance avec le code KICK original

La couche commune n'invente pas un second moteur de navigation. Elle remplace le matériel absent et doit progressivement alimenter les gestionnaires originaux :

- `Kick/modules/kick_ui/dialogs/kick_AppHome.cpp` centralise la navigation de l'application ;
- les fichiers `handleCommand` des différents modes reçoivent les actions matérielles ;
- `kick_AppTrack.cpp` et `kick_PadMixer.cpp` montrent quatre groupes logiques de quatre encodeurs (`EN_KNOB_1` à `EN_KNOB_16`) ;
- `modes/filebrowser/kick_fileBrowser.cpp` utilise l'encodeur central pour parcourir et sa pression pour confirmer.

Les deux écrans Windows à quatre encodeurs sont donc un adaptateur de surface. Ils exposent huit paramètres simultanément et les pages donnent accès aux suivants. Dans la branche KICK, les événements doivent être traduits vers les contrats matériels originaux. Dans Impulsion, ils pourront être reliés à une navigation différente sans toucher au moteur.

## Interface commune à construire par étapes

1. Afficher deux écrans au-dessus de la zone de travail, avec quatre encodeurs fixes sous chacun.
2. Conserver le séquenceur 16 pas cliquable : il remplace les pads et touches de pas physiques qui n'existent pas sur le PC.
3. Relier les pages des écrans aux quatre slots d'effets déjà fonctionnels.
4. Ajouter le navigateur en trois niveaux : pad/slot, catégorie, sample.
5. Charger le sample sélectionné en préécoute temporaire pendant la lecture du pattern, puis confirmer ou annuler.
6. Étendre l'adaptateur MIDI avec un écran d'apprentissage et des profils enregistrables.

Le chargement en préécoute devra préserver les réglages du pad. Aujourd'hui `Engine::load()` remplace le sample mais réinitialise également son édition et ses inserts ; il faut donc préparer le nouveau WAV hors verrou, conserver `PadEdit`, puis effectuer un échange sûr. Ce point doit être corrigé avant le défilement audio instantané dans la bibliothèque.

## Maschine+ comme surface de contrôle

Les documents officiels Native Instruments confirment :

- 16 pads sensibles à la vélocité ;
- deux écrans 480 × 272 ;
- huit encodeurs tactiles sous les écrans ;
- un encodeur 4D pouvant tourner, être poussé, touché et déplacé dans quatre directions ;
- transport assignable ;
- mode MIDI accessible avec `SHIFT + CHANNEL/MIDI` ;
- utilisation comme contrôleur MIDI d'une application Windows par USB ;
- pages d'encodeurs et pages de pads configurables dans Controller Editor.

Pour KICK Windows, le profil visé est :

| Commande Maschine+ | Commande commune |
| --- | --- |
| Pads 1–16 | Pads KICK 1–16, notes MIDI configurables, valeurs proposées 36–51 |
| Encodeurs 1–4 | Q-Link 1–4 de l'écran A |
| Encodeurs 5–8 | Q-Link 1–4 de l'écran B |
| Encodeur 4D, rotation | Déplacement dans une liste ou changement de valeur |
| Encodeur 4D, pression | Validation |
| Encodeur 4D, directions | Changement de champ, page ou écran |
| PLAY, STOP, REC | Transport KICK |
| SHIFT | Fonctions secondaires |

Le profil ne doit pas supposer des numéros CC fixes : Controller Editor permet de les modifier. Il faut proposer un profil par défaut et un mode **MIDI Learn**.

Les écrans matériels de Maschine+ affichent les affectations et pages gérées par son micrologiciel/Controller Editor. Le MIDI standard ne permet pas à KICK de leur envoyer librement son interface graphique. Les contrôles physiques sont donc exploitables, avec retour de LED MIDI possible, mais l'interface détaillée à deux écrans reste affichée sur le moniteur Windows. Une intégration propriétaire des écrans ne fait pas partie de cette première phase.

Sources officielles consultées :

- [Native Instruments — Maschine+](https://www.native-instruments.com/en/products/maschine/production-systems/maschine-plus/specifications/)
- [Manuel en ligne Maschine+](https://docs.native-instruments.com/online-guides/maschine-plus-manual/en/index-en)
- [Manuel PDF Maschine+](https://docs.native-instruments.com/pdf-guides/maschine/MASCHINE_Plus_Manual_English_13_05_2022.pdf)
- [Controller Editor Manual](https://docs.native-instruments.com/pdf-guides/Controller_Editor_Manual_English_2017_11.pdf), notamment le chapitre 10 consacré à Maschine MK3, dont la surface et le mode MIDI correspondent aux contrôles étudiés.

## Règles pour les deux branches

- Ne jamais remplacer le moteur audio stable pendant une étape d'interface.
- Une évolution à la fois, avec test audio et test de la commande concernée.
- Ne pas présenter une fonction inventée comme une fonction originale de KICK.
- Dans la branche KICK, conserver les noms, états et parcours du dépôt original chaque fois qu'ils sont disponibles.
- Dans Impulsion, documenter explicitement toute divergence ergonomique.
- Toute commande de souris, clavier ou MIDI doit passer par la couche de surface commune ; aucune nouvelle commande matérielle ne doit appeler directement le moteur.
- Les sources originales sous `upstream/0700-AAM` restent immuables. Les adaptations vivent dans le portage.

## Point de reprise conseillé

Les deux écrans et leurs huit encodeurs sont désormais reliés aux slots et paramètres d'effets existants, avec un focus, une sélection et un `DisplayBinding` indépendants. La prochaine étape doit construire le moniteur MIDI puis le MIDI Learn au-dessus de la même couche `SurfaceAction`, avant le profil Maschine+. Le navigateur de bibliothèque viendra ensuite sans remplacer ces contrôles.

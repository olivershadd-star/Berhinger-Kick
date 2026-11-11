# Adaptations Windows : valeurs et limites de fidélité

Les sources `upstream/` ne sont pas modifiées. Les adaptations sont appliquées à une copie et consignées dans `port-changes.json`.

## Configuration restaurée

Le module `port/modules/AudIO_constants` permet maintenant de dépasser les premières erreurs de constantes manquantes. Il ne prétend pas reproduire la configuration de la carte Behringer :

| Définitions | Origine / statut |
|---|---|
| Bloc de 480 échantillons | Imposé par `AudIO/AudIO/Daw/DawEngine.cpp`, pool mémoire de blocs fixes. |
| MIDI 0–127, 128 notes, canaux 1–16 | Convention MIDI ; facteur normalisé `1/127`. |
| Panoramique -1 à 1, centre 0 | Convention utilisée dans `AudIO/AudIO/Dsp/pan.h`. |
| Gain unitaire 1 ; racine de 2 | Gain linéaire et constante mathématique ; plage supérieure 2 et pas 0,01 provisoires. |
| 48 kHz, deux entrées/deux sorties, quatre retours, huit sous-mixages | Profil de compilation Windows choisi pour démarrer l'intégration ; configuration matérielle originale non récupérée. |
| Polyphonie max. 64, défaut 16 | Valeurs de compatibilité provisoires, non attestées par les définitions originales absentes. |
| Niveau MIDI par défaut 100, note par défaut 60, plancher de vumètre -100 dB | Choix provisoires ; ne pas les présenter comme des valeurs Behringer retrouvées. |

Les limites musicales déjà présentes dans `kick_Constants.h` (tempo, mesures, transposition, etc.) restent celles d'origine. Le séquenceur, les modèles, les voix et l'interface ne sont pas remplacés. Ces valeurs provisoires empêchent encore de certifier une équivalence fonctionnelle exacte au firmware.

Les types d'énumération `mt::Recorder::Direction` et `Mode` reprennent les valeurs présentes dans `AudIO/Objects/Recorder.h`. Ce contrat de types ne constitue pas une implémentation du sampler/enregistreur ou du looper.

## Compilations distinctes

- `port/checks` vérifie le transport audio et compile séparément des unités originales : `common`, `interface`, `project`, `audio_file_manager`, `input_events`, ainsi que sept modules de contrôles, composants, en-têtes, sélecteurs, éditeurs de programmes et mixage de l’interface originale. Compiler des objets ne valide pas leur édition de liens ou leur fonctionnement dans l'application.
- Le CMake racine prépare désormais le portage et invoque la cible originale `Kick` sous Windows. Il ne s'arrête plus volontairement sur l'inventaire des modules absents du snapshot immuable.
- Le workflow manuel `Original KICK application — Windows integration build` exécute cette compilation complète, assemble les ressources avec `Kick.exe`, vérifie que le processus reste lancé pendant vingt secondes et publie le ZIP comme artefact de test.

## Dépendances découvertes en compilant les vraies unités

Au-delà des constantes, `mt::SampleRecorder` et le moteur `mt::Looper` ne sont pas définis dans l'archive ; `mt_audio_recorder.h/.cpp` les utilisent. Le sampler/enregistreur attendu est maintenant relié à `AudIO::DRecorder`, déjà présent dans la publication mais désactivé par configuration. `SampleRecorder` adapte ses noms de méthodes à ceux utilisés par KICK. Le looper réutilise ce moteur d’enregistrement et expose le contrat attendu par l’interface. Les piles undo/redo du looper ne sont pas encore restaurées : les deux commandes sont actuellement sans effet et empêchent toujours de qualifier ce portage de complet.

`drum_voice.h/.cpp` attend aussi les types de traitement `Filter`, `FilterType` et `ADSR`. Les variantes `ADSRType` et les fonctions de mise à l’échelle existent déjà dans `mt_audio_timebase_types.h`. L’adaptateur restaure ce contrat au-dessus des primitives AudIO publiées : l’ADSR délègue au DSP AudIO et le filtre reprend sa topologie avec un état indépendant par canal.

Deux fonctions de `MTAudioProgram` renvoyaient l’adresse d’un objet local détruit. La copie de compilation conserve désormais l’objet interrogé dans le programme. `MTAudioInterface::getAssetName` renvoie également une chaîne par valeur, conformément au gestionnaire d’assets, plutôt qu’une référence vers une valeur temporaire.

La cible originale produit maintenant un `Kick.exe` complet au sens de la compilation et de l'édition de liens. Cela ne valide pas encore l'audio, l'affichage ni chaque fonction à l'exécution.


## Résultats constatés

La [première construction native de l’application](https://github.com/olivershadd-star/Berhinger-Kick/actions/runs/43025638621) avait configuré JUCE, AudIO local, ComProtocol local et la cible `Kick`, puis révélé les contrats absents des enregistreurs, du looper et des traitements de voix. Ces contrats et une erreur indépendante d’inclusion de `juce_events` dans `input_events` ont été corrigés à partir des journaux de compilation.

La [construction complète suivante](https://github.com/olivershadd-star/Berhinger-Kick/actions/runs/43027864838) compile et lie avec succès la cible originale : le journal MSVC confirme la production de `original-build/Kick.exe`. Cette preuve porte sur la construction ; le paquetage et le démarrage font l'objet d'une vérification distincte.

La [construction de la préversion portable](https://github.com/olivershadd-star/Berhinger-Kick/actions/runs/43030940113) assemble les ressources, lance l'exécutable sur le runner Windows et publie le [ZIP de test](https://github.com/olivershadd-star/Berhinger-Kick/releases/tag/v0.3.3-preview). Le même binaire ouvre visuellement l'écran original `Projects` sous Wine lorsque son pilote audio est désactivé. Le faux périphérique ALSA disponible dans l'environnement de test annonce 64 canaux et fait planter le pilote `winealsa` avant l'interface ; ce résultat Wine ne permet donc pas de valider l'audio Windows. Un essai sur un vrai périphérique Windows reste nécessaire.

La [vérification native des quatre premières unités originales](https://github.com/olivershadd-star/Berhinger-Kick/actions/runs/43025633935) est réussie avec MSVC. La [vérification étendue](https://github.com/olivershadd-star/Berhinger-Kick/actions/runs/43026300684) est également réussie avec MSVC : douze unités originales de modèles, d’interface et d’événements compilent séparément. Pour les modules répartis en plusieurs amalgamations, cette vérification porte sur l’unité principale sélectionnée, pas sur toutes leurs sources ni sur leur édition de liens. Les tests de transport audio restent réussis. Aucun de ces résultats ne constitue une validation sonore ou visuelle de l’application complète.

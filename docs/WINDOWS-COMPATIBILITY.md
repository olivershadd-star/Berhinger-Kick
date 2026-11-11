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

- `port/checks` vérifie le transport audio et compile séparément des unités originales : `common`, `interface`, `project`, `audio_file_manager`, ainsi que sept modules de contrôles, composants, en-têtes, sélecteurs, éditeurs de programmes et mixage de l’interface originale. Compiler des objets ne valide pas leur édition de liens ou leur fonctionnement dans l'application.
- Le CMake racine prépare désormais le portage et invoque la cible originale `Kick` sous Windows. Il ne s'arrête plus volontairement sur l'inventaire des modules absents du snapshot immuable.
- Le workflow manuel `Original KICK application — Windows integration build` exécute cette compilation complète. Il conserve des diagnostics en cas d'échec et ne publie aucun faux exécutable.

## Dépendances découvertes en compilant les vraies unités

Au-delà des constantes, `mt::SampleRecorder` et le moteur `mt::Looper` ne sont pas définis dans l'archive ; `mt_audio_recorder.h/.cpp` les utilisent. Les commandes du looper (dont undo/redo) ne sont pas intégralement fournies par l'ancien `AudIO::AudioRecorder`. Un simple alias serait trompeur.

`drum_voice.h/.cpp` attend aussi les types de traitement `Filter`, `FilterType` et `ADSR`. Les variantes `ADSRType` et les fonctions de mise à l’échelle existent déjà dans `mt_audio_timebase_types.h`. L'AudIO publié contient des primitives de filtre et d'ADSR, mais leur interface et les fonctionnalités attendues diffèrent. Il faut analyser et adapter ces contrats ; aucun filtre ni looper factice n'a été ajouté pour obtenir artificiellement une compilation réussie.

Il n'existe donc pas encore de `Kick.exe` complet validé. Les erreurs de compilation sont utilisées comme liste de travail, sans les confondre avec des fonctions portées.

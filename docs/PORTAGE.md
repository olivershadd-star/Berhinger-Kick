# Réanalyse de 0700-AAM et recentrage du projet

## Conclusion actuelle

Le prototype KICK Lab a validé de la lecture/mixage AudIO et quelques effets sur Windows. Il n'a pas porté l'application KICK. Prolonger son séquenceur et son éditeur aurait continué à reconstruire un autre produit.

Le projet repart donc de l'application et des ressources originales, conservées à l'identique. La version KICK originale n'est pas encore compilée et aucune nouvelle version portable complète n'est annoncée.

## Provenance vérifiée

Archive fournie : `0700-AAM-main.zip`. Son empreinte est dans `upstream/provenance.json`. Les 3 183 fichiers importés ont également été comparés aux empreintes Git des fichiers de `MusicTribeOpenSource/0700-AAM` à la révision `27194180ce5fbab3776ebce53e2d4b016597364e` : aucune différence.

Périmètre : `Kick/`, `AudIO/`, `ComProtocol/`, `docs/`, README et licence racine. Le système Buildroot n'est pas importé, sauf son README. Ce n'est pas une copie de l'ensemble du dépôt Linux, mais l'application et ses bibliothèques ne sont pas réécrites ni remplacées.

## Composants à reprendre

Les chemins ci-dessous sont relatifs à `upstream/0700-AAM/`.

| Fonction | Code KICK existant | Prototype précédent | Décision |
|---|---|---|---|
| Écran et navigation | `Kick/modules/kick_ui/dialogs/kick_AppHome.*` : classe `kick_ui::KickHome`, écrans de séquences, programmes, éditeur, mixeurs, effets | Dessin Win32 personnalisé dans `windows.cpp` et `editor_ui.inc` | Reprendre `KickHome` et les composants JUCE, pas refaire leurs écrans |
| Application | `Kick/apps/kick/Main.cpp` instancie `KickHome` et initialise `mt::getAudioInterface()` | `wWinMain` indépendant | Cibler l'application originale `Kick` |
| Séquences et transport | `Kick/modules/engine/audio_engine/engine/mt_audio_sequence.*`, `mt_audio_track.*`, `mt_audio_manager_transport.cpp` | Compteur `untilStep`, grille fixe de 16 pas | Porter les classes originales ; ne pas ajouter des pages à notre grille |
| Longueur en mesures | `MTSequence::setLength(int Bars)`, points de boucle ; constantes `BAR` dans `Kick/modules/common/source/kick_Constants.h` | Une mesure | Utiliser cette gestion existante ; aucune nouvelle limite 1–4 mesures inventée |
| Voix, programmes et slices | `Kick/modules/engine/audio_program/synth/`, `audio_file_manager/source/audio_sound_renderer.cpp`, `objects/sampler/` | Extension du lecteur générique `AudioPlayback` | Reprendre les voix et slices KICK avec leurs réglages et enveloppes |
| Effets et routage | AudIO `DawEngine`, `DawBus`, `DawInserts`, registre Airwindows et interfaces d'inserts KICK | Deux effets câblés par notre code de pad | Restaurer la chaîne d'inserts d'origine, sans recopier son fonctionnement |
| Projets | `Kick/modules/engine/project/` : `.kickPrj`, données ValueTree KICK | Nouveau format `.kicklab` | Reprendre `.kickPrj` ; pas de conversion automatique ou de nouveau format dans cette phase |
| Fichiers portables | `Kick/modules/common/source/kick_paths.cpp` a déjà une branche Windows basée sur le dossier de l'exécutable | Gestion de dossiers propre au prototype | Vérifier et adapter le chemin existant plutôt que le remplacer |
| Matériel | `input_events`, `hardware`, `PktMgr`, contrats DCL et IPC | Pads/mappage MIDI Win32 du prototype | Adapter les entrées Windows aux commandes existantes, signaler les fonctions physiques non disponibles |

Le moteur AudIO réimporté conserve ses options originales, notamment sa couche DAW. Le sous-ensemble modifié pour l'expérience n'est plus la bibliothèque de l'application cible.

## Pourquoi le simulateur existant ne suffit pas

`Kick/apps/kickem/simulator/kick_Simulator.cpp` appelle encore `AppHome::Instance()` et `AppHome::deletInstance()`. L'interface actuelle expose `kick_ui::KickHome`, avec une autre forme d'accès. Il ne suffit donc pas d'activer `KICK_SIMULATOR` pour obtenir l'interface actuelle complète.

L'application `Kick/apps/kick/Main.cpp` utilise bien `kick_ui::KickHome::Instance()`. C'est une meilleure référence à ce stade. La conservation de commandes virtuelles d'origine du simulateur pourra être étudiée ensuite, en réparant uniquement sa liaison à l'interface actuelle.

## Blocages confirmés de l'archive

| Élément | Référence qui le demande | État |
|---|---|---|
| `AudIO/juce_modules.cmake` | `Kick/3rdParty.cmake` | Absent |
| `AudIO_constants/AudIO_constants.h` | `Kick/modules/common/common.h` | Absent |
| `AudIO_juce/AudIO_juce.h` | `Kick/modules/audio_interface/audio_interface.h` | Absent |
| `juce_serialport/juce_serialport.h` | `Kick/modules/input_events/input_events.h` et CMake tiers | Absent ; retrait annoncé dans le README d'origine |

`MTAudioManager` attend notamment une classe `mt::AudioDevice`, avec démarrage du moteur et accès au gestionnaire de périphériques JUCE. Sa définition n'est pas fournie dans les sources importées. La présence du cœur AudIO ne remplace pas cette intégration.

Les paramètres de périphérique `AUDIO_SAMPLE_RATE`, `AUDIO_BLOCK_SIZE` et les nombres d'entrées/sorties sont utilisés dans KICK sans le module de constantes. AudIO possède aussi un pool de blocs de 480 trames dans son `DawEngine.cpp`. On ne doit pas réutiliser arbitrairement les 256 trames du prototype pour le port original : la cohérence de ces paramètres doit être rétablie.

Il faut restaurer les modules d'origine lorsqu'ils sont disponibles. Si un adaptateur JUCE/Windows doit être réécrit faute de publication, il devra rester une adaptation technique identifiée, fondée sur les interfaces publiées, sans réinventer de traitement musical. Aucune telle reconstruction n'est présentée ici comme du code original.

## Autres points découverts à traiter avant validation

- Le dépôt mélange une interface historique reliée à `MTAudioManager` et une couche de contrats/IPC plus récente. Dans `Kick/apps/kick/kick_audio_interface.h`, certains appels sont explicitement factices (`return dummy`, identifiants aléatoires). Il faut vérifier les chemins réellement utilisés ; leur présence n'implique pas que toute la couche historique soit factice.
- Plusieurs déclarations JUCE ont un identifiant qui ne correspond pas au dossier du module. L'audit les relève ; elles ne sont pas corrigées silencieusement dans le snapshot.
- Des sources matérielles contiennent des appels et chemins Linux : USB FunctionFS, I²C, Wi-Fi, firmware et outils système. Leurs branches Windows et les appels depuis l'interface doivent être examinés, sans prétendre disposer du matériel physique.
- Les samples d'usine, les sons du métronome, le SoundFont et la police Myriad Pro ont été retirés de la publication. Le thème et les ressources graphiques existantes sont présents. La police exacte et le contenu d'usine ne peuvent pas être promis à partir de cette archive seule.
- Les builds d'origine téléchargent JUCE 7.0.12 et lui appliquent un patch multitouch. La préparation doit préserver cette référence, pas remplacer JUCE par la nouvelle interface Win32 expérimentale.

## Réorganisation concrète

- `upstream/0700-AAM` : sources et ressources originales inchangées.
- `upstream/provenance.json` : preuve de provenance, périmètre et SHA-256 de chaque fichier.
- `experiments/kicklab` : prototype et toutes ses modifications isolés. Les anciens tags et téléchargements restent utilisables.
- `port/windows` : point d'entrée de compilation de l'application originale, avec échec explicite si les dépendances manquent.
- `tools/audit_original.py` : inventaire des modules et contrôle d'intégrité ; rapport dans `docs/source-audit.json`.
- CMake racine : aucune liaison vers le moteur, le séquenceur ou l'interface expérimentaux. La compilation originale est préparée dans un dossier séparé, car les CMake de l'auteur génèrent aussi des fichiers dans leur répertoire source.

## Ce qui a réellement été vérifié

- Comparaison des 3 183 fichiers au dépôt public et vérification locale des SHA-256 : sources originales identiques.
- Audit de 39 modules/entêtes : trois dépendances de modules non résolues, plus le fichier CMake AudIO absent.
- Configuration en mode audit : réussie, sans construction d'application.
- Configuration normale : bloquée explicitement par les dépendances absentes, sans création d'un exécutable de remplacement.
- Sonde C++ de l’interface originale avec MinGW-w64 et JUCE 7.0.12 : échec réel sur `AudIO_constants/AudIO_constants.h` absent, avant la construction de l’interface.
- Le fonctionnement de l'interface KICK originale sur Windows n'est pas encore validé.

La prochaine étape technique est de restaurer l'intégration AudIO/JUCE et les dépendances d'entrée, puis d'obtenir la première compilation et l'ouverture de `KickHome`. Elle précède toute nouvelle évolution musicale.

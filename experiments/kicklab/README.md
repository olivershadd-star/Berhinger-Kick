> **Base active du portage progressif.** Cette application conserve la sortie WinMM et le moteur AudIO déjà validés. Les modules KICK publiés y seront intégrés un par un, avec une commande souris pour toute fonction qui dépend normalement du matériel.

# KICK Lab 0.2.4 — première banque de 14 effets

Prototype communautaire indépendant utilisant le moteur **AudIO** et des effets **Airwindows** publiés dans [MusicTribeOpenSource/0700-AAM](https://github.com/MusicTribeOpenSource/0700-AAM).

**[Télécharger le build Windows portable 0.2.4](https://github.com/olivershadd-star/Berhinger-Kick/actions/runs/43166145754/artifacts/13920298199)**

Cette version conserve le moteur, la bibliothèque, les potentiomètres et le rack de quatre inserts validés. Elle ajoute douze processeurs au Lowpass et à PurestDrive déjà présents, tous repris des sources Airwindows du dépôt KICK.

![Éditeur de sample et effets par pad](APERCU-SAMPLE.png)

## Pour essayer

1. Extraire entièrement le ZIP dans un dossier accessible en écriture.
2. Ouvrir **KickLab.exe**, en gardant le dossier **demo-kit** à côté. Le dossier **samples** de la bibliothèque est créé automatiquement.
3. Dans **Jouer**, cliquer sur les pads, ou charger **Démo** avant de lancer le pattern. Le pattern démarre volontairement vide.
4. Cliquer dans l'écran A ou B, puis sur **Sample & FX**, charge les quatre inserts du pad sélectionné dans cet écran. Choisir un slot, cliquer sur son effet ou son bypass, puis régler ses paramètres avec P1–P4 ou P5–P8. L'éditeur détaillé transitoire reste accessible depuis **Sample Editor** dans le menu KICK.

Le kit chargé au démarrage se trouve dans `demo-kit`. La **Factory Sample Library** est le dossier `samples` créé à côté de l'exécutable ; elle commence sans son officiel.

Windows 10/11 x64, sans installation. Aucun compilateur, Python ou JUCE nécessaire pour lancer l'exécutable fourni. La fenêtre reste redimensionnable. Si aucun son ne sort, choisir une sortie Windows avec le bouton Audio de l'onglet Jouer.

## Sample & FX

La surface principale expose maintenant le slot d'effet sélectionné directement dans l'écran qui possède le focus. Les valeurs affichées viennent du véritable état du moteur : P1–P4 règlent l'écran A et P5–P8 l'écran B, par glissement vertical ou avec la molette. Un paramètre non disponible reste explicitement non assigné. Cliquer d'abord dans A ou B puis sur **Sample & FX** permet donc d'ouvrir deux slots différents, ou deux vues du même état audio partagé.

- **Début / fin** : glisser les deux repères sur la forme d'onde. Le WAV original n'est pas modifié.
- **Volume** : réglage indépendant par pad, de 0 à 200 %, lissé pendant la lecture.
- **Accordage** : de −24 à +24 demi-tons. Il s'agit d'une transposition par vitesse de lecture : la durée change aussi, sans time-stretch.
- **Fondus** : de 0 à 20 ms au début et à la fin, limités à la moitié de la zone lue. Valeur initiale : 2 ms.
- **Slots 1 à 4** : sélectionner un slot, puis choisir une catégorie et un effet. Les effets sont traités dans l'ordre 1 → 4.
- **Filter** : Lowpass, Highpass et Isolator.
- **Saturation** : PurestDrive, Tube2 et Mojo.
- **Dynamics** : DrumSlam, ButterComp2 et SoftGate.
- **Ambience** : Chorus et StarChild.
- **Reverb** : Reverb.
- **Distortion** : Loud et Mackity.
- **Paramètres** : chaque effet affiche ses noms et valeurs initiales d'origine, jusqu'à trois potentiomètres dans ce premier lot.
- **Bypass du slot** : contourne uniquement l'effet sélectionné. **Bypass de la chaîne** contourne les quatre slots du pad.
- **Potentiomètres** : glisser verticalement pour un réglage continu. La molette agit sur le potentiomètre survolé ; ailleurs, elle continue de régler le tempo.
- **Réinitialiser ce pad** : remet les réglages à leur valeur initiale et conserve son sample.
- Cliquer directement sur un pad déclenche son one-shot. **Stop** arrête le transport et les voix.
- **Ajouter à la bibliothèque** : copie le WAV source du pad dans Kicks, Snares, Hats, Percussions, Basses, Pads, Leads, FX ou Autres, puis rattache le pad à cette copie en conservant ses réglages. Les effets ne sont pas rendus dans le WAV copié.

La découpe, l'accordage et les fondus sont appliqués au prochain déclenchement. Le volume évolue pendant la lecture avec lissage. La chaîne est : lecteur AudIO étendu → inserts 1 à 4 → volume du pad → bus AudIO → volume général.

Charger un nouveau WAV sur un pad remet ses réglages à zéro. Cela coupe la voix de ce pad ; les autres pads continuent. Un nouveau déclenchement relance le sample : cette version n'a qu'une voix par pad.

## Séquenceur, clavier et MIDI

- 16 pads et 16 pas sur une mesure, doubles croches. Le pattern démarre vide afin qu'un WAV importé ne soit jamais redéclenché à l'insu de l'utilisateur ; **Démo** charge le pattern d'exemple. Cliquer dans la grille pour activer/désactiver un pas.
- Espace : lecture/pause du séquenceur. Échap : arrêt et coupure des voix.
- Pads au clavier, par rangée : **A Z E R / Q S D F / W X C V / 1 2 3 4**. Le clic est toujours disponible.
- Tempo : boutons − / + ou molette. Volume général : barre Master.
- Clic droit sur un pad : charger un WAV. Glisser-déposer un WAV sur un pad ou sur l'éditeur est également possible.
- MIDI : sélectionner une entrée avec le bouton MIDI ou depuis la page **MIDI Control** ; les notes 36 à 51 déclenchent les pads, tous canaux.
- **MIDI Control** : ouvrir l'élément 16 du menu KICK. Les deux pages affichent P1–P4 puis P5–P8. Cliquer sur une ligne arme le MIDI Learn ; le prochain CC reçu mémorise son numéro et son canal. Un clic droit alterne `ABS`, relatif `1/127` et relatif `63/65`.
- Un CC appris commande le même Q-Link que la souris dès que l'écran correspondant affiche une page d'inserts. Les affectations restent en mémoire pendant cette première étape et ne sont pas encore enregistrées dans un profil. Vélocité, Note Off, transport, retour LED et horloge MIDI ne sont pas encore gérés.

## WAV et projets

WAV PCM 16/24/32 bits ou flottant 32 bits, mono/stéréo, de 8 à 192 kHz, jusqu'à 30 secondes et 32 Mo par fichier. Les WAV sont convertis à 48 kHz au chargement par interpolation linéaire. WAV extensible, MP3, FLAC et AIFF ne sont pas pris en charge dans cette interface.

**Bibliothèque / WAV** propose trois chemins : parcourir la Factory Sample Library locale, importer un WAV depuis un autre emplacement du PC, ou ouvrir directement le dossier de la bibliothèque dans l'Explorateur. L'application crée les catégories au premier démarrage. Les samples d'usine Music Tribe ne font pas partie du dépôt public.

**Enregistrer projet / Ouvrir projet** sauvegarde le pattern, le tempo, le volume et les réglages de chaque pad dans un fichier `.kicklab`. Le nom du fichier enregistré ou chargé est confirmé dans la barre d'état. Les samples restent référencés par leur chemin absolu et ne sont pas incorporés : conserver les WAV à leur emplacement. Un chargement échoué conserve le projet courant.

Les projets **KICKLAB 1** et **KICKLAB 2** restent lisibles. Les nouveaux projets utilisent **KICKLAB 3** pour conserver le type, le bypass et les paramètres de chaque slot ; ils ne s'ouvrent pas dans les anciennes versions. Ce format est propre au prototype, pas à la machine KICK.

## Code réutilisé et adaptations

- **AudIO** : graphe `Design`, lecteur `AudioPlayback`, bus, mixage et modèle original `DawInserts` à quatre slots.
- **AudioFile** : décodeur WAV livré dans le dépôt, après validation et normalisation de l'en-tête.
- **Airwindows** : les quatorze processeurs utilisent les sources originales et l'adaptation `AudioEffectX` fournies dans 0700-AAM. Chaque instance est créée par l'`ObjectFactory` d'AudIO dans le slot choisi.
- **Ajouts du prototype** : interface Win32, éditeur, extension du lecteur (découpe, interpolation pour la hauteur, fondus), séquenceur simple, MIDI, sortie WinMM, persistance et tests.

L'éditeur JUCE complet et les classes de voix KICK dépendent encore des couches manquantes. Cette version étend le lecteur AudIO qui fonctionne déjà sur Windows ; elle ne prétend pas porter l'éditeur ou le séquenceur KICK complet. Voir `source/vendor/AudIO/DESKTOP-CHANGES.patch`.

Le kit fourni a été synthétisé pour ce prototype. Aucun sample d'usine Behringer n'est inclus, et ces sons ne permettent pas de juger du son du futur produit.

## Stabilité audio et limites

Les corrections de 0.1.1 et 0.1.2 sont conservées : ordre des buffers, attente par événements du pilote, priorité MMCSS, séparation des copies graphiques et du rendu, interface redimensionnable.

Sortie fixe : PCM stéréo 16 bits à 48 kHz via WinMM, quatre buffers de 1 024 trames (environ 85 ms de réserve, plus le pilote). Pas d'ASIO ni de VST3 pour l'instant. Le séquenceur est quantifié au bloc de 256 trames (environ 5,33 ms), sans dérive cumulative du compteur. Le volume général et une saturation de sécurité protègent la sortie ; une accumulation de pads très forts peut néanmoins être écrêtée.

Pas encore de kits avec médias intégrés, de séquenceur KICK avancé, de keygroups, d'enregistrement, de synthétiseurs KICK ni de banque complète d'effets. Une seule instance du moteur est utilisée par processus ; les pools globaux d'AudIO ne sont pas encore adaptés à un plugin multi-instance.

## Développer

Les sources nécessaires sont incluses dans **source/**, avec les licences. Les fichiers principaux :

- `source/src/engine.cpp` : chargement, réglages et persistance.
- `source/src/pad_voice.hpp` : lecteur étendu et rack de quatre inserts.
- `source/src/editor_ui.inc` : commandes et dessin de l'éditeur.
- `source/src/windows.cpp` : fenêtre, pads, MIDI et sortie Windows.

Sur Windows : installer CMake et Visual Studio 2022 Build Tools avec Développement Desktop C++, puis lancer `source/BUILD-WINDOWS.bat`. La compilation MSVC et les tests sont également exécutés par GitHub Actions.

```sh
cmake -S source -B build -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Avec MinGW-w64 : choisir le générateur `MinGW Makefiles` et `CMAKE_BUILD_TYPE=Release`. Pour tester le moteur sous Linux, omettre `-A x64` et utiliser `cmake --build build` puis `ctest --test-dir build --output-on-failure`.

Les vérifications audio couvrent la lecture WAV, les bornes de découpe, la durée et la fréquence à l'octave, les fades, les quatre inserts, les bypass, la persistance v3, la migration des projets v1/v2, la remise en file des buffers et la continuité sous activité graphique. Les tests automatisés ne remplacent pas un essai avec la carte son et le contrôleur MIDI de l'utilisateur.

## Licences

Code Music Tribe et modifications : **AGPL-3.0-or-later**. Airwindows et AudioFile conservent leur **licence MIT**. Voir LICENSE et NOTICES.txt. Les noms Behringer et KICK restent des marques de leurs propriétaires. Prototype indépendant, non officiel.

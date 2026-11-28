<div align="center">

# KICK
### ORIGINAL DRUM MACHINE · WINDOWS PORT · SEQUENCER DEVELOPMENT

![Status](https://img.shields.io/badge/STATUS-PUBLIC_TEST_BUILD-EF343E?style=for-the-badge&labelColor=10151E)
![Version](https://img.shields.io/badge/VERSION-0.24.3_PREVIEW-19B9FF?style=for-the-badge&labelColor=10151E)
![Windows](https://img.shields.io/badge/WINDOWS-10%20%2F%2011_X64-19B9FF?style=for-the-badge&labelColor=10151E)
![Licence](https://img.shields.io/badge/LICENCE-AGPL--3.0--or--later-F5C343?style=for-the-badge&labelColor=10151E)

**ANKS LABS // BUILD. BREAK. REWIRE. LISTEN.**

[![DOWNLOAD](https://img.shields.io/badge/DOWNLOAD-TEST_BUILD_0.24.3-EF343E?style=for-the-badge&labelColor=10151E)](https://github.com/olivershadd-star/Berhinger-Kick/releases/tag/v0.24.3-preview)
[![SOURCES](https://img.shields.io/badge/SOURCE-CODE-19B9FF?style=for-the-badge&labelColor=10151E)](https://github.com/olivershadd-star/Berhinger-Kick)
[![RELEASES](https://img.shields.io/badge/ALL-BUILDS-F5C343?style=for-the-badge&labelColor=10151E)](https://github.com/olivershadd-star/Berhinger-Kick/releases)

</div>

---

> **CURRENT STATUS — PUBLIC TEST BUILD, NOT A STABLE RELEASE.** The priority is the independent Windows port of the original KICK drum machine. Rebuilding and refining the sequencer comes first; expanding it into a groovebox comes later.
>
> **STATUT — VERSION PUBLIQUE DE TEST, NON STABLE.** La priorité est le portage indépendant sous Windows de la boîte à rythmes KICK d'origine. La reconstruction et l'amélioration du séquenceur passent d'abord ; son évolution vers une groovebox viendra ensuite.

## 01 // WHAT IS KICK? — QU'EST-CE QUE KICK ?

**EN**

**KICK** is a drum-machine project originally developed by **Music Tribe / Behringer**, whose source code has been made publicly available. **ANKS Labs** is working on an independent Windows adaptation of this existing codebase.

The software **remains KICK**. Some modifications have already been made to adapt it to a PC, but the project is intentionally still focused on **porting the original machine**, rather than replacing its design with an entirely different instrument.

The first major goal is to **rebuild, complete and improve the sequencer** at the heart of the drum machine, with reliable patterns, editing, timing, audio and MIDI control. Once that foundation is solid, the ambition is to **turn the drum machine into a fuller groovebox**, progressively extending composition and performance capabilities.

**FR**

**KICK** est un projet de boîte à rythmes développé à l'origine par **Music Tribe / Behringer**, dont le code source a été rendu public. **ANKS Labs** travaille sur une adaptation indépendante de ce code pour Windows.

Le logiciel **conserve le nom KICK**. Quelques modifications ont déjà été apportées pour adapter son utilisation sur PC, mais le projet reste volontairement **centré sur le portage de la machine d'origine**. Il ne s'agit pas, à ce stade, d'un nouvel instrument entièrement transformé.

Le premier grand objectif consiste à **reconstruire, compléter et améliorer le séquenceur** de cette boîte à rythmes : patterns, édition, timing, audio et contrôle MIDI. Sur cette base, l'ambition est ensuite de **faire évoluer la boîte à rythmes vers une véritable groovebox**, en enrichissant progressivement ses possibilités de composition et de jeu.

> **PORT FIRST. SEQUENCE NEXT. GROOVEBOX LATER.**
>
> **D'abord le portage. Ensuite le séquenceur. Puis la groovebox.**

## 02 // DEVELOPMENT DIRECTION — DIRECTION DU PROJET

| Phase | English | Français |
|:--|:--|:--|
| **01 — Windows port / Portage** | Preserve the original KICK architecture, adapt the interface and audio/MIDI operation to Windows, and correct incomplete functions. | Conserver l'architecture de KICK, adapter l'interface et les fonctions audio/MIDI à Windows, corriger les parties incomplètes. |
| **02 — Drum sequencer / Séquenceur** | Rebuild, stabilize and extend the drum-machine sequencer and pattern editing before expanding the instrument's scope. | Reconstruire, fiabiliser et améliorer le séquenceur de boîte à rythmes et l'édition des patterns avant d'élargir le projet. |
| **03 — Groovebox / Évolution** | Progressively add broader composition, sequencing and performance possibilities on top of a stable KICK foundation. | Ajouter progressivement des possibilités de composition, de séquençage et de jeu, à partir d'une base KICK stable. |
| **04 — VST3 / Future research** | Investigate a separate VST3 / Ableton-oriented workflow only when the core application is ready. | Envisager un format VST3 adapté notamment à Ableton lorsque le cœur de l'application sera prêt. |

*These are development intentions, not a feature-completion claim or release schedule.*

*Il s'agit d'orientations de développement, pas d'une liste de fonctions déjà finalisées ni d'un calendrier de sortie.*

## 03 // CURRENT INTERFACE — INTERFACE ACTUELLE

![KICK Windows interface with Maschine+ layout](experiments/kicklab/APERCU-MASCHINE-PLUS.png)

**EN** — The interface shown here belongs to the current Windows experiment. Its layout is being adapted for mouse/keyboard and MIDI controller operation. The visuals and workflow are still evolving.

**FR** — Cette capture présente l'interface du portage Windows actuel. Sa disposition est adaptée au travail à la souris, au clavier et avec des contrôleurs MIDI. L'apparence et le fonctionnement continuent d'évoluer.

## 04 // DOWNLOAD & FIRST LAUNCH — TÉLÉCHARGEMENT

### [⬇ Download / Télécharger KICK 0.24.3 preview — Windows x64](https://github.com/olivershadd-star/Berhinger-Kick/releases/download/v0.24.3-preview/KickLab-Windows-x64-0.24.3-preview.zip)

[Release 0.24.3](https://github.com/olivershadd-star/Berhinger-Kick/releases/tag/v0.24.3-preview) · [All releases / Toutes les versions](https://github.com/olivershadd-star/Berhinger-Kick/releases)

**EN — Quick start**

1. Extract the entire ZIP to a writable folder on Windows 10/11 x64.
2. Keep `demo-kit` next to the executable and create or keep a `samples` folder there.
3. Launch `KickLab.exe` and load WAV samples using the Browser or a pad's context menu.
4. Choose pattern A–H, then use Pad, Keyboard or Step mode.
5. Use File/Save to save projects (`.kicklab`), kits (`.kickkit`) or export WAV audio.

**FR — Démarrage rapide**

1. Extraire entièrement le ZIP dans un dossier accessible en écriture sous Windows 10/11 x64.
2. Conserver `demo-kit` près de l'exécutable et créer ou conserver un dossier `samples`.
3. Lancer `KickLab.exe` et charger les WAV par le Browser ou le clic droit sur un pad.
4. Choisir un pattern A–H, puis utiliser les modes Pad, Keyboard ou Step.
5. Utiliser File/Save pour sauvegarder un projet (`.kicklab`), un kit (`.kickkit`) ou exporter un WAV.

**Naming note / Note de nommage :** `KickLab.exe`, `.kicklab` and the ZIP archive are **current technical filenames kept for compatibility** with the published test build. The **project and software name presented here is KICK**; the binaries have not been renamed by this README change.

**Samples / Échantillons :** the `samples` library is external and belongs to the user; no commercial samples are included. / La bibliothèque `samples` est externe et personnelle ; aucun son commercial n'est fourni.

## 05 // WHAT WORKS IN 0.24.3? — FONCTIONS DE LA PREVIEW

The following features are documented for the current **test build**. Some workflows still require additional testing and refinement.

Les fonctions suivantes sont documentées pour la **version de test** actuelle. Certains comportements demandent encore des corrections et validations.

**0.24.3 stabilization / stabilisation :** Pad Edit and Browser reliably follow the visible pad; opening a project clears transient interface state; persistent edits are correctly marked for saving; engine snapshots are coherent; and the UI no longer copies the complete project 60 times per second.

| Area / Domaine | Current capabilities / Fonctions actuelles |
|:--|:--|
| **Pads & kits** | 16 pads, savable Programs and kits / Programs et kits sauvegardables |
| **Patterns** | 8 independent patterns A–H, 1–4 bars / 8 patterns indépendants de 1 à 4 mesures |
| **Playing & editing / Jeu et édition** | Pad, Keyboard, Chords, Step, quantized REC, Events, List Edit |
| **Sequencer / Séquenceur** | Graphical 16-track × 16-step grid / Grille graphique 16 pistes × 16 pas |
| **Editing / Édition** | Copy, Paste, Duplicate, Clear for bars, patterns, pads and kits |
| **Mixing / Mixage** | Track Mixer, Pad Mixer, mute, solo, dB meters / Vumètres en dB |
| **Automation** | Level/Pan and per-step/per-pattern effects parameters, AUTO WRITE |
| **Effects / Effets** | 4 inserts per pad, 31 Airwindows effects / 4 inserts par pad |
| **Export** | Stereo 48 kHz / 24-bit WAV for active pattern / WAV du pattern actif |
| **MIDI** | Maschine+ and LPD8 controller-oriented mappings / Mappings Maschine+ et LPD8 |
| **Interface** | Customizable appearance and importable background / Apparence et fond personnalisables |

**EN** — The port uses **AudIO / Windows WinMM**. Output and export are protected at **−1 dBFS**. The mixer distinguishes pad gain, track level, pre-master mix, pre-limiter level and final output.

**FR** — Le portage utilise **AudIO / Windows WinMM**. La sortie et l'export sont protégés à **−1 dBFS**. Le mixeur distingue le gain du pad, le niveau de piste, la somme pré-master, le niveau avant limiteur et la sortie finale.

## 06 // MIDI CONTROLLERS — MASCHINE+ & LPD8

**EN** — The goal is to make the original KICK workflow practical on a PC using the controllers already available: **Maschine+** for the main pad-based surface, and **Akai LPD8** for eight A–H selection pads and eight direct macro knobs. KICK can also be used with a mouse and keyboard, without these controllers.

**FR** — L'objectif est de rendre le fonctionnement de KICK utilisable sur PC avec les contrôleurs déjà disponibles : **Maschine+** pour la surface principale à pads et **Akai LPD8** pour huit pads de sélection A–H et huit potentiomètres macro. KICK reste utilisable à la souris et au clavier, sans ces contrôleurs.

| Controller / Contrôleur | Role / Rôle |
|:--|:--|
| **Native Instruments Maschine+** | Main pads, 16 Q-Links, buttons and two soft-key pages / Surface principale, pads, encodeurs et boutons |
| **Akai LPD8** | 8 pads for A–H selection and 8 knobs for macros M1–M8 / Sélection A–H et 8 macros |
| **Mouse & keyboard / Souris & clavier** | Standalone use without external hardware / Utilisation sans contrôleur |

**EN — MIDI setup:** Put Maschine+ in controller mode (`Shift + Channel/MIDI`), preferably select **Maschine Plus Virtual** in the MIDI Control page, and keep the mappings in `profiles/MaschinePlus.kickmidi`. Connect LPD8 to the PC and configure its mapping in `profiles/LPD8.kickmidi`. MIDI behavior depends on the available Windows ports.

**FR — Connexion MIDI :** Passer Maschine+ en mode contrôleur (`Shift + Channel/MIDI`), sélectionner de préférence **Maschine Plus Virtual** dans la page MIDI Control et conserver le mapping dans `profiles/MaschinePlus.kickmidi`. Brancher le LPD8 au PC et utiliser son profil `profiles/LPD8.kickmidi`. Le fonctionnement dépend des ports MIDI disponibles sous Windows.

## 07 // ROADMAP — FEUILLE DE ROUTE

**Public preview ≠ stable release. / Preview publique ≠ version stable.**

- [x] Windows standalone port usable with mouse and keyboard / Portage Windows utilisable à la souris et au clavier
- [x] Public test build 0.24.3 / Version de test 0.24.3
- [x] Maschine+ and LPD8 mapping workflow documented / Mappings Maschine+ et LPD8 documentés
- [ ] Complete and stabilize the original KICK port / Finaliser et fiabiliser le portage de KICK
- [ ] Rebuild and refine the drum-machine sequencer / Reconstruire et améliorer le séquenceur de boîte à rythmes
- [ ] Validate timing, editing, MIDI mapping and real-world performance / Valider le timing, l'édition, le MIDI et le fonctionnement en conditions réelles
- [ ] Extend the sequencer and progressively evolve toward a groovebox / Étendre le séquenceur et évoluer progressivement vers une groovebox
- [ ] Investigate a separate VST3 / Ableton workflow / Étudier un format VST3 distinct pour Ableton
- [ ] Publish alpha, then beta, only after appropriate validation / Publier les versions alpha puis bêta après validation

## 08 // SOURCE TREE — ARBORESCENCE

| Path / Chemin | Description |
|:--|:--|
| [`experiments/kicklab/`](experiments/kicklab/) | Active Windows application & documentation / Application Windows active et documentation |
| [`upstream/0700-AAM/`](upstream/0700-AAM/) | Original Music Tribe source reference / Sources originales de référence |
| [`port/`](port/) | Port adapters and research / Adaptateurs et recherches de portage |
| [`docs/`](docs/) | Analyses and documentation / Analyses et documentation |
| [`.github/workflows/`](.github/workflows/) | Automated Windows builds & tests / Compilations et tests automatisés |

**EN** — The `main` branch is the public project base. GitHub Actions files are build/test artifacts; public downloads are under **Releases**.

**FR** — La branche `main` est la base publique du projet. Les fichiers GitHub Actions sont des artefacts de compilation et de test ; les téléchargements publics sont dans **Releases**.

## 09 // BUILD FROM SOURCE — COMPILATION

**EN** — On Windows, use Visual Studio 2022 with CMake and the required C++ components.

**FR** — Sous Windows, utiliser Visual Studio 2022 avec CMake et les composants C++ nécessaires.

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Expected executable / Exécutable attendu : `build/incremental-port/Release/KickLab.exe`.

## 10 // LICENCE & DISCLAIMER — LICENCE ET AVERTISSEMENT

**EN** — This independent community port is based on open source code published by **Music Tribe**, under **AGPL-3.0-or-later**. Third-party components, including **Airwindows** and **AudioFile**, retain their own licences. KICK, Behringer, Music Tribe, Native Instruments and Maschine are trademarks of their respective owners. ANKS Labs is **not affiliated with or endorsed by these manufacturers**; this is not an official Behringer release.

**FR** — Ce portage communautaire indépendant repose sur des sources ouvertes publiées par **Music Tribe**, sous licence **AGPL-3.0-or-later**. Les composants tiers, notamment **Airwindows** et **AudioFile**, conservent leurs licences respectives. KICK, Behringer, Music Tribe, Native Instruments et Maschine sont des marques de leurs propriétaires. ANKS Labs **n'est ni affilié à ces fabricants ni approuvé par eux** ; ce projet n'est pas une version officielle de Behringer.

---

<div align="center">

**ANKS LABS // EXPERIMENTAL AUDIO DEVELOPMENT**

[![ANKS LABS](https://img.shields.io/badge/ANKS_LABS-VIEW_PROFILE-19B9FF?style=for-the-badge&labelColor=10151E)](https://github.com/olivershadd-star)

</div>

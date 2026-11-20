# KICK Lab — portage Windows de KICK

KICK Lab adapte progressivement le code KICK publié par Music Tribe à un ordinateur Windows, sans exiger la surface matérielle KICK. La souris et le clavier fonctionnent seuls ; un contrôleur Maschine+ peut également piloter les pads, les encodeurs et les commandes.

## Télécharger la version Windows

### [Télécharger KICK Lab 0.22.0 — Windows x64 portable](https://github.com/olivershadd-star/Berhinger-Kick/releases/download/v0.22.0-preview/KickLab-Windows-x64-0.22.0-preview.zip)

Aucune installation n'est nécessaire : extraire complètement le ZIP puis ouvrir `KickLab.exe`.

- [Page de la version 0.22.0](https://github.com/olivershadd-star/Berhinger-Kick/releases/tag/v0.22.0-preview)
- [Toutes les versions et leurs fichiers](https://github.com/olivershadd-star/Berhinger-Kick/releases)
- Windows 10/11 x64
- Bibliothèque `samples` externe et personnelle
- Aucun son commercial inclus

![Interface KICK Lab organisée pour Maschine+](experiments/kicklab/APERCU-MASCHINE-PLUS.png)

## Ce qui fonctionne dans la version 0.22.0

- moteur AudIO et sortie Windows WinMM ;
- seize pads, Programs et kits sauvegardables ;
- huit patterns A–H indépendants, de une à quatre mesures ;
- modes Pad, Keyboard, Chords et Step ;
- enregistrement quantifié, Events et List Edit ;
- séquenceur graphique 16 pistes × 16 pas ;
- sélection fixe de la mesure d'édition pendant la lecture ;
- Copy/Paste/Duplicate/Clear pour les mesures, patterns, pads et kits ;
- Track Mixer, Pad Mixer, mutes, solos et vumètres en dB ;
- quatre inserts par pad et 31 effets Airwindows ;
- export du pattern actif en WAV stéréo 48 kHz / 24 bits ;
- navigation récursive dans la bibliothèque externe `samples` ;
- profil MIDI portable `KICKMIDI 6` et intégration Maschine+ ;
- apparence personnalisable et fond d'écran importable.

La sortie et l'export sont protégés à −1 dBFS. Le Mixer distingue le gain du pad, le niveau de piste, la somme avant Master, le niveau avant limiteur et la sortie finale.

## Démarrage rapide

1. Extraire le ZIP dans un dossier où KICK Lab peut écrire.
2. Conserver `demo-kit` à côté de `KickLab.exe`.
3. Remettre ou créer le dossier `samples` à côté de l'exécutable.
4. Charger des WAV avec Browser ou par clic droit sur un pad.
5. Choisir A–H, puis utiliser Pad, Keyboard ou Step pour construire le pattern.
6. Utiliser File/Save pour enregistrer un projet `.kicklab`, un kit `.kickkit` ou exporter un WAV.

Le dossier `samples` peut contenir des catégories, des packs et autant de sous-dossiers que nécessaire. Il reste extérieur au programme afin de ne pas alourdir les téléchargements et de ne pas redistribuer les sons de l'utilisateur.

## Maschine+

Passer Maschine+ en mode contrôleur avec `Shift + Channel/MIDI`, puis choisir de préférence l'entrée **Maschine Plus Virtual** dans KICK Lab. La page MIDI Control apprend et sauvegarde les pads, les 16 Q-Links, les boutons et les deux pages de touches Soft dans `profiles/MaschinePlus.kickmidi`.

KICK Lab reste entièrement utilisable sans Maschine+.

## Sources et organisation

| Emplacement | Contenu |
|---|---|
| `experiments/kicklab/` | Application Windows active et documentation détaillée |
| `upstream/0700-AAM/` | Sources publiques originales de Music Tribe conservées comme référence |
| `port/` | Adaptateurs et recherches du port complet |
| `docs/` | Analyses et documents de portage |
| `.github/workflows/` | Compilation et tests Windows automatisés |

La branche `main` correspond désormais à la version publique recommandée. Les fichiers affichés dans **Actions** sont des artefacts de validation ; les téléchargements destinés au public se trouvent toujours dans **Releases**.

## Compiler

Sous Windows avec Visual Studio 2022 :

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

L'exécutable est produit dans `build/incremental-port/Release/KickLab.exe`.

## Licence et statut

Prototype communautaire indépendant sous AGPL-3.0-or-later. Airwindows et AudioFile conservent leurs licences respectives. KICK, Behringer, Maschine et Native Instruments sont des marques de leurs propriétaires. Ce projet n'est pas une version officielle de Behringer, Music Tribe ou Native Instruments.

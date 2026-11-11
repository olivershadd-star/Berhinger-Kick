# KICK Windows — portage progressif

Ce dépôt adapte le code KICK publié par Music Tribe à un ordinateur Windows **sans surface matérielle KICK**. Le travail repart de la dernière base réellement jouable : moteur AudIO, sortie Windows WinMM, pads à la souris, samples, séquenceur et effets déjà testés.

## Version recommandée

**[Télécharger KICK Lab Windows 0.2.0 — base portable fonctionnelle](https://github.com/olivershadd-star/Berhinger-Kick/raw/v0.2.0/downloads/KickLab-Windows-x64-0.2.0.zip)**

Cette base possède une sortie audio Windows fonctionnelle, seize pads cliquables, le chargement de WAV, un séquenceur 16 pas, l'éditeur de sample et les effets AudIO/Airwindows déjà intégrés. Elle reste une étape intermédiaire du portage.

La préversion `v0.3.3-preview` qui chargeait toute l'interface originale d'un seul coup est conservée comme **référence technique uniquement**. Elle dépend encore de commandes matérielles, ne contient pas les programmes ni les samples retirés de la publication et n'est pas recommandée pour jouer.

## Méthode de travail

Chaque évolution part désormais de la base sonore ci-dessus :

1. sélectionner une fonction précise déjà présente dans le code KICK publié ;
2. porter son modèle et son traitement sans remplacer tout le moteur ;
3. fournir une commande souris ou clavier lorsqu'elle dépend d'un pad, d'une molette ou d'un bouton physique ;
4. vérifier le son, l'interface et les fonctions déjà acquises avant d'intégrer le module suivant ;
5. ne publier une version que si le parcours musical testé fonctionne réellement.

Le port complet expérimental est conservé sur la branche [`port/kick-original-windows`](https://github.com/olivershadd-star/Berhinger-Kick/tree/port/kick-original-windows). Il sert à consulter les écrans et les dépendances, pas de base aux versions jouables.

## Organisation

| Dossier | Rôle |
|---|---|
| `experiments/kicklab/` | Base Windows active : AudIO, WinMM, pads, samples, séquenceur et effets |
| `upstream/0700-AAM/` | 3 183 fichiers originaux Music Tribe conservés sans modification |
| `port/` | Adaptateurs et recherches du port complet, conservés pour reprendre des modules précis |
| `docs/PORTAGE-INCREMENTAL.md` | Ordre d'intégration et critères de validation |
| `downloads/` | Archives Windows précédentes |

## Pourquoi la préversion complète était muette

Le dépôt public ne contient aucun sample d'usine, aucun son de métronome et pratiquement aucun contenu de programme. Les cinq démos livrées sont de petits fichiers de projet presque vides. L'interface originale attend aussi la surface matérielle pour plusieurs sélections.

La base 0.2.0 utilise directement WinMM avec quatre buffers stéréo à 48 kHz. La préversion complète l'avait remplacé par un périphérique JUCE et un nouvel adaptateur de blocs AudIO. Ce remplacement simultané du périphérique, du moteur et de l'interface a supprimé la base sonore connue et rendu les problèmes impossibles à isoler. Cette voie n'est plus la voie principale.

## Construire la base active

Sous Windows avec Visual Studio 2022 :

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

L'exécutable est produit dans `build/incremental-port/Release/KickLab.exe`. La compilation complète de référence reste possible avec `-DKICK_BUILD_ORIGINAL_REFERENCE=ON`.

## Licence

Les sources Music Tribe et les adaptations restent sous AGPL-3.0-or-later. Airwindows et AudioFile conservent leurs licences. KICK et Behringer restent des marques de leurs propriétaires ; ce travail communautaire n'est pas une version officielle.

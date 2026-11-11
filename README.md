# KICK — portage Windows du code original

L'objectif de ce dépôt est de faire fonctionner **l'application KICK publiée par Music Tribe sous Windows, en version portable**, avec son interface et ses fonctions existantes.

**Il n'y a pas encore de version Windows fonctionnelle du KICK original.** Les ZIP KICK Lab 0.1–0.2 restent des expérimentations indépendantes ; ils ne constituent pas un portage de la groovebox. Leur développement musical est arrêté dans cette direction.

## Organisation

| Dossier | Rôle |
|---|---|
| `upstream/0700-AAM/` | Sources originales non modifiées : application, AudIO, protocole, interface et ressources |
| `upstream/provenance.json` | Origine, périmètre, révision vérifiée et empreintes des 3 183 fichiers |
| `port/windows/` | Préparation technique du portage Windows ; pas de séquenceur ou d'éditeur de substitution |
| `tools/` | Audit des dépendances et vérification d'intégrité |
| `docs/PORTAGE.md` | Comparaison avec le prototype et état réel des blocages |
| `experiments/kicklab/` | Ancien prototype conservé pour référence, séparé de l'application cible |
| `downloads/` | Anciennes versions expérimentales, identifiées comme telles |

Les fichiers réimportés ont été comparés au dépôt [MusicTribeOpenSource/0700-AAM](https://github.com/MusicTribeOpenSource/0700-AAM), révision `27194180ce5fbab3776ebce53e2d4b016597364e` : ils sont identiques. Le système Buildroot Linux n'est pas recopié, à l'exception de son README ; le périmètre exact est consigné dans le manifeste.

## État de compilation

L'interface cible est `kick_ui::KickHome`, instanciée par `Kick/apps/kick/Main.cpp`. Le moteur et le séquenceur cibles sont ceux de `Kick/modules/engine` et d'AudIO.

Quatre éléments empêchent actuellement la configuration du projet original :

- `AudIO/juce_modules.cmake` ;
- le module `AudIO_constants` ;
- le module `AudIO_juce`, dont dépend l'intégration du périphérique audio ;
- `juce_serialport`, retiré de la publication mais toujours référencé.

Leur absence est détectée par la préparation du portage. Aucun remplacement musical, aucune interface factice et aucun exécutable KICK de substitution ne sont générés pour masquer un blocage.

## Adaptations Windows en cours

Un premier adaptateur `mt::AudioDevice` relie l'interface publique `AudIO::IAudioDevice` à JUCE. Il conserve les blocs de **480 échantillons** imposés par `DawEngine.cpp` et adapte les tailles de tampon du périphérique sans perdre ni rejouer d'échantillons. Il ne remplace aucune fonction musicale. Sa compilation Windows est vérifiée séparément ; il n'est **pas encore intégré ni validé dans KICK**.

`tools/stage_windows_port.py` prépare une copie des sources avec un journal des modifications techniques : identifiants de modules JUCE, exclusion Windows du port série Linux absent, compatibilité C++ Airwindows et déclaration de `<thread>`. Le snapshot original reste intact. Ce script ne produit pas encore d'application compilable.

La tentative de compiler l'en-tête de l'interface originale révèle aussi des définitions absentes dans `AudIO_constants` : `MIDI`, `MIDI_CH`, `POLYPHONY`, `GAIN`, `PAN`. Certaines concernent directement les valeurs musicales. Le fichier de compatibilité actuel ne fournit que la taille de bloc vérifiée ; il ne prétend pas restaurer tout ce module.

Pour vérifier uniquement le transport des tampons audio :

```sh
cmake -S port/checks -B build-platform
cmake --build build-platform --config Release
ctest --test-dir build-platform -C Release --output-on-failure
```

Sous Windows, ajouter `-DKICK_CHECK_AUDIO_DEVICE=ON` à la configuration compile également le véritable adaptateur JUCE/AudIO. **Ces vérifications ne produisent pas `Kick.exe`.**

## Vérifier la base

Avec Python 3 :

```sh
python tools/audit_original.py --verify-original --json build/source-audit.json
```

Pour constater aussi l'échec attendu tant que les dépendances manquent :

```sh
python tools/audit_original.py --strict --verify-original
```

Le code de sortie 2 signale les dépendances absentes. Le code 3 signale une modification du snapshot original. Un audit réussi n'est pas une compilation de KICK.

Avec CMake :

```sh
cmake -S . -B build-audit -DKICK_AUDIT_ONLY=ON
```

Cette dernière commande ne construit aucune application. La configuration normale sans `KICK_AUDIT_ONLY` est volontairement bloquée par les dépendances absentes. Quand les sources originales complètes seront disponibles, `port/windows/build.ps1` préparera une copie de compilation et invoquera la cible originale `Kick` ; cela ne garantit pas encore que toutes les adaptations Windows seront faites.

## Méthode retenue

1. Conserver le code musical et l'interface d'origine comme référence.
2. Restaurer les dépendances de compilation et documenter les adaptateurs Windows nécessaires.
3. Compiler et ouvrir l'interface KICK d'origine.
4. Vérifier le moteur, les séquences, les programmes, les effets et les projets **dans cette interface**.
5. Emballer une version portable seulement après ces vérifications.

Aucune nouvelle fonction musicale ne sera développée dans cette phase. Voir [les règles de portage](docs/REGLES.md) et [l'analyse détaillée](docs/PORTAGE.md).

## Licence et statut

Les sources Music Tribe gardent leurs licences AGPL-3.0-or-later et celles de leurs composants tiers. Les noms et logos restent des marques de leurs propriétaires. Ce dépôt est un travail communautaire indépendant, pas une version officielle approuvée par Music Tribe.

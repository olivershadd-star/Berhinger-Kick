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

Les adaptations de compilation sont maintenant préparées dans une copie des sources. Le CMake racine lance la **véritable cible `Kick`** sous Windows ; l'inventaire des dépendances originales manquantes ne constitue plus un arrêt préalable obligatoire. La compilation de l'application reste incomplète.

## Adaptations Windows en cours

- Liaison `mt::AudioDevice` entre l'interface publique `AudIO::IAudioDevice` et JUCE ; blocs de 480 échantillons conservés et adaptation des tailles de tampon du périphérique.
- Module de constantes de compatibilité, types d'énumération d'enregistrement, valeurs vides attendues par les modèles et corrections techniques de compilation. Certaines limites et valeurs par défaut restent **provisoires**, explicitement documentées dans [WINDOWS-COMPATIBILITY.md](docs/WINDOWS-COMPATIBILITY.md).
- Préparation traçable des sources originales et de leurs dépendances locales, sans requête vers le serveur interne fictif d'AudIO.
- Vérifications séparées des unités originales de modèles, projets, samples, programmes, moteur, événements et interface, en plus du pont audio. Une compilation d'objets ne valide pas l'application complète.

Les contrats de `SampleRecorder`, du moteur `Looper`, des filtres et des enveloppes des voix sont maintenant reliés aux primitives AudIO publiées. Le looper ne possède pas encore son historique undo/redo et l’application complète doit encore passer l’édition de liens et les essais d’exécution.

Pour les vérifications de transport audio :

```sh
cmake -S port/checks -B build-platform
cmake --build build-platform --config Release
ctest --test-dir build-platform -C Release --output-on-failure
```

Sous Windows, ajouter `-DKICK_CHECK_AUDIO_DEVICE=ON` compile aussi l'adaptateur JUCE/AudIO et les unités originales sélectionnées de modèles, d’interface et d’événements. Ces vérifications ne produisent pas `Kick.exe`.

Pour tenter l'intégration de l'application originale sous Windows :

```powershell
./port/windows/build.ps1
```

Cette commande n'est pas encore une livraison fonctionnelle : lire les erreurs de compilation et les limites documentées.

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

Cette dernière commande ne construit aucune application. L'audit strict décrit les absences du snapshot original, indépendamment des adaptations du portage. Sous Windows, la configuration normale prépare désormais les sources adaptées et tente la compilation réelle de `Kick`.

## Méthode retenue

1. Conserver le code musical et l'interface d'origine comme référence.
2. Restaurer les dépendances de compilation et documenter les adaptateurs Windows nécessaires.
3. Compiler et ouvrir l'interface KICK d'origine.
4. Vérifier le moteur, les séquences, les programmes, les effets et les projets **dans cette interface**.
5. Emballer une version portable seulement après ces vérifications.

Aucune nouvelle fonction musicale ne sera développée dans cette phase. Voir [les règles de portage](docs/REGLES.md) et [l'analyse détaillée](docs/PORTAGE.md).

## Licence et statut

Les sources Music Tribe gardent leurs licences AGPL-3.0-or-later et celles de leurs composants tiers. Les noms et logos restent des marques de leurs propriétaires. Ce dépôt est un travail communautaire indépendant, pas une version officielle approuvée par Music Tribe.

# Couche de portage

Ce dossier reçoit uniquement les adaptations techniques nécessaires à Windows : configuration de compilation, périphérique audio JUCE/AudIO, entrée clavier/souris vers les commandes d'origine, chemins et emballage portable.

Aucun séquenceur, éditeur, format de projet ou effet musical de remplacement ne doit être ajouté ici. L'application visée est `Kick/apps/kick/Main.cpp` et son écran `kick_ui::KickHome`, pas `KickLab.exe` et pas le simulateur `kickem` sans réparation préalable.

État actuel : les dépendances originales manquantes empêchent encore la compilation. `build.ps1` s'arrête sur ce diagnostic ; ce n'est pas un lancement de KICK.


Les premières adaptations se trouvent dans `modules/AudIO_juce`. Le test `tests/fixed_block_bridge.cpp` utilise le vrai `AudIO::Buffer<float>` et son pool mémoire pour vérifier la continuité stéréo avec des tampons de tailles irrégulières, la pause, la réinitialisation et l'absence d'entrées. Il n'exécute pas le moteur musical complet.

`modules/AudIO_constants` est **partiel** : seule la taille de bloc est prouvée par le code publié. Les nombres de canaux, de bus et les constantes musicales manquantes ne sont pas inventés. `tools/stage_windows_port.py --destination <dossier neuf hors dépôt>` prépare la copie de travail et écrit `port-changes.json` ; cette préparation n'est pas une preuve de compilation de l'application.

# Couche de portage

Ce dossier reçoit uniquement les adaptations techniques nécessaires à Windows : configuration de compilation, périphérique audio JUCE/AudIO, entrée clavier/souris vers les commandes d'origine, chemins et emballage portable.

Aucun séquenceur, éditeur, format de projet ou effet musical de remplacement ne doit être ajouté ici. L'application visée est `Kick/apps/kick/Main.cpp` et son écran `kick_ui::KickHome`, pas `KickLab.exe` et pas le simulateur `kickem` sans réparation préalable.

État actuel : les dépendances originales manquantes empêchent encore la compilation. `build.ps1` s'arrête sur ce diagnostic ; ce n'est pas un lancement de KICK.

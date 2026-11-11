# Couche de portage Windows

Ce dossier reçoit uniquement les adaptations nécessaires à Windows et la compatibilité avec les dépendances non publiées. L'application visée reste `Kick/apps/kick/Main.cpp` et son écran `kick_ui::KickHome`. Le séquenceur, les modèles, les voix et les écrans ne sont pas remplacés par le prototype KICK Lab.

`modules/AudIO_juce` contient le périphérique JUCE/AudIO et le transport des blocs audio. Les tests utilisent le véritable `AudIO::Buffer<float>` pour vérifier la continuité des échantillons avec des tailles de tampon irrégulières.

`modules/AudIO_constants` fournit les constantes nécessaires à l'intégration. Certaines valeurs non retrouvées sont provisoires : elles sont identifiées dans [WINDOWS-COMPATIBILITY.md](../docs/WINDOWS-COMPATIBILITY.md), sans prétendre être les valeurs originales de Behringer.

`tools/stage_windows_port.py --destination <dossier neuf>` prépare une copie avec les corrections techniques consignées dans `port-changes.json`. Une nouvelle invocation réutilise la copie si ses entrées sont identiques ; si elles ont changé, employer un nouveau dossier de compilation.

`windows/build.ps1` tente désormais de compiler la cible originale `Kick`. Le travail reste incomplet, notamment pour les contrats d'enregistrement, de looper et de traitement des voix absents de l'archive. Aucun exécutable complet n'est encore validé.

# Règles du portage Windows progressif

Objectif : construire une version Windows jouable du code KICK publié, utilisable sans la surface matérielle qui n'est pas disponible.

- Conserver la sortie WinMM et la base AudIO validées jusqu'à ce qu'un remplacement audio passe les mêmes essais sur Windows.
- Intégrer une fonction KICK à la fois et identifier ses fichiers d'origine.
- Ne jamais considérer une catégorie, un écran ou une compilation comme une fonction portée.
- Ajouter une commande souris ou clavier à chaque interaction qui dépend du matériel KICK.
- Vérifier le son et les fonctions précédentes après chaque intégration.
- Signaler les contenus absents de la publication, notamment programmes, SoundFont, samples et sons de métronome.
- Garder `upstream/` intact et placer toute adaptation Windows dans le code du portage.
- Conserver le port complet sur sa branche de référence jusqu'à ce qu'il soit réellement jouable.

Une version distribuée doit permettre au minimum de choisir un son, le déclencher, le séquencer et l'entendre sans matériel externe. Les étapes suivantes sont définies dans [PORTAGE-INCREMENTAL.md](PORTAGE-INCREMENTAL.md).

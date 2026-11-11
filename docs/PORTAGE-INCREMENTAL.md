# Portage progressif sans surface matérielle

## Base garantie

La base de chaque évolution est KICK Lab 0.2.1 : AudIO rend les seize lecteurs vers un bus stéréo, WinMM fournit la sortie Windows, et les commandes souris, clavier et MIDI déclenchent les pads. Le séquenceur, le chargement WAV, l'éditeur et les deux effets existants constituent le test de non-régression.

Une évolution ne remplace plus simultanément le périphérique audio, le graphe, les modèles et l'interface.

## Ce que la publication Music Tribe ne fournit pas

- aucun fichier audio d'usine ni son de métronome ;
- aucun programme jouable prêt à sélectionner ;
- cinq démos et cinq modèles dont les fichiers `.kickPrj` ne contiennent que des structures minimales ;
- plusieurs dépendances de compilation, constantes et contrats de périphérique ;
- la surface de contrôle physique utilisée par certains écrans.

Une catégorie visible dans l'interface ne prouve donc pas qu'un instrument, un preset ou un effet utilisable est fourni derrière cette catégorie.

## Ordre d'intégration

1. **Commandes sans doublon** : clarifier projet, transport et audition sans modifier le moteur sonore.
2. **Bibliothèque de samples** : reprendre le dossier Sounds, le choix Factory/ordinateur et l'ajout d'assets. La bibliothèque personnelle démarre sans les sons d'usine retirés de la publication.
3. **Potentiomètres** : remplacer les longues barres par des commandes rotatives liées aux paramètres du moteur.
4. **Effets et inserts** : porter les quatre slots du modèle KICK, puis intégrer une famille d'effets à la fois. Les paramètres doivent être visibles, sélectionnables et contournables sans molette matérielle.
5. **Enveloppes** : porter l'AHDSR d'amplitude, puis les enveloppes de filtre et de hauteur déjà présentes dans les voix KICK.
6. **Transport et longueur de séquence** : reprendre les modèles de temps KICK tout en conservant le rendu WinMM. Les pages et longueurs doivent être accessibles à la souris.
7. **Looper et export WAV** : reprendre l'enregistreur KICK déjà implémenté plutôt que créer un format audio parallèle.
8. **Programmes et mixage** : porter progressivement Keygroup, Clip, Audio, les mixeurs et le mode Song lorsque leurs dépendances sont validées.

## Critères obligatoires pour chaque étape

- le programme démarre sans contrôleur KICK, MIDI ou périphérique externe ;
- chaque nouvelle commande est utilisable à la souris ;
- pads, samples, séquenceur et effets déjà acquis continuent de produire du son ;
- le nouveau module possède un scénario audio vérifiable ;
- la manipulation de l'interface pendant la lecture ne coupe ni ne déforme le son ;
- la version portable est testée, pas seulement compilée.

Le port complet conservé sur `port/kick-original-windows` reste un inventaire utile : il montre quelles unités compilent et quels écrans existent. Il ne doit plus être distribué comme version jouable avant que les modules aient passé ces étapes séparément.

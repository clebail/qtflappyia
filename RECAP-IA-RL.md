# Récap — Montée en théorie IA/ML pour un solver de Rubik's cube

> Document de passation autonome. Il résume une session de travail/apprentissage avec
> Claude Code (juin 2026). Objectif : pouvoir **reprendre depuis n'importe quel dossier ou
> n'importe quel PC** sans avoir à re-dérouler tout le raisonnement.
> À coller en contexte d'une nouvelle session pour repartir là où on en est.

---

## 0. Contexte & objectif

- **Projet déclencheur** : `~/dev/clbrub` — un Rubik's cube 3D en **C++/Qt** avec un module
  **Python embarqué** (`rubik` : `init / melange / map / exec / win`).
  `rubik.map()` renvoie **324 valeurs** = 54 stickers × 6 couleurs en **one-hot**, ordonnées
  par faces (X-, X+, Y-, Y+, Z-, Z+).
- **But** : faire résoudre le cube par un **script Python lancé depuis l'interface**, dans une
  optique d'**apprentissage du ML** (pas juste « un solver qui marche » — sinon la lib
  `kociemba` réglerait l'affaire en 30 lignes, sans ML).
- **Décision actée** : on prend le chemin « le défi » → **reinforcement learning**.

### Mon profil (pour calibrer les explications)
Développeur **C++/Qt confirmé**. J'ai déjà codé **à la main** :
- **Neuroévolution** : réseaux de neurones (poids, seuils, sigmoïde) entraînés par
  **algorithme génétique** (sélection / croisement / mutation / élitisme).
  → `~/dev/ia` (voitures autonomes, 12 circuits pour la généralisation) et
  `~/dev/qtflappyia` (Flappy Bird, 1 couche cachée).
- **Recherche classique d'espace d'états** : `~/dev/solisolve` (solveur peg solitaire,
  **BFS couche par couche**, déduplication par forme canonique invariante aux symétries).

**Ce qui me manquait vraiment** (pas « toute la théorie », juste 2 briques) :
1. L'apprentissage par **gradient / backpropagation** (j'ai toujours entraîné mes réseaux
   par GA, jamais par descente de gradient).
2. Le **formalisme du RL** : MDP, fonction de valeur, équation de Bellman, value iteration.

---

## 1. BRIQUE 1 — Apprentissage par gradient (backpropagation)

### Mon neurone (rappel)
`CNeurone::eval()` calcule un neurone artificiel standard :

```
z = b + Σ wᵢ·xᵢ        (b = genes[0] = biais ; wᵢ = genes[i] ; xᵢ = inputs)
y = σ(z) = 1 / (1 + e^-z)   (sigmoïde ; le paramètre 'a'/PENTE_NEURONE est un gain)
```

### Ce que le GA ne faisait pas
`mute()` remplace un poids par une valeur **au hasard** → « je tripote, je garde si ça
survit mieux ». **Aveugle** : aucune direction, aucune amplitude.

### Ce que le gradient ajoute : une erreur dérivable
Le GA n'a besoin que d'un **classement**. Le gradient exige une **cible `t`** et une
**erreur continue**, ex. erreur quadratique : `L = ½(y − t)²`.

### La règle de la chaîne (cœur de la backprop)
Pour un poids `wᵢ`, en suivant `wᵢ → z → y → L` à l'envers :

```
∂L/∂wᵢ = (∂L/∂y) · (∂y/∂z) · (∂z/∂wᵢ)
       =  (y − t) ·  y(1−y)  ·   xᵢ
```

- `∂L/∂y = (y − t)`        → l'erreur (signe = faut-il monter/descendre)
- `∂y/∂z = y(1−y)`         → **dérivée de la sigmoïde, exprimée à partir de la sortie `y`
  que `eval()` renvoie déjà** (rien de plus à calculer)
- `∂z/∂wᵢ = xᵢ`           → l'entrée

### La mise à jour (remplaçant de `mute`)
```
wᵢ ← wᵢ − η · (y − t) · y(1−y) · xᵢ        (η = learning rate = taille du pas)
```

| | `mute()` (GA) | descente de gradient |
|---|---|---|
| Direction | aucune (aléatoire) | **calculée** (signe de `y−t`) |
| Amplitude | aucune | **proportionnée** à l'erreur et à `xᵢ` |
| Poids modifiés | 1, au hasard | **tous**, chacun sa correction, d'un coup |
| Besoin d'une cible `t` | non | **oui** (donnée étiquetée) |

**Backprop multi-couches** (pas encore détaillé en séance, à reprendre) : pour les neurones
**cachés** (mes 4 neurones cachés de Flappy), pas de cible directe → on **propage l'erreur**
de la sortie vers l'arrière à travers la couche. C'est la généralisation de la chaîne
ci-dessus. **= le "back" de backprop. À FAIRE.**

---

## 2. Mon objection clé (= la motivation de tout le RL)

> « J'utilise le GA pour ne pas avoir à savoir si, à cet instant, le piaf aurait dû monter
> ou descendre. »

100 % juste. C'est le **problème d'attribution du crédit** (*credit assignment*) : la
récompense arrive **tard** (à la mort) et **globalement** (sur toute la partie), jamais
décision par décision.

- **Le GA esquive le problème** : il évalue la *politique entière* par son *fitness total*
  et sélectionne les meilleurs cerveaux. Pas besoin de cible, pas besoin de dérivée.
  **Bon choix… mais ne passe pas à l'échelle** (OK pour ~5 neurones, pas pour un réseau à
  des millions de poids comme il en faut pour le Rubik).
- **Le gradient passe à l'échelle mais réclame une cible** qu'on n'a pas.

**Sortie du dilemme = le RL** : au lieu qu'un humain fournisse la cible, **l'agent la fabrique
à partir de la récompense**, via la fonction de valeur et l'équation de Bellman. Le gradient
a alors une cible à apprendre, sans qu'on ait jamais dit « ici il fallait monter ».

> Mon ancien `train.py` du Rubik **inventait** une cible bidon (« le bon coup = l'inverse du
> dernier mélange ») → c'était faux, d'où l'échec. DeepCubeA n'invente pas : il **calcule**
> la cible par Bellman.

---

## 3. BRIQUE 2 — RL : MDP, fonction de valeur, Bellman

### Le MDP (Markov Decision Process)
Quatre ingrédients ; « Markov » = **l'état suffit pour décider**.

| Ingrédient | Flappy | solisolve | Rubik |
|---|---|---|---|
| État `s` | les 11 distances de capteurs | plateau (37 trous) | les 324 stickers |
| Action `a` | battre / rien | un saut de bille | un des 12 coups |
| Transition `s→s'` | la physique (`next()`) | plateau après saut | cube après coup |
| Récompense `r` | survie / tuyau | 1 bille = gagné | résolu = fini |

- **Politique `π(s)`** = « quelle action dans `s` » = **mon "cerveau"/réseau**. Le GA cherche
  directement la meilleure politique.
- **Retour** = somme des récompenses sur la partie = **mon fitness**.

### Fonction de valeur = le "numéro de couche" de mon BFS
Pour le Rubik (formulé en **coût** : 1 par coup, 0 à l'état résolu) :

```
J(s) = nombre minimal de coups pour résoudre depuis s   (le "cost-to-go")
J(résolu) = 0
```

**Je l'ai déjà calculée sans le savoir dans solisolve** : le n° de couche d'un état (couche 0
= résolu, couche 1 = à 1 coup…) **EST** sa fonction de valeur.

### Équation de Bellman (la relaxation que mon BFS faisait déjà)
```
Rubik (minimiser un coût) :   J(s) = min_a [ 1 + J(enfant(s,a)) ],   J(résolu)=0
Flappy (maximiser survie)  :  Q(s,a) = r + γ · max_a' Q(s', a')
```
**Même équation** : « valeur de maintenant = immédiat + (meilleure) valeur de la suite ».
`min` vs `max` selon qu'on compte un **coût** ou une **récompense**. Le `γ` (actualisation)
apparaît pour les parties potentiellement infinies (Flappy) ; le Rubik fini/déterministe y
échappe. BFS et Dijkstra sont des cas particuliers de Bellman.

### Value iteration (Bellman → algorithme)
```
1. Initialiser J(s) à une estimation quelconque
2. Répéter : J(s) ← min_a [ 1 + J(enfant(s,a)) ]   (J(résolu)=0 fixé)
3. Jusqu'à stabilité.
```
À convergence, `J` est exact. C'est mon balayage couche-par-couche de solisolve, généralisé.

### Q-learning / DQN (côté Flappy)
`Q(s,a)` = « si je bats des ailes maintenant (ou pas), puis joue bien, qu'est-ce que ça
rapporte ». Décider = comparer `Q(s,battre)` vs `Q(s,rien)`, prendre le max. On **bootstrappe**
la cible `r + γ·max_a' Q(s',a')` depuis sa propre estimation, et on entraîne `Q` par gradient.
C'est le **Q-learning** ; avec un réseau dessus = **DQN** (celui des jeux Atari).

> Détail Markov utile : dans Flappy, `inputs[]` ne contient QUE les 11 rayons, **pas la
> vitesse verticale** (`inc`, `onUp`, `nbCycleUp`). Strictement, l'état n'est donc pas
> pleinement markovien. Le GA s'en sort (les rayons varient dans le temps) mais une vraie
> approche RL devrait ajouter la vitesse à l'état.

---

## 4. La leçon centrale : pourquoi le GA gagne sur Flappy mais meurt sur le Rubik

| | Flappy (GA marche) | Rubik (GA s'effondre) |
|---|---|---|
| Cerveau aléatoire au départ | survit parfois 20-30 ticks → fitness > 0 | ne résout JAMAIS → fitness = 0 partout |
| Paysage de fitness | a des pentes → la sélection grimpe | **plat à zéro** → rien à quoi s'accrocher |

**Récompense rare (*sparse*)** = le tueur du GA. La **fonction de valeur** est l'outil qui
**fabrique une pente là où la récompense est rare** : en partant de l'état résolu (`J=0`) et
en propageant Bellman vers l'extérieur, on attribue une valeur **graduée** (« à 3 coups »,
« à 7 coups ») même là où aucune récompense directe n'existe. On **crée du signal dense à
partir d'un signal rare.**

---

## 5. L'approche cible : DeepCubeA (assemblage des 2 briques)

Réf. : McAleer / Agostinelli et al., 2019 — *« Solving the Rubik's Cube with Deep RL and
Search »* (+ *« …Without Human Knowledge »* pour l'Autodidactic Iteration).

Le Rubik 3×3 a **4,3 × 10¹⁹ états** → impossible de stocker `J(s)` dans une table ou de
faire un BFS complet (solisolve le pouvait car le peg solitaire est petit). Donc :

1. **Approximer `J` par un réseau** `J_θ(s)` (entrée 324, sortie **1 scalaire** = cost-to-go).
2. **Générer les données** : partir du **cube résolu** et mélanger en arrière de `k` coups
   (`k` = 1..K) → curriculum du facile au difficile. Pas de label fourni.
3. **Entraîner par value iteration neuronale (Autodidactic Iteration)** :
   ```
   cible(s) = min_a [ 1 + J_θ(enfant(s,a)) ]     (avec J(résolu)=0)
   ```
   régresser `J_θ` vers cette cible **par gradient** (brique 1), avec un **réseau-cible**
   mis à jour périodiquement (stabilité). Itérer.
4. **Résoudre** : **A\*** (ou weighted A*) guidé par l'heuristique `J_θ` → sort la **séquence**
   de coups. (= la moitié "recherche", que je sais déjà faire dans solisolve.)

### Décisions techniques actées
- Sortie réseau = **1 scalaire (régression, MSE)**, PAS 18 probas de classification.
- **MLP** (denses + éventuels blocs résiduels), PAS de CNN (la convolution spatiale n'a pas
  de sens sur des stickers).
- **12 coups** quart-de-tour (U D L R F B + inverses), PAS 18 — les slices M/E/S sont
  redondantes et doublent l'espace pour rien.
- Cube réimplémenté en **numpy pur pour l'entraînement** (rapide, batchable, hors Qt) ; le
  module C++ `rubik` reste pour **l'interface + l'exécution finale** de la solution.
  ⚠️ **Contrainte critique** : la représentation d'état doit être **identique** des deux côtés
  (même ordre de stickers que `rubik.map()`), sinon rien ne marche.
- **Scaler down** : viser des mélanges profondeur ~7-12 (atteignable sur machine modeste),
  pas le God's number = 20 (DeepCubeA complet = des jours sur plusieurs GPU).

### L'ancien code à remplacer (`clbrub/train.py`, `resolvpy.py`)
CNN supervisé **conceptuellement cassé** : label = inverse du dernier coup de mélange
(≠ bon coup vers la solution) + décodage **glouton** sans recherche (d'où le hack `old` pour
interdire de revenir en arrière). `loss='cosine_proximity'` aussi inadapté.

---

## 6. Plan de la roadmap (côté code, quand on y sera)

- **Phase 0 — Découplage & parité** : cube en numpy + script qui vérifie **sticker par
  sticker** qu'une même séquence donne le même `map()` qu'en C++. *Tout repose là-dessus.*
- **Phase 1 — Génération** : scramble-depuis-résolu, profondeur 1..K, en batch numpy.
- **Phase 2 — Réseau** : MLP value-net (324 → 1).
- **Phase 3 — Entraînement** : boucle ADI avec réseau-cible.
- **Phase 4 — Solver A\*** : heuristique = `J_θ`, retourne une séquence → `rubik.exec()`.

---

## 7. Où on en est & prochaines étapes

**Phase actuelle : Phase A ✅ TERMINÉE — Phase B ✅ TERMINÉE.**

### Ce qui est codé (branche `resolve-gradient`)

**Refactoring de la hiérarchie de neurones :**
```
CNeurone (base, eval() pure virtuelle, backward(), getGene/setGene)
├── CNeuroneSigmoide (eval = sigmoïde avec PENTE_NEURONE)
│   └── CNeuroneGA    (+ seuil, from, mute, copyFrom — tout le GA)
├── CNeuroneRelu      (eval = max(0,z), cache z pré-activation pour backprop)
└── CNeuroneLineaire  (eval = z, sortie non bornée pour régression)
```
`CCapteur` enrichi de `setValue()`. Le GA existant est **intact**.

**`CMLP` — le réseau gradient :**
- Architecture : 12 entrées → `RL_NB_HIDDEN=16` `CNeuroneRelu` cachés → 2 `CNeuroneLineaire` sorties
- `forward(inputs[])` : calcule et cache `sortieCaches[]` et `sortie[]`
- `act()` : argmax sur `sortie[]` → action 0 (rien) ou 1 (battre)
- `backward(action, cible, eta)` : backprop complète 2 couches
  - couche sortie : `erreur = sortie[action] - cible`, mise à jour poids
  - couche cachée : `erreur_cachée[i] = gradInputs[i] * (z[i] > 0 ? 1 : 0)`, mise à jour poids
- Accesseurs : `getQ(i)` **public** (API de prod) ; `getSortieCache(i)`, `getNeuroneSortie(i)`,
  `getNeuroneCache(i)` passés **`private`** + `friend class TestCMLP;` (réservés au test white-box).

**Gradient-check : ✅ promu en vrai test unitaire Qt (dette technique close).**
- `checkGradient()` de `main.cpp` **supprimé** ; remplacé par `tests/TestCMLP` (Qt Test).
- Le test valide **tous les poids** du réseau (biais + poids, couche sortie + cachée) en
  **un seul parcours à plat**, et **interroge `backward()` lui-même** via l'astuce
  `grad_analytique = (poids_avant − poids_après) / η` (donc indépendant de toute formule
  recodée à la main). Gradient numérique = différence centrée sur `L = ½(Q[action] − cible)²`.
  Tous passent (`make check` vert).
- ⚠️ Pièges rencontrés (pour mémoire) : le numérique doit utiliser **exactement** la perte
  qu'implique `backward` (le **½** sinon facteur 2) ; remettre `idxGene = 0` avant chaque boucle.

**Réorganisation du dépôt (structure `subdirs`) :**
```
qtflappyia/
├── qtflappyia.pro      (chapeau : TEMPLATE=subdirs, SUBDIRS = src tests, tests.depends=src)
├── src/   src.pro      (l'app ; TARGET = qtflappyia)  + tous les .cpp/.h/.ui/.qrc/icônes
└── tests/ tests.pro    (Qt Test : QT+=testlib, compile TestCMLP.cpp + les ../src/*.cpp utiles)
```
Build depuis la racine : `qmake && make` ; lancer le test : `make check` (ou `./tests/tests`).

**Reste optionnel (non bloquant) :** durcir le test en bouclant sur `action ∈ {0,1}` et
quelques jeux d'`inputs` (positifs/négatifs) → couvre le 2e neurone de sortie et des ReLU
éteints (`z<0`).

**Conventions de code appliquées à tout le dépôt :**
- Toutes les déclarations sans paramètre passées en `(void)` (style C strict).
- `const` manquants ajoutés : `CNeurone::getNbGene(void) const`,
  `Common::getSpritesImage(void) const`.

**Capteur de vitesse markovien — `CFlappy::getVitesse(void) const` ✅**
- `FLAPPY_NB_INPUTS = FLAPPY_NB_FRONT + FLAPPY_NB_REAR + 1 = 12` (slot `+1` existait déjà).
- Formule : `onUp ? (double)(MAX_CYCLE_UP − nbCycleUp) / MAX_CYCLE_UP : (onDown ? −1.0 : 0.0)`
  Plage `[−1 ; 1]`. Décroît linéairement pendant la montée (1.0 → ~0), constant à −1.0 en chute.
- Markovien pour tout ce qui pilote la survie ; micro non-markovianité du balancement neutre
  sans conséquence pratique.
- Méthode `public` : le DQN lira l'état de l'extérieur sans passer par `think()`.

**Refactoring hiérarchie `CFlappy` ✅**
```
CFlappy   (base abstraite : physique, capteurs, getImage via spriteType)
├── CFlappyGA  (+ CNeuroneGA, think GA, from, getFitness)
└── CFlappyRL  (CMLP + DQN) ✅
```
- `CFlappy(int x, int y, int ySol, Common::ESpriteType spriteType)` : le type de sprite
  est passé au constructeur de base (pas de virtuelle pure dans le constructeur).
- `CFlappyGA` passe `Common::estFlappy` ; futur `CFlappyRL` passera `Common::estFlappyRL`.
- `from()` et `getFitness()` dans `CFlappyGA` uniquement.
- `CNeuroneGA` inclus dans `CFlappyGA.h`, plus dans `CFlappy.h`.
- Classe renommée `Flappy` → `CFlappy` (fichiers `CFlappy.h/.cpp`, `CFlappyGA.h/.cpp`).
- `CGenetic` utilise `QList<CFlappyGA*>`.

**Sprite RL vert ✅**
- `Common::ESpriteType` : ajout de `estFlappyRL`.
- Frames vertes générées par décalage de teinte HSV +120° sur les frames originales,
  insérées dans `flappy.png` à `y = 150` (juste sous les frames rouges à `y = 121`).
- Enregistrées dans `Common::Common()` : `flappyRL.rects` aux positions `(688/729/771, 150)`.

**Prochaine étape : Phase B — étape 4 (état & récompense).**

---

## 8. DÉCISION : on code le prototype RL de Flappy ici (option 4 du §7)

Branche `resolve-gradient`. On transforme le Flappy entraîné par **GA** (existant : `CNeurone`
+ `CGenetic`, forward sigmoïde, sélection/croisement/mutation) en un Flappy entraîné par
**descente de gradient + Q-learning (DQN)**, **à côté** du GA, pour comparer concrètement.
Cela rend tangibles les deux briques : brique 1 (backprop multi-couches, le « back » encore
À FAIRE) et brique 2 (Q-learning / cible de Bellman).

### Méthode de travail (actée)
**Socratique, pas à pas.** Pour chaque brique : Claude explique la théorie + la décision →
**Corentin code** → Claude relit et corrige. On n'avance qu'une fois la brique comprise (et
testée quand c'est possible). Claude **n'écrit pas** le code à la place de Corentin (le but
est de *comprendre exactement* ce qu'on fait) ; il guide, relit, propose des tests.

### Rappel de l'existant (pour s'y greffer)
- Réseau GA actuel : **12 entrées** (`FLAPPY_NB_INPUTS` = 9 rayons avant + 2 arrière +
  1 vitesse normalisée) → **4 neurones cachés** sigmoïde → **1 neurone de sortie** sigmoïde.
  Décision dans `CFlappyGA::think()` : `sortie ≥ seuil → up()`.
- `CNeurone::eval(a)` : `z = genes[0] + Σ inputs[i]·genes[i]` ; `y = e^{az}/(e^{az}+1)`
  (`a = PENTE_NEURONE = 0.01`). `genes[0]` = biais, `genes[1..]` = poids (objets `CCapteur`).
- Entraînement GA dans `CGenetic` : tri par `CFlappyGA::getFitness()` (= `score*100000 + age`),
  croisement `CFlappyGA::from()`, mutation aléatoire `CNeurone::mute()`. Boucle de sim dans
  `MainWindow::onTimer()` : `think → next → collision → markDead`, et `nextGeneration()`
  quand tout le monde est mort.

### Décisions de conception (fixées)
- On **garde `CNeurone`/`CGenetic` intacts** (le GA reste pour comparer).
- **Nouvelle classe MLP dédiée** (réutilisable ensuite pour le value-net du Rubik) : forward
  avec **mise en cache** des activations + backward (backprop + mise à jour SGD).
- **Activations : ReLU caché, sortie linéaire.** Pourquoi : Q est un réel **non borné**
  (régression / MSE) → la sigmoïde saturerait. Cohérent avec le MLP-régression de DeepCubeA.
- **État markovien** : ajouter la **vitesse verticale** à l'état (le §3 notait que les rayons
  seuls ne le sont pas). Représentation compacte vs 11 rayons → à trancher en brique 4.
- **2 sorties = Q(s, rien) et Q(s, battre)** ; décider = **argmax**.
- **Stabilité DQN** : **replay buffer** + **réseau-cible** copié périodiquement.

### Curriculum (chaque point = une étape socratique)

**Phase A — Brique 1 concrète : backprop multi-couches** ✅ TERMINÉE
1. **Forward de l'MLP, reformulé.** ✅ `CMLP::forward()` — ReLU caché, linéaire sortie,
   cache `sortieCaches[]` et `sortie[]`.
2. **Backprop de la couche de sortie.** ✅ `erreur = sortie[action] - cible` ;
   `CNeurone::backward()` met à jour biais + poids et retourne `gradInputs[]`.
3. **Backprop vers la couche cachée.** ✅ `erreur_cachée[i] = gradInputs[i] * relu'(z[i])` ;
   gradient-check : diff = 2×10⁻⁸. ← brique 1 close.

**Phase B — Brique 2 concrète : Q-learning sur Flappy** ✅ TERMINÉE
4. **État & récompense.** ✅ Mêmes 12 entrées que le GA (comparaison à iso). Constantes :
   `RL_GAMMA=0.99`, `RL_REWARD_TICK=1.0`, `RL_REWARD_DEAD=-10.0`, `RL_REWARD_PIPE=10.0`.
5. **Q-valeurs & cible de Bellman.** ✅ `actEpsilonGreedy()` (tirage vs argmax + décroissance
   ε par épisode) ; `computeTarget()` utilise `mlpTarget` pour `max Q(s',a')`.
6. **Le pas d'apprentissage.** ✅ `learn()` : `computeTarget` → re-forward sur `s` → `backward`.
   Ordre critique : `computeTarget` écrase le cache avec `s'`, le re-forward le remet sur `s`.
7. **Stabilité.** ✅ Replay buffer circulaire `RL_BUFFER_SIZE=10000` ; réseau-cible copié tous
   les `RL_TARGET_UPDATE=500` steps. Batch=32 transitions aléatoires par tick.
8. **Intégration & UI.** ✅ `think()` + `afterStep(bool died, tuyaux)` dans la boucle
   `onTimer`. Labels : RL épisode / score / meilleur score / ε. Timer à 1ms pour accélérer.

### Fichiers à toucher (au fil des étapes)
- Existants réorganisés : `CFlappy.h/.cpp` (base), `CFlappyGA.h/.cpp` (GA, intact).
- À créer : `CFlappyRL.h/.cpp` (passe `Common::estFlappyRL` à la base, contient `CMLP*`).
- `common.h` : constantes (η, γ, ε, tailles de couches, taille du buffer).
- `mainwindow.{cpp}` + `mainwindow.ui` : bascule GA/RL + labels (étape 8).
- `src.pro` : ajouter `CFlappyRL` aux SOURCES/HEADERS.

### Vérification
- **Étape 3** : gradient-check numérique (|grad analytique − grad par différences finies| petit).
- **Phase B** : compiler (`qmake` + `make`), lancer, observer le score RL grimper au fil des
  épisodes et le comparer au GA ; toggle pour basculer à chaud.

> **REPRISE : Phase C — DeepCubeA sur `~/dev/clbrub`.**
> Les deux briques sont comprises et implémentées. Prochaine étape : cube numpy (Phase 0 de la roadmap §6).

---

## 9. Phase B — Résultats, pièges et conclusion

### Architecture finale de `CFlappyRL`

```
CFlappyRL : public CFlappy
├── CMLP *mlp          (réseau entraîné)
├── CMLP *mlpTarget    (réseau-cible, copié depuis mlp tous les RL_TARGET_UPDATE steps)
├── STransition buffer[RL_BUFFER_SIZE]   (ring buffer)
├── think(tuyaux)      : forward → save lastInputs/lastAction → actEpsilonGreedy → up()
└── afterStep(died, tuyaux) :
      1. Construit inputsNext[]
      2. reward = RL_REWARD_TICK (ou DEAD si died, + PIPE si score augmente)
      3. pushTransition()
      4. si died : bestScore = max(bestScore, score) ; lastScore=0 ; episode++ ; ε decay
      5. batch=32 : learn() sur transitions aléatoires du buffer
      6. maybeUpdateTarget()
```

### Constantes (common.h)

```cpp
#define RL_NB_HIDDEN        16
#define RL_GAMMA            0.99
#define RL_REWARD_TICK      1.0
#define RL_REWARD_DEAD      -10.0
#define RL_REWARD_PIPE      10.0
#define RL_EPSILON_START    1.0
#define RL_EPSILON_MIN      0.05
#define RL_EPSILON_DECAY    0.995     // par épisode (pas par tick)
#define RL_ETA              0.0003
#define RL_BUFFER_SIZE      10000
#define RL_TARGET_UPDATE    500
```

### Pièges rencontrés (à retenir)

1. **`CCapteur::init()` initialise les poids dans [-25, 25]** — conçu pour le GA, catastrophique
   pour ReLU (neurones morts ou sorties explosives). Fix : `CNeurone::initGenesSmall()` →
   valeurs dans [-0.1, 0.1], appelé dans `CMLP::CMLP()`.

2. **Division entière dans ε-greedy** : `rand() / RAND_MAX` = toujours 0 (int/int).
   Fix : `(double)rand() / RAND_MAX`.

3. **`computeTarget()` lisait les Q sur `mlp` après forward sur `mlpTarget`** — les deux
   lignes doivent utiliser `mlpTarget`.

4. **`pushTransition()` utilisait `qMax` au lieu de `qMin`** pour plafonner `bufferSize`.

5. **`FLAPPY_NB_HIDDEN` vs `RL_NB_HIDDEN` dans `CMLP::CMLP()`** — seul le constructeur avait
   été oublié, les 12 autres slots du tableau étaient des pointeurs non initialisés.

6. **`lastScore` non remis à 0 en fin d'épisode** → la récompense pipe ne se déclenchait que
   quand le score dépassait celui du dernier épisode. Fix : `lastScore = 0` quand `died`.

7. **Décroissance ε par tick vs par épisode** : avec des épisodes courts (~60 ticks),
   `DECAY=0.9995` par tick consomme l'exploration en 100 épisodes. Switché en décroissance
   par épisode avec `DECAY=0.995` → epsilon atteint 0.05 après ~600 épisodes.

8. **`stepCount` non incrémenté dans `maybeUpdateTarget()`** → cible mise à jour à chaque appel.

### Résultats de la comparaison GA vs DQN

| Métrique | GA | DQN |
|---|---|---|
| Génération / épisodes | 60 générations | ~5000 épisodes |
| Meilleur score | **100+** | **8** |
| Vitesse d'apprentissage | Très rapide | Lent |
| Variance par épisode | Faible (population stable) | Forte |

### Conclusion

Le résultat confirme exactement la théorie du §4 :

- Flappy Bird = **récompense dense** (+1/tick) → le GA évalue des politiques entières avec
  une fitness naturelle. Population de 100 → exploration massive en parallèle.
- Le DQN **fonctionne** (score 8 ≠ 0, politique réelle apprise) mais est structurellement
  désavantagé sur ce problème.
- **Ce prototype n'était pas fait pour battre le GA sur Flappy**. Il était fait pour comprendre
  et implémenter backprop + Bellman avant d'attaquer le vrai problème :

> Le Rubik's cube a une récompense **sparse** (0 partout sauf à l'état résolu).
> Le GA s'y effondre (fitness = 0 partout, rien à sélectionner).
> Le RL / DeepCubeA est **indispensable** : Bellman propage la valeur depuis l'état résolu
> et crée une pente là où il n'y en a pas.

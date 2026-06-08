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

**Phase actuelle : Phase A terminée, Phase B à démarrer.**

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
- Architecture : 11 entrées → 4 `CNeuroneRelu` cachés → 2 `CNeuroneLineaire` sorties
- `forward(inputs[])` : calcule et cache `sortieCaches[]` et `sortie[]`
- `act()` : argmax sur `sortie[]` → action 0 (rien) ou 1 (battre)
- `backward(action, cible, eta)` : backprop complète 2 couches
  - couche sortie : `erreur = sortie[action] - cible`, mise à jour poids
  - couche cachée : `erreur_cachée[i] = gradInputs[i] * (z[i] > 0 ? 1 : 0)`, mise à jour poids
- Accesseurs : `getQ(i)`, `getSortieCache(i)`, `getNeuroneSortie(i)`, `getNeuroneCache(i)`

**Gradient-check :** `checkGradient()` dans `main.cpp` — diff analytique/numérique = 2×10⁻⁸ ✓

**À faire avant de reprendre (dette technique) :**
- Tester quelques poids supplémentaires dans `checkGradient` (un poids non-biais de la sortie,
  un biais caché, un poids caché) et en faire un vrai test unitaire Qt.

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
- Réseau actuel : **11 entrées** (`FLAPPY_NB_INPUTS` = 9 rayons avant + 2 arrière, distances
  normalisées) → **4 neurones cachés** sigmoïde → **1 neurone de sortie** sigmoïde.
  Décision dans `Flappy::think()` : `sortie ≥ seuil → up()`.
- `CNeurone::eval(a)` : `z = genes[0] + Σ inputs[i]·genes[i]` ; `y = e^{az}/(e^{az}+1)`
  (`a = PENTE_NEURONE = 0.01`). `genes[0]` = biais, `genes[1..]` = poids (objets `CCapteur`).
- Entraînement GA dans `CGenetic` : tri par `getFitness()` (= `score*100000 + age`),
  croisement `Flappy::from()`, mutation aléatoire `CNeurone::mute()`. Boucle de sim dans
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

**Phase B — Brique 2 concrète : Q-learning sur Flappy**
4. **État & récompense.** État markovien (+ vitesse), actions {rien, battre}, shaping de
   récompense, γ. Trancher : état compact (dx, dy au trou, vitesse, y) vs 11 rayons.
5. **Q-valeurs & cible de Bellman.** Sortie = 2 Q-valeurs ; cible `y = r + γ·max Q(s',a')`
   (ou `y = r` si mort). → câbler `act()` ε-greedy + le calcul de la cible.
6. **Le pas d'apprentissage.** Perte `½(Q(s,a) − y)²`, backprop sur la **seule action jouée**.
   → coder `learn()` en réutilisant le backward de l'MLP.
7. **Stabilité.** Replay buffer (ring) + réseau-cible copié périodiquement, et *pourquoi*
   (décorrélation i.i.d. ; cible non mouvante). → ajouter le buffer + la copie.
8. **Intégration & UI.** Piloter un piaf épisode par épisode dans `MainWindow::onTimer`,
   bascule GA ↔ RL, afficher ε / épisode / score. → regarder apprendre.

### Fichiers à toucher (au fil des étapes)
- Nouveaux : `CMlp.h/.cpp` (phase A), `CDqn*.h/.cpp` (phase B) — noms à fixer ensemble.
- `common.h` : constantes (η, γ, ε, tailles de couches, taille du buffer).
- `flappy.{h,cpp}` : exposer l'état markovien + un mode piloté par le DQN.
- `mainwindow.{cpp}` + `mainwindow.ui` : bascule GA/RL + labels (étape 8).
- `qtflappyia.pro` : ajouter les nouveaux fichiers aux SOURCES/HEADERS.

### Vérification
- **Étape 3** : gradient-check numérique (|grad analytique − grad par différences finies| petit).
- **Phase B** : compiler (`qmake` + `make`), lancer, observer le score RL grimper au fil des
  épisodes et le comparer au GA ; toggle pour basculer à chaud.

> **REPRISE : Phase B, étape 4 — état & récompense.** Claude explique, Corentin code.

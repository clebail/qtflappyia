# Documentation technique

Ce document explique **comment le projet est construit** : la physique du vol, le
fonctionnement des capteurs, la structure du réseau de neurones et le déroulement de
l'algorithme génétique. Il s'adresse aux développeurs qui veulent comprendre ou modifier le
code. Pour une présentation grand public, voir le [README](README.md).

---

## 1. Vue d'ensemble

Le programme est une boucle de jeu Qt (`QTimer` à 10 ms) qui pilote simultanément une
population de piafs autonomes. L'interface (`MainWindow` + `SceneWidget`) affiche la scène
et les statistiques ; `CGenetic` gère le cycle évolutif.

```
MainWindow (QTimer 10 ms)
  │
  ├─ pour chaque tick :
  │    1. déplacer les tuyaux
  │    2. think() → chaque piaf consulte son réseau
  │    3. next()  → physique + détection sol/plafond
  │    4. toucheUnTuyau() → détection collision
  │    5. scoring → incrément si un tuyau est franchi
  │    6. si tous morts → CGenetic::nextGeneration()
  │
  └─ SceneWidget::paintEvent() → rendu
```

Hiérarchie des classes liées au piaf :

```
Flappy      physique (position, angle, battement d'ailes) + cerveau (neurones) + scoring
CGenetic    population de Flappy, tri, sélection, croisement
CNeurone    un neurone : poids (CCapteur[]) + seuil + eval()
CCapteur    conteneur d'un poids (double)
```

| Domaine | Fichiers clés |
|---|---|
| Boucle de jeu | `MainWindow.{h,cpp}` |
| Piaf | `flappy.{h,cpp}` |
| Tuyaux | `tuyau.{h,cpp}` |
| Rendu | `SceneWidget.{h,cpp}` |
| Cerveau | `CNeurone.{h,cpp}`, `CCapteur.{h,cpp}` |
| Évolution | `CGenetic.{h,cpp}` |
| Sprites & constantes | `common.{h,cpp}` |

---

## 2. Physique du vol (`Flappy::next`)

La simulation avance par **ticks discrets** (timer Qt à 10 ms). À chaque tick, `next()`
fait évoluer l'état du piaf et renvoie `false` si le piaf doit mourir (sortie sol/plafond).

### États de vol

Le piaf est dans l'un de trois états mutuellement exclusifs :

| État | Condition | Comportement |
|---|---|---|
| Montée | `onUp == true` | `y -= INC_UP` par tick, angle → `ANGLE_UP_MAX` |
| Chute | `onDown == true` | `y += INC_DOWN` par tick, angle → `ANGLE_DOWN_MAX` |
| Flottement | ni l'un ni l'autre | oscillation lente (`INC` pixels, alternance) |

### Transition battement d'ailes

`up()` est le seul déclencheur de montée :

```cpp
void Flappy::up() {
    angle = 0;
    nbCycleUp = 0;
    onUp = true;
    onDown = false;
    nbBattements++;
}
```

Après `MAX_CYCLE_UP = 40` ticks de montée, `next()` bascule vers `onDown = true`. Le piaf
reste en chute jusqu'au prochain `up()`, au sol ou au plafond.

### Constantes physiques

| Constante | Valeur | Rôle |
|---|---|---|
| `INC_UP` | 1 px/tick | vitesse de montée |
| `MAX_CYCLE_UP` | 40 ticks | durée de la montée (= 40 px) |
| `INC_DOWN` | 2 px/tick | vitesse de chute |
| `ANGLE_UP_MAX` | −30° | inclinaison max vers le haut |
| `ANGLE_STEP_UP` | 1°/tick | vitesse d'inclinaison en montée |
| `ANGLE_DOWN_MAX` | +90° | inclinaison max vers le bas |
| `ANGLE_STEP_DOWN` | 5°/tick | vitesse d'inclinaison en chute |

---

## 3. Détection de collision

### Sol et plafond

`Flappy::next()` retourne `false` (mort) dès que :

```cpp
y < 0 || y >= ySol - FLAPPY_WIDTH + 2 * INC_DOWN
```

`ySol` est la position du sol, mise à jour par le signal `ysolChange` de `SceneWidget`.

### Tuyaux (`Flappy::toucheUnTuyau`)

Collision AABB entre la hitbox du piaf et chaque tuyau :

```cpp
const int m = 4;   // marge de 4 px sur chaque bord
QRect birdRect(x + m, y + m, FLAPPY_WIDTH - 2*m, FLAPPY_HEIGHT - 2*m);
// → boîte effective 27×17 px
QRect pipeRect(t->getX(), t->getY(), ts.width(), TUYAU_HEIGHT);
```

La marge de 4 px compense les pixels transparents autour du sprite, pour que la collision
coïncide visuellement avec le contact entre le sprite et le tuyau.

---

## 4. Perception : les capteurs (`Flappy::raycast`)

`getSensors()` génère `FLAPPY_NB_INPUTS = FLAPPY_NB_FRONT + FLAPPY_NB_REAR = 9 + 2 = 11`
rayons, en deux groupes.

**Rayons avant** (`FLAPPY_NB_FRONT = 9`) — partent du côté droit du piaf (`getTopRight()` /
`getRight()` / `getBotomRight()` selon l'angle du rayon), répartis de −90° à +90° :

```cpp
for (int i = 0; i < FLAPPY_NB_FRONT; i++) {
    double angle = -90.0 + i * (180.0 / (FLAPPY_NB_FRONT - 1));  // -90° à +90° par 22,5°
    QPoint origin = (angle < 0) ? topRight : (angle > 0) ? botRight : right;
    result.append({ origin, raycast(origin, angle, tuyaux) });
}
```

**Rayons arrière** (`FLAPPY_NB_REAR = 2`) — deux rayons strictement verticaux partant de la
queue : l'un du coin arrière-haut (`getTopLeft()`) vers le haut (−90°), l'autre du coin
arrière-bas (`getBotomLeft()`) vers le bas (+90°) :

```cpp
QPoint topLeft = getTopLeft();
QPoint botLeft = getBotomLeft();
result.append({ topLeft, raycast(topLeft, -90.0, tuyaux) });
result.append({ botLeft, raycast(botLeft,  90.0, tuyaux) });
```

Ils donnent au piaf la distance à un obstacle situé juste au-dessus ou au-dessous de son
arrière-train, ce qui évite qu'il se cogne la queue contre un tuyau qu'il vient de dépasser
de justesse. Les coins arrière sont calculés comme `getTopRight()` / `getBotomRight()`, mais
avec un décalage de `π` (`PI`) sur l'angle pour viser le côté gauche du sprite.

`raycast(start, angleDeg, tuyaux)` avance en virgule flottante dans la direction donnée et
renvoie le premier point d'intersection rencontré, dans cet ordre de priorité :

1. bord bas d'un tuyau haut (`pipe.y + TUYAU_HEIGHT`) ;
2. bord gauche ou droit d'un tuyau (face latérale) ;
3. bord haut d'un tuyau bas (`pipe.y`) ;
4. sol (`ySol`) ou plafond (`y = 0`) ;
5. bord droit de l'écran (`SCENE_WIDTH`) en dernier recours.

La longueur euclidienne de chaque rayon, normalisée par la diagonale de l'écran
(`√(SCENE_WIDTH² + SCENE_HEIGHT²)`), constitue l'entrée correspondante du réseau. Un rayon
court signifie un obstacle proche.

---

## 5. Le cerveau : réseau de neurones

### 5.1 Un neurone (`CNeurone`)

Un neurone possède `nbGene` gènes (`CCapteur`, conteneur d'un `double`) :

- `genes[0]` = **biais** ;
- `genes[1..nbGene-1]` = **poids** appliqués aux entrées.

Évaluation (`CNeurone::eval`) — somme pondérée passée dans une **sigmoïde** :

```
σ = genes[0] + Σ  inputs[i-1] · genes[i]
                i=1..nbGene-1

sortie = e^(a·σ) / (e^(a·σ) + 1)        avec a = PENTE_NEURONE = 0.01
```

La pente `a = 0.01` aplatit la sigmoïde pour éviter un comportement tout-ou-rien. La
sortie ∈ ]0, 1[ est comparée au **seuil** propre au neurone (`getSeuil`, initialisé dans
[0.1, 0.9]) pour décider si l'action se déclenche.

Les poids sont initialisés dans [−25, +25[ (`CCapteur::init`).

### 5.2 Architecture du réseau (`Flappy`)

```
11 entrées  →  4 neurones cachés  →  1 neurone de sortie  →  battre des ailes ?
```

Chaque neurone caché : `CNeurone(FLAPPY_NB_INPUTS + 1)` = **12 gènes** (biais + 11 entrées).
Neurone de sortie : `CNeurone(FLAPPY_NB_HIDDEN + 1)` = **5 gènes** (biais + 4 sorties cachées).

Total par piaf : **4 × 12 + 5 = 53 poids**.

Propagation avant dans `Flappy::think()` :

```cpp
// Couche cachée
double hiddenOut[FLAPPY_NB_HIDDEN];
for (int i = 0; i < FLAPPY_NB_HIDDEN; i++) {
    neuronesCaches[i]->setInputs(inputs);
    hiddenOut[i] = neuronesCaches[i]->eval(PENTE_NEURONE);
}
// Neurone de sortie
neuroneSortie->setInputs(hiddenOut);
if (neuroneSortie->eval(PENTE_NEURONE) >= neuroneSortie->getSeuil() && !onUp) {
    up();
}
```

La garde `&& !onUp` empêche d'appeler `up()` en pleine montée, ce qui réinitialiserait
`nbCycleUp` et bloquerait la physique (angle figé, transition vers la chute jamais déclenchée).

---

## 6. L'algorithme génétique (`CGenetic`)

`TAILLE_POPULATION = 100` piafs. Cycle par génération (`nextGeneration`) :

```
calculer allTimeBestScore
triPopulation()          → tri décroissant par fitness
keepCount = max(2, popSize/5)  → top 20 % = parents (20 individus)

pour i dans [keepCount, popSize[ :
    choisir i1, i2 aléatoirement dans [0, keepCount[
    population[i]->from(population[i1], population[i2])

pour tout piaf :
    reset(startX, startY, ySol)   → position initiale, neurones conservés
```

### 6.1 Fitness (`Flappy::getFitness`)

```cpp
if (nbBattements == 0) return -1;          // n'a jamais battu des ailes → écarté
return score * 100000 + age;
```

- `score` = nombre de paires de tuyaux franchies (incrémenté quand le bord droit d'un
  tuyau bas atteint `FLAPPY_START_X`) ;
- `age` = nombre de ticks survécus.

La pondération 100 000 garantit que franchir un tuyau vaut toujours plus que survivre
indéfiniment sans en passer aucun. Les piafs avec `nbBattements == 0` (fitness −1) tombent en
bas du tri et ne sont jamais tirés comme parents.

### 6.2 Croisement (`Flappy::from`)

Pour chaque neurone (4 cachés + 1 sortie) :

```cpp
int s = rand() % neurone->getNbGene();
neurone->from(*f1->neurone, *f2->neurone, s);  // croisement à un point
if (rand() % 100 < TAUX_MUTATION)              // 25 % de chance
    neurone->mute(rand() % neurone->getNbGene());
```

**Croisement** (`CNeurone::from`) : croisement à un point sur le vecteur de gènes — les
gènes `[0, s[` viennent du parent 1, les gènes `[s, nbGene[` du parent 2. Le **seuil** est
hérité aléatoirement de l'un ou l'autre parent.

**Mutation** (`CNeurone::mute`) : réinitialise un gène au hasard (`CCapteur::init` →
nouvelle valeur dans [−25, +25[), avec 50 % de chances de réinitialiser aussi le seuil.

**Élitisme** : les 2 meilleurs individus (indices 0 et 1 après tri) ne reçoivent pas de
`from()` — seul leur `reset()` de position est effectué. Leurs poids sont transmis intacts
à la génération suivante.

---

## 7. Géométrie des tuyaux

Un tuyau haut (`estTuyauHaut`) stocke `y = yT - TUYAU_HEIGHT - TUYAU_GAP` en interne, de
sorte que son **bord inférieur** (ouverture haute de la fenêtre) soit à `y + TUYAU_HEIGHT =
yT - TUYAU_GAP`.

Un tuyau bas (`estTuyauBas`) stocke `y = yT`, son **bord supérieur** (ouverture basse) étant
directement `yT`.

La fenêtre (gap) entre les deux tuyaux vaut exactement `TUYAU_GAP = 120` px, centrée sur
`yT`.

`yT` est tiré uniformément dans `[TUYAU_GAP, SCENE_HEIGHT - SOL_HEIGHT[` à chaque nouvelle
paire, soit `[120, 400[` avec les constantes actuelles.

---

## 8. Constantes de réglage

Tous les principaux leviers se trouvent dans `common.h` et `flappy.h` :

| Constante | Valeur | Rôle |
|---|---|---|
| `TAILLE_POPULATION` | 100 | nombre de piafs par génération |
| `TAUX_MUTATION` | 25 | probabilité (%) de mutation par neurone |
| `FLAPPY_NB_FRONT` | 9 | rayons avant (−90° à +90°) |
| `FLAPPY_NB_REAR` | 2 | rayons arrière verticaux (queue) |
| `FLAPPY_NB_INPUTS` | 11 | total des capteurs = `FRONT + REAR` |
| `FLAPPY_NB_HIDDEN` | 4 | nombre de neurones cachés |
| `PENTE_NEURONE` | 0.01 | pente de la sigmoïde |
| `TUYAU_GAP` | 120 px | ouverture entre les deux tuyaux |
| `INC_UP` | 1 px/tick | vitesse de montée |
| `MAX_CYCLE_UP` | 40 ticks | durée d'un battement d'ailes |
| `INC_DOWN` | 2 px/tick | vitesse de chute |
| `FLAPPY_WIDTH/HEIGHT` | 35×25 px | dimensions du sprite |

---

## 9. Pour démarrer dans le code

Quelques bons points d'entrée pour la lecture :

1. `MainWindow::onTimer` — la boucle de jeu de haut niveau.
2. `Flappy::think` — propagation avant + décision de battre des ailes.
3. `Flappy::getSensors` / `Flappy::raycast` — construction des 11 entrées (9 avant + 2 arrière).
4. `Flappy::next` — la physique du vol tick par tick.
5. `CGenetic::nextGeneration` — sélection, croisement, reset de la population.
6. `Flappy::from` + `CNeurone::from` / `mute` — la mécanique génétique.

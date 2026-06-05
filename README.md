# FlappyIA — Apprendre à voler par évolution

Ce projet apprend à un piaf à **traverser des tuyaux tout seul**, sans jamais lui expliquer
quand battre des ailes. Personne ne programme « bats des ailes ici » ou « attends encore un peu » : les oiseaux
partent de zéro, la grande majorité chute immédiatement… puis, génération après génération,
une population entière finit par naviguer entre les tuyaux.

Le tout repose sur deux idées :

- un **réseau de neurones** sert de cerveau à chaque piaf (il transforme ce qu'il perçoit en
  une décision : battre des ailes ou non) ;
- un **algorithme génétique** joue le rôle de la sélection naturelle (les meilleurs se
  reproduisent, les pires disparaissent).

---

## Comment « pense » un piaf

Chaque oiseau est autonome. À chaque instant, il ne dispose que de ce qu'il « voit » autour
de lui, et il doit décider quoi faire.

### Ce qu'il perçoit (les entrées)

Le piaf lance **9 rayons vers l'avant** depuis son côté droit, répartis uniformément de
−90° (vers le haut) à +90° (vers le bas), par pas de 22,5°, plus **2 rayons verticaux vers
l'arrière** (un vers le haut, un vers le bas) partant de sa queue. Chaque rayon avance
jusqu'à toucher :

- le bord inférieur d'un tuyau du haut,
- le bord supérieur d'un tuyau du bas,
- ou une limite de l'écran.

La **longueur de chaque rayon**, normalisée, constitue une des **11 entrées** du réseau.
Plus un rayon est court, plus un obstacle est proche dans cette direction. Les 2 rayons
arrière permettent au piaf de « sentir » un tuyau juste au-dessus ou au-dessous de sa
queue, pour ne plus s'y cogner l'arrière après l'avoir dépassé de justesse.

```
   ↑    ↑  (-90°)         ↑ = rayons avant (côté droit)
   ┊  ↗    (-67,5°)       ┊ = rayons arrière verticaux (queue)
   ┊ ↗     (-45°)
   ┊↗      (-22,5°)
   🐤→      (0°)
   ┊↘      (+22,5°)
   ┊ ↘     (+45°)
   ┊  ↘    (+67,5°)
   ↓    ↓  (+90°)
```

### Ce qu'il décide (la sortie)

Le cerveau produit **une seule décision** à chaque instant : battre des ailes ou non. La
décision est prise par un neurone de sortie dont la valeur (comprise entre 0 et 1) est
comparée à un seuil. Si elle dépasse ce seuil, le piaf bat des ailes.

Une contrainte physique s'applique : le piaf ne peut pas battre des ailes en pleine montée. Il doit
terminer son élan avant de pouvoir rebondir, ce qui force des rythmes de battement d'ailes naturels
plutôt que des mises à puissance continue.

---

## La physique du vol

- **Battement d'ailes** : à l'impulsion, le piaf monte à raison de 1 pixel par tick pendant 40 ticks,
  en s'inclinant progressivement jusqu'à −30°.
- **Chute** : après les 40 ticks, il redescend à 2 pixels par tick, en basculant vers +90°.
- **Ouverture** : l'espace entre les deux tuyaux fait 120 pixels. Le piaf mesure 35×25 px
  (hitbox effective 27×17 px, avec une marge de 4 px de chaque côté pour que le contact
  corresponde visuellement à la collision).

---

## Comment la population apprend

On ne corrige jamais un piaf individuellement. On fait **évoluer une population entière**
(100 oiseaux) sur plusieurs générations.

Une génération se déroule ainsi :

1. **Épreuve.** Les 100 piafs sont lâchés en même temps sur le même niveau. Chacun vole
   avec son propre cerveau jusqu'à toucher un tuyau, le sol ou le plafond.
2. **Notation.** Chaque piaf reçoit un **score de fitness** :
   - les piafs qui n'ont **jamais battu des ailes** sont écartés d'office (fitness −1) ;
   - pour les autres : `pipes passés × 100 000 + nombre de ticks survécus`.
   Traverser un tuyau vaut donc 100 000 fois plus que survivre une seconde de plus — ce qui
   force l'émergence du comportement de navigation plutôt que d'une simple survie passive.
3. **Sélection.** On trie les piafs par fitness. Le **top 20 %** (20 individus) constitue
   l'élite parentale.
4. **Reproduction.** Les 80 % restants sont remplacés par des enfants issus du croisement de
   deux parents tirés aléatoirement dans l'élite. Les 2 meilleurs individus sont conservés
   tels quels (élitisme).
5. **Mutation.** Chaque neurone d'un enfant a 25 % de chances de subir une mutation :
   un de ses poids est réinitialisé aléatoirement, et son seuil a 50 % de chances de l'être
   aussi. C'est ce qui introduit de la nouveauté et évite que toute la population converge
   trop vite vers une stratégie médiocre.

Au fil des générations, les bonnes stratégies de vol se diffusent et se raffinent.

---

## L'interface

La fenêtre affiche :

- à gauche : la scène de jeu — tous les piafs vivants volent en même temps ; les capteurs
  du premier piaf encore en vie peuvent être affichés via la case **« Afficher capteurs »** ;
- à droite : un panneau de suivi — numéro de génération, nombre de piafs encore en vie,
  meilleur score de tous les temps, meilleur score de la génération en cours.

---

## Construire et lancer

Le projet est écrit en **C++ avec Qt 5**.

Dépendances (exemple sous Debian/Ubuntu) :

```bash
sudo apt install qtbase5-dev
```

Compilation puis lancement :

```bash
qmake
make
./qtflappyia
```

---

## Documentation technique

Pour comprendre le fonctionnement interne (physique du vol, capteurs raycast, structure du
réseau de neurones, détail de l'algorithme génétique), voir la
**[documentation technique → `ARCHITECTURE.md`](ARCHITECTURE.md)**.

---

## En résumé

> On ne dit jamais au piaf quand battre des ailes. On lui donne des yeux (11 rayons), un cerveau
> (4 neurones cachés + 1 neurone de sortie), et une seule règle :
> **les meilleurs se reproduisent.** Le reste émerge tout seul.

# CY-Connect

CY-Connect est un jeu de stratégie développé en langage C, inspiré du principe du Puissance 4.

Le but du jeu est d’aligner **5 pièces identiques** sur un plateau composé de **8 colonnes** et **6 lignes**.  
Le projet a été réalisé de manière **modulaire**, avec plusieurs fichiers séparés afin de rendre le code plus clair, plus organisé et plus facile à maintenir.

---

## Principe du jeu

Le jeu se joue à **2 ou 3 joueurs**.

Chaque joueur possède un symbole :

| Joueur | Symbole |
|---|---|
| Joueur 1 | `X` |
| Joueur 2 | `O` |
| Joueur 3 | `A` |

À chaque tour, le joueur actif doit :

1. choisir une colonne où insérer sa pièce ;
2. laisser la pièce tomber avec la gravité ;
3. choisir un pivot pour une zone de rotation ;
4. choisir le sens de rotation ;
5. appliquer la rotation ;
6. laisser la gravité s’appliquer à nouveau.

Le premier joueur qui réussit à aligner **5 pièces identiques** gagne la partie.

---

## Plateau de jeu

Le plateau contient :

- **8 colonnes**
- **6 lignes**
- des cases vides représentées par `.`
- des cases bloquées représentées par `#`
- des pièces de joueurs représentées par `X`, `O` ou `A`

Les quatre coins du plateau sont bloqués au début de la partie.

---

## Règles principales

- Une partie peut se jouer à **2 ou 3 joueurs**.
- Une pièce ne peut être placée que dans une colonne jouable.
- Une colonne est jouable si sa case du haut est vide.
- Après l’insertion d’une pièce, une zone de rotation est imposée.
- La zone de rotation peut être de taille **3x3** ou **5x5**.
- Le pivot choisi doit permettre à la zone de rester entièrement dans le plateau.
- La zone choisie doit contenir la pièce qui vient d’être jouée.
- La rotation peut se faire vers la gauche ou vers la droite.
- Après chaque rotation, la gravité est appliquée.
- La partie se termine lorsqu’un joueur aligne **5 pièces** ou lorsque le plateau est plein.

---

## Fonctionnalités

- Jeu jouable à **2 ou 3 joueurs**
- Plateau de **8 x 6**
- Alignement de **5 pièces** pour gagner
- Gestion des cases bloquées
- Rotation de zones **3x3** ou **5x5**
- Choix du sens de rotation
- Application de la gravité
- Détection automatique du ou des gagnants
- Gestion des égalités
- Sauvegarde d’une partie
- Chargement d’une partie sauvegardée
- Projet structuré en plusieurs fichiers

---

## Compilation


Pour compiler le programme, exécuter la commande suivante dans le terminal :

```bash
make

Si la compilation ne fonctionne pas correctement, il est possible qu’il y ait eu un problème avec une ancienne compilation ou avec certains fichiers générés. Dans ce cas, tapez d’abord :

make clean

Puis relancez la compilation avec :

make

Une fois la compilation terminée, lancez le jeu avec :

./cy6connect

Le jeu devrait alors se lancer correctement.

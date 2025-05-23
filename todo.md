# Wacky Assholes Fighting For Loot: EX.

## ABSTRACTION EN CLASSES

client-side:
- séparer la logique network et la logique graphique
- créer un systeme de scène pour avoir un menu principal
- gérer le fullscreen


## DEPLACEMENTS [DONE]

client-side:
- [x] refactor
- [ ] envoyer les positions que si elles ont changé
- [ ] systeme de scene
- [ ] inclure un systeme de gui pour envoyer un pseudo


server-side:
- [x] refactor
- [x] appliquer les skins a tous les joueurs avec un packet dédié
- [ ] enregistrer des username pour tous les joueurs avec un packet dédié



```
formattage des paquets
CONNEXION `0|ID`
DECONNEXION `1|ID`
POSITION `2|ID|X|Y`
ENVOI_ID `3|ID`
ENVOI_SKIN `4|ID|lien_skin.png`
```

dump des sources pour aider au debug
```bash
find . -type f \( -name '*.cpp' -o -name '*.hpp' \) \
  -print0 | sort -z | \
  xargs -0 -I{} sh -c 'echo "### {}" ; cat "{}"; echo' \
  > all_source_dump.txt
```

compter les lignes
```bash
find . -type f -exec wc -l '{}' \; | awk '{ SUM += $1; } END { print SUM; }'
```

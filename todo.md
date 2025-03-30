## ABSTRACTION EN CLASSES

client-side:
- séparer la logique network et la logique graphique
- créer un systeme de scène pour avoir un menu principal
- gérer le fullscreen


## DEPLACEMENTS [DONE]

client-side:
- [x] liste des joueurs présents via une unordered_map
- [x] draw de tous les joueurs
- [x] reformatter les strings de paquets

server-side:
- [x] broadcast de des evenements de connexion et deconnexion
- [x] reformatter les strings de paquets


```
formattage des paquets
CONNEXION `0|ID`
DECONNEXION `1|ID`
POSITION `2|ID|X|Y`
```
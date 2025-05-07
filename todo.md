## ABSTRACTION EN CLASSES

client-side:
- séparer la logique network et la logique graphique
- créer un systeme de scène pour avoir un menu principal
- gérer le fullscreen


## DEPLACEMENTS [DONE]

client-side:
- [ ] refactor
- [ ] systeme de scene
- [ ] inclure un systeme de gui pour envoyer un pseudo


server-side:
- [ ] refactor
- [ ] appliquer les skins a tous les joueurs avec un packet dédié
- [ ] enregistrer des username pour tous les joueurs avec un packet dédié
- [ ] autorité et sécurité
- [ ] système de ban
- [ ] systeme de mute


```
formattage des paquets
CONNEXION `0|ID`
DECONNEXION `1|ID`
POSITION `2|ID|X|Y`
ENVOI ID `3|ID`
```
/***************************************************


pour récupérer un projet entier :

Établir la branche de suivi : 
git branch --set-upstream-to=origin/main main

Synchronisation avec le dépôt :
git reset --hard origin/main


En général :

1. Vérifier l’état du dépôt : 
git status
2. Stager les modifications : 
git add .
3. Valider les changements : 
git commit -m "Message de validation"
4. Récupérer les mises à jour du dépôt distant : 
git fetch
5. Fusionner les changements du dépôt distant : 
git merge origin/<nom_de_branche>
6. Mettre à jour la branche locale directement (remplace fetch et merge): 
git pull
7. Pousser les changements locaux : 
git push











**************************************************/

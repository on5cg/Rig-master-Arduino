/***************************************************
Token, expire 06/02/2025 :
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
*****************************************************
Activez le stockage des identifiants en permanence :
git config --global credential.helper store

Activez le stockage des identifiants sécurisé pendant 1 heure :
git config --global credential.helper 'cache --timeout=3600'

pour récupérer un projet entier :

Établir la branche de suivi : 
git branch --set-upstream-to=origin/main main

Synchronisation avec le dépôt :
git reset --hard origin/main

Débloquer si erreur "Repository rule violations found for refs/heads/main":
git commit --amend

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
git add .
ou :
git commit -a
git push











**************************************************/

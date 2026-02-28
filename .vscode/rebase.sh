git reset --soft HEAD~$1

git commit -m "$2"

git push --force

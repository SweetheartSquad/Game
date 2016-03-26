"C:\Program Files (x86)\Google\Chrome\Application\chrome.exe" www.sweetheartsquad.com/scenario/service/dump_data

ping 127.0.0.1 -n 200 > nul

git checkout master
git checkout .
git pull origin master

git pull origin ee
git add --all
git commit -m "merge"

git pull origin ian
git add --all
git commit -m "merge"

git pull origin sean
git add --all
git commit -m "merge"

git checkout sean
git merge master
git add --all
git commit -m "merge"


git checkout ian
git merge master
git add --all
git commit -m "merge"


git checkout ee
git merge master
git add --all
git commit -m "merge"

git push

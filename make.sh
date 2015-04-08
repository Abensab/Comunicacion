#!/bin/bash

printf "\nChecking for missing packages...\n"
tmp=$(dpkg-query -W -f='${Package} ${Status} \n' libasound2-dev | grep "no package found")

if [[ $tmp =~ "no package found" ]]
then
printf "Working\n\n"
apt-get install $tmp
else
printf "Packages installed\n"
fi

printf "Location:\n"
route=$(pwd)
echo $route

printf "\nCompilating lib...\n"
gcc SuperWavApp.c $route/lib/funcions.c $route/lib/connection/socketUtils.c $route/lib/connection/client.c $route/lib/connection/server.c $route/lib/sound/spatiallib.c -o SuperWavApp

printf "\nCompilating SuperWav5...\n"
cd ./lib/sound
sudo gcc superwavlib.c spatiallib.c ./../funcions.c -o superwav5 -lasound -lm

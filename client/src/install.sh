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
cd ..
route=$(pwd)
echo $route


printf "\nCompilating SuperWavClient...\n"
#sudo gcc $route/src/SuperWavAppClient.c $route/lib/funcions.c $route/lib/connection/socketUtils.c $route/lib/connection/client.c $route/lib/sound/spatiallib.c $route/lib/sound/superwavlib.c -o SuperWavAppClient -lasound -lm -lpthread
sudo gcc -Wall -Wextra $route/src/SuperWavAppClient.c $route/lib/funcionslib.c $route/lib/connection/socketUtils.c $route/lib/connection/client.c $route/lib/sound/spatiallib.c $route/lib/sound/processSoundlib.c $route/lib/sound/superwavlib.c -o SuperWavAppClient -lasound -lm -lpthread

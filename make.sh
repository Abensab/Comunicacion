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

printf "\nCompilating lib...\n"
gcc SuperWavApp.c ./lib/connection.c ./lib/spatiallib.c -o SuperWavApp

printf "\nCompilating SuperWav5...\n"
cd ./lib
sudo gcc superwavlib.c spatiallib.c connection.c -o superwav5 -lasound -lm

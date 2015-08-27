#!/bin/bash

echo "Compiling..."
echo " "
cd src/
g++ -c main.cpp ECal.cpp -I/Documents/SFML/SFML_SRC/include 
echo "Linking..."
echo " "

mv *.o ../linkers
cd ../linkers

g++ main.o ECal.o -o ecal -L/Documents/SFML/SFML_SRC/lib -lsfml-graphics -lsfml-window -lsfml-system

mv ecal ../
cd ../

if [ -e compile.sh~ ] ;
then 
    rm compile.sh~ 
fi

cd include/
if [ -e *.hh~ ];
then
    rm *.hh~
fi

cd ..
echo "Executing..."
echo " "
./ecal


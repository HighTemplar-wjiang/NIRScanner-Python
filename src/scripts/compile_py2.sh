#!/usr/bin/env bash
# Get script directory.
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo $DIR

if swig_loc="$(type -p "swig")" || [[ -z $swig_loc ]];
then
    swig -c++ -python $DIR/../NIRScanner.i
else
    echo "Did not detect swig, using generated Python Interface."
fi

gcc -fpic -c $DIR/../*.c
g++ -fpic -c $DIR/../*.cpp
g++ -fpic -c $DIR/../*.cxx -I/usr/include/python2.7
mv ./*.o $DIR/../build
# g++ -o ../build/main ../build/*.o -ludev
g++ -shared $DIR/../build/*.o -ludev -lpython2.7 -o $DIR/../build/_NIRScanner.so.2
cp $DIR/../build/_NIRScanner.so.2 $DIR/../../lib/

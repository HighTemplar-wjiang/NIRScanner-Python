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

# Find Python version & set library path.
PYTHON3_VERSION=$(python3 -c 'import sys; print(".".join(map(str, sys.version_info[:2])))')

# Compile.
gcc -fpic -c $DIR/../*.c
g++ -fpic -c $DIR/../*.cpp
g++ -fpic -c $DIR/../*.cxx -I/usr/include/python${PYTHON3_VERSION}
mv ./*.o $DIR/../build
# g++ -o ../build/main ../build/*.o -ludev
g++ -shared $DIR/../build/*.o -ludev -lpython${PYTHON3_VERSION}m -o $DIR/../build/_NIRScanner.so.3
cp $DIR/../build/_NIRScanner.so.3 $DIR/../../lib/

# Clean .o files.
rm $DIR/../build/*.o

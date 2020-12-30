#!/bin/bash


### -m32 -m64 Generates code for a 32-bit or 64-bit environment
if $(uname -m | grep '64'); then
  FLAG="-m64"
else
  FLAG="-m32"
fi

PROG_NAME="main"
cd src
gcc import_ex.c ./utils.c ./netcdf_utils.c -o $PROG_NAME -Wall $FLAG -lm -lnetcdf -O3
./$PROG_NAME ./liste_fic.txt


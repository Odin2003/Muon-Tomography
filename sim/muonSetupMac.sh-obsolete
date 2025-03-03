#!/bin/bash

#  setup ROOT environment
source /Applications/root_v6.22.06/bin/thisroot.sh

#  drop_from_path taken from /root/build/bin/thisroot.sh
drop_from_path()
{
   # Assert that we got enough arguments
   if test $# -ne 2 ; then
      echo "drop_from_path: needs 2 arguments"
      return 1
   fi

   local p=$1
   local drop=$2

   newpath=`echo $p | sed -e "s;:${drop}:;:;g" \
                          -e "s;:${drop}\$;;g"   \
                          -e "s;^${drop}:;;g"   \
                          -e "s;^${drop}\$;;g"`
}

export CRYHOME=/Users/kunori/skdir/hep/cry/cry_v1.7
export CRYDATAPATH=/Users/kunori/skdir/hep/cry/cry_v1.7/data

# source /Users/kunori/skdir/hep/g4/geant4.10.07-install/share/Geant4-10.7.0/geant4make/geant4make.sh
cd /Users/kunori/skdir/hep/g4/geant4.10.07-install/share/Geant4-10.7.0/geant4make
source geant4make.sh
cd -
export G4BIN="$PWD"


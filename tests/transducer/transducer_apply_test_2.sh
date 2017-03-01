#!/bin/bash

VataCliDir=../../build/cli
VataCliExec=vata

InputAut=Automaton_2.txt
InputTrans=Transducer_2.txt
ResultAut=Result.txt
ReferenceDir=$(pwd)/ref
ReferenceOutput=apply_res_2.txt

#
# Check if exists vata executable, tree automaton and tree transducer
#

if [ ! -f $VataCliDir/$VataCliExec ]; then
    echo "transducer_apply_test_2.sh : Error, unable to find $VataCliExec file in $VataCliDir directory. Try to run make release in libvata directory. Terminating ..."
    exit 1
fi

if [ ! -f $InputAut ]; then
    echo "transducer_apply_test_2.sh : Error, unable to find $InputAut file. Terminating ..."
    exit 1
fi

if [ ! -f $InputTrans ]; then
    echo "transducer_apply_test_2.sh : Error, unable to find $InputTrans file. Terminating ..."
    exit 1
fi

#
# Apply transducer on automaton. Store result to the file
#

$VataCliDir/$VataCliExec apply $InputAut $InputTrans > $ResultAut

#
# Check both inclusion
#

$VataCliDir/$VataCliExec incl $ResultAut $ReferenceDir/$ReferenceOutput > /dev/null

if [ $? -ne 0 ]; then
    echo "$(tput setaf 1)Test transducer_apply_test_2 failed. $(tput sgr0)"
    exit 1
fi

$VataCliDir/$VataCliExec incl $ReferenceDir/$ReferenceOutput $ResultAut > /dev/null

if [ $? -ne 0 ]; then
    echo "$(tput setaf 1)Test transducer_apply_test_2 failed. $(tput sgr0)"
    exit 1
fi

echo "$(tput setaf 2)Test transducer_apply_test_2 passed. $(tput sgr0)"

#rm $ResultAut
exit 0
#!/bin/bash

VataCliDir=../../build/cli
VataCliExec=vata

AutomatonDir=automata
ResultAut=ResultAut.txt

if [ ! -f $VataCliDir/$VataCliExec ]; then
    echo "transducer_apply_test_2.sh : Error, unable to find $VataCliExec file in $VataCliDir directory. Try to run make release in libvata directory. Terminating ..."
    exit 1
fi

#Arguments = input_automaton, transducer, ref_result, num_of_test
function apply_test
{
  local InputAut=$1
  local InputTrans=$2
  local ReferenceOutput=$3

  local RetVal=""

  if [ ! -f $AutomatonDir/$InputAut ]; then
      echo "$(tput setaf 1)transducer_apply_tests.sh (Test no. $4) : Error, unable to find $AutomatonDir/$InputAut file. Terminating ...$(tput sgr0)"
      return
  fi

  if [ ! -f $AutomatonDir/$InputTrans ]; then
      echo "$(tput setaf 1)transducer_apply_tests.sh (Test no. $4) : Error, unable to find $AutomatonDir/$InputTrans file. Terminating ...$(tput sgr0)"
      return
  fi

  #
  # Apply transducer on automaton. Store result to the file
  #

  $VataCliDir/$VataCliExec apply $AutomatonDir/$InputAut $AutomatonDir/$InputTrans > $ResultAut

  #
  # Check both inclusion
  #

  RetVal=$($VataCliDir/$VataCliExec incl $ResultAut $AutomatonDir/$ReferenceOutput)

  if [ $RetVal != "1" ]; then
      echo "$(tput setaf 1)transducer_apply_tests.sh (Test no. $4) Test failed. $(tput sgr0)"
      rm -rf $ResultAut
      return
  fi

  RetVal=$($VataCliDir/$VataCliExec incl $AutomatonDir/$ReferenceOutput $ResultAut)

  if [ $RetVal != "1" ]; then
      echo "$(tput setaf 1)transducer_apply_tests.sh (Test no. $4) Test failed. $(tput sgr0)"
      rm -rf $ResultAut
      return
  fi

  echo "$(tput setaf 2)transducer_apply_tests.sh (Test no. $4) Test passed. $(tput sgr0)"
  rm -f $ResultAut
}   #end of function apply_test


apply_test Automaton_0.txt Transducer_0.txt Ref_output_0.txt 0
apply_test Automaton_1.txt Transducer_1.txt Ref_output_1.txt 1
apply_test Automaton_2.txt Transducer_2.txt Ref_output_2.txt 2

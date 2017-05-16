// Two-way token passing protocol

// VATA headers
#include <vata/explicit_tree_aut.hh>
#include <vata/explicit_tree_trans.hh>
#include "../src/loadable_aut.hh"
#include "../src/vata_abstraction.hh"
#include <vata/parsing/timbuk_parser.hh>
#include <vata/serialization/timbuk_serializer.hh>

#include "VataArtmc.hh"

const char* Init =
    "Ops            \n"
    "Automaton     aut\n"
    "States        qN qD qF\n"
    "Final States  qD\n"
    "Transitions\n"
    "null         -> qN\n"
    "dot          -> qD\n"
    "null(qN, qN) -> qN\n"
    "null(qD, qN) -> qD\n"
    "null(qN, qD) -> qD\n"
    "dot(qD, qN)  -> qF\n"
    "dot(qN, qD)  -> qF\n"
    "null(qD, qD) -> qF\n"
    "dot(qD, qD)  -> qF\n";


const char* Bad1 =
        "Ops            \n"
                "Automaton     aut\n"
                "States        qN qF qD\n"
                "Final States  qN qF\n"
                "Transitions\n"
                "null           -> qN\n"
                "dot            -> qD\n"
                "null(qN, qN)   -> qN\n"
                "dot(qN, qN)    -> qD\n" 
                "null(qD, qN)   -> qD\n"
                "null(qN, qD)   -> qD\n"
                "dot(qD, qN)    -> qF\n"
                "dot(qN, qD)    -> qF\n"
                "dot(qD, qD)    -> qF\n"//
                "dot(qF, qN)    -> qF\n"
                "dot(qF, qD)    -> qF\n"
                "dot(qN, qF)    -> qF\n"
                "dot(qD, qF)    -> qF\n"
                "null(qF, qN)    -> qF\n"
                "null(qF, qD)    -> qF\n"
                "null(qN, qF)    -> qF\n"
                "null(qD, qF)    -> qF\n"
                "null(qF, qF)    -> qF\n"//
                "dot(qF, qF)    -> qF\n";//

const char* transition =
    "Ops           \n"
    "Automaton     aut\n"
    "States        qN qD qDU qDD qF\n"
    "Final States  qA\n" //dot-up, dot-down
    "Transitions\n"
    "null/null         -> qN\n"
    "dot/null          -> qDU\n"
    "null/dot          -> qDD\n"
    "null/dot (qDU, qN)-> qA\n"
    "null/dot (qN, qDU)-> qA\n"
    "dot/null (qDD, qN)-> qA\n"
    "null/null (qN, qN)-> qN\n"
    "dot/null (qN, qDD)-> qA\n"
    "dot/null (qN, qN) -> qDU\n"
    "null/dot (qN, qN) -> qDD\n"
    "null/null (qA, qN)-> qA\n"
    "null/null (qN, qA)-> qA\n"
    "dot/dot (qA, qN)  -> qF\n"
    "dot/dot (qN, qA)  -> qF\n"
    "null/null (qA, qDU) -> qF\n"
    "null/null (qDU, qA) -> qF\n"
    "dot/dot (qA, qDU) -> qF\n"
    "dot/dot (qDU, qA) -> qF\n"
    "null/null (qA, qDD) -> qF\n"
    "null/null (qDD, qA) -> qF\n"
    "dot/dot (qA, qDD) -> qF\n"
    "dot/dot (qDD, qA) -> qF\n";

int main()
{

    std::cout << "Result : " << VATA::ARTMC::artmcPredicate(Init, Bad1, transition) << std::endl;

    return 0;
}


// Token Passing Protocol

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
                "States        qN qD\n"
                "Final States  qD\n"
                "Transitions\n"
                "null         -> qN\n"
                "dot          -> qD\n"
                "null(qN, qN) -> qN\n"
                "null(qD, qN) -> qD\n"
                "null(qN, qD) -> qD\n";



const char* Bad1 =
        "Ops            \n"
                "Automaton     aut\n"
                "States        qN qD qF qNN\n"
                "Final States  qF qNN\n"
                "Transitions\n"
                "null           -> qN\n"
                "null           -> qNN\n"
                "dot            -> qD\n"
                "null(qNN, qNN) -> qNN\n"
                "null(qN, qN)   -> qN\n"
                "dot (qN, qN)   -> qD\n"
                "null(qD, qN)   -> qD\n"
                "null(qN, qD)   -> qD\n"
                "null(qD, qD)   -> qF\n"
                "null(qF, qD)   -> qF\n"
                "null(qF, qN)   -> qF\n"
                "null(qF, qF)   -> qF\n"
                "null(qN, qF)   -> qF\n"
                "null(qD, qF)   -> qF\n"
                "dot (qF, qD)   -> qF\n"
                "dot (qF, qN)   -> qF\n"
                "dot (qF, qF)   -> qF\n"
                "dot (qN, qF)   -> qF\n"
                "dot (qD, qF)   -> qF\n";


const char* transition =
        "Ops           \n"
                "Automaton     aut\n"
                "States        qN q1\n"
                "Final States  qN\n"
                "Transitions\n"
                "null/null         -> qN\n"
                "dot/null          -> q1\n"
                "dot/null (qN, qN) -> q1\n"
                "null/dot (qN, q1) -> qN\n"
                "null/dot (q1, qN) -> qN\n"
                "null/null(qN, qN) -> qN\n";


int main()
{
    std::cout << "Result : " << VATA::ARTMC::artmcPredicate(Bad1, Init, transition) << std::endl;

    return 0;
}
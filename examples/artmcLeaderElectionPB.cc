// Tree Arbiter

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
    "States        p0 p1\n"
    "Final States  p0\n"
    "Transitions\n"
    "cc         -> p0\n"
    "nn         -> p1\n"
    "u(p0,p0)   -> p0\n"
    "u(p0,p1)   -> p0\n"
    "u(p1,p0)   -> p0\n"
    "u(p1,p1)   -> p1\n";


const char* Bad1 =
    "Ops           \n" 
    "Automaton     aut\n"
    "States        p0 p1 p2 p3\n"
    "Final States  p0 p3\n"
    "Transitions   \n"
    "nn -> p0      \n"
    "cc -> p1      \n"   
    "ee -> p2      \n"
    "u(p1,p0) -> p1   \n" 
    "c(p1,p2) -> p2   \n"
    "u(p0,p3) -> p3   \n"  
    "c(p2,p3) -> p3   \n"
    "n(p1,p1) -> p1   \n"
    "e(p1,p2) -> p2   \n"
    "n(p3,p0) -> p3   \n"
    "e(p2,p3) -> p3   \n"  
    "c(p1,p1) -> p1   \n"
    "u(p1,p2) -> p2   \n"
    "c(p3,p0) -> p3   \n"
    "u(p2,p3) -> p3   \n"
    "n(p0,p0) -> p0   \n"
    "e(p1,p1) -> p1   \n"
    "n(p2,p1) -> p2   \n"
    "e(p3,p0) -> p3   \n"
    "n(p3,p2) -> p3   \n"
    "c(p0,p0) -> p0   \n"
    "u(p1,p1) -> p1   \n"
    "c(p2,p1) -> p2   \n"
    "u(p3,p0) -> p3   \n"
    "c(p3,p2) -> p3   \n"
    "e(p0,p0) -> p0   \n"
    "n(p0,p2) -> p2   \n"
    "e(p2,p1) -> p2   \n"
    "n(p1,p3) -> p3   \n"
    "e(p3,p2) -> p3   \n"
    "u(p0,p0) -> p0   \n"
    "c(p0,p2) -> p2   \n"
    "u(p2,p1) -> p2   \n"
    "c(p1,p3) -> p3   \n"
    "u(p3,p2) -> p3   \n"
    "n(p0,p1) -> p1   \n"
    "e(p0,p2) -> p2   \n"
    "n(p2,p2) -> p3   \n"
    "e(p1,p3) -> p3   \n"
    "n(p3,p3) -> p3   \n"
    "c(p0,p1) -> p1   \n"
    "u(p0,p2) -> p2   \n"
    "c(p2,p2) -> p3   \n"
    "u(p1,p3) -> p3   \n"
    "c(p3,p3) -> p3   \n"
    "e(p0,p1) -> p1   \n"
    "n(p2,p0) -> p2   \n"
    "e(p2,p2) -> p3   \n"
    "n(p3,p1) -> p3   \n"
    "e(p3,p3) -> p3   \n"
    "u(p0,p1) -> p1   \n"
    "c(p2,p0) -> p2   \n"
    "u(p2,p2) -> p3   \n"
    "c(p3,p1) -> p3   \n"
    "u(p3,p3) -> p3   \n"
    "n(p1,p0) -> p1   \n"
    "e(p2,p0) -> p2   \n"
    "n(p0,p3) -> p3   \n"
    "e(p3,p1) -> p3   \n"
    "c(p1,p0) -> p1   \n"
    "u(p2,p0) -> p2   \n"
    "c(p0,p3) -> p3   \n"
    "u(p3,p1) -> p3   \n"
    "e(p1,p0) -> p1   \n"
    "n(p1,p2) -> p2   \n"
    "e(p0,p3) -> p3   \n"
    "n(p2,p3) -> p3   \n";

const char* transition =
    "Ops           \n"
	"Automaton     aut\n"
	"States        qC qN qEl qU qJel qCh\n"
	"Final States  qEl qCh\n"
	"Transitions\n"
	"cc/cc         -> qC\n"
	"cc/ee         -> qJel\n"
	"nn/nn         -> qN\n"
	"ee/ee         -> qEl\n"
	"c/c (qC, qC)  -> qC\n"
	"c/c (qC, qN)  -> qC\n"
	"c/c (qN, qC)  -> qC\n"
	"c/e (qC, qN)  -> qJel\n"
	"c/e (qC, qC)  -> qJel\n"
	"c/e (qN, qC)  -> qJel\n"
	"n/n (qN, qN)  -> qN\n"
	"u/u (qU, qU)  -> qU\n"
	"u/u (qN, qU)  -> qU\n"
	"u/u (qU, qN)  -> qU\n"
	"u/u (qU, qC)  -> qU\n"
	"u/u (qC, qU)  -> qU\n"
	"u/u (qC, qC)  -> qU\n"
	"u/u (qN, qN)  -> qU\n"
	"u/u (qN, qC)  -> qU\n"
	"u/u (qC, qN)  -> qU\n"
	"u/u (qCh, qU)  -> qCh\n"
	"u/u (qU, qCh)  -> qCh\n"
	"u/u (qCh, qN)  -> qCh\n"
	"u/u (qN, qCh)  -> qCh\n"
	"u/u (qCh, qC)  -> qCh\n"
	"u/u (qC, qCh)  -> qCh\n"
	"u/c (qC, qC)  -> qCh\n"
	"u/c (qN, qC)  -> qCh\n"
	"u/c (qC, qN)  -> qCh\n"
	"u/n (qN, qN)  -> qCh\n"
	"e/e (qN, qEl)  -> qEl\n"
	"e/e (qEl, qN)  -> qEl\n"
	"e/e (qEl, qC)  -> qEl\n"
	"e/e (qC, qEl)  -> qEl\n"
	"e/e (qN, qJel)  -> qEl\n"
	"e/e (qJel, qN)  -> qEl\n"
	"e/e (qJel, qC)  -> qEl\n"
	"e/e (qC, qJel)  -> qEl\n";

typedef VATA::ExplicitTreeAut Automaton;

typedef VATA::ExplicitTreeTrans Transducer;
typedef VATA::LoadableAut<Transducer> LoadableTransducer;
typedef VATA::LoadableAut<VATA::ExplicitTreeAutCore> LoadableAutCore;

int main()
{
    std::cout << "Result : " << VATA::ARTMC::artmcPredicate(Bad1, Init, transition) << std::endl;

    return 0;
}

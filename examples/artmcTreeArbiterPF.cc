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
    "Final States  p1\n"
    "Transitions\n"
    "ii           -> p0\n"
    "i(p0,p0)     -> p0\n"
    "t(p0,p0)     -> p1\n";


const char* Bad1 =
    "Ops           \n"
    "Automaton     aut\n"
    "States        p0 pt perr\n"
    "Final States  p0 perr\n"
    "Transitions\n"
    "rr             -> p0\n"
    "ii             -> p0\n"
    "tt             -> pt\n"
    "i(p0,p0)       -> p0\n"
    "i(pt,p0)       -> pt\n"
    "i(p0,pt)       -> pt\n"
    "i(pt,pt)       -> perr\n"
    "i(perr,p0)     -> perr\n"
    "i(perr,pt)     -> perr\n"
    "i(p0,perr)     -> perr\n"
    "i(pt,perr)     -> perr\n"
    "i(perr,perr)   -> perr\n"
    "b(p0,p0)       -> p0\n"
    "b(pt,p0)       -> pt\n"
    "b(p0,pt)       -> pt\n"
    "b(pt,pt)       -> perr\n"
    "b(perr,p0)     -> perr\n"
    "b(perr,pt)     -> perr\n"
    "b(p0,perr)     -> perr\n"
    "b(pt,perr)     -> perr\n"
    "b(perr,perr)   -> perr\n"
    "r(p0,p0)       -> p0\n"
    "r(pt,p0)       -> pt\n"
    "r(p0,pt)       -> pt\n"
    "r(pt,pt)       -> perr\n"
    "r(perr,p0)     -> perr\n"
    "r(perr,pt)     -> perr\n"
    "r(p0,perr)     -> perr\n"
    "r(pt,perr)     -> perr\n"
    "r(perr,perr)   -> perr\n"
    "t(p0,p0)       -> pt\n"
    "t(pt,p0)       -> perr\n"
    "t(p0,pt)       -> perr\n"
    "t(pt,pt)       -> perr\n"
    "t(perr,p0)     -> perr\n"
    "t(perr,pt)     -> perr\n"
    "t(p0,perr)     -> perr\n"
    "t(pt,perr)     -> perr\n"
    "t(perr,perr)   -> perr\n";


const char* transition =
    "Ops           \n"
    "Automaton     aut\n"
    "States        qi qreq qrel qgrant qcomp\n"
    "Final States  qcomp\n"
    "Transitions\n"
    "ii/ii               -> qi\n"
    "tt/ii               -> qrel\n"
    "i/r(qreq, qreq)     -> qreq\n"	
    "r/t(qreq, qi)       -> qgrant\n"
    "t/b(qi, qgrant)     -> qcomp\n"
    "t/i(qi, qreq)       -> qrel\n"
    "b/t(qi, qrel)       -> qcomp\n"
    "b/b(qi, qcomp)      -> qcomp\n"
    "ii/rr               -> qreq\n"
    "i/i(qi, qi)         -> qi\n"
    "r/r(qreq, qi)       -> qreq\n"
    "r/t(qi, qreq)       -> qgrant\n"
    "t/b(qgrant, qreq)   -> qcomp\n"
    "t/i(qreq, qi)       -> qrel\n"
    "b/t(qrel, qreq)     -> qcomp\n"
    "b/b(qcomp, qreq)    -> qcomp\n"
    "rr/rr               -> qreq\n"
    "i/r(qreq, qi)       -> qreq\n"
    "r/r(qi, qreq)       -> qreq\n"
    "r/t(qreq, qreq)     -> qgrant\n"
    "t/b(qreq, qgrant)   -> qcomp\n"
    "t/i(qreq, qreq)     -> qrel\n"
    "b/t(qreq, qrel)     -> qcomp\n"
    "b/b(qreq, qcomp)    -> qcomp\n"
    "rr/tt               -> qgrant\n"
    "i/r(qi, qreq)       -> qreq\n"
    "r/r(qreq, qreq)     -> qreq\n"
    "t/b(qgrant, qi)     -> qcomp\n"
    "t/i(qi, qi)         -> qrel\n"
    "b/t(qrel, qi)       -> qcomp\n"
    "b/b(qcomp, qi)      -> qcomp\n"
    "t/t(qreq, qreq)     -> qcomp\n"
    "t/t(qi, qi)         -> qcomp\n"
    "t/t(qreq, qi)       -> qcomp\n"
    "t/t(qi, qreq)       -> qcomp\n";


int main()
{
    std::cout << "Result : " << VATA::ARTMC::artmcPredicate(Init, Bad1, transition) << std::endl;

    return 0;
}

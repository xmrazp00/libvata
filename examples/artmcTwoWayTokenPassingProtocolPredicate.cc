// Two-way token passing protocol

// VATA headers
#include <vata/explicit_tree_aut.hh>
#include <vata/explicit_tree_trans.hh>
#include "../src/loadable_aut.hh"
#include "../src/vata_abstraction.hh"
#include <vata/parsing/timbuk_parser.hh>
#include <vata/serialization/timbuk_serializer.hh>

#include "VataArtmc.hh"

//declaration for backward run
bool backwardRun(VATA::ExplicitTreeAut& AutAlpha,
                 VATA::ExplicitTreeTrans& transducer,
                 const std::vector<VATA::ExplicitTreeAut>& autStack);

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
    "Ops           \n"
    "Automaton     aut\n"
    "States        qN\n"
    "Final States  qN\n"
    "Transitions\n"
    "null          -> qN\n"
    "null(qN, qN)  -> qN\n";


const char* Bad2 =
    "Ops            \n"
    "Automaton     aut\n"
    "States        qN qD qF\n"
    "Final States  qF\n"
    "Transitions\n"
    "null         -> qN\n"
    "dot          -> qD\n"
    "null(qN, qN) -> qN\n"
    "dot (qN, qN) -> qD\n"
    "null(qD, qN) -> qD\n"
    "null(qN, qD) -> qD\n"
    "null(qD, qD) -> qF\n"
    "null(qF, qD) -> qF\n"
    "null(qF, qN) -> qF\n"
    "null(qF, qF) -> qF\n"
    "null(qN, qF) -> qF\n"
    "null(qD, qF) -> qF\n"
    "dot (qF, qD) -> qF\n"
    "dot (qF, qN) -> qF\n"
    "dot (qF, qF) -> qF\n"
    "dot (qN, qF) -> qF\n"
    "dot (qD, qF) -> qF\n";


const char* transition =
    "Ops           \n"
    "Automaton     aut\n"
    "States        qN q1\n"
    "Final States  qN\n"
    "Transitions\n"
    "null/null         -> qN\n"
    "dot/null          -> q1\n"
    "null/dot          -> q2\n"
    "dot/null (qN, qN) -> q1\n"
    "null/dot (qN, q1) -> qN\n"
    "null/dot (q1, qN) -> qN\n"
    "null/null(qN, qN) -> qN\n"
    "null/dot (qN, qN) -> q2\n"
    "dot/null (qN, q2) -> qN\n"
    "dot/null (q2, qN) -> qN\n";


typedef VATA::ExplicitTreeAut Automaton;

typedef VATA::ExplicitTreeTrans Transducer;
typedef VATA::LoadableAut<Transducer> LoadableTransducer;
typedef VATA::LoadableAut<VATA::ExplicitTreeAutCore> LoadableAutCore;

int main()
{

    // Initialize timbuk parser and state dicts (needed only for print out aut/trans)
    std::unique_ptr<VATA::Parsing::AbstrParser> parser(
            new VATA::Parsing::TimbukParser());

    VATA::AutBase::StateDict stateDictInit;
    VATA::AutBase::StateDict stateDictTrans;

    VATA::AutBase::StateDict stateDictBad1;
    VATA::AutBase::StateDict stateDictBad2;

    //Loading Init automaton
    Automaton AutAlpha;
    AutAlpha.LoadFromString(*parser, Init, stateDictInit);
    const VATA::ExplicitTreeAut autInit = AutAlpha;

    //Loading Bad States Automatons and making their union
    Automaton BadStates1;
    Automaton BadStates2;

    BadStates1.LoadFromString(*parser, Bad1, stateDictBad1);
    BadStates2.LoadFromString(*parser, Bad2, stateDictBad2);

    VATA::ExplicitTreeAutCore::StateToStateMap stateTranslMap1;
    VATA::ExplicitTreeAutCore::StateToStateMap stateTranslMap2;

    VATA::ExplicitTreeAut BadStates =
            VATA::ExplicitTreeAut::Union(BadStates1, BadStates2, &stateTranslMap1, &stateTranslMap2);

    VATA::AutBase::StateDict stateDictBad = VATA::Util::CreateUnionStringToStateMap(stateDictBad1, stateDictBad2,
                                                                    &stateTranslMap1, &stateTranslMap2);

    //Loading transducer and setting symbol alphabet for Transducer - it has to be shared
    Transducer transTransitions;
    transTransitions.SetAlphabet(AutAlpha.GetAlphabet());
    transTransitions.LoadFromString(*parser, transition, stateDictTrans);

    std::cout << "Result  : " << VATA::ARTMC::artmcPredicate(AutAlpha, BadStates, transTransitions) << std::endl;

    return 0;
}

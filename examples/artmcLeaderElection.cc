// Tree Arbiter

// VATA headers
#include <vata/explicit_tree_aut.hh>
#include <vata/explicit_tree_trans.hh>
#include "../src/loadable_aut.hh"
#include "../src/vata_abstraction.hh"
#include <vata/parsing/timbuk_parser.hh>
#include <vata/serialization/timbuk_serializer.hh>

//declaration for backward run
bool backwardRun(VATA::ExplicitTreeAut& AutAlpha,
                 VATA::ExplicitTreeTrans& transducer,
                 const std::vector<VATA::ExplicitTreeAut>& autStack);

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
    // Initialize timbuk parser and state dicts (needed only for print out aut/trans)
    std::unique_ptr<VATA::Parsing::AbstrParser> parser(
            new VATA::Parsing::TimbukParser());

    VATA::AutBase::StateDict stateDictInit;
    VATA::AutBase::StateDict stateDictTrans;

    VATA::AutBase::StateDict stateDictBad;

    //Loading Init automaton
    Automaton AutAlpha;
    AutAlpha.LoadFromString(*parser, Init, stateDictInit);
    const VATA::ExplicitTreeAut autInit = AutAlpha;

    //Loading Bad States Automaton
    Automaton BadStates;

    BadStates.LoadFromString(*parser, Bad1, stateDictBad);

    //Loading transducer and setting symbol alphabet for Transducer - it has to be shared
    Transducer transTransitions;
    transTransitions.SetAlphabet(AutAlpha.GetAlphabet());
    transTransitions.LoadFromString(*parser, transition, stateDictTrans);


    VATA::AutBase::StateDict stateDictAlpha = VATA::AutBase::StateDict(stateDictInit);
    VATA::AutBase::StateDict stateDictFixpoint = VATA::AutBase::StateDict(stateDictInit);

    //Definition of fixpoint automaton and stack for backward run
    VATA::ExplicitTreeAut autFixpoint (autInit);
    std::vector<VATA::ExplicitTreeAut> autStack;

    //Main loop ARTMC
    bool fixpoint = false;
    int n = 0;
    int height = 1;

    while(!fixpoint)
    {
        //Push actual aut alpha to stack - each round
        autStack.push_back(AutAlpha);
        VATA::AutBase::StateDict stateDictTmp;

        //checking whether aut alpha has empty intersection with bad states automaton
        //if it is not empty, trigger backward run, which has 2 cases : 1)return value is true - exists real counterexample
        //2)return value is false - counterexample was spurious, increasing height of abstraction
        if(!VATA::ExplicitTreeAut::Intersection(AutAlpha, BadStates).IsLangEmpty())
        {
            AutAlpha = VATA::ExplicitTreeAut::Intersection(AutAlpha, BadStates);

            if((n == 0 && height == 1) ||
               backwardRun(AutAlpha, transTransitions, autStack))
            {
                std::cout << "Result: Real counterexample." << std::endl;
                break;
            }
            else
            {
                std::cout << "Info: Increase height on " << height + 1 << " after " << n << " steps." << std::endl;

                autStack.clear();
                AutAlpha = Automaton(autInit);
                autFixpoint.Clear();
                stateDictFixpoint = VATA::AutBase::StateDict();

                height++;
                n = 0;
                continue;
            }
        }

        //make abstraction and apply transducer
        AutAlpha = Automaton(VATAAbstraction::GetHeightAbstraction(AutAlpha.GetCore(), height));
        AutAlpha = transTransitions.Apply(AutAlpha, false, &stateDictTmp, &stateDictTrans);
        stateDictAlpha = stateDictTmp;

        //checking whether abstraction and apply brings some new state/states
        //if so, fixpoint was reached, end of computation
        if(VATA::ExplicitTreeAut::CheckInclusion(AutAlpha, autFixpoint))
        {
            std::cout << "Result:  Fixpoint reached after " << n << " steps." << std::endl;
            fixpoint = true;
        }
        else
        {
            Automaton::StateToStateMap stateTranslMap1;
            Automaton::StateToStateMap stateTranslMap2;

            autFixpoint = VATA::ExplicitTreeAut::Union(AutAlpha, autFixpoint, &stateTranslMap1, &stateTranslMap2);
            stateDictFixpoint =
                    VATA::Util::CreateUnionStringToStateMap(stateDictAlpha, stateDictFixpoint, &stateTranslMap1, &stateTranslMap2);
        }

        //removing useless and unreachable states - works also without them
        AutAlpha = AutAlpha.RemoveUselessStates();
        AutAlpha = AutAlpha.RemoveUnreachableStates();

        n++;
    }

    return 0;
}

//Definition of backward run
bool backwardRun(VATA::ExplicitTreeAut& AutAlpha,
                 VATA::ExplicitTreeTrans& transducer,
                 const std::vector<VATA::ExplicitTreeAut>& autStack)
{
    int h = autStack.size();

    while(h)
    {
        VATA::ExplicitTreeAut StoredAut = autStack.at(h - 1);

        AutAlpha = transducer.Apply(AutAlpha, true);              //without dicts...

        AutAlpha = VATA::ExplicitTreeAut::Intersection(StoredAut, AutAlpha);

        AutAlpha = AutAlpha.RemoveUselessStates();
        AutAlpha = AutAlpha.RemoveUnreachableStates();

        if(AutAlpha.IsLangEmpty())
        {
            return false;
        }
        h--;
    }
    return true;
}